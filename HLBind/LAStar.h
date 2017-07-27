#ifndef H_LASTAR_H_
#define H_LASTAR_H_

#include "../HBase/HBase.h"
#include "lua5.3/lua.hpp"
#include "luabridge/LuaBridge.h"

H_BNAMSP

//for lua
class CLAStar : public CAStar
{
public:
    CLAStar(H_LSTATE *pLState) : m_pLState(pLState)
    {};
    ~CLAStar(void)
    {};

    luabridge::LuaRef astarPath(float fX1, float fY1, float fX2, float fY2, CAMap *pAMap)
    {
        std::vector<PointF> vcPath(findPath(fX1, fY1, fX2, fY2, pAMap));
        luabridge::LuaRef luaTable = luabridge::newTable((struct lua_State *)m_pLState->pLState);

        for (std::vector<PointF>::iterator it = vcPath.begin(); vcPath.end() != it; ++it)
        {
            luabridge::LuaRef luaPoint = luabridge::newTable((struct lua_State *)m_pLState->pLState);
            luaPoint.append(it->fX);
            luaPoint.append(it->fY);

            luaTable.append(luaPoint);
        }

        return luaTable;
    };
    void printMap(luabridge::LuaRef tablePoint, CAMap *pAMap) 
    {
        PointF stPoint;
        std::vector<PointF> vcPath;
        for (int i = 1; i <= tablePoint.length(); i++)
        {
            luabridge::LuaRef objTmp = tablePoint[i];
            if (!objTmp.isTable())
            {
                continue;
            }
            if (objTmp.length() < 2)
            {
                continue;
            }

            stPoint.fX = objTmp[1];
            stPoint.fY = objTmp[2];
            vcPath.push_back(stPoint);
        }

        Print(vcPath, pAMap);
    };

private:
    CLAStar(void);
    H_DISALLOWCOPY(CLAStar);

private:
    struct H_LSTATE *m_pLState;
};

H_ENAMSP

#endif//H_LASTAR_H_
