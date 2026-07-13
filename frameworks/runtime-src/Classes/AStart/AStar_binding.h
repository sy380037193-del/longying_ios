#ifndef __ASTAR_BINDING_H__
#define __ASTAR_BINDING_H__

extern "C" {
#include "lua.h"
#include "tolua++.h"
}
#include "tolua_fix.h"
#include "CCLuaEngine.h"

int luaopen_AStar_luabinding(lua_State *L);

int lua_register_AStar(lua_State* tolua_S);

#endif  // __ASTAR_BINDING_H__