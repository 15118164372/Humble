
#include "SnowFlake.h"
#include "Funcs.h"

H_BNAMSP

#define Epoch 1358902800000LL
//������ʶλ��
#define WorkerIdBits 5
//�������ı�ʶλ��
#define DatacenterIdBits 5
//����ID���ֵ
#define MaxWorkerId  (-1 ^ (-1 << WorkerIdBits))
//��������ID���ֵ
#define MaxDatacenterId (-1 ^ (-1 << DatacenterIdBits))
//����������λ
#define SequenceBits 12
//����IDƫ����12λ
#define WorkerIdShift SequenceBits
//��������ID����17λ
#define DatacenterIdShift (SequenceBits + WorkerIdBits)
//ʱ���������22λ
#define TimestampLeftShift (SequenceBits + WorkerIdBits + DatacenterIdBits)
#define SequenceMask (-1 ^ (-1 << SequenceBits))

CSnowFlake::CSnowFlake(void) : m_uiWorkid(H_INIT_NUMBER), m_lSequence(H_INIT_NUMBER)
{
    m_ulLastTime = H_MilSecond();
}

CSnowFlake::~CSnowFlake(void)
{
}

void CSnowFlake::setWorkid(const int uiID)
{
    H_ASSERT((uiID <= MaxWorkerId) && (uiID >= 0), "param error.");

    m_uiWorkid = uiID;
}

void CSnowFlake::setCenterid(const int uiID)
{
    H_ASSERT((uiID <= MaxDatacenterId) && (uiID >= 0), "param error.");

    m_uiCenterid = uiID;
}

uint64_t CSnowFlake::tilNextMillis(void)
{
    uint64_t ulCur(H_MilSecond());

    while (ulCur <= m_ulLastTime) 
    {
        ulCur = H_MilSecond();
    }

    return ulCur;
}

uint64_t CSnowFlake::getID(void)
{
    uint64_t uiCurTime(H_MilSecond());
    H_ASSERT(uiCurTime >= m_ulLastTime, "time error.");

    if (uiCurTime == m_ulLastTime)
    {
        m_lSequence = (m_lSequence + 1) & SequenceMask;
        if (0 == m_lSequence) 
        {
            //��ǰ�����ڼ������ˣ���ȴ���һ��
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
