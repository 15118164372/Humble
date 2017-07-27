
#ifndef H_LAOI_H_
#define H_LAOI_H_

#include "../HBase/HBase.h"
#include "lua5.3/lua.hpp"
#include "luabridge/LuaBridge.h"

H_BNAMSP

//for lua
class CLAOI : public CAOI
{
public:
    CLAOI(H_LSTATE *pLState, int iMaxX, int iMaxY, int iMaxZ) : CAOI(iMaxX, iMaxY, iMaxZ), m_pLState(pLState)
    {};
    ~CLAOI(void)
    {};

    luabridge::LuaRef getLArea(const int64_t iId, const int iXDist, const int iYDist, const int iZDist) 
    {
        std::vector<int64_t> vcIds = getArea(iId, iXDist, iYDist, iZDist);
        return vector2Table(vcIds);
    };

private:
    luabridge::LuaRef vector2Table(std::vector<int64_t> &vcIds)
    {
        luabridge::LuaRef luaTable = luabridge::newTable((struct lua_State *)m_pLState->pLState);
        for (std::vector<int64_t>::iterator it = vcIds.begin(); vcIds.end() != it; ++it)
        {
            luaTable.append(*it);
        }

        return luaTable;
    };

private:
    CLAOI(void);
    H_DISALLOWCOPY(CLAOI);

private:
    struct H_LSTATE *m_pLState;
};

H_ENAMSP

#endif//H_LAOI_H_
