
#include "LWorker.h"

H_BNAMSP

CLWorker::CLWorker(CHumble *pHumble, const char *pszFile, const char *pszName, const size_t &uiCapacity) :
    CWorker(pszName, uiCapacity), m_pHumble(pHumble), m_strFile(pszFile)
{
    initLua();
}

CLWorker::~CLWorker(void)
{
    freeLua();
}

void CLWorker::reSet(const char *pszFile, const char *pszName, const size_t &uiCapacity)
{
    freeLua();

    m_strFile = pszFile;
    setName(pszName);
    cleanAdjureQu();
    resetCapacity(uiCapacity);

    initLua();
}

void CLWorker::initLua(void)
{
    m_stState = luaL_newstate();
    H_ASSERT(NULL != m_stState, "luaL_newstate error.");

    luaL_openlibs(m_stState);
    CReg2Lua::regAll(m_stState);
    luabridge::setGlobal(m_stState, getName(), "g_taskName");
    luabridge::setGlobal(m_stState, (CWorker*)this, "g_curWorker");

    for (int i = 0; i < LFUNC_COUNT; ++i)
    {
        m_pLFunc[i] = new(std::nothrow) luabridge::LuaRef(m_stState);
        H_ASSERT(NULL != m_pLFunc[i], "malloc memory error.");
    }
}
void CLWorker::freeLua(void)
{
    for (int i = 0; i < LFUNC_COUNT; ++i)
    {
        H_SafeDelete(m_pLFunc[i]);
    }

    if (NULL != m_stState)
    {
        lua_close(m_stState);
        m_stState = NULL;
    }
}

void CLWorker::initTask(void)
{
    std::string strLuaFile = g_pHumble->getScriptPath() + m_strFile;
    if (H_RTN_OK != luaL_dofile(m_stState, strLuaFile.c_str()))
    {
        const char *pErr = lua_tostring(m_stState, -1);
        H_ASSERT(false, pErr);
    }

    *(m_pLFunc[LFUNC_TASK_INIT]) = luabridge::getGlobal(m_stState, "initTask");
    *(m_pLFunc[LFUNC_TASK_DEL]) = luabridge::getGlobal(m_stState, "destroyTask");

    *(m_pLFunc[LFUNC_TIME_TIMEOUT]) = luabridge::getGlobal(m_stState, "CCALL_TIMEOUT");

    *(m_pLFunc[LFUNC_NET_ACCEPT]) = luabridge::getGlobal(m_stState, "CCALL_ACCEPT");
    *(m_pLFunc[LFUNC_NET_CONNECT]) = luabridge::getGlobal(m_stState, "CCALL_CONNECT");
    *(m_pLFunc[LFUNC_NET_CLOSE]) = luabridge::getGlobal(m_stState, "CCALL_CLOSED");
    
    *(m_pLFunc[LFUNC_NET_READ_I]) = luabridge::getGlobal(m_stState, "CCALL_NETREADI");
    *(m_pLFunc[LFUNC_NET_READ_HTTPD]) = luabridge::getGlobal(m_stState, "CCALL_NETREADHTTPD");

    *(m_pLFunc[LFUNC_NET_RPC]) = luabridge::getGlobal(m_stState, "CCALL_NETRPC");

    *(m_pLFunc[LFUNC_TASK_RPCCALL]) = luabridge::getGlobal(m_stState, "CCALL_RPCCALL");
    *(m_pLFunc[LFUNC_TASK_RPCRTN]) = luabridge::getGlobal(m_stState, "CCALL_RPCRTN");

    *(m_pLFunc[LFUNC_TASK_DEBUG]) = luabridge::getGlobal(m_stState, "CCALL_DEBUG");

    *(m_pLFunc[LFUNC_TASK_BIND_HTTPC]) = luabridge::getGlobal(m_stState, "CCALL_BIND_HTTPC");
    *(m_pLFunc[LFUNC_TASK_BIND_I]) = luabridge::getGlobal(m_stState, "CCALL_BIND_I");

    try
    {
        (*(m_pLFunc[LFUNC_TASK_INIT]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::destroyTask(void)
{
    try
    {
        (*(m_pLFunc[LFUNC_TASK_DEL]))();
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onTimeOut(CTaskTimeOutAdjure *pTimeOut)
{
    try
    {
        (*(m_pLFunc[LFUNC_TIME_TIMEOUT]))(pTimeOut->getId());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onAccepted(CTaskNetEvAdjure *pNetEv)
{
    try
    {
        (*(m_pLFunc[LFUNC_NET_ACCEPT]))(pNetEv->getSock(), pNetEv->getType(), pNetEv->getBindId());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onConnected(CTaskNetEvAdjure *pNetEv)
{
    try
    {
        (*(m_pLFunc[LFUNC_NET_CONNECT]))(pNetEv->getSock(), pNetEv->getType(), pNetEv->getBindId());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onClosed(CTaskNetEvAdjure *pNetEv)
{
    try
    {
        (*(m_pLFunc[LFUNC_NET_CLOSE]))(pNetEv->getSock(), pNetEv->getType(), pNetEv->getBindId());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onNetReadI(CINetReadAdjure *pNetRead)
{
    try
    {
        (*(m_pLFunc[LFUNC_NET_READ_I]))(pNetRead->getSock(), pNetRead->getType(), 
            pNetRead->getProto(), pNetRead->getPack());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onNetReadHttpd(CTaskHttpdAdjure *pNetRead)
{
    try
    {
        (*(m_pLFunc[LFUNC_NET_READ_HTTPD]))(pNetRead->getSock(), pNetRead->getType(), pNetRead->getPath(), pNetRead);
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onNetRPC(CNetRPCAdjure *pRPCAdjure)
{
    try
    {
        (*(m_pLFunc[LFUNC_NET_RPC]))(pRPCAdjure->getSock(), pRPCAdjure->getType(), 
            (const char *)(pRPCAdjure->getHead()->acFrom), pRPCAdjure->getContent());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}
void CLWorker::onRPCCall(CRPCCallAdjure *pRPCCall)
{
    try
    {
        (*(m_pLFunc[LFUNC_TASK_RPCCALL]))(pRPCCall->getFrom(), pRPCCall->getRPCName(), pRPCCall->getPack(), pRPCCall->getId());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLWorker::onRPCRtn(CRPCRtnAdjure *pRPCRtn)
{
    try
    {
        (*(m_pLFunc[LFUNC_TASK_RPCRTN]))(pRPCRtn->getPack(), pRPCRtn->getId());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLWorker::onDebug(CDebugAdjure *pDebug)
{
    try
    {
        (*(m_pLFunc[LFUNC_TASK_DEBUG]))(pDebug->getSock(), pDebug->getType(), pDebug->getDebug());
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLWorker::onUnNorProc(CAdjure *pAdjure)
{
    switch (pAdjure->getAdjure())
    {
        case MSG_NET_READ_HTTPC:
        {
            CTaskHttcdAdjure *pHttcdAdjure((CTaskHttcdAdjure *)pAdjure);
            try
            {
                (*(m_pLFunc[LFUNC_TASK_BIND_HTTPC]))(pHttcdAdjure->getSock(), pHttcdAdjure->getType(), pHttcdAdjure->getBindId(),
                    pHttcdAdjure->getStatus(), pHttcdAdjure->getHead(), pHttcdAdjure->getBody());
            }
            catch (luabridge::LuaException &e)
            {
                H_LOG(LOGLV_ERROR, "%s", e.what());
            }
        }
        break;
        case MSG_NET_READ_I:
        {
            CINetReadAdjure *pINetRead((CINetReadAdjure *)pAdjure);
            try
            {
                (*(m_pLFunc[LFUNC_TASK_BIND_I]))(pINetRead->getSock(), pINetRead->getType(), 
                    pINetRead->getProto(), pINetRead->getPack());
            }
            catch (luabridge::LuaException &e)
            {
                H_LOG(LOGLV_ERROR, "%s", e.what());
            }
        }
        break;
        default:
            break;
    }
}

H_ENAMSP
