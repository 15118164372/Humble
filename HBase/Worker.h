
#ifndef H_WORKER_H_
#define H_WORKER_H_

#include "RecvTask.h"
#include "Chan.h"
#include "CirQueue.h"

H_BNAMSP

enum
{
    TCMD_INIT = 0,
    TCMD_RUN,
    TCMD_DEL
};

//服务基类
class CWorkerTask : public CTask
{
public:
    CWorkerTask(const char *pszName, const int iCapacity) : m_objChan(iCapacity), m_quCMD(iCapacity * 2),
        m_uiStatus(H_INIT_NUMBER), m_pCMD(NULL), m_strName(pszName)
    {
        m_objChan.setTask(this);
    };
    ~CWorkerTask()
    {};

    void Run(void);

    virtual void initTask(void) = 0;
    virtual void runTask(void) = 0;
    virtual void destroyTask(void) = 0;

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

private:
    CWorkerTask(void);
    H_DISALLOWCOPY(CWorkerTask);

private:    
    unsigned int m_uiStatus;
    unsigned int *m_pCMD;
    CChan m_objChan;
    CCirQueue m_quCMD;
    CAtomic m_objCMDLock;
    std::string m_strName;
};

//服务执行者
class CWorker : public CRecvTask<CWorkerTask>
{
public:
    CWorker(void);
    ~CWorker(void);

    void runTask(CWorkerTask *pMsg);
    void addWorker(CWorkerTask *pWorker)
    {
        pWorker->setStatus(1);
        addTask(pWorker);
    };

    unsigned int getStatus(void);
    void setBusy(void);
    void setIndex(const unsigned short &usIndex)
    {
        m_usIndex = usIndex;
    };

private:
    H_DISALLOWCOPY(CWorker);

private:
    unsigned short m_usIndex;
    unsigned int m_uiStatus;
};

H_ENAMSP

#endif
