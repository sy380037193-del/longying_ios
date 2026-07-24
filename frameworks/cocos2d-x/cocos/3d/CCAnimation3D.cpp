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

#include "3d/CCAnimation3D.h"
#include "3d/CCBundle3D.h"
#include "3d/CCIOSHeadRenderDiagnostics.h"
#include "platform/CCFileUtils.h"
#include "platform/CCPlatformConfig.h"

#include <sstream>

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include <limits.h>
#include <mach-o/dyld.h>
#endif

NS_CC_BEGIN

namespace
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
const int IOS_HEADLOCK_SHAPE_IDS[] = {
    1001, 1002, 1011, 1012, 1031, 1032,
    1101, 1102, 1111, 1112, 1131, 1132,
    2003, 2004, 2013, 2014, 2033, 2034,
    3005, 3006, 3015, 3016, 3035, 3036,
    4007, 4008, 4017, 4018, 4037, 4038,
};

std::string getIosAppBundleDirectory()
{
    char executablePath[PATH_MAX] = {};
    uint32_t pathSize = sizeof(executablePath);
    if (_NSGetExecutablePath(executablePath, &pathSize) != 0)
    {
        return "";
    }

    const std::string path(executablePath);
    const auto separator = path.find_last_of('/');
    return separator == std::string::npos ? "" : path.substr(0, separator);
}

bool resolveIosHeadlockAnimation(const std::string& requestedPath, std::string* resolvedPath)
{
    for (const auto shapeId : IOS_HEADLOCK_SHAPE_IDS)
    {
        const std::string id = std::to_string(shapeId);
        const std::string expectedSuffix = "/" + id + "/" + id + "_dress-stand.c3b";
        if (requestedPath.size() < expectedSuffix.size() ||
            requestedPath.compare(requestedPath.size() - expectedSuffix.size(), expectedSuffix.size(), expectedSuffix) != 0)
        {
            continue;
        }

        const std::string appBundleDirectory = getIosAppBundleDirectory();
        if (appBundleDirectory.empty())
        {
            return false;
        }

        const std::string payloadDirectory = appBundleDirectory + "/headlock_test_payload";
        const std::string payloadPath = payloadDirectory + "/codex_headlock_" + id + ".c3b";
        if (!FileUtils::getInstance()->isFileExist(payloadPath))
        {
            return false;
        }

        *resolvedPath = payloadPath;
        return true;
    }

    return false;
}

void logIosHeadlockAnimation(const std::string& requestedPath,
                             const std::string& resolvedPath,
                             const std::string& animationName,
                             const char* source,
                             bool success)
{
    std::ostringstream output;
    output << "event=ios_headlock_animation"
           << " requested=" << requestedPath
           << " resolved=" << resolvedPath
           << " animation=" << animationName
           << " source=" << source
           << " success=" << (success ? 1 : 0);
    ios_head_render_diagnostics::log(output.str());
}
#endif
}

Animation3D* Animation3D::create(const std::string& fileName, const std::string& animationName)
{
    std::string resolvedFileName = fileName;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    bool usesHeadlockAnimation = resolveIosHeadlockAnimation(fileName, &resolvedFileName);
    if (!usesHeadlockAnimation && fileName.find("codex_headlock_") != std::string::npos)
    {
        usesHeadlockAnimation = true;
    }
#endif

    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(resolvedFileName);
    std::string key = fullPath + "#" + animationName;
    auto animation = Animation3DCache::getInstance()->getAnimation(key);
    if (animation != nullptr)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        if (usesHeadlockAnimation)
        {
            logIosHeadlockAnimation(fileName, fullPath, animationName, "cache", true);
        }
#endif
        return animation;
    }
    
    animation = new (std::nothrow) Animation3D();
    if(animation->initWithFile(resolvedFileName, animationName))
    {
        animation->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(animation);
    }

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    if (usesHeadlockAnimation)
    {
        logIosHeadlockAnimation(fileName, fullPath, animationName, "file", animation != nullptr);
    }
#endif
    
    return animation;
}

bool Animation3D::initWithFile(const std::string& filename, const std::string& animationName)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
    
    //load animation here
    auto bundle = Bundle3D::createBundle();
    Animation3DData animationdata;
    if (bundle->load(fullPath) && bundle->loadAnimationData(animationName, &animationdata) && init(animationdata))
    {
        std::string key = fullPath + "#" + animationName;
        Animation3DCache::getInstance()->addAnimation(key, this);
        Bundle3D::destroyBundle(bundle);
        return true;
    }
    
    Bundle3D::destroyBundle(bundle);
    
    return false;
}

Animation3D::Curve* Animation3D::getBoneCurveByName(const std::string& name) const
{
    auto it = _boneCurves.find(name);
    if (it != _boneCurves.end())
        return it->second;
    
    return nullptr;
}

Animation3D::Animation3D()
: _duration(0)
{
    
}

Animation3D::~Animation3D()
{
    for (const auto& itor : _boneCurves) {
        Curve* curve = itor.second;
        CC_SAFE_DELETE(curve);
    }
}

Animation3D::Curve::Curve()
: translateCurve(nullptr)
, rotCurve(nullptr)
, scaleCurve(nullptr)
{
    
}
Animation3D::Curve::~Curve()
{
    CC_SAFE_RELEASE_NULL(translateCurve);
    CC_SAFE_RELEASE_NULL(rotCurve);
    CC_SAFE_RELEASE_NULL(scaleCurve);
}

bool Animation3D::init(const Animation3DData &data)
{
    _duration = data._totalTime;

    for(const auto& iter : data._translationKeys)
    {
        Curve* curve = _boneCurves[iter.first];
        if( curve == nullptr)
        {
            curve = new (std::nothrow) Curve();
            _boneCurves[iter.first] = curve;
        }
        
        if(iter.second.size() == 0) continue;
        std::vector<float> keys;
        std::vector<float> values;
        for(const auto& keyIter : iter.second)
        {
            keys.push_back(keyIter._time);
            values.push_back(keyIter._key.x);
            values.push_back(keyIter._key.y);
            values.push_back(keyIter._key.z);
        }
        
        curve->translateCurve = Curve::AnimationCurveVec3::create(&keys[0], &values[0], (int)keys.size());
        if(curve->translateCurve) curve->translateCurve->retain();
    }
    
    for(const auto& iter : data._rotationKeys)
    {
        Curve* curve = _boneCurves[iter.first];
        if( curve == nullptr)
        {
            curve = new (std::nothrow) Curve();
            _boneCurves[iter.first] = curve;
        }
        
        if(iter.second.size() == 0) continue;
        std::vector<float> keys;
        std::vector<float> values;
        for(const auto& keyIter : iter.second)
        {
            keys.push_back(keyIter._time);
            values.push_back(keyIter._key.x);
            values.push_back(keyIter._key.y);
            values.push_back(keyIter._key.z);
            values.push_back(keyIter._key.w);
        }
        
        curve->rotCurve = Curve::AnimationCurveQuat::create(&keys[0], &values[0], (int)keys.size());
        if(curve->rotCurve) curve->rotCurve->retain();
    }
    
    for(const auto& iter : data._scaleKeys)
    {
        Curve* curve = _boneCurves[iter.first];
        if( curve == nullptr)
        {
            curve = new (std::nothrow) Curve();
            _boneCurves[iter.first] = curve;
        }
        
        if(iter.second.size() == 0) continue;
        std::vector<float> keys;
        std::vector<float> values;
        for(const auto& keyIter : iter.second)
        {
            keys.push_back(keyIter._time);
            values.push_back(keyIter._key.x);
            values.push_back(keyIter._key.y);
            values.push_back(keyIter._key.z);
        }
        
        curve->scaleCurve = Curve::AnimationCurveVec3::create(&keys[0], &values[0], (int)keys.size());
        if(curve->scaleCurve) curve->scaleCurve->retain();
    }
    
    return true;
}

////////////////////////////////////////////////////////////////
Animation3DCache* Animation3DCache::_cacheInstance = nullptr;

Animation3DCache* Animation3DCache::getInstance()
{
    if (_cacheInstance == nullptr)
        _cacheInstance = new (std::nothrow) Animation3DCache();
    
    return _cacheInstance;
}
void Animation3DCache::destroyInstance()
{
    CC_SAFE_DELETE(_cacheInstance);
}

Animation3D* Animation3DCache::getAnimation(const std::string& key)
{
    auto it = _animations.find(key);
    if (it != _animations.end())
        return it->second;
    return nullptr;
}

void Animation3DCache::addAnimation(const std::string& key, Animation3D* animation)
{
    const auto& it = _animations.find(key);
    if (it != _animations.end())
    {
        return; // already have this key
    }
    _animations[key] = animation;
    animation->retain();
}

void Animation3DCache::removeAllAnimations()
{
    for (auto itor : _animations) {
        CC_SAFE_RELEASE(itor.second);
    }
    _animations.clear();
}
void Animation3DCache::removeUnusedAnimation()
{
    for (auto itor = _animations.begin(); itor != _animations.end(); ) {
        if (itor->second->getReferenceCount() == 1)
        {
            itor->second->release();
            itor = _animations.erase(itor);
        }
        else
            ++itor;
    }
}

Animation3DCache::Animation3DCache()
{
    
}
Animation3DCache::~Animation3DCache()
{
    removeAllAnimations();
}

NS_CC_END
