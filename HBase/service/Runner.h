
#ifndef H_RUNNER_H_
#define H_RUNNER_H_

#include "Service.h"
#include "Worker.h"
#include "Clock.h"

H_BNAMSP

//任务执线程
class CRunner : public CService
{
public:
    CRunner(void);
    ~CRunner(void);

    //执行 pAdjure为CWorker成员变量，无需删除
    void onAdjure(CAdjure *pAdjure);    
    //调整负载
    void adjustLoad(const unsigned short &usToIndex);
    //请求执行Worker 锁在调用处
    bool addWorker(CWorker *pWorker);

    void setIndex(const unsigned short &usIndex);
    //所有Runner
    void setAllRunner(CRunner *pAllRunner);
    //该线程任务数
    void addWorkerNum(const int iNum);
    unsigned int getWorkerNum(void);
    //执行耗时总和
    unsigned int getRunTime(void);
    void resetRunTime(void);
    //单次执行耗时告警时间
    void setAlarmTime(const unsigned int &uiTime);

private:
    void logAlarm(const unsigned short &usAdjure, const double &dRunTime, CWorker *pWorker, CAdjure *pAdjure);
    CRunner *adjustLoad(CWorker *pWorker, const unsigned short &usToIndex);
    void adjustInGloble(CWorker *pWorker, CRunner *pRunner);

private:
    unsigned short m_usToIndex;
    unsigned int m_uiWorkerNum;
    unsigned int m_uiRunTime;
    unsigned int m_uiAlarmTime;
    unsigned int m_uiAdjustLoad;
    CRunner *m_pAllRunner;
    CClock m_objClock;
};

H_ENAMSP

#endif//H_RUNNER_H_
