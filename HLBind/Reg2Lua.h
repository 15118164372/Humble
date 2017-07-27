
#ifndef H_REGTOLUA_H_
#define H_REGTOLUA_H_

#include "../HBase/HBase.h"
#include "lua5.3/lua.hpp"
#include "luabridge/LuaBridge.h"

H_BNAMSP

void H_RegFuncs(struct lua_State *pLState);
void H_RegLState(struct lua_State *pLState);
void H_RegAOI(struct lua_State *pLState);
void H_RegAStar(struct lua_State *pLState);
void H_RegMail(struct lua_State *pLState);
void H_RegCurLink(struct lua_State *pLState);
void H_RegClock(struct lua_State *pLState);
void H_RegIni(struct lua_State *pLState);
void H_RegSnowFlake(struct lua_State *pLState);
void H_RegTableFile(struct lua_State *pLState);
void H_RegUUID(struct lua_State *pLState);
void H_RegAES(struct lua_State *pLState);
void H_RegDESEncrypt(struct lua_State *pLState);
void H_RegRSA(struct lua_State *pLState);
void H_RegSha1(struct lua_State *pLState);
void H_SetPackPath(struct lua_State *pLState);
void H_RegOther(struct lua_State *pLState);

void H_RegAll(struct lua_State *pLState);

H_ENAMSP

#endif//H_REGTOLUA_H_
