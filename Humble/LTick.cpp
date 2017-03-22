
#include "LTick.h"

H_BNAMSP

SINGLETON_INIT(CLTick)
CLTick objLTick;

CLTick::CLTick(void) : m_pLState(NULL), m_pLFunc(NULL)
{
    m_pLFunc = new(std::nothrow) luabridge::LuaRef *[LCount];
    H_ASSERT(NULL != m_pLFunc, "malloc memory error.");

    m_pLState = luaL_newstate();
    H_ASSERT(NULL != m_pLState, "luaL_newstate error.");
    luaL_openlibs(m_pLState);

    luabridge::LuaRef *pRef = NULL;
    for (int i = 0; i < LCount; ++i)
    {
        pRef = new(std::nothrow) luabridge::LuaRef(m_pLState);
        H_ASSERT(NULL != pRef, "malloc memory error.");
        m_pLFunc[i] = pRef;
    }
}

CLTick::~CLTick(void)
{
    if (NULL != m_pLFunc)
    {
        luabridge::LuaRef *pRef = NULL;
        for (int i = 0; i < LCount; ++i)
        {
            pRef = m_pLFunc[i];
            H_SafeDelete(pRef);
        }
        H_SafeDelArray(m_pLFunc);
    }

    if (NULL != m_pLState)
    {
        lua_close(m_pLState);
        m_pLState = NULL;
    }
}

void CLTick::onStart(void)
{
    H_RegAll(m_pLState);

    std::string strLuaFile = g_strScriptPath + "tick.lua";
    if (H_RTN_OK != luaL_dofile(m_pLState, strLuaFile.c_str()))
    {
        const char *pError = lua_tostring(m_pLState, -1);
        H_LOG(LOGLV_ERROR, "%s", pError);
        H_ASSERT(false, pError);
    }

    *(m_pLFunc[LOnStart]) = luabridge::getGlobal(m_pLState, "onStart");
    *(m_pLFunc[LOnStop]) = luabridge::getGlobal(m_pLState, "onStop");
    *(m_pLFunc[LOnTime]) = luabridge::getGlobal(m_pLState, "onTimer");

    try
    {
        (*(m_pLFunc[LOnStart]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTick::onStop(void)
{
    try
    {
        (*(m_pLFunc[LOnStop]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_INLINE void CLTick::onTime(const unsigned int &uiTick, const unsigned int &uiCount)
{
    try
    {
        (*(m_pLFunc[LOnTime]))(uiTick, uiCount);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_ENAMSP
