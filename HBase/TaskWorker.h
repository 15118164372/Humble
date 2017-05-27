
#ifndef H_TASKWORKER_H_
#define H_TASKWORKER_H_

#include "Task.h"
#include "Chan.h"
#include "HEnum.h"
#include "HStruct.h"

H_BNAMSP

enum
{
    TCMD_INIT = 0,
    TCMD_RUN,
    TCMD_DEL
};

//服务基类
class CTaskWorker : public CTask
{
private:
    struct H_LISTENTYPE
    {
        std::vector<unsigned short> vcType;
    };

public:
    CTaskWorker(const char *pszName, const int iCapacity) : m_uiDestroy(H_INIT_NUMBER),
        m_uiStatus(H_INIT_NUMBER), m_pCMD(NULL), m_objChan(this, iCapacity),
        m_quCMD(iCapacity * 2)
    {
        H_ASSERT(strlen(pszName) < H_TASKNAMELENS, "task name too long.");
        m_strName = pszName;
    };
    ~CTaskWorker()
    {};
    
    void Run(void);
    void runTask(void);

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
    virtual const char *onRPCCall(const char *pszName, char *pszInfo,
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
    
    void setStatus(const unsigned int uiStatus)
    {
        H_AtomicSet(&m_uiStatus, uiStatus);
    };
    unsigned int getStatus(void)
    {
        return H_AtomicGet(&m_uiStatus);
    };

    void setCMD(unsigned int *pCMD)
    {
        m_pCMD = pCMD;
    };

    std::string *getName(void)
    {
        return &m_strName;
    };

    CChan *getChan(void)
    {
        return &m_objChan;
    };

    CCirQueue *getCMDQu(void)
    {
        return &m_quCMD;
    };

    CAtomic *getCMDLock(void)
    {
        return &m_objCMDLock;
    };

    void setDestroy(void)
    {
        H_AtomicSet(&m_uiDestroy, 1);
    };
    bool getDestroy(void)
    {
        return (H_INIT_NUMBER == H_AtomicGet(&m_uiDestroy)) ? false : true;
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
    void sendTaskRPCRtn(H_RPC *pRPC, const char *pszMsg, const size_t &iLens);

private:
    CTaskWorker(void);
    H_DISALLOWCOPY(CTaskWorker);

private:
    unsigned int m_uiDestroy;
    unsigned int m_uiStatus;
    unsigned int *m_pCMD;
    CChan m_objChan;
    CCirQueue m_quCMD;
    CAtomic m_objCMDLock;
    std::string m_strName;
    H_LISTENTYPE m_stListenType[MSG_NET_CLOSE + 1];
};

H_ENAMSP

#endif//H_TASKWORKER_H_
