
#ifndef H_WORKER_H_
#define H_WORKER_H_

#include "RecvTask.h"
#include "Chan.h"

H_BNAMSP

enum
{
    WS_FREE = 0,
    WS_BUSY,
};

//服务基类
class CWorkerTask : public CTask
{
public:
    CWorkerTask(const int iCapacity) : m_uiRef(H_INIT_NUMBER), m_objChan(iCapacity)
    {};
    ~CWorkerTask()
    {};

    void Run(void)
    {
        runTask();
    };

    virtual void initTask(void) = 0;
    virtual void runTask(void) = 0;
    virtual void destroyTask(void) = 0;

    void addRef(void)
    {
        H_AtomicAdd(&m_uiRef, 1);
    };
    void subRef(void)
    {
        H_AtomicAdd(&m_uiRef, -1);
    };
    unsigned int getRef(void)
    {
        return H_AtomicGet(&m_uiRef);
    };
    void setName(const char *pszName)
    {
        m_strName = pszName;
        m_objChan.setTaskNam(&m_strName);
    };
    std::string *getName(void)
    {
        return &m_strName;
    };

    CChan *getChan(void)
    {
        return &m_objChan;
    };

private:
    CWorkerTask(void);
    H_DISALLOWCOPY(CWorkerTask);

private:
    unsigned int m_uiRef;
    CChan m_objChan;
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
        pWorker->addRef();
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
