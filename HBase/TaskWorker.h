
#ifndef H_TASKWORKER_H_
#define H_TASKWORKER_H_

#include "Chan.h"
#include "HEnum.h"
#include "HStruct.h"
#include "Atomic.h"

H_BNAMSP

//服务基类
class CTaskWorker
{
private:
    struct H_LISTENTYPE
    {
        std::vector<unsigned short> vcType;
    };

public:
    CTaskWorker(const char *pszName, const int iCapacity) : m_bInGloble(false),
        m_usIndex(H_INIT_NUMBER), m_objChan(this, iCapacity)
    {
        H_ASSERT(strlen(pszName) < H_TASKNAMELENS, "task name too long.");
        m_strName = pszName;
    };
    virtual ~CTaskWorker()
    {};
    
    H_PROTOTYPE Run(H_MSG *pMsg);

    virtual void initTask(void) {};
    virtual void destroyTask(void) {};
    virtual void onAccept(H_LINK *pLink) {};
    virtual void onLinked(H_LINK *pLink) {};
    virtual void onClosed(H_LINK *pLink) {};
    virtual void onNetRead(H_TCPBUF *pTcpBuf) {};
    virtual void onFrame(H_TICK *pTick) {};
    virtual void onSec(H_TICK *pTick) {};
    virtual const char *onCMD(const char *pszCmd, const char *pszInfo, size_t &iOutLens)
    { 
        return NULL;
    }
    virtual const char *onRPCCall(H_LINK *pLink, const char *pszName, char *pszInfo,
        const size_t &uiLens, size_t &iOutLens)
    { 
        return NULL;
    };
    virtual void onRPCRtn(const unsigned int &uiId, H_Binary *pBinary) {};
    virtual const char *onTaskRPCCall(const char *pszName, char *pszInfo,
        const size_t &uiLens, size_t &iOutLens)
    {
        return NULL;
    };
    virtual void onTaskRPCRtn(const unsigned int &uiId, H_Binary *pBinary) {};    

    //mqtt
    //CONNECT
    virtual void onMQTTCONNECT(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_CONNECT_Info *pCONNECTInfo) {};
    virtual void onMQTTPUBLISH(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBLISH_Info *pPUBLISHInfo) {};
    virtual void onMQTTPUBACK(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBACK_Info *pPUBACKInfo) {};
    virtual void onMQTTPUBREC(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBREC_Info *pPUBRECInfo) {};
    virtual void onMQTTPUBREL(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBREL_Info *pPUBRELInfo) {};
    virtual void onMQTTPUBCOMP(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_PUBCOMP_Info *pPUBCOMPInfo) {};
    virtual void onMQTTSUBSCRIBE(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_SUBSCRIBE_Info *pSUBSCRIBEInfo) {};
    virtual void onMQTTUNSUBSCRIBE(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead, struct MQTT_UNSUBSCRIBE_Info *pUNSUBSCRIBEInfo) {};
    virtual void onMQTTPINGREQ(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead) {};
    virtual void onMQTTDISCONNECT(H_LINK *pLink, struct MQTT_FixedHead *pFixedHead) {};

    std::string *getName(void)
    {
        return &m_strName;
    };

    void setIndex(const unsigned short &usIndex)
    {
        m_usIndex = usIndex;
    };
    unsigned short getIndex(void)
    {
        return m_usIndex;
    };

    void setInGloble(const bool bInGloble)
    {
        m_bInGloble = bInGloble;
    };
    bool getInGloble(void)
    {
        return m_bInGloble;
    };
    CAtomic *getLock(void)
    {
        return &m_objLock;
    };

    CChan *getChan(void)
    {
        return &m_objChan;
    };
    void addType(const unsigned short &usEvent, const unsigned short &usType)
    {
        if (!haveType(usEvent, usType))
        {
            H_LISTENTYPE *pType(&m_stListenType[usEvent]);
            pType->vcType.push_back(usType);
        }
    };
    bool haveType(const unsigned short &usEvent, const unsigned short &usType)
    {
        H_LISTENTYPE *pType(&m_stListenType[usEvent]);
        return pType->vcType.end() != std::find(pType->vcType.begin(), pType->vcType.end(), usType);
    };

private:
    CTaskWorker(void);
    H_DISALLOWCOPY(CTaskWorker);

private:
    bool m_bInGloble;
    unsigned short m_usIndex;
    CChan m_objChan;
    std::string m_strName;
    CAtomic m_objLock;
    H_LISTENTYPE m_stListenType[MSG_NET_CLOSE + 1];
};

H_ENAMSP

#endif//H_TASKWORKER_H_
