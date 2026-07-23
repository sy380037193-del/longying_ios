/****************************************************************************
版权所有（c）2017-2018厦门雅基软件有限公司。


http://www.cocos2d-x.org


特此免费向任何获得副本的人授予许可

本软件和相关文档文件（“软件”），以处理

在软件中不受限制，包括但不限于权利

使用、复制、修改、合并、发布、分发、再许可和/或销售

软件的副本，并允许软件的接收者

根据以下条件提供：


上述版权声明和本许可声明应包含在

软件的所有副本或实质性部分。


软件按“原样”提供，不提供任何形式的明示或明示担保

隐含的，包括但不限于适销性保证，

适用于特定目的且不造成伤害。在任何情况下

作者或版权持有人对任何索赔、损害赔偿或其他

责任，无论是在合同诉讼中
 ****************************************************************************/

#include "AppDelegate.h"
#include "3d/CCIOSHeadRenderDiagnostics.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"
#include "cocos2d.h"
#include "scripting/lua-bindings/manual/lua_module_register.h"
#include "external/xxtea/xxtea.h"

// A星
#include "AStart/AStar_binding.h"

USING_NS_CC;
using namespace std;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

// 如果需要不同的上下文，请修改glContextAttrs的值
// 它将影响所有平台
void AppDelegate::initGLContextAttrs()
{
    // 设置OpenGL上下文属性：红色、绿色、蓝色、阿尔法、深度、模具、多样本计
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0 };

    GLView::setGLContextAttrs(glContextAttrs);
}

int register_custom_module(lua_State* L)
{
    // 在此处注册您的自定义模块插件类。

    // A星
    luaopen_AStar_luabinding(L);

    return 1;
}

static void decoder(Data &data)
{
    unsigned char PngDatas[] = "resource8010.png";
    unsigned char JpgDatas[] = "tG5^hK0+hT2!cC7&";

    // decrypt XXTEA
    if (!data.isNull()) {
        bool ishave = false;
        unsigned char *buf = data.getBytes();
        ssize_t size = data.getSize();
        ssize_t len = strlen((char *)PngDatas);
        if (size <= len) {
            return;
        }

        for (int i = 0; i < len; ++i) {
            ishave = buf[i] == PngDatas[i];
            if (!ishave) {
                break;
            }
        }

        if (ishave) {
//            xxtea_long newLen = 0;
//            unsigned char* buffer = cosos_ccnm(buf + len,
//                    (xxtea_long)(size - len),
//                    (unsigned char*)JpgDatas,
//                    (xxtea_long)strlen((char *)JpgDatas),
//                    &newLen);
//            data.clear();
//            data.fastSet(buffer, newLen);
        }
    }
}

bool AppDelegate::applicationDidFinishLaunching()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    cocos2d::ios_head_render_diagnostics::log("event=app_launch stage=applicationDidFinishLaunching");
#endif

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = cocos2d::GLViewImpl::create("dhxy");
        director->setOpenGLView(glview);
        director->startAnimation();
    }

    //设置默认FPS
    director->setAnimationInterval(1.0 / 60.0f);
    director->setClearColor(Color4F(0.0f, 0.0f, 0.0f, 0.0f));

    // 注册lua模块
    auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    LuaStack *stack = engine->getLuaStack();
    lua_State *L = stack->getLuaState();
    lua_module_register(L);
    //注册自定义函数
    register_custom_module(L);

//    // 资源解码, game32.zip & game64.zip deal as resoruce.
    FileUtils::getInstance()->setFileDataDecoder(decoder);
//#if 1 // set to 1 for release mode
//    // use luajit bytecode package
//#if defined(__aarch64__) || defined(__arm64__) || defined(__x86_64__)
//    stack->loadChunksFromZIP("res/game64.zip");
//#else
//    stack->loadChunksFromZIP("res/game32.zip");
//#endif
//    stack->executeString("require 'main'");
//#else // #if 0
//    // 使用离散文件
    engine->executeScriptFile("src/main.lua");
//#endif

    return true;
}

// 当应用程序处于非活动状态时，将调用此函数。注意，当接到电话时，它会被调用。
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("APP_ENTER_BACKGROUND_EVENT");
}

// 当应用程序再次处于活动状态时，将调用此函数
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("APP_ENTER_FOREGROUND_EVENT");
}
