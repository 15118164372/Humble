
#ifndef H_TIMER_H_
#define H_TIMER_H_

#include "Macros.h"

H_BNAMSP

/*
��ʱ��
*/
class CClock
{
public:
    CClock(void);
    ~CClock(void){};

    /*���ü�ʱ��*/
    void reStart(void);
    /*�������ŵ�ʱ��(ms)*/
    double Elapsed(void);

private:
    H_DISALLOWCOPY(CClock);

private:
#ifdef H_OS_WIN
    bool m_bUseFrequency;
    LARGE_INTEGER m_StartCount;//��¼��ʼʱ��     
    LARGE_INTEGER m_EndCount;//��¼����ʱ��     
    LARGE_INTEGER m_Freq;//����CPUʱ��Ƶ��
    struct H_TIMEB m_BgTime;
    struct H_TIMEB m_EndTime;
#else
    struct timeval m_stStart;  
    struct timeval m_stEnd;
#endif    
};

H_ENAMSP

#endif//H_TIMER_H_
