
#ifndef H_WORKERDISP_H_
#define H_WORKERDISP_H_

#include "Worker.h"
#include "Singleton.h"
#include "Funcs.h"

H_BNAMSP

//服务调度,服务管理
class CWorkerDisp : public CRecvTask<std::string>, public CSingleton<CWorkerDisp>
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

    void initRun(void);
    void runTask(std::string *pszTask);
    void stopRun(void);
    void runSurplusTask(std::string *pszTask);
    void destroyRun(void);

    H_INLINE void Notify(std::string *pstrName)
    {
        addTask(pstrName);
    };

private:
    CWorker *getFreeWorker(unsigned short &usIndex);
    CWorkerTask* getTask(std::string *pstrName);
    void stopNet(void);
    void stopWorker(void);

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
    CWorker *m_pWorker;
    task_map m_mapTask;
};

H_ENAMSP

#endif //H_WORKERDISP_H_
