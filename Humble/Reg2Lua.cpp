
#include "Reg2Lua.h"

H_BNAMSP

void luaLog(const unsigned short usLV, const char *pInfo)
{
    H_LOG((LOG_LEVEL)usLV, "%s", pInfo);
}

void LSleep(const unsigned int uiMs)
{
    H_Sleep(uiMs);
}

CWorkerTask *netLuaTask(void)
{
    return new CLuaTask();
}

void H_RegAll(struct lua_State *pLState)
{
    H_RegBinary(pLState);
    H_RegEvbuffer(pLState);
    H_RegSession(pLState);
    H_RegNetWorker(pLState);
    H_RegSender(pLState);
    H_RegMail(pLState);
    H_RegFuncs(pLState);
    H_RegCharset(pLState);
    H_RegClock(pLState);
    H_RegIni(pLState);
    H_RegSnowFlake(pLState);
    H_RegTableFile(pLState);
    H_RegUUID(pLState);
    H_RegAES(pLState);
    H_RegBase64(pLState);
    H_RegRSA(pLState);
    H_RegWorkerDisp(pLState);
    H_RegWorkerTask(pLState);
    H_RegLog(pLState);
    H_RegChan(pLState);
    H_RegGlobal(pLState);
}

void H_RegLog(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CLog>("CLog")
            .addFunction("setPriority", &CLog::setPriority)
            .addFunction("setLogFile", &CLog::setLogFile)
            .addFunction("Open", &CLog::Open)
        .endClass();
}

void H_RegWorkerTask(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CTask>("CTask")
        .endClass()
        .deriveClass<CWorkerTask, CTask>("CWorkerTask")            
        .endClass()
        .deriveClass<CLuaTask, CWorkerTask>("CLuaTask")
        .endClass();
}

void H_RegWorkerDisp(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CWorkerDisp>("CWorkerDisp")
            .addFunction("regSendChan", &CWorkerDisp::regSendChan)
            .addFunction("regRecvChan", &CWorkerDisp::regRecvChan)
            .addFunction("getChan", &CWorkerDisp::getChan)
            .addFunction("regTask", &CWorkerDisp::regTask)
            .addFunction("setThreadNum", &CWorkerDisp::setThreadNum)
        .endClass();
}

void H_RegChan(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CChan>("CChan")
            .addFunction("Send", &CChan::Send)
            .addFunction("Recv", &CChan::Recv)
            .addFunction("canSend", &CChan::canSend)
            .addFunction("canRecv", &CChan::canRecv)
        .endClass();
}

void H_RegRSA(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<R_RANDOM_STRUCT>("R_RANDOM_STRUCT")
        .endClass();
    luabridge::getGlobalNamespace(pLState)
        .beginClass<R_RSA_PUBLIC_KEY>("R_RSA_PUBLIC_KEY")
        .endClass();
    luabridge::getGlobalNamespace(pLState)
        .beginClass<R_RSA_PRIVATE_KEY>("R_RSA_PRIVATE_KEY")
        .endClass();

    luabridge::getGlobalNamespace(pLState)
        .beginClass<CRSAKey>("CRSAKey")
            .addConstructor<void(*) (void)>()
            .addFunction("creatKey", &CRSAKey::creatKey)
            .addFunction("getRandom", &CRSAKey::getRandom)
            .addFunction("getPubKey", &CRSAKey::getPubKey)
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

void H_RegBase64(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CBase64>("CBase64")
            .addConstructor<void(*) (void)>()
            .addStaticFunction("Encode", &CBase64::Encode)
            .addStaticFunction("Decode", &CBase64::Decode)
        .endClass();
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

void H_RegClock(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CClock>("CClock")
            .addConstructor<void(*) (void)>()
            .addFunction("reStart", &CClock::reStart)
            .addFunction("Elapsed", &CClock::Elapsed)
        .endClass();
}

void H_RegCharset(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CCharset>("CCharset")
            .addConstructor<void(*) (void)>()
            .addFunction("getStrCharset", &CCharset::getStrCharset)
            .addFunction("getFileCharset", &CCharset::getFileCharset)
        .endClass();
}

void H_RegGlobal(struct lua_State *pLState)
{
    luabridge::setGlobal(pLState, g_strProPath, "g_strProPath");
    luabridge::setGlobal(pLState, g_strScriptPath, "g_strScriptPath");
    luabridge::setGlobal(pLState, CNetWorker::getSingletonPtr(), "g_pNetWorker");
    luabridge::setGlobal(pLState, CSender::getSingletonPtr(), "g_pSender");
    luabridge::setGlobal(pLState, CMail::getSingletonPtr(), "g_pEmail");
    luabridge::setGlobal(pLState, CWorkerDisp::getSingletonPtr(), "g_pWorkerMgr");
    luabridge::setGlobal(pLState, CLog::getSingletonPtr(), "g_pLog");
}

void H_RegFuncs(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .addFunction("Sleep", LSleep)
        .addFunction("H_LOG", luaLog)
        .addFunction("CRC16", H_CRC16)
        .addFunction("CRC32", H_CRC32)
        .addFunction("urlEncode", H_UEncode)
        .addFunction("urlDecode", H_UDecode)
        .addFunction("zlibEncode", H_ZEncode)
        .addFunction("zlibDecode", H_ZDecode)
        .addFunction("netLuaTask", netLuaTask);
}

void H_RegBinary(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CBinary>("CBinary")
            .addFunction("getRBufLens", &CBinary::getRBufLens)

            .addFunction("reSetWrite", &CBinary::reSetWrite)
            .addFunction("skipRead", &CBinary::skipRead)
            .addFunction("skipWrite", &CBinary::skipWrite)
            .addFunction("getSurpLens", &CBinary::getSurpLens)

            .addFunction("setSint8", &CBinary::setSint8)
            .addFunction("getSint8", &CBinary::getSint8)
            .addFunction("setUint8", &CBinary::setUint8)
            .addFunction("getUint8", &CBinary::getUint8)

            .addFunction("setBool", &CBinary::setBool)
            .addFunction("getBool", &CBinary::getBool)

            .addFunction("setSint16", &CBinary::setSint16)
            .addFunction("getSint16", &CBinary::getSint16)
            .addFunction("setUint16", &CBinary::setUint16)
            .addFunction("getUint16", &CBinary::getUint16)

            .addFunction("setSint32", &CBinary::setSint32)
            .addFunction("getSint32", &CBinary::getSint32)
            .addFunction("setUint32", &CBinary::setUint32)
            .addFunction("getUint32", &CBinary::getUint32)

            .addFunction("setSint64", &CBinary::setSint64)
            .addFunction("getSint64", &CBinary::getSint64)
            .addFunction("setUint64", &CBinary::setUint64)
            .addFunction("getUint64", &CBinary::getUint64)

            .addFunction("setDouble", &CBinary::setDouble)
            .addFunction("getDouble", &CBinary::getDouble)

            .addFunction("setFloat", &CBinary::setFloat)
            .addFunction("getFloat", &CBinary::getFloat)

            .addFunction("setString", &CBinary::setString)
            .addFunction("getString", &CBinary::getString)

            .addFunction("setByte", &CBinary::setByte)
            .addFunction("getByte", &CBinary::getByte)
        .endClass();
}

void H_RegSession(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<H_Session>("H_Session")
            .addData("sockType", &H_Session::usSockType)
            .addData("status", &H_Session::usStatus)
            .addData("session", &H_Session::uiSession)
            .addData("sock", &H_Session::sock)
            .addData("id", &H_Session::uiID)
        .endClass();
}

void H_RegEvbuffer(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
            .beginClass<evbuffer_ptr>("evbuffer_ptr")
                .addConstructor<void(*) (void)>()
                .addData("pos", &evbuffer_ptr::pos)
        .endClass();

    luabridge::getGlobalNamespace(pLState)
        .beginClass<CEvBuffer>("CEvBuffer")
            .addFunction("getTotalLens", &CEvBuffer::getTotalLens)
            .addFunction("readBuffer", &CEvBuffer::readBuffer)
            .addFunction("delBuffer", &CEvBuffer::delBuffer)
            .addFunction("readBuffer", &CEvBuffer::readBuffer)
            .addFunction("readBuffer", &CEvBuffer::readBuffer)
        .endClass();
}

void H_RegNetWorker(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CNetWorker>("CNetWorker")
            .addFunction("tcpListen", &CNetWorker::tcpListen)
            .addFunction("addTcpLink", &CNetWorker::addTcpLink)
            .addFunction("closeSock", &CNetWorker::closeSock)
            .addFunction("setTick", &CNetWorker::setTick)
            //.addFunction("udpListen", &CNetWorker::udpListen)
            //.addFunction("sendTo", &CNetWorker::sendTo)
        .endClass();
}

void H_RegSender(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CSender>("CSender")
            .addFunction("Send", &CSender::Send)
            .addFunction("broadCast", &CSender::lbroadCast)
        .endClass();
}

void H_RegMail(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CMail>("CMail")
            .addFunction("setServer", &CMail::setServer)
            .addFunction("setAuthType", &CMail::setAuthType)
            .addFunction("setSender", &CMail::setSender)
            .addFunction("setUserName", &CMail::setUserName)
            .addFunction("setPassWord", &CMail::setPassWord)
            .addFunction("sendMail", &CMail::sendMail)
        .endClass();
}

H_ENAMSP
