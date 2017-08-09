
#ifndef H_TASKDISPATCH_H_
#define H_TASKDISPATCH_H_

#include "Singleton.h"
#include "TaskLazy.h"
#include "TaskRunner.h"
#include "RWLock.h"

H_BNAMSP

class CTaskMgr : public CTaskLazy<unsigned int>, public CSingleton<CTaskMgr>
{
public:
    CTaskMgr(void);
    ~CTaskMgr(void);

    void setDiffer(unsigned int uiDiff);
    void adjustLoad(unsigned int &uiTick);
    void runTask(unsigned int *);
    
    CChan *getChan(const char *pszTaskName);
    size_t getQueueSize(const char *pszTaskName);
    void taskRPCCall(unsigned int &uiId, const char *pszRPCName, const char *pszToTask, const char *pszSrcTask,
        const char *pMsg, const size_t &iLens);

    void setThreadNum(const unsigned short usNum);
    unsigned short getThreadNum(void)
    {
        return m_usThreadNum;
    };

    void regTask(CTaskWorker *pTask);
    void unregTask(const char *pszName);
    void stopWorker(void);

    std::vector<std::string> getAllName(void);

private:
    H_INLINE void notifyInit(CTaskWorker *pTask)
    {
        Notify(pTask, MSG_TASK_INIT);
    };
    H_INLINE void notifyDestroy(CTaskWorker *pTask)
    {
        Notify(pTask, MSG_TASK_DEL);
    };
    H_INLINE void Notify(CTaskWorker *pTask, const unsigned short usEv)
    {
        H_MSG *pMsg = new(std::nothrow) H_MSG;
        H_ASSERT(NULL != pMsg, "malloc memory error.");
        pMsg->usEnevt = usEv;

        if (!pTask->getChan()->Send(pMsg))
        {
            H_SafeDelete(pMsg);
            return;
        }
    };
    
private:
    H_DISALLOWCOPY(CTaskMgr);
#ifdef H_OS_WIN 
    #define taskit std::unordered_map<std::string , CTaskWorker*>::iterator
    #define task_map std::unordered_map<std::string , CTaskWorker*>
#else
    #define taskit std::tr1::unordered_map<std::string, CTaskWorker*>::iterator
    #define task_map std::tr1::unordered_map<std::string, CTaskWorker*>
#endif

private:
    unsigned short m_usThreadNum;
    unsigned int m_uiDiffer;
    CTaskRunner *m_pRunner;
    task_map m_mapTask;
    CRWLock m_objTaskLock;
    std::list<std::string> m_lstAllName;
    CRWLock m_objAllNamLock;
};


H_ENAMSP

#endif//H_TASKDISPATCH_H_

