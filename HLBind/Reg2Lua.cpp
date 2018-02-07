
#include "Reg2Lua.h"
#include "LWorker.h"

H_BNAMSP

//注册任务
void regLWorker(const char *pszFile, const char *pszName, const size_t uiCapacity)
{
    H_ASSERT(strlen(pszName) < H_TASKNAMELENS, "task name too long.");    
    CWorker *pWorker(g_pHumble->popPool());
    if (NULL == pWorker)
    {
        pWorker = new(std::nothrow) CLWorker(g_pHumble, pszFile, pszName, uiCapacity);
        H_ASSERT(NULL != pWorker, H_ERR_MEMORY);
    }
    else
    {
        ((CLWorker*)pWorker)->reSet(pszFile, pszName, uiCapacity);
    }

    g_pHumble->regTask(pWorker);
}
//广播
void broadCast(luabridge::LuaRef refSocks, const char *pszBuf, const size_t iLens)
{
    std::vector<H_SOCK> vcSocks;
    for (int i = 1; i <= refSocks.length(); ++i)
    {
        vcSocks.push_back(refSocks[i]);
    }

    g_pHumble->broadCast(vcSocks, pszBuf, iLens);
}
//lua日志
void luaLog(const unsigned short usLV, const char *pszLuaFile, const int iLine, const char *pInfo)
{
    g_pLog->writeLog((LOG_LEVEL)usLV, "[%s %d] %s", pszLuaFile, iLine, pInfo);
}
//new CMailAdjure
CMailAdjure *newMail(void)
{
    CMailAdjure *pMail = new(std::nothrow) CMailAdjure();
    H_ASSERT(NULL != pMail, "malloc memory error.");

    return pMail;
}

CReg2Lua::CReg2Lua(void)
{
}

CReg2Lua::~CReg2Lua(void)
{
}

void CReg2Lua::regAll(struct lua_State *pLState)
{
    regFuncs(pLState);
    regWorker(pLState);
    regAdjure(pLState);
    regMail(pLState);
    regHumble(pLState);
    regObject(pLState);
    regPackPath(pLState);

    luabridge::setGlobal(pLState, g_pHumble, "g_Humble");
}

void CReg2Lua::regFuncs(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .addFunction("regTask", regLWorker)
        .addFunction("broadCast", broadCast)
        .addFunction("newMail", newMail)
        .addFunction("H_LOG", luaLog);
}

void CReg2Lua::regWorker(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CWorker>("CWorker")
        .endClass();
}

void CReg2Lua::regPackPath(struct lua_State *pLState)
{
    std::list<std::string> lstDirs;

    CUtils::subDirName(g_pHumble->getScriptPath(), lstDirs);
    luabridge::LuaRef objPack = luabridge::getGlobal(pLState, "package");
    std::string strPack = objPack["path"];
    std::string strVal = CUtils::formatStr("%s?.lua", g_pHumble->getScriptPath());

    std::list<std::string>::iterator itDir;
    for (itDir = lstDirs.begin(); lstDirs.end() != itDir; ++itDir)
    {
        strVal = CUtils::formatStr("%s;%s%s/?.lua", strVal.c_str(), g_pHumble->getScriptPath(), itDir->c_str());
    }

    objPack["path"] = CUtils::formatStr("%s;%s", strPack.c_str(), strVal.c_str());
}

void CReg2Lua::regHumble(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CHumble>("CHumble")
            .addFunction("unRegTask", &CHumble::unRegTask)
            .addFunction("getTask", &CHumble::getTask)
            .addFunction("getAllName", &CHumble::getAllName)
            .addFunction("netRPC", &CHumble::netRPC)
            .addFunction("taskRPC", &CHumble::taskRPC)
            .addFunction("taskRPCRtn", &CHumble::taskRPCRtn)
            .addFunction("regTimeOut", &CHumble::regTimeOut)
            .addFunction("regAcceptEvent", &CHumble::regAcceptEvent)
            .addFunction("regConnectEvent", &CHumble::regConnectEvent)
            .addFunction("regCloseEvent", &CHumble::regCloseEvent)
            .addFunction("regIProto", &CHumble::regIProto)
            .addFunction("regHttpdProto", &CHumble::regHttpdProto)
            .addFunction("sendMail", &CHumble::sendMail)
            .addFunction("setLogPriority", &CHumble::setLogPriority)
            .addFunction("getLogPriority", &CHumble::getLogPriority)
            .addFunction("addListener", &CHumble::addListener)
            .addFunction("addLinker", &CHumble::addLinker)
            .addFunction("closeLink", &CHumble::closeLink)
            .addFunction("bindWorker", &CHumble::bindWorker)
            .addFunction("unBindWorker", &CHumble::unBindWorker)
            .addFunction("sendMsg", &CHumble::sendMsg)
            .addFunction("getLinkById", &CHumble::getLinkById)
            .addFunction("getALinkById", &CHumble::getALinkById)
            .addFunction("getLinkByType", &CHumble::getLinkByType)
            .addFunction("getALinkByType", &CHumble::getALinkByType)
            .addFunction("getServiceId", &CHumble::getServiceId)
            .addFunction("getServiceType", &CHumble::getServiceType)
            .addFunction("getProPath", &CHumble::getProPath)
            .addFunction("getScriptPath", &CHumble::getScriptPath)
            .addFunction("getRPCKey", &CHumble::getRPCKey)
        .endClass();
}

void CReg2Lua::regAdjure(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CHttpAdjure>("CHttpAdjure")
            .addFunction("getHead", &CHttpAdjure::getHead)
            .addFunction("getBody", &CHttpAdjure::getBody)
        .endClass()
        .deriveClass<CTaskHttpdAdjure, CHttpAdjure>("CTaskHttpdAdjure")
            .addFunction("getUrl", &CTaskHttpdAdjure::getUrl)
            .addFunction("getMethod", &CTaskHttpdAdjure::getMethod)
        .endClass();
}

void CReg2Lua::regMail(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CMailAdjure>("CMailAdjure")
            .addFunction("setAuthLogin", &CMailAdjure::setAuthLogin)
            .addFunction("setAuthPlain", &CMailAdjure::setAuthPlain)
            .addFunction("setSMTPSV", &CMailAdjure::setSMTPSV)
            .addFunction("setFromAddr", &CMailAdjure::setFromAddr)
            .addFunction("setUserName", &CMailAdjure::setUserName)
            .addFunction("setPSW", &CMailAdjure::setPSW)
            .addFunction("setSubject", &CMailAdjure::setSubject)
            .addFunction("setMsg", &CMailAdjure::setMsg)
            .addFunction("setHtml", &CMailAdjure::setHtml)
            .addFunction("setHtmlFile", &CMailAdjure::setHtmlFile)
            .addFunction("addToAddr", &CMailAdjure::addToAddr)
            .addFunction("addAttach", &CMailAdjure::addAttach)
        .endClass();
}

void CReg2Lua::regObject(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CObject>("CObject")
            .addConstructor<void(*) (void)>()
        .endClass()
        .deriveClass<CIniFile, CObject>("CIniFile")
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
        .endClass()

        .deriveClass<CTableFile, CObject>("CTableFile")
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
        .endClass()

        .deriveClass<CConHash, CObject>("CConHash")
            .addConstructor<void(*)(void)>()
            .addFunction("addNode", &CConHash::addNode)
            .addFunction("delNode", &CConHash::delNode)
            .addFunction("findNode", &CConHash::findNode)
            .addFunction("getVNodeNum", &CConHash::getVNodeNum)
        .endClass()

        .deriveClass<CSnowFlake, CObject>("CSnowFlake")
            .addConstructor<void(*) (void)>()
            .addFunction("setWorkId", &CSnowFlake::setWorkId)
            .addFunction("setCenterId", &CSnowFlake::setCenterId)
            .addFunction("getId", &CSnowFlake::getId)
        .endClass()

        .deriveClass<CClock, CObject>("CClock")
            .addConstructor<void(*) (void)>()
            .addFunction("reStart", &CClock::reStart)
            .addFunction("Elapsed", &CClock::Elapsed)
        .endClass()
#ifdef H_UUID
        .deriveClass<CUUID, CObject>("CUUID")
            .addConstructor<void(*) (void)>()
            .addFunction("getUUID", &CUUID::getUUID)
        .endClass()
#endif
        .deriveClass<CUtils, CObject>("CUtils")
            .addConstructor<void(*) (void)>()
            .addStaticFunction("threadId", &CUtils::threadId)
            .addStaticFunction("coreCount", &CUtils::coreCount)
            .addStaticFunction("nowMilSecond", &CUtils::nowMilSecond)
            .addStaticFunction("setNTo1", &CUtils::setNTo1)
            .addStaticFunction("setNTo0", &CUtils::setNTo0)
        .endClass()

        .deriveClass<CAES, CObject>("CAES")
            .addConstructor<void(*)(void)>()
            .addFunction("setKey", &CAES::setKey)
            .addFunction("Encode", &CAES::Encode)
            .addFunction("Decode", &CAES::Decode)
        .endClass()

        .deriveClass<CDESEncrypt, CObject>("CDESEncrypt")
            .addConstructor<void(*)(void)>()
            .addFunction("setKey", &CDESEncrypt::setKey)
            .addFunction("Encrypt", &CDESEncrypt::Encrypt)
        .endClass()

        .deriveClass<CRSAKey, CObject>("CRSAKey")
            .addConstructor<void(*)(void)>()
            .addFunction("creatKey", &CRSAKey::creatKey)
            .addFunction("saveRandom", &CRSAKey::saveRandom)
            .addFunction("savePubKey", &CRSAKey::savePubKey)
            .addFunction("savePriKey", &CRSAKey::savePriKey)
            .addFunction("loadPubKey", &CRSAKey::loadPubKey)
            .addFunction("loadPriKey", &CRSAKey::loadPriKey)
            .addFunction("loadRandom", &CRSAKey::loadRandom)
        .endClass()

        .deriveClass<CRSA, CObject>("CRSA")
            .addConstructor<void(*)(void)>()
            .addFunction("setKey", &CRSA::setKey)
            .addFunction("pubEncrypt", &CRSA::pubEncrypt)
            .addFunction("priDecrypt", &CRSA::priDecrypt)
            .addFunction("priEncrypt", &CRSA::priEncrypt)
            .addFunction("pubDecrypt", &CRSA::pubDecrypt)
        .endClass()

        .deriveClass<CUtils, CObject>("CUtils")
            .addConstructor<void(*)(void)>()
            .addStaticFunction("nowMilSecond", &CUtils::nowMilSecond)
            .addStaticFunction("nowStrMilSecond", &CUtils::nowStrMilSecond)
        .endClass()

        .deriveClass<CEnUtils, CObject>("CEnUtils")
            .addConstructor<void(*)(void)>()
            .addStaticFunction("zEncode", &CEnUtils::zEncode)
            .addStaticFunction("zDecode", &CEnUtils::zDecode)
            .addStaticFunction("uEncode", &CEnUtils::uEncode)
            .addStaticFunction("uDecode", &CEnUtils::uDecode)
            .addStaticFunction("crc16", &CEnUtils::crc16)
            .addStaticFunction("crc32", &CEnUtils::crc32)
            .addStaticFunction("xorEncode", &CEnUtils::xorEncode)
            .addStaticFunction("xorDecode", &CEnUtils::xorDecode)
            .addStaticFunction("b64Encode", &CEnUtils::b64Encode)
            .addStaticFunction("b64Decode", &CEnUtils::b64Decode)
            .addStaticFunction("md5Str", &CEnUtils::md5Str)
        .endClass();
}

H_ENAMSP
