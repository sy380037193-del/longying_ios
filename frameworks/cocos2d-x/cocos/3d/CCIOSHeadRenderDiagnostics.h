#pragma once

#include <string>

namespace cocos2d
{
namespace ios_head_render_diagnostics
{

bool isHeadTexturePath(const std::string& path);
bool markFirstDraw(const void* mesh);
void log(const std::string& line);

}
}
