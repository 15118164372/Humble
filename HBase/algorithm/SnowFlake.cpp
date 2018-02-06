
#include "SnowFlake.h"
#include "Utils.h"

H_BNAMSP

#define Epoch 1358902800000LL
//机器标识位数
#define WorkerIdBits 5
//数据中心标识位数
#define DatacenterIdBits 5
//机器ID最大值
#define MaxWorkerId  (-1 ^ (-1 << WorkerIdBits))
//数据中心ID最大值
#define MaxDatacenterId (-1 ^ (-1 << DatacenterIdBits))
//毫秒内自增位
#define SequenceBits 12
//机器ID偏左移12位
#define WorkerIdShift SequenceBits
//数据中心ID左移17位
#define DatacenterIdShift (SequenceBits + WorkerIdBits)
//时间毫秒左移22位
#define TimestampLeftShift (SequenceBits + WorkerIdBits + DatacenterIdBits)
#define SequenceMask (-1 ^ (-1 << SequenceBits))

CSnowFlake::CSnowFlake(void) : m_uiWorkid(H_INIT_NUMBER), m_lSequence(H_INIT_NUMBER)
{
    m_ulLastTime = CUtils::nowMilSecond();
}

CSnowFlake::~CSnowFlake(void)
{
}

void CSnowFlake::setWorkId(const int uiId)
{
    H_ASSERT((uiId <= MaxWorkerId) && (uiId >= 0), "param error.");

    m_uiWorkid = uiId;
}

void CSnowFlake::setCenterId(const int uiId)
{
    H_ASSERT((uiId <= MaxDatacenterId) && (uiId >= 0), "param error.");

    m_uiCenterid = uiId;
}

uint64_t CSnowFlake::tilNextMillis(void)
{
    uint64_t ulCur(CUtils::nowMilSecond());

    while (ulCur <= m_ulLastTime) 
    {
        ulCur = CUtils::nowMilSecond();
    }

    return ulCur;
}

uint64_t CSnowFlake::getId(void)
{
    uint64_t uiCurTime(CUtils::nowMilSecond());
    H_ASSERT(uiCurTime >= m_ulLastTime, "time error.");

    if (uiCurTime == m_ulLastTime)
    {
        m_lSequence = (m_lSequence + 1) & SequenceMask;
        if (0 == m_lSequence) 
        {
            //当前毫秒内计数满了，则等待下一秒
            uiCurTime = tilNextMillis();
        }
    }
    else
    {
        m_lSequence = 0;
    }

    m_ulLastTime = uiCurTime;

    return ((uint64_t)(uiCurTime - Epoch) << TimestampLeftShift) | 
        ((uint64_t)m_uiCenterid << DatacenterIdShift) |
        ((uint64_t)m_uiWorkid << WorkerIdShift) |
        m_lSequence;
}

H_ENAMSP;
