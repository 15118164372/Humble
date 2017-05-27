
#include "LTask.h"

H_BNAMSP

CLTask::CLTask(const char *pszFile, const char *pszName, const int iCapacity) : CTaskWorker(pszName, iCapacity), m_strFile(pszFile)
{
    m_pLState = luaL_newstate();
    H_ASSERT(NULL != m_pLState, "luaL_newstate error.");

    luaL_openlibs(m_pLState);
    H_RegAll(m_pLState);
    luabridge::setGlobal(m_pLState, getName()->c_str(), "g_taskName");

    m_pLFunc = new(std::nothrow) luabridge::LuaRef *[LFUNC_COUNT];
    H_ASSERT(NULL != m_pLFunc, "malloc memory error.");

    luabridge::LuaRef *pRef = NULL;
    for (int i = 0; i < LFUNC_COUNT; ++i)
    {
        pRef = new(std::nothrow) luabridge::LuaRef(m_pLState);
        H_ASSERT(NULL != pRef, "malloc memory error.");
        m_pLFunc[i] = pRef;
    }
}

CLTask::~CLTask(void)
{
    if (NULL != m_pLFunc)
    {
        luabridge::LuaRef *pRef = NULL;
        for (int i = 0; i < LFUNC_COUNT; ++i)
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

void CLTask::initTask(void)
{
    std::string strLuaFile = g_strScriptPath + m_strFile;
    if (H_RTN_OK != luaL_dofile(m_pLState, strLuaFile.c_str()))
    {
        const char *pErr = lua_tostring(m_pLState, -1);
        H_ASSERT(false, pErr);
    }

    *(m_pLFunc[LFUNC_INITTASK]) = luabridge::getGlobal(m_pLState, "initTask");
    *(m_pLFunc[LFUNC_DELTASK]) = luabridge::getGlobal(m_pLState, "destroyTask");
    *(m_pLFunc[LFUNC_ONNETEVENT]) = luabridge::getGlobal(m_pLState, "onNetEvent");
    *(m_pLFunc[LFUNC_ONNETREAD]) = luabridge::getGlobal(m_pLState, "onNetRead");
    *(m_pLFunc[LFUNC_ONTIME]) = luabridge::getGlobal(m_pLState, "onTimeEvent");
    *(m_pLFunc[LFUNC_ONCMD]) = luabridge::getGlobal(m_pLState, "onCMD");
    *(m_pLFunc[LFUNC_RPCCALL]) = luabridge::getGlobal(m_pLState, "onRPCCall");
    *(m_pLFunc[LFUNC_RPCRTN]) = luabridge::getGlobal(m_pLState, "onRPCRtn");

    try
    {
        (*(m_pLFunc[LFUNC_INITTASK]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::destroyTask(void)
{
    try
    {
        (*(m_pLFunc[LFUNC_DELTASK]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onAccept(H_LINK *pLink)
{
    try
    {
        (*(m_pLFunc[LFUNC_ONNETEVENT]))((unsigned short)MSG_NET_ACCEPT, pLink->sock, pLink->usType);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onLinked(H_LINK *pLink)
{
    try
    {
        (*(m_pLFunc[LFUNC_ONNETEVENT]))((unsigned short)MSG_NET_LINKED, pLink->sock, pLink->usType);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onClosed(H_LINK *pLink)
{
    try
    {
        (*(m_pLFunc[LFUNC_ONNETEVENT]))((unsigned short)MSG_NET_CLOSE, pLink->sock, pLink->usType);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onNetRead(H_TCPBUF *pTcpBuf)
{
    try
    {
        (*(m_pLFunc[LFUNC_ONNETREAD]))(pTcpBuf->stLink.sock, pTcpBuf->stLink.usType, pTcpBuf->iProto, pTcpBuf->stBinary);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onFrame(H_TICK *pTick)
{
    try
    {
        (*(m_pLFunc[LFUNC_ONTIME]))((unsigned short)MSG_TIME_FRAME, pTick->uiMS, pTick->uiCount);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onSec(H_TICK *pTick)
{
    try
    {
        (*(m_pLFunc[LFUNC_ONTIME]))((unsigned short)MSG_TIME_SEC, pTick->uiMS, pTick->uiCount);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

const char *CLTask::onCMD(const char *pszCmd, const char *pszInfo, size_t &iOutLens)
{
    try
    {
        m_stBinary = (*(m_pLFunc[LFUNC_ONCMD]))(pszCmd, pszInfo);
        iOutLens = m_stBinary.iLens;

        return m_stBinary.pBufer;
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());        
    }

    return NULL;
}

const char *CLTask::onRPCCall(const char *pszName, char *pszInfo,
    const size_t &uiLens, size_t &iOutLens)
{
    try
    {
        m_stBinary.iLens = uiLens;
        m_stBinary.pBufer = pszInfo;
        m_stBinary = (*(m_pLFunc[LFUNC_RPCCALL]))(pszName, m_stBinary);
        iOutLens = m_stBinary.iLens;

        return m_stBinary.pBufer;
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }

    return NULL;
}

void CLTask::onRPCRtn(const unsigned int &uiId, H_Binary *pBinary)
{
    try
    {
        (*(m_pLFunc[LFUNC_RPCRTN]))(uiId, *pBinary);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

const char *CLTask::onTaskRPCCall(const char *pszName, char *pszInfo,
    const size_t &uiLens, size_t &iOutLens)
{
    try
    {
        m_stBinary.iLens = uiLens;
        m_stBinary.pBufer = pszInfo;
        m_stBinary = (*(m_pLFunc[LFUNC_RPCCALL]))(pszName, m_stBinary);
        iOutLens = m_stBinary.iLens;

        return m_stBinary.pBufer;
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }

    return NULL;
}

void CLTask::onTaskRPCRtn(const unsigned int &uiId, H_Binary *pBinary)
{
    try
    {
        (*(m_pLFunc[LFUNC_RPCRTN]))(uiId, *pBinary);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_ENAMSP
