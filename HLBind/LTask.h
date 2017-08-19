
#ifndef H_LTASK_H_
#define H_LTASK_H_

#include "Reg2Lua.h"

H_BNAMSP

class CLTask : public CTaskWorker
{
public:
    CLTask(const char *pszFile, const char *pszName, const int iCapacity);
    ~CLTask(void);

    void initTask(void);
    void destroyTask(void);
    void onAccept(H_LINK *pLink);
    void onLinked(H_LINK *pLink);
    void onClosed(H_LINK *pLink);
    void onNetRead(H_TCPBUF *pTcpBuf);
    void onFrame(H_TICK *pTick);
    void onSec(H_TICK *pTick);
    const char *onCMD(const char *pszCmd, const char *pszInfo, size_t &iOutLens);
    const char *onRPCCall(H_LINK *pLink, const char *pszName, char *pszInfo,
        const size_t &uiLens, size_t &iOutLens);
    void onRPCRtn(const unsigned int &uiId, H_Binary *pBinary);
    const char *onTaskRPCCall(const char *pszName, char *pszInfo,
        const size_t &uiLens, size_t &iOutLens);
    void onTaskRPCRtn(const unsigned int &uiId, H_Binary *pBinary);

    void onMQTTCONNECT(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_CONNECT_Info *pCONNECTInfo);
    void onMQTTPUBLISH(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBLISH_Info *pPUBLISHInfo);
    void onMQTTPUBACK(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBACK_Info *pPUBACKInfo);
    void onMQTTPUBREC(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBREC_Info *pPUBRECInfo);
    void onMQTTPUBREL(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBREL_Info *pPUBRELInfo);
    void onMQTTPUBCOMP(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBCOMP_Info *pPUBCOMPInfo);
    void onMQTTSUBSCRIBE(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_SUBSCRIBE_Info *pSUBSCRIBEInfo);
    void onMQTTUNSUBSCRIBE(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_UNSUBSCRIBE_Info *pUNSUBSCRIBEInfo);
    void onMQTTPINGREQ(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead);
    void onMQTTDISCONNECT(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead);

private:
    luabridge::LuaRef createMQTTHead(struct MQTT_FixedHead *pFixedHead);
    luabridge::LuaRef createCONNECTInfo(struct MQTT_CONNECT_Info *pCONNECTInfo);
    luabridge::LuaRef createPUBLISHInfo(struct MQTT_PUBLISH_Info *pPUBLISHInfo);
    luabridge::LuaRef createPUBACKInfo(struct MQTT_PUBACK_Info *pPUBACKInfo);
    luabridge::LuaRef createPUBRECInfo(struct MQTT_PUBREC_Info *pPUBRECInfo);
    luabridge::LuaRef createPUBRELInfo(struct MQTT_PUBREL_Info *pPUBRELInfo);
    luabridge::LuaRef createPUBCOMPInfo(struct MQTT_PUBCOMP_Info *pPUBCOMPInfo);
    luabridge::LuaRef createSUBSCRIBEInfo(struct MQTT_SUBSCRIBE_Info *pSUBSCRIBEInfo);
    luabridge::LuaRef createUNSUBSCRIBEInfo(struct MQTT_UNSUBSCRIBE_Info *pUNSUBSCRIBEInfo);

private:
    CLTask(void);
    H_DISALLOWCOPY(CLTask);

    enum
    {
        LFUNC_INITTASK = 0,
        LFUNC_DELTASK,
        LFUNC_ONNETEVENT,
        LFUNC_ONNETREAD,
        LFUNC_ONTIME,
        LFUNC_ONCMD,
        LFUNC_RPCCALL,
        LFUNC_RPCRTN,

        LFUNC_MQTT_CONNECT,
        LFUNC_MQTT_PUBLISH,
        LFUNC_MQTT_PUBACK,
        LFUNC_MQTT_PUBREC,
        LFUNC_MQTT_PUBREL,
        LFUNC_MQTT_PUBCOMP,
        LFUNC_MQTT_SUBSCRIBE,
        LFUNC_MQTT_UNSUBSCRIBE,
        LFUNC_MQTT_PINGREQ,
        LFUNC_MQTT_DISCONNECT,

        LFUNC_COUNT,
    };

private:
    luabridge::LuaRef **m_pLFunc;
    std::string m_strFile;
    H_Binary m_stBinary;
    H_LSTATE m_stState;
    H_CURLINK m_curRPCLink;
};

H_ENAMSP

#endif//H_LTASK_H_