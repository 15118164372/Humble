
#include "LTask.h"

H_BNAMSP

CLTask::CLTask(const char *pszFile, const char *pszName, const int iCapacity) : CTaskWorker(pszName, iCapacity), m_strFile(pszFile)
{
    m_stState.pLState = luaL_newstate();
    H_ASSERT(NULL != m_stState.pLState, "luaL_newstate error.");

    luaL_openlibs((struct lua_State *)m_stState.pLState);

    H_RegAll((struct lua_State *)m_stState.pLState);
    luabridge::setGlobal((struct lua_State *)m_stState.pLState, getName()->c_str(), "g_taskName");
    luabridge::setGlobal((struct lua_State *)m_stState.pLState, &m_stState, "thisState");
    luabridge::setGlobal((struct lua_State *)m_stState.pLState, &m_curRPCLink, "g_curRPCLink");

    m_pLFunc = new(std::nothrow) luabridge::LuaRef *[LFUNC_COUNT];
    H_ASSERT(NULL != m_pLFunc, "malloc memory error.");

    luabridge::LuaRef *pRef = NULL;
    for (int i = 0; i < LFUNC_COUNT; ++i)
    {
        pRef = new(std::nothrow) luabridge::LuaRef((struct lua_State *)m_stState.pLState);
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

    if (NULL != m_stState.pLState)
    {
        struct lua_State *pState = (struct lua_State *)m_stState.pLState;
        lua_close(pState);
        m_stState.pLState = NULL;
    }
}

void CLTask::initTask(void)
{
    std::string strLuaFile = g_strScriptPath + m_strFile;
    if (H_RTN_OK != luaL_dofile((struct lua_State *)m_stState.pLState, strLuaFile.c_str()))
    {
        const char *pErr = lua_tostring((struct lua_State *)m_stState.pLState, -1);
        H_ASSERT(false, pErr);
    }

    *(m_pLFunc[LFUNC_INITTASK]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "initTask");
    *(m_pLFunc[LFUNC_DELTASK]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "destroyTask");
    *(m_pLFunc[LFUNC_ONNETEVENT]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onNetEvent");
    *(m_pLFunc[LFUNC_ONNETREAD]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onNetRead");
    *(m_pLFunc[LFUNC_ONTIME]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onTimeEvent");
    *(m_pLFunc[LFUNC_ONCMD]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onCMD");
    *(m_pLFunc[LFUNC_RPCCALL]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onRPCCall");
    *(m_pLFunc[LFUNC_RPCRTN]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onRPCRtn");

    *(m_pLFunc[LFUNC_MQTT_CONNECT]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTCONNECT");
    *(m_pLFunc[LFUNC_MQTT_PUBLISH]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTPUBLISH");
    *(m_pLFunc[LFUNC_MQTT_PUBACK]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTPUBACK");
    *(m_pLFunc[LFUNC_MQTT_PUBREC]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTPUBREC");
    *(m_pLFunc[LFUNC_MQTT_PUBREL]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTPUBREL");
    *(m_pLFunc[LFUNC_MQTT_PUBCOMP]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTPUBCOMP");
    *(m_pLFunc[LFUNC_MQTT_SUBSCRIBE]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTSUBSCRIBE");
    *(m_pLFunc[LFUNC_MQTT_UNSUBSCRIBE]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTUNSUBSCRIBE");
    *(m_pLFunc[LFUNC_MQTT_PINGREQ]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTPINGREQ");
    *(m_pLFunc[LFUNC_MQTT_DISCONNECT]) = luabridge::getGlobal((struct lua_State *)m_stState.pLState, "onMQTTDISCONNECT");

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

const char *CLTask::onRPCCall(H_LINK *pLink, const char *pszName, char *pszInfo,
    const size_t &uiLens, size_t &iOutLens)
{
    try
    {
        m_stBinary.iLens = uiLens;
        m_stBinary.pBufer = pszInfo;
        m_curRPCLink.sock = pLink->sock;
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

luabridge::LuaRef CLTask::createMQTTHead(struct MQTT_FixedHead *pFixedHead)
{
    luabridge::LuaRef lFixedHead = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lFixedHead["MsgType"] = int(pFixedHead->MsgType);
    lFixedHead["DUP"] = int(pFixedHead->DUP);
    lFixedHead["QoS"] = int(pFixedHead->QoS);
    lFixedHead["RETAIN"] = int(pFixedHead->RETAIN);

    return lFixedHead;
}

luabridge::LuaRef CLTask::createCONNECTInfo(struct MQTT_CONNECT_Info *pCONNECTInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["UserNameFlag"] = int(pCONNECTInfo->UserNameFlag);
    lInfo["PswFlag"] = int(pCONNECTInfo->PswFlag);
    lInfo["WillRetain"] = int(pCONNECTInfo->WillRetain);
    lInfo["WillQoS"] = int(pCONNECTInfo->WillQoS);
    lInfo["WillFlag"] = int(pCONNECTInfo->WillFlag);
    lInfo["CleanSession"] = int(pCONNECTInfo->CleanSession);
    lInfo["Reserved"] = int(pCONNECTInfo->Reserved);
    lInfo["ProtoLevel"] = int(pCONNECTInfo->ProtoLevel);
    lInfo["KeepAlive"] = pCONNECTInfo->KeepAlive;

    lInfo["ProtoName"] = pCONNECTInfo->ProtoName;
    lInfo["UserName"] = pCONNECTInfo->UserName;
    lInfo["Psw"] = pCONNECTInfo->Psw;
    lInfo["WillTopic"] = pCONNECTInfo->WillTopic;
    lInfo["WillMessage"] = pCONNECTInfo->WillMessage;
    lInfo["ClientId"] = pCONNECTInfo->ClientId;

    return lInfo;
}

luabridge::LuaRef CLTask::createPUBLISHInfo(struct MQTT_PUBLISH_Info *pPUBLISHInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["MsgId"] = pPUBLISHInfo->MsgId;

    lInfo["Topic"] = pPUBLISHInfo->Topic;
    lInfo["Payload"] = pPUBLISHInfo->Payload;

    return lInfo;
}

luabridge::LuaRef CLTask::createPUBACKInfo(struct MQTT_PUBACK_Info *pPUBACKInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["MsgId"] = pPUBACKInfo->MsgId;

    return lInfo;
}

luabridge::LuaRef CLTask::createPUBRECInfo(struct MQTT_PUBREC_Info *pPUBRECInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["MsgId"] = pPUBRECInfo->MsgId;

    return lInfo;
}

luabridge::LuaRef CLTask::createPUBRELInfo(struct MQTT_PUBREL_Info *pPUBRELInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["MsgId"] = pPUBRELInfo->MsgId;

    return lInfo;
}

luabridge::LuaRef CLTask::createPUBCOMPInfo(struct MQTT_PUBCOMP_Info *pPUBCOMPInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["MsgId"] = pPUBCOMPInfo->MsgId;

    return lInfo;
}

luabridge::LuaRef CLTask::createSUBSCRIBEInfo(struct MQTT_SUBSCRIBE_Info *pSUBSCRIBEInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["MsgId"] = pSUBSCRIBEInfo->MsgId;

    std::vector<SUBSCRIBETopic>::iterator itTopic;
    luabridge::LuaRef lTopics = luabridge::newTable((struct lua_State *)m_stState.pLState);
    for (itTopic = pSUBSCRIBEInfo->vcTopic.begin(); pSUBSCRIBEInfo->vcTopic.end() != itTopic; ++itTopic)
    {
        lTopics[itTopic->Topic] = int(itTopic->QoS);
    }
    lInfo["Topics"] = lTopics;

    return lInfo;
}

luabridge::LuaRef CLTask::createUNSUBSCRIBEInfo(struct MQTT_UNSUBSCRIBE_Info *pUNSUBSCRIBEInfo)
{
    luabridge::LuaRef lInfo = luabridge::newTable((struct lua_State *)m_stState.pLState);

    lInfo["MsgId"] = pUNSUBSCRIBEInfo->MsgId;

    std::vector<std::string>::iterator itTopic;
    luabridge::LuaRef lTopics = luabridge::newTable((struct lua_State *)m_stState.pLState);
    for (itTopic = pUNSUBSCRIBEInfo->vcTopic.begin(); pUNSUBSCRIBEInfo->vcTopic.end() != itTopic; ++itTopic)
    {
        lTopics.append(*itTopic);
    }
    lInfo["Topics"] = lTopics;

    return lInfo;
}

void CLTask::onMQTTCONNECT(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_CONNECT_Info *pCONNECTInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_CONNECT]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createCONNECTInfo(pCONNECTInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTPUBLISH(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBLISH_Info *pPUBLISHInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_PUBLISH]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createPUBLISHInfo(pPUBLISHInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTPUBACK(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBACK_Info *pPUBACKInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_PUBACK]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createPUBACKInfo(pPUBACKInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTPUBREC(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBREC_Info *pPUBRECInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_PUBREC]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createPUBRECInfo(pPUBRECInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTPUBREL(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBREL_Info *pPUBRELInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_PUBREL]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createPUBRELInfo(pPUBRELInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTPUBCOMP(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBCOMP_Info *pPUBCOMPInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_PUBCOMP]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createPUBCOMPInfo(pPUBCOMPInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTSUBSCRIBE(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_SUBSCRIBE_Info *pSUBSCRIBEInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_SUBSCRIBE]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createSUBSCRIBEInfo(pSUBSCRIBEInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTUNSUBSCRIBE(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_UNSUBSCRIBE_Info *pUNSUBSCRIBEInfo)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_UNSUBSCRIBE]))(pLink->sock, pLink->usType, 
            createMQTTHead(pFixedHead), createUNSUBSCRIBEInfo(pUNSUBSCRIBEInfo));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTPINGREQ(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_PINGREQ]))(pLink->sock, pLink->usType, createMQTTHead(pFixedHead));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

void CLTask::onMQTTDISCONNECT(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead)
{
    try
    {
        (*(m_pLFunc[LFUNC_MQTT_DISCONNECT]))(pLink->sock, pLink->usType, createMQTTHead(pFixedHead));
    }
    catch (luabridge::LuaException &e)
    {
        H_LOG(LOGLV_ERROR, "%s", e.what());
    }
}

H_ENAMSP
