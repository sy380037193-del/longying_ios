/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "3d/CCMesh.h"
#include "3d/CCMeshSkin.h"
#include "3d/CCSkeleton3D.h"
#include "3d/CCMeshVertexIndexData.h"
#include "3d/CCVertexAttribBinding.h"
#include "2d/CCLight.h"
#include "2d/CCScene.h"
#include "base/CCEventDispatcher.h"
#include "base/CCDirector.h"
#include "base/CCConfiguration.h"
#include "renderer/CCTextureCache.h"
#include "renderer/CCMaterial.h"
#include "renderer/CCTechnique.h"
#include "renderer/CCPass.h"
#include "renderer/CCRenderer.h"
#include "renderer/backend/Buffer.h"
#include "renderer/backend/Device.h"
#include "renderer/backend/Program.h"
#include "math/Mat4.h"
#include "platform/CCPlatformMacros.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS && defined(CC_USE_METAL)
#include <cmath>
#endif

using namespace std;

NS_CC_BEGIN

// Helpers

//sampler uniform names, only diffuse and normal texture are supported for now
std::string s_uniformSamplerName[] =
{
    "",//NTextureData::Usage::Unknown,
    "",//NTextureData::Usage::None
    "",//NTextureData::Usage::Diffuse
    "",//NTextureData::Usage::Emissive
    "",//NTextureData::Usage::Ambient
    "",//NTextureData::Usage::Specular
    "",//NTextureData::Usage::Shininess
    "u_normalTex",//NTextureData::Usage::Normal
    "",//NTextureData::Usage::Bump
    "",//NTextureData::Usage::Transparency
    "",//NTextureData::Usage::Reflection
};


// helpers
void Mesh::resetLightUniformValues()
{
    const auto& conf = Configuration::getInstance();
    int maxDirLight = conf->getMaxSupportDirLightInShader();
    int maxPointLight = conf->getMaxSupportPointLightInShader();
    int maxSpotLight = conf->getMaxSupportSpotLightInShader();

    _dirLightUniformColorValues.assign(maxDirLight, Vec3::ZERO);
    _dirLightUniformDirValues.assign(maxDirLight, Vec3::ZERO);

    _pointLightUniformColorValues.assign(maxPointLight, Vec3::ZERO);
    _pointLightUniformPositionValues.assign(maxPointLight, Vec3::ZERO);
    _pointLightUniformRangeInverseValues.assign(maxPointLight, 0.0f);

    _spotLightUniformColorValues.assign(maxSpotLight, Vec3::ZERO);
    _spotLightUniformPositionValues.assign(maxSpotLight, Vec3::ZERO);
    
    //TODO It's strange that init _spotLightUniformDirValues to zeros will cause no light effects on iPhone6 and iPhone6s, but works well on iPhoneX
    //fix no light effects on iPhone6 and iPhone6s
    _spotLightUniformDirValues.assign(maxSpotLight, Vec3(FLT_EPSILON, 0.0f, 0.0f));
    _spotLightUniformInnerAngleCosValues.assign(maxSpotLight, 1.0f);
    _spotLightUniformOuterAngleCosValues.assign(maxSpotLight, 0.0f);
    _spotLightUniformRangeInverseValues.assign(maxSpotLight, 0.0f);
}

//Generate a dummy texture when the texture file is missing
static Texture2D * getDummyTexture()
{
    auto texture = Director::getInstance()->getTextureCache()->getTextureForKey("/dummyTexture");
    if(!texture)
    {
#ifdef NDEBUG
        unsigned char data[] ={0,0,0,0};//1*1 transparent picture
#else
        unsigned char data[] ={255,0,0,255};//1*1 red picture
#endif
        Image * image =new (std::nothrow) Image();
        image->initWithRawData(data,sizeof(data),1,1,sizeof(unsigned char));
        texture=Director::getInstance()->getTextureCache()->addImage(image,"/dummyTexture");
        image->release();
    }
    return texture;
}


Mesh::Mesh()
: _skin(nullptr)
, _visible(true)
, _isTransparent(false)
, _force2DQueue(false)
, _meshIndexData(nullptr)
, _blend(BlendFunc::ALPHA_NON_PREMULTIPLIED)
, _blendDirty(true)
, _material(nullptr)
, _texFile("")
{
    
}
Mesh::~Mesh()
{
    for (auto &tex : _textures){
        CC_SAFE_RELEASE(tex.second);
    }
    CC_SAFE_RELEASE(_skin);
    CC_SAFE_RELEASE(_meshIndexData);
    CC_SAFE_RELEASE(_material);
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS && defined(CC_USE_METAL)
    CC_SAFE_RELEASE(_iosCpuSkinningBuffer);
#endif
}

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS && defined(CC_USE_METAL)
backend::Buffer* Mesh::updateIosCpuSkinningBuffer(const Vec4* matrixPalette, ssize_t matrixPaletteRows)
{
    const auto* vertexData = _meshIndexData->getMeshVertexData();
    const auto& source = vertexData->_vertexData;
    const auto& indices = _meshIndexData->_indexData;
    const auto stride = vertexData->getSizePerVertex();
    if (!matrixPalette || matrixPaletteRows <= 0 || source.empty() || indices.empty() || stride <= 0)
        return nullptr;

    int positionOffset = -1;
    int normalOffset = -1;
    int tangentOffset = -1;
    int binormalOffset = -1;
    int blendWeightOffset = -1;
    int blendIndexOffset = -1;
    backend::VertexFormat positionFormat = backend::VertexFormat::FLOAT;
    backend::VertexFormat normalFormat = backend::VertexFormat::FLOAT;
    backend::VertexFormat tangentFormat = backend::VertexFormat::FLOAT;
    backend::VertexFormat binormalFormat = backend::VertexFormat::FLOAT;
    backend::VertexFormat blendWeightFormat = backend::VertexFormat::FLOAT;
    backend::VertexFormat blendIndexFormat = backend::VertexFormat::FLOAT;

    int offset = 0;
    for (const auto& attribute : vertexData->_attribs)
    {
        switch (attribute.vertexAttrib)
        {
            case shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION:
                positionOffset = offset;
                positionFormat = attribute.type;
                break;
            case shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL:
                normalOffset = offset;
                normalFormat = attribute.type;
                break;
            case shaderinfos::VertexKey::VERTEX_ATTRIB_TANGENT:
                tangentOffset = offset;
                tangentFormat = attribute.type;
                break;
            case shaderinfos::VertexKey::VERTEX_ATTRIB_BINORMAL:
                binormalOffset = offset;
                binormalFormat = attribute.type;
                break;
            case shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_WEIGHT:
                blendWeightOffset = offset;
                blendWeightFormat = attribute.type;
                break;
            case shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_INDEX:
                blendIndexOffset = offset;
                blendIndexFormat = attribute.type;
                break;
            default:
                break;
        }
        offset += attribute.getAttribSizeBytes();
    }

    if (offset != stride || positionOffset < 0 || blendWeightOffset < 0 || blendIndexOffset < 0 ||
        positionFormat != backend::VertexFormat::FLOAT3 ||
        blendWeightFormat != backend::VertexFormat::FLOAT4 ||
        blendIndexFormat != backend::VertexFormat::FLOAT4 ||
        (normalOffset >= 0 && normalFormat != backend::VertexFormat::FLOAT3) ||
        (tangentOffset >= 0 && tangentFormat != backend::VertexFormat::FLOAT3) ||
        (binormalOffset >= 0 && binormalFormat != backend::VertexFormat::FLOAT3))
    {
        return nullptr;
    }

    const auto dataSize = source.size() * sizeof(source[0]);
    if (dataSize % stride != 0)
        return nullptr;

    _iosCpuSkinningVertices = source;
    const auto* sourceBytes = reinterpret_cast<const unsigned char*>(source.data());
    auto* outputBytes = reinterpret_cast<unsigned char*>(_iosCpuSkinningVertices.data());
    const auto vertexCount = dataSize / stride;

    for (const auto vertexIndex : indices)
    {
        if (vertexIndex >= vertexCount)
            return nullptr;

        const auto* sourceVertex = sourceBytes + vertexIndex * stride;
        auto* outputVertex = outputBytes + vertexIndex * stride;
        const auto* position = reinterpret_cast<const float*>(sourceVertex + positionOffset);
        const auto* weights = reinterpret_cast<const float*>(sourceVertex + blendWeightOffset);
        const auto* blendIndices = reinterpret_cast<const float*>(sourceVertex + blendIndexOffset);

        Vec4 rows[3] = {Vec4::ZERO, Vec4::ZERO, Vec4::ZERO};
        for (int influence = 0; influence < 4; ++influence)
        {
            const float weight = weights[influence];
            if (influence > 0 && weight <= 0.0f)
                break;
            if (!std::isfinite(weight) || !std::isfinite(blendIndices[influence]))
                return nullptr;

            const int boneIndex = static_cast<int>(blendIndices[influence]);
            const int paletteOffset = boneIndex * 3;
            if (boneIndex < 0 || paletteOffset + 2 >= matrixPaletteRows)
                return nullptr;

            for (int row = 0; row < 3; ++row)
            {
                const auto& paletteRow = matrixPalette[paletteOffset + row];
                rows[row].x += paletteRow.x * weight;
                rows[row].y += paletteRow.y * weight;
                rows[row].z += paletteRow.z * weight;
                rows[row].w += paletteRow.w * weight;
            }
        }

        auto* outputPosition = reinterpret_cast<float*>(outputVertex + positionOffset);
        outputPosition[0] = position[0] * rows[0].x + position[1] * rows[0].y + position[2] * rows[0].z + rows[0].w;
        outputPosition[1] = position[0] * rows[1].x + position[1] * rows[1].y + position[2] * rows[1].z + rows[1].w;
        outputPosition[2] = position[0] * rows[2].x + position[1] * rows[2].y + position[2] * rows[2].z + rows[2].w;

        const int directionOffsets[] = {normalOffset, tangentOffset, binormalOffset};
        for (const int directionOffset : directionOffsets)
        {
            if (directionOffset < 0)
                continue;
            const auto* direction = reinterpret_cast<const float*>(sourceVertex + directionOffset);
            auto* outputDirection = reinterpret_cast<float*>(outputVertex + directionOffset);
            outputDirection[0] = direction[0] * rows[0].x + direction[1] * rows[0].y + direction[2] * rows[0].z;
            outputDirection[1] = direction[0] * rows[1].x + direction[1] * rows[1].y + direction[2] * rows[1].z;
            outputDirection[2] = direction[0] * rows[2].x + direction[1] * rows[2].y + direction[2] * rows[2].z;
        }
    }

    if (!_iosCpuSkinningBuffer || _iosCpuSkinningBuffer->getSize() != dataSize)
    {
        CC_SAFE_RELEASE(_iosCpuSkinningBuffer);
        _iosCpuSkinningBuffer = backend::Device::getInstance()->newBuffer(
            dataSize, backend::BufferType::VERTEX, backend::BufferUsage::DYNAMIC);
    }
    if (!_iosCpuSkinningBuffer)
        return nullptr;

    _iosCpuSkinningBuffer->updateData(_iosCpuSkinningVertices.data(), dataSize);
    return _iosCpuSkinningBuffer;
}
#endif

backend::Buffer* Mesh::getVertexBuffer() const
{
    return _meshIndexData->getVertexBuffer();
}

bool Mesh::hasVertexAttrib(shaderinfos::VertexKey attrib) const
{
    return _meshIndexData->getMeshVertexData()->hasVertexAttrib(attrib);
}

ssize_t Mesh::getMeshVertexAttribCount() const
{
    return _meshIndexData->getMeshVertexData()->getMeshVertexAttribCount();
}

const MeshVertexAttrib& Mesh::getMeshVertexAttribute(int idx)
{
    return _meshIndexData->getMeshVertexData()->getMeshVertexAttrib(idx);
}

int Mesh::getVertexSizeInBytes() const
{
    return _meshIndexData->getMeshVertexData()->getSizePerVertex();
}

Mesh* Mesh::create(const std::vector<float>& positions, const std::vector<float>& normals, const std::vector<float>& texs, const IndexArray& indices)
{
    int perVertexSizeInFloat = 0;
    std::vector<float> vertices;
    std::vector<MeshVertexAttrib> attribs;
    MeshVertexAttrib att;
    att.type = backend::VertexFormat::FLOAT3;
    
    if (positions.size())
    {
        perVertexSizeInFloat += 3;
        att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
        attribs.push_back(att);
    }
    if (normals.size())
    {
        perVertexSizeInFloat += 3;
        att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL;
        attribs.push_back(att);
    }
    if (texs.size())
    {
        perVertexSizeInFloat += 2;
        att.type = backend::VertexFormat::FLOAT2;
        att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
        attribs.push_back(att);
    }
    
    bool hasNormal = (normals.size() != 0);
    bool hasTexCoord = (texs.size() != 0);
    //position, normal, texCoordinate into _vertexs
    size_t vertexNum = positions.size() / 3;
    for(size_t i = 0; i < vertexNum; i++)
    {
        vertices.push_back(positions[i * 3]);
        vertices.push_back(positions[i * 3 + 1]);
        vertices.push_back(positions[i * 3 + 2]);

        if (hasNormal)
        {
            vertices.push_back(normals[i * 3]);
            vertices.push_back(normals[i * 3 + 1]);
            vertices.push_back(normals[i * 3 + 2]);
        }
    
        if (hasTexCoord)
        {
            vertices.push_back(texs[i * 2]);
            vertices.push_back(texs[i * 2 + 1]);
        }
    }
    return create(vertices, perVertexSizeInFloat, indices, attribs);
}

Mesh* Mesh::create(const std::vector<float>& vertices, int /*perVertexSizeInFloat*/, const IndexArray& indices, const std::vector<MeshVertexAttrib>& attribs)
{
    MeshData meshdata;
    meshdata.attribs = attribs;
    meshdata.vertex = vertices;
    meshdata.subMeshIndices.push_back(indices);
    meshdata.subMeshIds.push_back("");
    auto meshvertexdata = MeshVertexData::create(meshdata);
    auto indexData = meshvertexdata->getMeshIndexDataByIndex(0);
    
    return create("", indexData);
}

Mesh* Mesh::create(const std::string& name, MeshIndexData* indexData, MeshSkin* skin)
{
    auto state = new (std::nothrow) Mesh();
    state->autorelease();
    state->bindMeshCommand();
    state->_name = name;
    state->setMeshIndexData(indexData);
    state->setSkin(skin);
    
    return state;
}

void Mesh::setVisible(bool visible)
{
    if (_visible != visible)
    {
        _visible = visible;
        if (_visibleChanged)
            _visibleChanged();
    }
}

bool Mesh::isVisible() const
{
    return _visible;
}

void Mesh::setTexture(const std::string& texPath)
{
    _texFile = texPath;
    auto tex = Director::getInstance()->getTextureCache()->addImage(texPath);
    setTexture(tex, NTextureData::Usage::Diffuse);
}

void Mesh::setTexture(Texture2D* tex)
{
    setTexture(tex, NTextureData::Usage::Diffuse);
}

void Mesh::setTexture(Texture2D* tex, NTextureData::Usage usage, bool cacheFileName)
{
    // Texture must be saved for future use
    // it doesn't matter if the material is already set or not
    // This functionality is added for compatibility issues
    if (tex == nullptr)
        tex = getDummyTexture();

    CC_SAFE_RETAIN(tex);
    CC_SAFE_RELEASE(_textures[usage]);
    _textures[usage] = tex;   
    
    if (usage == NTextureData::Usage::Diffuse){
        if (_material) {
            auto technique = _material->_currentTechnique;
            for(auto& pass: technique->_passes)
            {
                pass->setUniformTexture(0, tex->getBackendTexture());
            }
        }
        
        bindMeshCommand();
        if (cacheFileName)
            _texFile = tex->getPath();
    }
    else if (usage == NTextureData::Usage::Normal) // currently only diffuse and normal are supported
    {
        if (_material){
            auto technique = _material->_currentTechnique;
            for(auto& pass: technique->_passes)
            {
                pass->setUniformNormTexture(1, tex->getBackendTexture());
            }
        }
    }
}

void Mesh::setTexture(const std::string& texPath, NTextureData::Usage usage)
{
    auto tex = Director::getInstance()->getTextureCache()->addImage(texPath);
    setTexture(tex, usage);
}

Texture2D* Mesh::getTexture() const
{
    return _textures.at(NTextureData::Usage::Diffuse);
}

Texture2D* Mesh::getTexture(NTextureData::Usage usage)
{
    return _textures[usage];
}

void Mesh::setMaterial(Material* material)
{
    if (_material != material) {
        CC_SAFE_RELEASE(_material);
        _material = material;
        CC_SAFE_RETAIN(_material);
    }
    _meshCommands.clear();

    if (_material)
    {
        for (auto technique: _material->getTechniques())
        {
            //allocate MeshCommand vector for technique
            //allocate MeshCommand for each pass
            _meshCommands[technique->getName()] = std::vector<MeshCommand>(technique->getPasses().size());
            auto &list = _meshCommands[technique->getName()];
            
            int i = 0;
            for (auto pass: technique->getPasses())
            {
#ifdef COCOS2D_DEBUG
                //make it crashed when missing attribute data
                if(_material->getTechnique()->getName().compare(technique->getName()) == 0)
                {
                    auto program = pass->getProgramState()->getProgram();
                    auto& attributes = program->getActiveAttributes();
                    auto meshVertexData = _meshIndexData->getMeshVertexData();
                    auto attributeCount = meshVertexData->getMeshVertexAttribCount();
                    CCASSERT(attributes.size() <= attributeCount, "missing attribute data");
                }
#endif
                //TODO
                auto vertexAttribBinding = VertexAttribBinding::create(_meshIndexData, pass, &list[i]);
                pass->setVertexAttribBinding(vertexAttribBinding);
                i += 1;
            }
        }
    }
    // Was the texture set before the GLProgramState ? Set it
    for(auto& tex : _textures)
        setTexture(tex.second, tex.first);
        
    
    if (_blendDirty)
        setBlendFunc(_blend);
    
    bindMeshCommand();
}

Material* Mesh::getMaterial() const
{
    return _material;
}

void Mesh::draw(Renderer* renderer, float globalZOrder, const Mat4& transform, uint32_t flags, unsigned int lightMask, const Vec4& color, bool forceDepthWrite)
{
    if (! isVisible())
        return;

    bool isTransparent = (_isTransparent || color.w < 1.f);
    float globalZ = isTransparent ? 0 : globalZOrder;
    if (isTransparent)
        flags |= Node::FLAGS_RENDER_AS_3D;

//TODO
//    _meshCommand.init(globalZ,
//                      _material,
//                      getVertexBuffer(),
//                      getIndexBuffer(),
//                      getPrimitiveType(),
//                      getIndexFormat(),
//                      getIndexCount(),
//                      transform,
//                      flags);


    if (isTransparent && !forceDepthWrite)
        _material->getStateBlock().setDepthWrite(false);
    else
        _material->getStateBlock().setDepthWrite(true);

    _material->getStateBlock().setBlend(_force2DQueue || isTransparent);

    backend::Buffer* vertexBuffer = getVertexBuffer();
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS && defined(CC_USE_METAL)
    Vec4* matrixPalette = nullptr;
    ssize_t matrixPaletteRows = 0;
    float useIosCpuSkinning = 0.0f;
    if (_skin)
    {
        bool allPassesSupportCpuSkinning = true;
        for (const auto pass : _material->_currentTechnique->_passes)
        {
            if (!pass->getProgramState()->getUniformLocation("u_iosCpuSkinning"))
            {
                allPassesSupportCpuSkinning = false;
                break;
            }
        }

        matrixPalette = _skin->getMatrixPalette();
        matrixPaletteRows = _skin->getMatrixPaletteSize();
        if (allPassesSupportCpuSkinning)
        {
            auto* cpuSkinningBuffer = updateIosCpuSkinningBuffer(matrixPalette, matrixPaletteRows);
            if (cpuSkinningBuffer)
            {
                vertexBuffer = cpuSkinningBuffer;
                useIosCpuSkinning = 1.0f;
            }
        }
    }
#endif

    // set default uniforms for Mesh
    // 'u_color' and others
    const auto scene = Director::getInstance()->getRunningScene();
    auto technique = _material->_currentTechnique;
    for(const auto pass : technique->_passes)
    {
        pass->setUniformColor(&color, sizeof(color));

        if (_skin)
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS && defined(CC_USE_METAL)
        {
            pass->setUniformMatrixPalette(matrixPalette, matrixPaletteRows * sizeof(matrixPalette[0]));
            auto location = pass->getProgramState()->getUniformLocation("u_iosCpuSkinning");
            if (location)
                pass->getProgramState()->setUniform(location, &useIosCpuSkinning, sizeof(useIosCpuSkinning));
        }
#else
            pass->setUniformMatrixPalette(_skin->getMatrixPalette(), _skin->getMatrixPaletteSizeInBytes());
#endif

        if (scene && scene->getLights().size() > 0)
        {
            setLightUniforms(pass, scene, color, lightMask);
        }
    }
    auto &commands = _meshCommands[technique->getName()];

    for (auto &command : commands)
    {
        command.init(globalZ, transform);
        command.setSkipBatching(isTransparent);
        command.setTransparent(isTransparent);
        command.set3D(!_force2DQueue);
    }

    _material->draw(commands.data(), globalZ,
                    vertexBuffer,
                    getIndexBuffer(),
                    getPrimitiveType(),
                    getIndexFormat(),
                    getIndexCount(),
                    transform);

}

void Mesh::setSkin(MeshSkin* skin)
{
    if (_skin != skin)
    {
        CC_SAFE_RETAIN(skin);
        CC_SAFE_RELEASE(_skin);
        _skin = skin;
        calculateAABB();
    }
}

void Mesh::setMeshIndexData(MeshIndexData* subMesh)
{
    if (_meshIndexData != subMesh)
    {
        CC_SAFE_RETAIN(subMesh);
        CC_SAFE_RELEASE(_meshIndexData);
        _meshIndexData = subMesh;
        calculateAABB();
        bindMeshCommand();
    }
}

void Mesh::setProgramState(backend::ProgramState* programState)
{
    auto material = Material::createWithProgramState(programState);
    if (_material)
    {
        material->setStateBlock(_material->getStateBlock());
    }
    setMaterial(material);
}

backend::ProgramState* Mesh::getProgramState() const
{
    return _material ? _material->_currentTechnique->_passes.at(0)->getProgramState() : nullptr;
}

void Mesh::calculateAABB()
{
    if (_meshIndexData)
    {
        _aabb = _meshIndexData->getAABB();
        if (_skin)
        {
            //get skin root
            Bone3D* root = nullptr;
            Mat4 invBindPose;
            if (_skin->_skinBones.size())
            {
                root = _skin->_skinBones.at(0);
                while (root) {
                    auto parent = root->getParentBone();
                    bool parentInSkinBone = false;
                    for (const auto& bone : _skin->_skinBones) {
                        if (bone == parent)
                        {
                            parentInSkinBone = true;
                            break;
                        }
                    }
                    if (!parentInSkinBone)
                        break;
                    root = parent;
                }
            }
            
            if (root)
            {
                _aabb.transform(root->getWorldMat() * _skin->getInvBindPose(root));
            }
        }
    }
}

void Mesh::bindMeshCommand()
{
    if (_material && _meshIndexData)
    {
        _material->getStateBlock().setCullFace(true);
        _material->getStateBlock().setDepthTest(true);
    }
}

void Mesh::setLightUniforms(Pass* pass, Scene* scene, const Vec4& color, unsigned int lightmask)
{
    CCASSERT(pass, "Invalid Pass");
    CCASSERT(scene, "Invalid scene");

    const auto& conf = Configuration::getInstance();
    int maxDirLight = conf->getMaxSupportDirLightInShader();
    int maxPointLight = conf->getMaxSupportPointLightInShader();
    int maxSpotLight = conf->getMaxSupportSpotLightInShader();
    auto &lights = scene->getLights();

    auto bindings= pass->getVertexAttributeBinding();

    if (bindings &&  bindings->hasAttribute(shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL))
    {
        resetLightUniformValues();

        int enabledDirLightNum = 0;
        int enabledPointLightNum = 0;
        int enabledSpotLightNum = 0;
        Vec3 ambientColor;
        for (const auto& light : lights)
        {
            bool useLight = light->isEnabled() && ((unsigned int)light->getLightFlag() & lightmask);
            if (useLight)
            {
                float intensity = light->getIntensity();
                switch (light->getLightType())
                {
                    case LightType::DIRECTIONAL:
                    {
                        if(enabledDirLightNum < maxDirLight)
                        {
                            auto dirLight = static_cast<DirectionLight *>(light);
                            Vec3 dir = dirLight->getDirectionInWorld();
                            dir.normalize();
                            const Color3B &col = dirLight->getDisplayedColor();
                            _dirLightUniformColorValues[enabledDirLightNum].set(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                            _dirLightUniformDirValues[enabledDirLightNum] = dir;
                            ++enabledDirLightNum;
                        }

                    }
                        break;
                    case LightType::POINT:
                    {
                        if(enabledPointLightNum < maxPointLight)
                        {
                            auto pointLight = static_cast<PointLight *>(light);
                            Mat4 mat= pointLight->getNodeToWorldTransform();
                            const Color3B &col = pointLight->getDisplayedColor();
                            _pointLightUniformColorValues[enabledPointLightNum].set(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                            _pointLightUniformPositionValues[enabledPointLightNum].set(mat.m[12], mat.m[13], mat.m[14]);
                            _pointLightUniformRangeInverseValues[enabledPointLightNum] = 1.0f / pointLight->getRange();
                            ++enabledPointLightNum;
                        }
                    }
                        break;
                    case LightType::SPOT:
                    {
                        if(enabledSpotLightNum < maxSpotLight)
                        {
                            auto spotLight = static_cast<SpotLight *>(light);
                            Vec3 dir = spotLight->getDirectionInWorld();
                            dir.normalize();
                            Mat4 mat= light->getNodeToWorldTransform();
                            const Color3B &col = spotLight->getDisplayedColor();
                            _spotLightUniformColorValues[enabledSpotLightNum].set(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                            _spotLightUniformPositionValues[enabledSpotLightNum].set(mat.m[12], mat.m[13], mat.m[14]);
                            _spotLightUniformDirValues[enabledSpotLightNum] = dir;
                            _spotLightUniformInnerAngleCosValues[enabledSpotLightNum] = spotLight->getCosInnerAngle();
                            _spotLightUniformOuterAngleCosValues[enabledSpotLightNum] = spotLight->getCosOuterAngle();
                            _spotLightUniformRangeInverseValues[enabledSpotLightNum] = 1.0f / spotLight->getRange();
                            ++enabledSpotLightNum;
                        }
                    }
                        break;
                    case LightType::AMBIENT:
                    {
                        auto ambLight = static_cast<AmbientLight *>(light);
                        const Color3B &col = ambLight->getDisplayedColor();
                        ambientColor.add(col.r / 255.0f * intensity, col.g / 255.0f * intensity, col.b / 255.0f * intensity);
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        if (0 < maxDirLight)
        {
            pass->setUniformDirLightColor(&_dirLightUniformColorValues[0], _dirLightUniformColorValues.size() * sizeof(_dirLightUniformColorValues[0]));
            pass->setUniformDirLightDir(&_dirLightUniformDirValues[0], _dirLightUniformDirValues.size() * sizeof(_dirLightUniformDirValues[0]));
        }

        if (0 < maxPointLight)
        {
            pass->setUniformPointLightColor(&_pointLightUniformColorValues[0], _pointLightUniformColorValues.size() * sizeof(_pointLightUniformColorValues[0]));
            pass->setUniformPointLightPosition(&_pointLightUniformPositionValues[0], _pointLightUniformPositionValues.size() * sizeof(_pointLightUniformPositionValues[0]));
            pass->setUniformPointLightRangeInverse(&_pointLightUniformRangeInverseValues[0], _pointLightUniformRangeInverseValues.size() * sizeof(_pointLightUniformRangeInverseValues[0]));
        }

        if (0 < maxSpotLight)
        {
            pass->setUniformSpotLightColor(&_spotLightUniformColorValues[0], _spotLightUniformColorValues.size() * sizeof(_spotLightUniformColorValues[0]));
            pass->setUniformSpotLightPosition(&_spotLightUniformPositionValues[0], _spotLightUniformPositionValues.size() * sizeof(_spotLightUniformPositionValues[0]));
            pass->setUniformSpotLightDir(&_spotLightUniformDirValues[0], _spotLightUniformDirValues.size() * sizeof(_spotLightUniformDirValues[0]));
            pass->setUniformSpotLightInnerAngleCos(&_spotLightUniformInnerAngleCosValues[0], _spotLightUniformInnerAngleCosValues.size() * sizeof(_spotLightUniformInnerAngleCosValues[0]));
            pass->setUniformSpotLightOuterAngleCos(&_spotLightUniformOuterAngleCosValues[0], _spotLightUniformOuterAngleCosValues.size() * sizeof(_spotLightUniformOuterAngleCosValues[0]));
            pass->setUniformSpotLightRangeInverse(&_spotLightUniformRangeInverseValues[0], _spotLightUniformRangeInverseValues.size() * sizeof(_spotLightUniformRangeInverseValues[0]));
        }

        auto ambientLightColor = Vec3(ambientColor.x, ambientColor.y, ambientColor.z);
        pass->setUniformAmbientLigthColor(&ambientLightColor, sizeof(ambientLightColor));
    }
    else // normal does not exist
    {
        Vec3 ambient(0.0f, 0.0f, 0.0f);
        bool hasAmbient = false;
        for (const auto& light : lights)
        {
            if (light->getLightType() == LightType::AMBIENT)
            {
                bool useLight = light->isEnabled() && ((unsigned int)light->getLightFlag() & lightmask);
                if (useLight)
                {
                    hasAmbient = true;
                    const Color3B &col = light->getDisplayedColor();
                    ambient.x += col.r * light->getIntensity();
                    ambient.y += col.g * light->getIntensity();
                    ambient.z += col.b * light->getIntensity();
                }
            }
        }
        if (hasAmbient)
        {
            ambient.x /= 255.f; ambient.y /= 255.f; ambient.z /= 255.f;
            //override the uniform value of u_color using the calculated color
            auto fcolor = Vec4(color.x * ambient.x, color.y * ambient.y, color.z * ambient.z, color.w);
            pass->setUniformColor(&fcolor, sizeof(fcolor));
        }
    }
}

void Mesh::setBlendFunc(const BlendFunc &blendFunc)
{
    // Blend must be saved for future use
    // it doesn't matter if the material is already set or not
    // This functionality is added for compatibility issues
    if(_blend != blendFunc)
    {
        _blendDirty = true;
        _blend = blendFunc;
    }

    if (_material) {
        //TODO set blend to Pass
        _material->getStateBlock().setBlendFunc(blendFunc);
        bindMeshCommand();
    }
}

const BlendFunc& Mesh::getBlendFunc() const
{
// return _material->_currentTechnique->_passes.at(0)->getBlendFunc();
    return _blend;
}

CustomCommand::PrimitiveType Mesh::getPrimitiveType() const
{
    return _meshIndexData->getPrimitiveType();
}

ssize_t Mesh::getIndexCount() const
{
    return _meshIndexData->getIndexBuffer()->getSize() / sizeof(uint16_t);
}

CustomCommand::IndexFormat Mesh::getIndexFormat() const
{
    return CustomCommand::IndexFormat::U_SHORT;
}

backend::Buffer* Mesh::getIndexBuffer() const
{
    return _meshIndexData->getIndexBuffer();
}
NS_CC_END
