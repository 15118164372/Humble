
#ifndef H_TIMEELAPSED_H_
#define H_TIMEELAPSED_H_

#include "Clock.h"
#include "Utils.h"

H_BNAMSP

class CTimeElapsed
{
public:
    CTimeElapsed(const double dAlarmTime, const char *pszFormat, ...) : m_dAlarmTime(dAlarmTime)
    {
        va_list va;
        va_start(va, pszFormat);
        m_strFormat.append(CUtils::formatVa(pszFormat, va));
        va_end(va);

        m_objClock.reStart();
    };
    ~CTimeElapsed(void)
    {
        double dRunTime(m_objClock.Elapsed());
        if (dRunTime >= m_dAlarmTime)
        {
            printf("[%s]%s ------> %f\n", CUtils::nowStrMilSecond().c_str(), m_strFormat.c_str(), dRunTime);
        }
    };

private:
    CTimeElapsed(void);
    double m_dAlarmTime;
    std::string m_strFormat;
    CClock m_objClock;
};

#define H_TIMER(dAlarmTime, pszFormat, ...) \
    H_ANONYMOUS(CTimeElapsed)(dAlarmTime, H_CONCAT2("[%s %d] ", pszFormat), __FUNCTION__, __LINE__, ##__VA_ARGS__)

H_ENAMSP

#endif//H_TIMEELAPSED_H_
