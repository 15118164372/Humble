
#ifndef H_CLOCK_H_
#define H_CLOCK_H_

#include "HObject.h"

H_BNAMSP

/*
��ʱ��
*/
class CClock : public CObject
{
public:
    CClock(void) 
    {
#ifdef H_OS_WIN
        if (QueryPerformanceFrequency(&m_Freq))
        {
            m_bUseFrequency = true;
            (void)QueryPerformanceCounter(&m_StartCount);
            return;
        }
        
        m_bUseFrequency = false;
        ftime(&m_BgTime);
#else
        gettimeofday(&m_stStart, NULL);
#endif
    };
    ~CClock(void){};

    /*���ü�ʱ��*/
    void reStart(void) 
    {
#ifdef H_OS_WIN
        if (m_bUseFrequency)
        {
            (void)QueryPerformanceCounter(&m_StartCount);
            return;
        }
        
        ftime(&m_BgTime);
#else
        gettimeofday(&m_stStart, NULL);
#endif
    };
    /*�������ŵ�ʱ��(ms)*/
    double Elapsed(void)
    {
#ifdef H_OS_WIN
        if (m_bUseFrequency)
        {
            (void)QueryPerformanceCounter(&m_EndCount);
            return ((double)(m_EndCount.QuadPart - m_StartCount.QuadPart)
                / (double)m_Freq.QuadPart) * 1000.0;
        }
     
        ftime(&m_EndTime);
        return ((double)(m_EndTime.time - m_BgTime.time) * 1000.0 +
            (double)(m_EndTime.millitm - m_BgTime.millitm));
#else
        gettimeofday(&m_stEnd, NULL);
        return 1000.0 * (double)(m_stEnd.tv_sec - m_stStart.tv_sec) +
            (double)(m_stEnd.tv_usec - m_stStart.tv_usec) / 1000.0;
#endif
    };

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

#endif//H_CLOCK_H_
