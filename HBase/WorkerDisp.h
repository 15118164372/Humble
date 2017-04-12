
#ifndef H_WORKERDISP_H_
#define H_WORKERDISP_H_

#include "Worker.h"
#include "Singleton.h"
#include "Funcs.h"
#include "RWLock.h"

H_BNAMSP

//服务调度,服务管理
class CWorkerDisp : public CTask, public CSingleton<CWorkerDisp>
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

    void regTask(const char *pszName, CWorkerTask *pTask);

    void Run(void);
    void Join(void);
    void waitStart(void);

    H_INLINE void Notify(std::string *pstrName)
    {
        m_taskLck.Lock();
        m_quTask.push(pstrName);
        m_taskLck.unLock();
    };

private:
    CWorker *getFreeWorker(unsigned short &usIndex);
    CWorkerTask* getTask(std::string *pstrName);
    void stopNet(void);
    void stopWorker(void);
    void runSurpTask(void);
    void initTask(void);
    void destroyTask(void);

private:
    H_DISALLOWCOPY(CWorkerDisp);
#ifdef H_OS_WIN 
    #define taskit std::unordered_map<std::string , CWorkerTask*>::iterator
    #define task_map std::unordered_map<std::string , CWorkerTask*>
#else
    #define taskit std::tr1::unordered_map<std::string, CWorkerTask*>::iterator
    #define task_map std::tr1::unordered_map<std::string, CWorkerTask*>
#endif
    enum
    {
        RS_RUN = 0,
        RS_STOP,
    };
private:
    unsigned short m_usThreadNum;
    long m_lExit;
    long m_lCount;
    CWorker *m_pWorker;
    task_map m_mapTask;
    CAtomic m_taskLck;
    std::queue<std::string *> m_quTask;
};

H_ENAMSP

#endif //H_WORKERDISP_H_
