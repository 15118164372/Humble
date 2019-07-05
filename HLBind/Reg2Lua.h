
#ifndef H_REGTOLUA_H_
#define H_REGTOLUA_H_

#include "HBase.h"
#include "LuabridgeStack.h"

H_BNAMSP

class CReg2Lua : public CObject
{
public:
    CReg2Lua(void);
    ~CReg2Lua(void);

    static void regAll(struct lua_State *pLState);

private:
    static void regFuncs(struct lua_State *pLState);
    static void regAdjure(struct lua_State *pLState);
    static void regWorker(struct lua_State *pLState);
    static void regHumble(struct lua_State *pLState);
    static void regMail(struct lua_State *pLState);
    static void regPackPath(struct lua_State *pLState);
    static void regObject(struct lua_State *pLState);
};

H_ENAMSP

#endif //H_REGTOLUA_H_

