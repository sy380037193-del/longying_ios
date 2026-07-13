#include <LuaBasicConversions.cpp>
#include "AStar_binding.h"
#include "ANodeGrid.h"
#include "Binary.h"
#include "AStar.h"
#include "ANode.h"
#include "AVector.h"

/* method: create of class ANodeGrid */
static int tolua_ANodeGrid_create(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 4, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'create'.", &tolua_err);
        return 0;
    }
    int numCols = ((int)tolua_tonumber(tolua_S, 2, 0));
    int numRows = ((int)tolua_tonumber(tolua_S, 3, 0));
    ANodeGrid* tolua_ret = (ANodeGrid*)ANodeGrid::create(numCols, numRows);
    tolua_pushusertype(tolua_S, (void*)tolua_ret, "dp.ANodeGrid");
    return 1;
}

/* method: setSize of class ANodeGrid */
static int tolua_ANodeGrid_setSize(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 4, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'setSize'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    int numCols = ((int)tolua_tonumber(tolua_S, 2, 0));
    int numRows = ((int)tolua_tonumber(tolua_S, 3, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'setSize'", NULL);
        return 0;
    }
    self->setSize(numCols, numRows);
    return 0;
}

/* method: getNode of class ANodeGrid */
static int tolua_ANodeGrid_getNode(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 4, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'getNode'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    int x = ((int)tolua_tonumber(tolua_S, 2, 0));
    int y = ((int)tolua_tonumber(tolua_S, 3, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'getNode'", NULL);
        return 0;
    }
    ANode* tolua_ret = (ANode*) self->getNode(x, y);
    tolua_pushusertype(tolua_S, (void*) tolua_ret, "dp.ANode");
    return 1;
}

/* method: setEndNode of class ANodeGrid */
static int tolua_ANodeGrid_setEndNode(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 4, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'setEndNode'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    int x = ((int)tolua_tonumber(tolua_S, 2, 0));
    int y = ((int)tolua_tonumber(tolua_S, 3, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'setEndNode'", NULL);
        return 0;
    }
    self->setEndNode(x, y);
    return 0;
}

/* method: setStartNode of class ANodeGrid */
static int tolua_ANodeGrid_setStartNode(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 4, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'setStartNode'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    int x = ((int)tolua_tonumber(tolua_S, 2, 0));
    int y = ((int)tolua_tonumber(tolua_S, 3, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'setStartNode'", NULL);
        return 0;
    }
    self->setStartNode(x, y);
    return 1;
}

/* method: setWalkable of class ANodeGrid */
static int tolua_ANodeGrid_setWalkable(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isboolean(tolua_S, 4, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 5, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'setWalkable'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    int x = ((int)tolua_tonumber(tolua_S, 2, 0));
    int y = ((int)tolua_tonumber(tolua_S, 3, 0));
    bool value = ((bool)tolua_toboolean(tolua_S, 4, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'setWalkable'", NULL);
        return 0;
    }
    self->setWalkable(x, y, value);
    return 0;
}

/* method: hasBarrier of class ANodeGrid */
static int tolua_ANodeGrid_hasBarrier(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
        !tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 6, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'hasBarrier'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    int staratX = ((int)tolua_tonumber(tolua_S, 2, 0));
    int starttY = ((int)tolua_tonumber(tolua_S, 3, 0));
    int endX = ((int)tolua_tonumber(tolua_S, 4, 0));
    int endY = ((int)tolua_tonumber(tolua_S, 5, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'hasBarrier'", NULL);
        return 0;
    }
    bool tolua_ret = (bool)self->hasBarrier(staratX, starttY, endX, endY);
    tolua_pushboolean(tolua_S, (bool)tolua_ret);
    return 1;
}

/* method: findReplacer of class ANodeGrid */
static int tolua_ANodeGrid_findReplacer(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isusertype(tolua_S, 2, "dp.ANode", 0, &tolua_err) ||
        !tolua_isusertype(tolua_S, 3, "dp.ANode", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 4, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'findReplacer'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    ANode* fromNode = ((ANode*)tolua_tousertype(tolua_S, 2, 0));
    ANode* toNode = ((ANode*)tolua_tousertype(tolua_S, 3, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'findReplacer'", NULL);
        return 0;
    }
    ANode* tolua_ret = (ANode*)self->findReplacer(fromNode, toNode);
    tolua_pushusertype(tolua_S, (void*)tolua_ret, "dp.ANode");
    return 1;
}

/* method: calculateBuriedDepth of class ANodeGrid */
static int tolua_ANodeGrid_calculateBuriedDepth(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'calculateBuriedDepth'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'calculateBuriedDepth'", NULL);
        return 0;
    }
    self->calculateBuriedDepth();
    return 1;
}

/* method: getEndNode of class ANodeGrid */
static int tolua_ANodeGrid_getEndNode(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'getEndNode'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'getEndNode'", NULL);
        return 0;
    }
    ANode* tolua_ret = (ANode*)self->getEndNode();
    tolua_pushusertype(tolua_S, (void*)tolua_ret, "dp.ANode");
    return 1;
}

/* method: getStartNode of class ANodeGrid */
static int tolua_ANodeGrid_getStartNode(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'getStartNode'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'getStartNode'", NULL);
        return 0;
    }
    ANode* tolua_ret = (ANode*) self->getStartNode();
    tolua_pushusertype(tolua_S, (void*)tolua_ret, "dp.ANode");
    return 1;
}

/* method: create of class AStar */
static int tolua_AStar_create(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertable(tolua_S, 1, "dp.AStar", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'create'.", &tolua_err);
        return 0;
    }
    AStar* tolua_ret = (AStar*)AStar::create();
    tolua_pushusertype(tolua_S, (void*)tolua_ret, "dp.AStar");
    return 1;
}

/* method: findPath of class AStar */
static int tolua_AStar_findPath(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.AStar", 0, &tolua_err) ||
        !tolua_isusertype(tolua_S, 2, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 3, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'findPath'.", &tolua_err);
        return 0;
    }
    AStar* self = (AStar*)tolua_tousertype(tolua_S, 1, 0);
    ANodeGrid* grid = ((ANodeGrid*)tolua_tousertype(tolua_S, 2, 0));
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'findPath'", NULL);
        return 0;
    }
    bool tolua_ret = (bool)self->findPath(grid);
    tolua_pushboolean(tolua_S, (bool)tolua_ret);
    return 1;
}

/* method: search of class AStar */
static int tolua_AStar_search(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.AStar", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'search'.", &tolua_err);
        return 0;
    }
    AStar* self = (AStar*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'search'", NULL);
        return 0;
    }
    bool tolua_ret = (bool)self->search();
    tolua_pushboolean(tolua_S, (bool)tolua_ret);
    return 1;
}

/* method: floyd of class AStar */
static int tolua_AStar_floyd(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.AStar", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'floyd'.", &tolua_err);
        return 0;
    }
    AStar* self = (AStar*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'floyd'", NULL);
        return 0;
    }
    self->floyd();
    return 1;
}

/* method: clearSelf of class AStar */
static int tolua_AStar_clearSelf(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.AStar", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'clearSelf'.", &tolua_err);
        return 0;
    }
    AStar* self = (AStar*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'clearSelf'", NULL);
        return 0;
    }
    self->clearSelf();
    return 1;
}

/* method: retain of class ANodeGrid */
static int tolua_ANodeGrid_retain(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.ANodeGrid", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'retain'.", &tolua_err);
        return 0;
    }
    ANodeGrid* self = (ANodeGrid*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'retain'", NULL);
        return 0;
    }
    self->retain();
    return 1;
}

void avector_to_luaval(lua_State* L, char* type, AVector<ANode*>& ret) {
    lua_newtable(L);
    int index = 1;
    for (auto& value : ret) {
        object_to_luaval<ANode>(L, type, value);
        lua_rawseti(L, -2, index);
        ++index;
    }
}

/* method: getFloydPath of class AStar */
static int tolua_AStar_getFloydPath(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "dp.AStar", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 2, &tolua_err))
    {
        tolua_error(tolua_S, "#ferror in function 'getFloydPath'.", &tolua_err);
        return 0;
    }
    AStar* self = (AStar*)tolua_tousertype(tolua_S, 1, 0);
    if (!self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'getFloydPath'", NULL);
        return 0;
    }
    AVector<ANode*> tolua_ret = (AVector<ANode*>) self->getFloydPath();
    avector_to_luaval(tolua_S, "dp.ANode", tolua_ret);
    return 1;
}

/* get function: x of class ANode */
static int tolua_get_ANode_x(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'x'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->x);
    return 1;
}

/* set function: x of class ANode */
static int tolua_set_ANode_x(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'x'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->x = ((int)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: y of class ANode */
static int tolua_get_ANode_y(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'y'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->y);
    return 1;
}

/* set function: y of class ANode */
static int tolua_set_ANode_y(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'y'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->y = ((int)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: f of class ANode */
static int tolua_get_ANode_f(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'f'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->f);
    return 1;
}

/* set function: f of class ANode */
static int tolua_set_ANode_f(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'f'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->f = ((float)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: g of class ANode */
static int tolua_get_ANode_g(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'g'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->g);
    return 1;
}

/* set function: g of class ANode */
static int tolua_set_ANode_g(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'g'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->g = ((float)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: h of class ANode */
static int tolua_get_ANode_h(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'h'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->h);
    return 1;
}

/* set function: h of class ANode */
static int tolua_set_ANode_h(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'h'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->h = ((float)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: walkable of class ANode */
static int tolua_get_ANode_walkable(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'walkable'", NULL);
    tolua_pushboolean(tolua_S, (bool)self->walkable);
    return 1;
}

/* set function: walkable of class ANode */
static int tolua_set_ANode_walkable(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'walkable'", NULL);
    if (!tolua_isboolean(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->walkable = ((bool)tolua_toboolean(tolua_S, 2, 0));
    return 0;
}

/* get function: costMultiplier of class ANode */
static int tolua_get_ANode_costMultiplier(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'costMultiplier'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->costMultiplier);
    return 1;
}

/* set function: costMultiplier of class ANode */
static int tolua_set_ANode_costMultiplier(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'costMultiplier'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->costMultiplier = ((float)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: posX of class ANode */
static int tolua_get_ANode_posX(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'posX'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->posX);
    return 1;
}

/* set function: posX of class ANode */
static int tolua_set_ANode_posX(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'posX'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->posX = ((float)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: posY of class ANode */
static int tolua_get_ANode_posY(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'posY'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->posY);
    return 1;
}

/* set function: posY of class ANode */
static int tolua_set_ANode_posY(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'posY'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->posY = ((float)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: buriedDepth of class ANode */
static int tolua_get_ANode_buriedDepth(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'buriedDepth'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->buriedDepth);
    return 1;
}

/* set function: buriedDepth of class ANode */
static int tolua_set_ANode_buriedDepth(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'buriedDepth'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->buriedDepth = ((int)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* get function: distance of class ANode */
static int tolua_get_ANode_distance(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'distance'", NULL);
    tolua_pushnumber(tolua_S, (lua_Number)self->distance);
    return 1;
}

/* set function: distance of class ANode */
static int tolua_set_ANode_distance(lua_State* tolua_S)
{
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    tolua_Error tolua_err;
    if (!self) tolua_error(tolua_S, "invalid 'self' in accessing variable 'distance'", NULL);
    if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
        tolua_error(tolua_S, "#vinvalid type in variable assignment.", &tolua_err);
    self->distance = ((float)tolua_tonumber(tolua_S, 2, 0));
    return 0;
}

/* method: getParent of class ANode */
static int tolua_ANode_getParent(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S, 1, "dp.ANode", 0, &tolua_err) || !tolua_isnoobj(tolua_S, 2, &tolua_err)) {
        tolua_error(tolua_S, "#ferror in function 'getParent'.", &tolua_err);
        return 0;
    }
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    if (!self) {
        tolua_error(tolua_S, "invalid 'self' in function 'getParent'", NULL);
    }
 else {
        ANode* tolua_ret = (ANode*)self->getParent();
        tolua_pushusertype(tolua_S, (void*)tolua_ret, "dp.ANode");
    }
    return 1;
}

/* method: setParent of class ANode */
static int tolua_ANode_setParent(lua_State* tolua_S)
{
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S, 1, "dp.ANode", 0, &tolua_err) ||
        !tolua_isusertype(tolua_S, 2, "dp.ANode", 0, &tolua_err) ||
        !tolua_isnoobj(tolua_S, 3, &tolua_err)
        ) {
        tolua_error(tolua_S, "#ferror in function 'setParent'.", &tolua_err);
        return 0;
    }
    ANode* self = (ANode*)tolua_tousertype(tolua_S, 1, 0);
    ANode* node = ((ANode*)tolua_tousertype(tolua_S, 2, 0));
    if (!self) {
        tolua_error(tolua_S, "invalid 'self' in function 'setParent'", NULL);
    } else {
        self->setParent(node);
    }
    return 0;
}

int lua_register_AStar(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "dp.ANodeGrid");
    tolua_cclass(tolua_S, "ANodeGrid", "dp.ANodeGrid", "cc.Ref", nullptr);
    tolua_beginmodule(tolua_S, "ANodeGrid");
    tolua_function(tolua_S, "create", tolua_ANodeGrid_create);
    tolua_function(tolua_S, "setSize", tolua_ANodeGrid_setSize);
    tolua_function(tolua_S, "getNode", tolua_ANodeGrid_getNode);
    tolua_function(tolua_S, "setEndNode", tolua_ANodeGrid_setEndNode);
    tolua_function(tolua_S, "setStartNode", tolua_ANodeGrid_setStartNode);
    tolua_function(tolua_S, "setWalkable", tolua_ANodeGrid_setWalkable);
    tolua_function(tolua_S, "hasBarrier", tolua_ANodeGrid_hasBarrier);
    tolua_function(tolua_S, "findReplacer", tolua_ANodeGrid_findReplacer);
    tolua_function(tolua_S, "calculateBuriedDepth", tolua_ANodeGrid_calculateBuriedDepth);
    tolua_function(tolua_S, "getEndNode", tolua_ANodeGrid_getEndNode);
    tolua_function(tolua_S, "getStartNode", tolua_ANodeGrid_getStartNode);
    tolua_function(tolua_S, "retain", tolua_ANodeGrid_retain);
    tolua_endmodule(tolua_S);

    tolua_usertype(tolua_S, "dp.AStar");
    tolua_cclass(tolua_S, "AStar", "dp.AStar", "cc.Ref", nullptr);
    tolua_beginmodule(tolua_S, "AStar");
    tolua_function(tolua_S, "create", tolua_AStar_create);
    tolua_function(tolua_S, "findPath", tolua_AStar_findPath);
    tolua_function(tolua_S, "search", tolua_AStar_search);
    tolua_function(tolua_S, "floyd", tolua_AStar_floyd);
    tolua_function(tolua_S, "clearSelf", tolua_AStar_clearSelf);
    tolua_function(tolua_S, "getFloydPath", tolua_AStar_getFloydPath);
    tolua_endmodule(tolua_S);

    tolua_usertype(tolua_S, "dp.ANode");
    tolua_cclass(tolua_S, "ANode", "dp.ANode", "cc.Ref", nullptr);
    tolua_beginmodule(tolua_S, "ANode");
    tolua_variable(tolua_S, "x", tolua_get_ANode_x, tolua_set_ANode_x);
    tolua_variable(tolua_S, "y", tolua_get_ANode_y, tolua_set_ANode_y);
    tolua_variable(tolua_S, "f", tolua_get_ANode_f, tolua_set_ANode_f);
    tolua_variable(tolua_S, "g", tolua_get_ANode_g, tolua_set_ANode_g);
    tolua_variable(tolua_S, "h", tolua_get_ANode_h, tolua_set_ANode_h);
    tolua_variable(tolua_S, "walkable", tolua_get_ANode_walkable, tolua_set_ANode_walkable);
    tolua_variable(tolua_S, "costMultiplier", tolua_get_ANode_costMultiplier, tolua_set_ANode_costMultiplier);
    tolua_variable(tolua_S, "posX", tolua_get_ANode_posX, tolua_set_ANode_posX);
    tolua_variable(tolua_S, "posY", tolua_get_ANode_posY, tolua_set_ANode_posY);
    tolua_variable(tolua_S, "buriedDepth", tolua_get_ANode_buriedDepth, tolua_set_ANode_buriedDepth);
    tolua_variable(tolua_S, "distance", tolua_get_ANode_distance, tolua_set_ANode_distance);
    tolua_function(tolua_S, "getParent", tolua_ANode_getParent);
    tolua_function(tolua_S, "setParent", tolua_ANode_setParent);
    tolua_endmodule(tolua_S);

    return 1;
}

int luaopen_AStar_luabinding(lua_State* L)
{
    printf("luaopen_AStar_luabinding\n");
    tolua_open(L);
    tolua_module(L, "dp", 0);
    tolua_beginmodule(L, "dp");
    lua_register_AStar(L);
    tolua_endmodule(L);

    return 1;
}
