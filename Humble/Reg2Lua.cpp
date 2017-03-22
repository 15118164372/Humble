
#include "Reg2Lua.h"
#include "ProtoDisp.h"

H_BNAMSP

void luaLog(const unsigned short usLV, const char *pInfo)
{
    H_LOG((LOG_LEVEL)usLV, "%s", pInfo);
}

void LSleep(const unsigned int uiMs)
{
    H_Sleep(uiMs);
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

CWorkerTask *newLuaTask(void)
{
    return new CLuaTask();
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

void H_RegAll(struct lua_State *pLState)
{
    H_RegBinary(pLState);
    H_RegNetParser(pLState);
    H_RegNetWorker(pLState);
    H_RegSender(pLState);
    H_RegFuncs(pLState);
    H_RegCharset(pLState);
    H_RegClock(pLState);
    H_RegIni(pLState);
    H_RegSnowFlake(pLState);
    H_RegTableFile(pLState);
    H_RegUUID(pLState);
    H_RegAES(pLState);
    H_RegRSA(pLState);
    H_RegSha1(pLState);
    H_RegWorkerDisp(pLState);
    H_RegWorkerTask(pLState);
    H_RegChan(pLState);
    H_RegProtoDisp(pLState);
    H_SetPackPath(pLState);
    H_RegOther(pLState);
    H_RegGlobal(pLState);    
}

void H_RegProtoDisp(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CProtoDisp>("CProtoDisp")
            .addFunction("regStrProto", &CProtoDisp::regStrProto)
            .addFunction("regIProto", &CProtoDisp::regIProto)
            .addFunction("getStrProto", &CProtoDisp::getStrProto)
            .addFunction("getIProto", &CProtoDisp::getIProto)
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
            .addFunction("getChan", &CWorkerDisp::getChan)
            .addFunction("regTask", &CWorkerDisp::regTask)
        .endClass();
}

void H_RegChan(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CChan>("CChan")
            .addFunction("Send", &CChan::Send)
            .addFunction("Recv", &CChan::Recv)
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
    luabridge::setGlobal(pLState, CNetWorker::getSingletonPtr(), "g_pNetWorker");
    luabridge::setGlobal(pLState, CSender::getSingletonPtr(), "g_pSender");
    luabridge::setGlobal(pLState, CWorkerDisp::getSingletonPtr(), "g_pWorkerMgr");
    luabridge::setGlobal(pLState, CNetParser::getSingletonPtr(), "g_pNetParser");
    luabridge::setGlobal(pLState, CProtoDisp::getSingletonPtr(), "g_pProtoDisp");
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
        .addFunction("b64Encode", H_B64Encode)
        .addFunction("b64Decode", H_B64Decode)
        .addFunction("md5Str", md5Str)
        .addFunction("md5File", md5File)
        .addFunction("newLuaTask", newLuaTask)
        .addFunction("getPathSeparator", getPathSeparator)
        .addFunction("getProPath", getProPath)
        .addFunction("getScriptPath", getScriptPath);
}

void H_RegNetParser(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CNetParser>("CNetParser")
            .addFunction("setParser", &CNetParser::setParser)
            .addFunction("getParserNam", &CNetParser::getParserNam)
        .endClass();
}

void H_RegBinary(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CBinary>("CBinary")
            .addConstructor<void(*) (void)>()
            
            .addFunction("setReadBuffer", &CBinary::setReadBuffer)
            .addFunction("getRBufLens", &CBinary::getRBufLens)
            .addFunction("getReadedLens", &CBinary::getReadedLens)

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
            .addFunction("getByte", &CBinary::getLByte)
            
            .addFunction("readLine", &CBinary::readLine)
            .addFunction("Find", &CBinary::Find)

            .addFunction("getWritedBuf", &CBinary::getWritedBuf)
            .addFunction("setW2R", &CBinary::setW2R)
            .addFunction("resetW2R", &CBinary::resetW2R)
            .addFunction("Append", &CBinary::Append)
        .endClass();
}

void H_RegNetWorker(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CNetBase>("CNetBase")
            .addFunction("closeSock", &CNetBase::closeSock)
            .addFunction("closeByType", &CNetBase::closeByType)
        .endClass()
        .deriveClass<CNetWorker, CNetBase>("CNetWorker")
            .addFunction("addListener", &CNetWorker::addListener)
            .addFunction("delListener", &CNetWorker::delListener)
            .addFunction("addTcpLink", &CNetWorker::addTcpLink)
            .addFunction("delTcpLink", &CNetWorker::delTcpLink)

            .addFunction("addUdp", &CNetWorker::addUdp)
            .addFunction("delUdp", &CNetWorker::delUdp)
        .endClass();
}

void H_RegSender(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .beginClass<CSender>("CSender")
            .addFunction("Send", &CSender::Send)
            .addFunction("sendB", &CSender::sendBinary)
            .addFunction("broadCast", &CSender::lbroadCast)
            .addFunction("broadCastB", &CSender::broadCastBinary)
            
            .addFunction("sendU", &CSender::sendU)
            .addFunction("sendUB", &CSender::sendUBinary)
            .addFunction("broadCastU", &CSender::broadCastU)
            .addFunction("broadCastUB", &CSender::broadCastUBinary)
        .endClass();
}

H_ENAMSP
