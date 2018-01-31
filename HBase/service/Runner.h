
#ifndef H_RUNNER_H_
#define H_RUNNER_H_

#include "Service.h"
#include "Worker.h"
#include "Clock.h"

H_BNAMSP

//����ִ�߳�
class CRunner : public CService
{
public:
    CRunner(void);
    ~CRunner(void);

    //ִ�� pAdjureΪCWorker��Ա����������ɾ��
    void onAdjure(CAdjure *pAdjure);    
    //��������
    void adjustLoad(const unsigned short &usToIndex);
    //����ִ��Worker ���ڵ��ô�
    bool addWorker(CWorker *pWorker);

    void setIndex(const unsigned short &usIndex);
    //����Runner
    void setAllRunner(CRunner *pAllRunner);
    //���߳�������
    void addWorkerNum(const int iNum);
    unsigned int getWorkerNum(void);
    //ִ�к�ʱ�ܺ�
    unsigned int getRunTime(void);
    void resetRunTime(void);
    //����ִ�к�ʱ�澯ʱ��
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
