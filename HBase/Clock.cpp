
#include "Clock.h"

H_BNAMSP

CClock::CClock(void)
{
#ifdef H_OS_WIN
    if (QueryPerformanceFrequency(&m_Freq))
    {
        m_bUseFrequency = true;
        (void)QueryPerformanceCounter(&m_StartCount);
    }
    else
    {
        m_bUseFrequency = false;
        ftime(&m_BgTime);
    }
#else
    gettimeofday(&m_stStart, NULL);
#endif
}

/************************************************************************
* Function name:reStart
* Description  :重置计时器
* IN           :NONE
* OUT          :NONE
* Return       :NONE
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
void CClock::reStart(void)
{
#ifdef H_OS_WIN
    if (m_bUseFrequency)
    {
        (void)QueryPerformanceCounter(&m_StartCount);
    }
    else
    {
        ftime(&m_BgTime);
    }
#else
    gettimeofday(&m_stStart, NULL);
#endif
}

/************************************************************************
* Function name:Elapsed
* Description  :返回流逝的时间(ms)
* IN           :NONE
* OUT          :NONE
* Return       :double
* Make By      :lqf/200309129@163.com
* Date Time    :2014/04/30
* Modification 
* ......record :first program
************************************************************************/
double CClock::Elapsed(void)
{
#ifdef H_OS_WIN
    if (m_bUseFrequency)
    {
        (void)QueryPerformanceCounter(&m_EndCount);

        return ((double)(m_EndCount.QuadPart - m_StartCount.QuadPart) 
            / (double)m_Freq.QuadPart) * 1000.0;
    }
    else
    {
        ftime(&m_EndTime);

        return ((double)(m_EndTime.time - m_BgTime.time) * 1000.0 + 
            (double)(m_EndTime.millitm - m_BgTime.millitm));
    }
#else
    gettimeofday(&m_stEnd, NULL);

    return 1000.0 * (double)(m_stEnd.tv_sec - m_stStart.tv_sec) + 
        (double)(m_stEnd.tv_usec - m_stStart.tv_usec) / 1000.0;
#endif
}

H_ENAMSP
