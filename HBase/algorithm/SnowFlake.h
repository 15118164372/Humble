
#ifndef H_SNOWFLAKE_H_
#define H_SNOWFLAKE_H_

#include "HObject.h"

H_BNAMSP

//SnowFlake id
class CSnowFlake : public CObject
{
public:
    CSnowFlake(void);
    ~CSnowFlake(void);

    void setWorkId(const int uiID);
    void setCenterId(const int uiID);
    uint64_t getId(void);

private:
    uint64_t tilNextMillis(void); 

private:
    int m_uiWorkid;
    int m_uiCenterid;
    long m_lSequence;
    uint64_t m_ulLastTime;
};

H_ENAMSP

#endif//H_SNOWFLAKE_H_
