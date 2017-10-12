#ifndef H_LORDEREDQU_H_
#define H_LORDEREDQU_H_

#include "../HBase/HBase.h"
#include "lua5.3/lua.hpp"
#include "luabridge/LuaBridge.h"

H_BNAMSP

class CLOrderedQu : public COrderedQu
{
public:
    CLOrderedQu(H_LSTATE *pLState) : m_pLState(pLState)
    {};
    ~CLOrderedQu(void) {};

    luabridge::LuaRef popLNode(uint64_t iMark)
    {
        std::vector<uint64_t>::iterator itId;
        std::vector<uint64_t> vcNode(popNode(iMark));
        luabridge::LuaRef outNode = luabridge::newTable((struct lua_State *)m_pLState->pLState);

        for (itId = vcNode.begin(); vcNode.end() != itId; ++itId)
        {
            outNode.append(*itId);
        }

        return outNode;
    };

private:
    struct H_LSTATE *m_pLState;
};

H_ENAMSP

#endif//H_LORDEREDQU_H_
