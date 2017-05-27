
#ifndef H_TIMER_H_
#define H_TIMER_H_

#include "Macros.h"

H_BNAMSP

/*
计时器
*/
class CClock
{
public:
    CClock(void);
    ~CClock(void){};

    /*重置计时器*/
    void reStart(void);
    /*返回流逝的时间(ms)*/
    double Elapsed(void);

private:
    H_DISALLOWCOPY(CClock);

private:
#ifdef H_OS_WIN
    bool m_bUseFrequency;
    LARGE_INTEGER m_StartCount;//记录开始时间     
    LARGE_INTEGER m_EndCount;//记录结束时间     
    LARGE_INTEGER m_Freq;//本机CPU时钟频率
    struct H_TIMEB m_BgTime;
    struct H_TIMEB m_EndTime;
#else
    struct timeval m_stStart;  
    struct timeval m_stEnd;
#endif    
};

H_ENAMSP

#endif//H_TIMER_H_
