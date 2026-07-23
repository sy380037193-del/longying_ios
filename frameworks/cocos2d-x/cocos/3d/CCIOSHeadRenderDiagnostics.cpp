#include "3d/CCIOSHeadRenderDiagnostics.h"

#include "base/ccMacros.h"
#include "platform/CCFileUtils.h"
#include "platform/CCPlatformConfig.h"

#include <fstream>
#include <mutex>
#include <unordered_set>

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace cocos2d
{
namespace ios_head_render_diagnostics
{

namespace
{
const char* LOG_FILE_NAME = "longying_head_render.log";
const char* RECEIVER_IP = "192.168.1.78";
const unsigned short RECEIVER_PORT = 39091;

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
void emit(const std::string& line)
{
    const std::string path = FileUtils::getInstance()->getWritablePath() + LOG_FILE_NAME;
    std::ofstream output(path, std::ios::out | std::ios::app);
    output << line << '\n';

    static int udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket >= 0)
    {
        sockaddr_in receiver = {};
        receiver.sin_family = AF_INET;
        receiver.sin_port = htons(RECEIVER_PORT);
        inet_pton(AF_INET, RECEIVER_IP, &receiver.sin_addr);
        sendto(udpSocket, line.data(), line.size(), 0,
               reinterpret_cast<const sockaddr*>(&receiver), sizeof(receiver));
    }

    CCLOG("%s", line.c_str());
}
#endif
}

bool markFirstDraw(const void* mesh)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    static std::mutex mutex;
    static std::unordered_set<const void*> loggedMeshes;
    std::lock_guard<std::mutex> lock(mutex);
    return loggedMeshes.insert(mesh).second;
#else
    return false;
#endif
}

void log(const std::string& line)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    static std::mutex mutex;
    static bool markerWritten = false;
    std::lock_guard<std::mutex> lock(mutex);
    if (!markerWritten)
    {
        emit("marker=LONGYING_IOS_HEAD_DIAGNOSTICS_V4 receiver=192.168.1.78:39091");
        markerWritten = true;
    }
    emit(line);
#else
    (void)line;
#endif
}

}
}
