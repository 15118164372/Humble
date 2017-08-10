
#ifndef H_TASKGLOBLEQU_H_
#define H_TASKGLOBLEQU_H_

#include "CirQueue.h"
#include "Singleton.h"
#include "LockThis.h"
#include "Atomic.h"
#include "Clock.h"

H_BNAMSP

struct TaskQueue
{
    unsigned int uiWait;
    unsigned int uiTime;
    unsigned int uiTaskNum;
    CCirQueue objQueue;
    CClock objClock;
    pthread_mutex_t objMutex;
    pthread_cond_t objCond;

    TaskQueue(const int iCapacity) : uiWait(H_INIT_NUMBER), 
        uiTime(H_INIT_NUMBER), uiTaskNum(H_INIT_NUMBER), objQueue(iCapacity)
    {
        pthread_mutex_init(&objMutex, NULL);
        pthread_cond_init(&objCond, NULL);
    };
    ~TaskQueue(void)
    {
        pthread_cond_destroy(&objCond);
        pthread_mutex_destroy(&objMutex);
    };
};

class CTaskGlobleQu : public CSingleton<CTaskGlobleQu>
{
public:
    CTaskGlobleQu(void);
    ~CTaskGlobleQu(void);

    void setThreadNum(const unsigned short usNum);
    TaskQueue *getQueue(const unsigned short &usIndex) 
    {
        H_ASSERT(usIndex < m_usThreadNum, "out of rang.");
        return m_vcQueue[usIndex];
    };

private:
    H_DISALLOWCOPY(CTaskGlobleQu);

private:
    unsigned short m_usThreadNum;
    std::vector<TaskQueue *> m_vcQueue;
};

H_ENAMSP

#endif//H_TASKGLOBLEQU_H_
