
#ifndef H_TIMER_H_
#define H_TIMER_H_

#include "Task.h"
#include "HMutex.h"
#include "PriorityQu.h"

H_BNAMSP

class CTimerTask : public CTask
{
public:
    CTimerTask(class CTimer *pTimer, class CMsgTrigger *pMsgTrigger,
        class CWorkerMgr *pWorkerMgr, class CLinker *pLinker);
    ~CTimerTask(void);

    void Run(void);
    void Stop(void);

private:
    bool m_bStop;
    unsigned int m_iRunFlage;
    class CTimer *m_pTimer;
    class CMsgTrigger *m_pMsgTrigger;
    class CWorkerMgr *m_pWorkerMgr;
    class CLinker *m_pLinker;
};

//定时器
class CTimer : public CObject
{
public:
    CTimer(class CMsgTrigger *pMsgTrigger, class CWorkerMgr *pWorkerMgr, class CLinker *pLinker);
    ~CTimer(void);

    //添加一超时
    void timeOut(class CWorker *pWorker, const uint64_t &ulTime, const uint64_t &ulId);

    CPriQuNode *getNode(const uint64_t &ulMark);
    void Start(void);
    void Stop(void);

private:
    CTimer(void);
    CTimerTask m_objTimerTask;
    CPriorityQu m_objPriorityQu;
};

H_ENAMSP

#endif//H_TIMER_H_
