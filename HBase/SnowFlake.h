
#ifndef H_SNOWFLAKE_H_
#define H_SNOWFLAKE_H_

#include "Macros.h"

H_BNAMSP

//SnowFlake id
class CSnowFlake
{
public:
    CSnowFlake(void);
    ~CSnowFlake(void);

    void setWorkid(const int uiID);
    void setCenterid(const int uiID);
    uint64_t getID(void);

private:
    H_DISALLOWCOPY(CSnowFlake);
    uint64_t getCurMS(void);
    uint64_t tilNextMillis(void); 

private:
    int m_uiWorkid;
    int m_uiCenterid;
    long m_lSequence;
    uint64_t m_ulLastTime;
};

H_ENAMSP

#endif//H_SNOWFLAKE_H_
