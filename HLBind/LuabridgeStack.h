#ifndef H_LBS_H_
#define H_LBS_H_

#include "lua.hpp"
#include "LuaBridge.h"

namespace luabridge
{
    template <>
    struct Stack <Humble::CBuffer>
    {
        static void push(lua_State* L, Humble::CBuffer &pBuffer)
        {
            lua_pushlstring(L, pBuffer.getBuffer(), pBuffer.getLens());
        }
    };
}

#endif//H_LBS_H_
