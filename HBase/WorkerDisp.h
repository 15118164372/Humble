
#ifndef H_WORKERDISP_H_
#define H_WORKERDISP_H_

#include "Worker.h"
#include "Singleton.h"
#include "Funcs.h"

H_BNAMSP

//服务调度,服务管理
class CWorkerDisp : public CRecvTask<CWorkerTask>, public CSingleton<CWorkerDisp>
{
public:
    CWorkerDisp(void);
    ~CWorkerDisp(void);

    CChan *getChan(const char *pszTaskName);

    void setThreadNum(const unsigned short usNum);
    unsigned short getThreadNum(void)
    {
        return m_usThreadNum;
    };

    void regTask(CWorkerTask *pTask);
    void unregTask(const char *pszName);

    void runTask(CWorkerTask *pTask);
    void stopRun(void);
    void runSurplusTask(CWorkerTask *pTask);
    void destroyRun(void);
    
    void notifyInit(CWorkerTask *pTask) 
    {
        Notify(pTask, &m_uiInitCMD);
    };
    void notifyRun(CWorkerTask *pTask)
    {
        Notify(pTask, &m_uiRunCMD);
    };
    void notifyDestroy(CWorkerTask *pTask)
    {
        Notify(pTask, &m_uiDestroyCMD);
    };

private:
    CWorker *getFreeWorker(void);
    void stopNet(void);
    void stopWorker(void);
    void Notify(CWorkerTask *pTask, unsigned int *pCMD)
    {
        CCirQueue *pCMDQu = pTask->getCMDQu();
        CAtomic *pCMDLock = pTask->getCMDLock();

        pCMDLock->Lock();
        pCMDQu->Push((void*)pCMD);
        pCMDLock->unLock();

        addTask(pTask);
    };

private:
    H_DISALLOWCOPY(CWorkerDisp);
#ifdef H_OS_WIN 
    #define taskit std::unordered_map<std::string , CWorkerTask*>::iterator
    #define task_map std::unordered_map<std::string , CWorkerTask*>
#else
    #define taskit std::tr1::unordered_map<std::string, CWorkerTask*>::iterator
    #define task_map std::tr1::unordered_map<std::string, CWorkerTask*>
#endif

private:
    unsigned short m_usThreadNum;
    unsigned int m_uiInitCMD;
    unsigned int m_uiRunCMD;
    unsigned int m_uiDestroyCMD;
    CWorker *m_pWorker;
    task_map m_mapTask;
    CAtomic m_objTaskLock;
};

H_ENAMSP

#endif //H_WORKERDISP_H_
