
#include "Reg2Lua.h"
#include "LTask.h"

H_BNAMSP

int getSVId(void)
{
    return g_iSVId;
}

int getSVType(void)
{
    return g_iSVType;
}

void setLogPriority(unsigned short usLV)
{
    CLog::getSingletonPtr()->setPriority(usLV);
}
unsigned short getLogPriority(void)
{
    return CLog::getSingletonPtr()->getPriority();
}
void luaLog(const unsigned short usLV, const char *pszLuaFile, const int iLine, const char *pInfo)
{
    CLog::getSingletonPtr()->writeLog((LOG_LEVEL)usLV, "[%s %d] %s", pszLuaFile, iLine, pInfo);
}

std::string md5Str(const char *pszVal, const size_t iLens)
{
    CMD5 objMD5;

    objMD5.update(pszVal, iLens);

    return objMD5.toString();
}

std::string md5File(const char *pszFile)
{
    CMD5 objMD5;

    objMD5.updatefile(pszFile);

    return objMD5.toString();
}

const char *getProPath(void)
{
    return g_strProPath.c_str();
}

const char *getScriptPath(void)
{
    return g_strScriptPath.c_str();
}

const char *getPathSeparator(void)
{
    return H_PATH_SEPARATOR;
}

void regEvent(unsigned short usEvent, const char *pszTask, const short sType)
{
    CChan *pChan(CTaskMgr::getSingletonPtr()->getChan(pszTask));
    if (NULL == pChan)
    {
        H_LOG(LOGLV_WARN, "get task %s error", pszTask);
        return;
    }

    CTaskWorker *pTask(pChan->getTask());
    CMSGDispatch::getSingletonPtr()->regEvent(usEvent, pTask, sType);
}

void unRegTime(unsigned short usEvent, const char *pszTask)
{
    CChan *pChan(CTaskMgr::getSingletonPtr()->getChan(pszTask));
    if (NULL == pChan)
    {
        H_LOG(LOGLV_WARN, "get task %s error", pszTask);
        return;
    }

    CTaskWorker *pTask(pChan->getTask());
    CMSGDispatch::getSingletonPtr()->unRegTime(usEvent, pTask);
}

void regIProto(H_PROTOTYPE iProto, const char *pszTask)
{
    CChan *pChan(CTaskMgr::getSingletonPtr()->getChan(pszTask));
    if (NULL == pChan)
    {
        H_LOG(LOGLV_WARN, "get task %s error", pszTask);
        return;
    }

    CMSGDispatch::getSingletonPtr()->regNetProto(iProto, pChan);
}

void regStrProto(const char *pszProto, const char *pszTask)
{
    CChan *pChan(CTaskMgr::getSingletonPtr()->getChan(pszTask));
    if (NULL == pChan)
    {
        H_LOG(LOGLV_WARN, "get task %s error", pszTask);
        return;
    }

    CMSGDispatch::getSingletonPtr()->regStrProto(pszProto, pChan);
}

void addListener(const char *pszParser, const unsigned short usType,
    const char *pszHost, const unsigned short usPort)
{
    CNetListener::getSingletonPtr()->addListener(pszParser, usType, pszHost, usPort);
}

void linkTo(const char *pszParser, const unsigned short usType, 
    const char *pszHost, const unsigned short usPort)
{
    CLinker::getSingletonPtr()->addLink(pszParser, usType, pszHost, usPort);
}

void closeLink(H_SOCK sock)
{
    (void)CNetWorkerMgr::getSingletonPtr()->closeLink(sock);
}
void removeLink(H_SOCK sock)
{
    (void)CNetWorkerMgr::getSingletonPtr()->removeLink(sock);
}

void httpResponse(H_SOCK sock, const char *pszBuf, const size_t iLens)
{
    H_PROTOTYPE iProto(H_INIT_NUMBER);
    CHttp::getSingletonPtr()->Response(sock, iProto, pszBuf, iLens);
}

void tcp1Response(H_SOCK sock, H_PROTOTYPE iProto, const char *pszBuf, const size_t iLens)
{
    CTcp1::getSingletonPtr()->Response(sock, iProto, pszBuf, iLens);
}

void tcp2Response(H_SOCK sock, H_PROTOTYPE iProto, const char *pszBuf, const size_t iLens)
{
    CTcp2::getSingletonPtr()->Response(sock, iProto, pszBuf, iLens);
}

void rpcCall(H_SOCK sock, unsigned int uiId, const char *pszRPCName, const char *pszToTask, const char *pszSrcTask,
    const char *pMsg, const size_t iLens)
{
    CSender::getSingletonPtr()->sendRPCCall(sock, uiId, pszRPCName, pszToTask, pszSrcTask, pMsg, iLens);
}

void taskRPCCall(unsigned int uiId, const char *pszRPCName, const char *pszToTask, const char *pszSrcTask,
    const char *pMsg, const size_t iLens)
{
    CTaskMgr::getSingletonPtr()->taskRPCCall(uiId, pszRPCName, pszToTask, pszSrcTask, pMsg, iLens);
}

void regTask(const char *pszFile, const char *pszName, const int iCapacity)
{
    CTaskWorker *pTask = new(std::nothrow) CLTask(pszFile, pszName, iCapacity);
    H_ASSERT(NULL != pTask, "malloc memory error.");
    CTaskMgr::getSingletonPtr()->regTask(pTask);
}

void unregTask(const char *pszName)
{
    CTaskMgr::getSingletonPtr()->unregTask(pszName);
}

void rpcLinkRegister(const int iSVId, const int iSVType, H_SOCK sock)
{
    CRPCLink::getSingletonPtr()->Register(iSVId, iSVType, sock);
}
void rpcLinkUnregister(const int iSVId, const int iSVType)
{
    CRPCLink::getSingletonPtr()->Unregister(iSVId, iSVType);
}
H_SOCK getLinkById(const int iSVId)
{
    return CRPCLink::getSingletonPtr()->getLinkById(iSVId);
}
luabridge::LuaRef getLinkByType(H_LSTATE *pState, const int iSVType)
{
    luabridge::LuaRef luaTable = luabridge::newTable((struct lua_State*)pState->pLState);
    std::vector<H_SOCK> vcSock(CRPCLink::getSingletonPtr()->getLinkByType(iSVType));
    for (std::vector<H_SOCK>::iterator itSock = vcSock.begin(); vcSock.end() != itSock; ++itSock)
    {
        luaTable.append(*itSock);
    }

    return luaTable;
}

CMail *newMail(void)
{
    CMail *pMail = new(std::nothrow) CMail();
    H_ASSERT(NULL != pMail, "malloc memory error.");

    return pMail;
}


void H_RegFuncs(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .addFunction("getSVId", getSVId)
        .addFunction("getSVType", getSVType)
        .addFunction("getLogPriority", getLogPriority)
        .addFunction("setLogPriority", setLogPriority)
        .addFunction("H_LOG", luaLog)
        .addFunction("CRC16", H_CRC16)
        .addFunction("CRC32", H_CRC32)
        .addFunction("urlEncode", H_UEncode)
        .addFunction("urlDecode", H_UDecode)
        .addFunction("zlibEncode", H_ZEncode)
        .addFunction("zlibDecode", H_ZDecode)
        .addFunction("b64Encode", H_B64Encode)
        .addFunction("b64Decode", H_B64Decode)
        .addFunction("md5Str", md5Str)
        .addFunction("md5File", md5File)
        .addFunction("getPathSeparator", getPathSeparator)
        .addFunction("getProPath", getProPath)
        .addFunction("getScriptPath", getScriptPath)
        .addFunction("regEvent", regEvent)
        .addFunction("unRegTime", unRegTime)
        .addFunction("regIProto", regIProto)
        .addFunction("regStrProto", regStrProto)
        .addFunction("addListener", addListener)
        .addFunction("linkTo", linkTo)
        .addFunction("closeLink", closeLink)
        .addFunction("removeLink", removeLink)
        .addFunction("httpResponse", httpResponse)
        .addFunction("tcp1Response", tcp1Response)
        .addFunction("tcp2Response", tcp2Response)
        .addFunction("rpcCall", rpcCall)
        .addFunction("taskRPCCall", taskRPCCall)
        .addFunction("regTask", regTask)
        .addFunction("unregTask", unregTask)
        .addFunction("rpcLinkRegister", rpcLinkRegister)
        .addFunction("rpcLinkUnregister", rpcLinkUnregister)
        .addFunction("getLinkById", getLinkById)
        .addFunction("getLinkByType", getLinkByType)
        .addFunction("newMail", newMail);
}

void H_RegLState(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<H_LSTATE>("H_LSTATE")
        .endClass();
}

void H_RegMail(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CMail>("CMail")
            .addFunction("setAuthLogin", &CMail::setAuthLogin)
            .addFunction("setAuthPlain", &CMail::setAuthPlain)
            .addFunction("setSMTPSV", &CMail::setSMTPSV)
            .addFunction("setFromAddr", &CMail::setFromAddr)
            .addFunction("setUserName", &CMail::setUserName)
            .addFunction("setPSW", &CMail::setPSW)
            .addFunction("setSubject", &CMail::setSubject)
            .addFunction("setMsg", &CMail::setMsg)
            .addFunction("setHtml", &CMail::setHtml)
            .addFunction("setHtmlFile", &CMail::setHtmlFile)
            .addFunction("addToAddr", &CMail::addToAddr)
            .addFunction("addAttach", &CMail::addAttach)
            .addFunction("Send", &CMail::Send)
        .endClass();
}

void H_RegCurLink(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<H_CURLINK>("H_CURLINK")
            .addFunction("getSock", &H_CURLINK::getSock)
        .endClass();
}

void H_RegClock(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CClock>("CClock")
            .addConstructor<void(*) (void)>()
            .addFunction("reStart", &CClock::reStart)
            .addFunction("Elapsed", &CClock::Elapsed)
        .endClass();
}

void H_RegIni(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CIniFile>("CIniFile")
            .addConstructor<void(*) (void)>()
            .addFunction("setFile", &CIniFile::setFile)
            .addFunction("getStringValue", &CIniFile::getStringValue)
            .addFunction("getIntValue", &CIniFile::getIntValue)
            .addFunction("getFloatValue", &CIniFile::getFloatValue)
            .addFunction("setStringValue", &CIniFile::setStringValue)
            .addFunction("setIntValue", &CIniFile::setIntValue)
            .addFunction("setFloatValue", &CIniFile::setFloatValue)
            .addFunction("delKey", &CIniFile::delKey)
            .addFunction("delNode", &CIniFile::delNode)
            .addFunction("Save", &CIniFile::Save)
            .addFunction("saveTo", &CIniFile::saveTo)
        .endClass();
}

void H_RegSnowFlake(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CSnowFlake>("CSnowFlake")
            .addConstructor<void(*) (void)>()
            .addFunction("setWorkid", &CSnowFlake::setWorkid)
            .addFunction("setCenterid", &CSnowFlake::setCenterid)
            .addFunction("getID", &CSnowFlake::getID)
        .endClass();
}

void H_RegTableFile(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CTableFile>("CTableFile")
            .addConstructor<void(*) (void)>()
            .addFunction("setFile", &CTableFile::setFile)
            .addFunction("setSplitFlag", &CTableFile::setSplitFlag)
            .addFunction("Parse", &CTableFile::Parse)
            .addFunction("eof", &CTableFile::eof)
            .addFunction("nextRow", &CTableFile::nextRow)
            .addFunction("reSet", &CTableFile::reSet)
            .addFunction("getStringValue", &CTableFile::getStringValue)
            .addFunction("getIntValue", &CTableFile::getIntValue)
            .addFunction("getFloatValue", &CTableFile::getFloatValue)
        .endClass();
}

void H_RegUUID(struct lua_State *pLState)
{
#ifdef H_UUID
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CUUID>("CUUID")
            .addConstructor<void(*) (void)>()
            .addFunction("getUUID", &CUUID::getUUID)
        .endClass();
#endif
}

void H_RegAES(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CAES>("CAES")
            .addConstructor<void(*) (void)>()
            .addFunction("setKey", &CAES::setKey)
            .addFunction("Encode", &CAES::Encode)
            .addFunction("Decode", &CAES::Decode)
        .endClass();
}

void H_RegDESEncrypt(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CDESEncrypt>("CDESEncrypt")
            .addConstructor<void(*) (void)>()
            .addFunction("setKey", &CDESEncrypt::setKey)
            .addFunction("Encrypt", &CDESEncrypt::Encrypt)
        .endClass();
}

void H_RegRSA(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CRSAKey>("CRSAKey")
            .addConstructor<void(*) (void)>()
            .addFunction("creatKey", &CRSAKey::creatKey)
            .addFunction("saveRandom", &CRSAKey::saveRandom)
            .addFunction("savePubKey", &CRSAKey::savePubKey)
            .addFunction("savePriKey", &CRSAKey::savePriKey)
            .addFunction("loadPubKey", &CRSAKey::loadPubKey)
            .addFunction("loadPriKey", &CRSAKey::loadPriKey)
            .addFunction("loadRandom", &CRSAKey::loadRandom)
        .endClass();

    luabridge::getGlobalNamespace(pLState)
        .beginClass<CRSA>("CRSA")
            .addConstructor<void(*) (void)>()
            .addFunction("setKey", &CRSA::setKey)
            .addFunction("pubEncrypt", &CRSA::pubEncrypt)
            .addFunction("priDecrypt", &CRSA::priDecrypt)
            .addFunction("priEncrypt", &CRSA::priEncrypt)
            .addFunction("pubDecrypt", &CRSA::pubDecrypt)
        .endClass();
}

void H_RegSha1(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CSHA1>("CSHA1")
            .addConstructor<void(*) (void)>()
            .addFunction("reSet", &CSHA1::Reset)
            .addFunction("upDate", &CSHA1::Update)
            .addFunction("hashFile", &CSHA1::HashFile)
            .addFunction("Final", &CSHA1::Final)
            .addFunction("getHash", &CSHA1::getHash)
        .endClass();
}

void H_RegAll(struct lua_State *pLState)
{
    H_RegLState(pLState);
    H_RegMail(pLState);
    H_RegCurLink(pLState);
    H_RegFuncs(pLState);
    H_RegClock(pLState);
    H_RegIni(pLState);
    H_RegSnowFlake(pLState);
    H_RegTableFile(pLState);
    H_RegUUID(pLState);
    H_RegAES(pLState);
    H_RegDESEncrypt(pLState);
    H_RegRSA(pLState);
    H_RegSha1(pLState);
    H_SetPackPath(pLState);
    H_RegOther(pLState);
}

H_ENAMSP
