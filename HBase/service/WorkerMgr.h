
#ifndef H_DISPATCHER_H_
#define H_DISPATCHER_H_

#include "Runner.h"
#include "Adjure_Task.h"
#include "MsgTrigger.h"

H_BNAMSP

//任务管理
class CWorkerMgr : public CObject
{
public:
    explicit CWorkerMgr(CMsgTrigger *pMsgTrigger);
    ~CWorkerMgr(void);

    //向任务发送消息
    void addAdjureToTask(CWorker *pWorker, CAdjure *pAdjure)
    {
        CMutex *pLckWorker(pWorker->getWorkerLck());

        pLckWorker->Lock();
        pWorker->addAdjure(pAdjure);
        if (!pWorker->getInGloble())
        {
            m_pAllRunner[pWorker->getIndex()].addWorker(pWorker);
            pWorker->setInGloble(true);
        }
        pLckWorker->unLock();
    };
    //获取任务
    CWorker *getWorker(const char *pszName);
    //WorkerPool
    CWorker *popPool(void);
    void pushPool(CWorker *pWorker);
    //任务注册
    void regTask(CWorker *pWorker);
    //任务删除
    void unRegTask(const char *pszName);    
    //获取所有任务名
    void getAllName(std::list<std::string> *pvcName);

    //退出
    void Stop(void);
    //usRunnerNum 工作线程数  uiAlarmTime任务执行告警时间(ms)  uiAdjustTime Runner执行调整时间差
    void Start(const unsigned short &usRunnerNum, const unsigned int &uiAlarmTime, const unsigned int &uiAdjustTime);

    //调整线程负载,根据时间间隔内执行耗时
    void adjustLoad(void);

private:
    CWorkerMgr(void);
    //注册任务时计算任务在那一工作线程
    unsigned short calIndex(const char *pszName);
    void destroyAll(void);

private:
#ifdef H_OS_WIN
    typedef std::unordered_map<std::string, CWorker*>::iterator taskit;
    typedef std::unordered_map<std::string, CWorker*> task_map;
#else
    typedef std::tr1::unordered_map<std::string, CWorker*>::iterator taskit;
    typedef std::tr1::unordered_map<std::string, CWorker*> task_map;
#endif

    unsigned short m_usRunnerNum;
    unsigned int m_uiAdjustTime;
    CRunner *m_pAllRunner;    
    CMsgTrigger *m_pMsgTrigger;
    task_map m_mapTask;
    CRWLock m_objTaskLock;
    CSafeQueue m_objWorkerPool;
};

H_ENAMSP

#endif//H_DISPATCHER_H_
