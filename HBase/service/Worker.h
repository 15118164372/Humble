
#ifndef H_WORKER_H_
#define H_WORKER_H_

#include "HMutex.h"
#include "Adjure_Runner.h"
#include "Adjure_NetWorker.h"
#include "Adjure_Task.h"
#include "CirQueue.h"

H_BNAMSP

//服务基类
class CWorker : public CObject
{
public:
    CWorker(const char *pszName, const size_t &uiCapacity) : m_bInGloble(false), m_usIndex(H_INIT_NUMBER),
        m_pWorkerMgr(NULL), m_objRunnerAdjure(this), m_strName(pszName), m_objAdjureQu(uiCapacity)
    {};
    ~CWorker(void) 
    {
        cleanAdjureQu();
    };

    //执行  pAdjure在Runner中删除
    void Run(CAdjure *pAdjure);

    virtual void initTask(void) {};
    virtual void destroyTask(void) {};

    virtual void onTimeOut(CTaskTimeOutAdjure *pTimeOut) {};
    virtual void onAccepted(CTaskNetEvAdjure *pNetEv) {};
    virtual void onConnected(CTaskNetEvAdjure *pNetEv) {};
    virtual void onClosed(CTaskNetEvAdjure *pNetEv) {};
    virtual void onNetReadI(CINetReadAdjure *pNetRead) {};
    virtual void onNetReadHttpd(CTaskHttpdAdjure *pNetRead) {};
    virtual void onNetRPC(CNetRPCAdjure *pRPCAdjure) {};
    virtual void onRPCCall(CRPCCallAdjure *pRPCCall) {};
    virtual void onRPCRtn(CRPCRtnAdjure *pRPCRtn) {};
    virtual void onDebug(CDebugAdjure *pDebug) {};
    virtual void onUnNorProc(CAdjure *pAdjure) {};

    //任务执行命令
    CRunWorkerAdjure *getRunnerAdjure(void)
    {
        return &m_objRunnerAdjure;
    };
    //工作线程序号
    void setIndex(const unsigned short &usIndex)
    {
        m_usIndex = usIndex;
    };
    H_INLINE const unsigned short &getIndex(void)
    {
        return m_usIndex;
    };
    //任务锁
    CMutex *getWorkerLck(void)
    {
        return &m_objWorkerLck;
    };
    //是否在执行队列
    void setInGloble(const bool &bInGloble)
    {
        m_bInGloble = bInGloble;
    };
    H_INLINE const bool &getInGloble(void)
    {
        return m_bInGloble;
    };
    //任务队列操作 锁在调用处
    void addAdjure(CAdjure *pAdj);
    CAdjure *getAdjure(void)
    {
        return (CAdjure *)m_objAdjureQu.Pop();
    };
    const size_t &getAdjureNum(void)
    {
        return m_objAdjureQu.Size();
    };
    void cleanAdjureQu(void)
    {
        CAdjure *pAdjure(NULL);
        while (NULL != (pAdjure = getAdjure()))
        {
            H_SafeDelete(pAdjure);
        }
    };
    void setAdjureCapacity(const size_t &uiCapacity)
    {
        m_objAdjureQu.setCapacity(uiCapacity);
    };

    void setFreeze(const bool &bFreeze)
    {
        m_bFreeze = bFreeze;
    };
    const bool &getFreeze(void)
    {
        return m_bFreeze;
    };

    void setName(const char *pszName)
    {
        m_strName = pszName;
    };
    const char *getName(void)
    {
        return m_strName.c_str();
    };
    const std::string &getStrName(void)
    {
        return m_strName;
    };
    void setWorkerMgr(class CWorkerMgr *pWorkerMgr)
    {
        m_pWorkerMgr = pWorkerMgr;
    };
    class CWorkerMgr *getWorkerMgr(void)
    {
        return m_pWorkerMgr;
    };
    std::list<std::string> *storageStr(void)
    {
        return &m_lstString;
    };
    std::list<H_SOCK> *storageSock(void)
    {
        return &m_lstSock;
    };

private:
    CWorker(void);
    bool m_bInGloble;
    bool m_bFreeze;
    unsigned short m_usIndex;
    class CWorkerMgr *m_pWorkerMgr;
    CRunWorkerAdjure m_objRunnerAdjure;
    CMutex m_objWorkerLck;
    std::string m_strName;
    CCirQueue m_objAdjureQu;
    std::list<H_SOCK> m_lstSock;
    std::list<std::string> m_lstString;
};

H_ENAMSP

#endif//H_WORKER_H_
