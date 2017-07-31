
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
    CTaskWorker(const char *pszName, const int iCapacity) : m_bInGloble(false), m_objChan(this, iCapacity)
    {
        H_ASSERT(strlen(pszName) < H_TASKNAMELENS, "task name too long.");
        m_strName = pszName;
    };
    virtual ~CTaskWorker()
    {};
    
    void Run(H_MSG *pMsg);

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

    std::string *getName(void)
    {
        return &m_strName;
    };

    void setInGloble(const bool bInGloble)
    {
        m_bInGloble = bInGloble;
    };
    bool getInGloble(void)
    {
        return m_bInGloble;
    };
    CAtomic *getInGlobleLock(void)
    {
        return &m_objInGlobleLock;
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
    void sendTaskRPCRtn(H_RPC *pRPC, const char *pszMsg, const size_t &iLens);

private:
    CTaskWorker(void);
    H_DISALLOWCOPY(CTaskWorker);

private:
    bool m_bInGloble;
    CChan m_objChan;
    std::string m_strName;
    CAtomic m_objInGlobleLock;
    H_LISTENTYPE m_stListenType[MSG_NET_CLOSE + 1];
};

H_ENAMSP

#endif//H_TASKWORKER_H_
