
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
    m_ulLastTime = getCurMS();
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

uint64_t CSnowFlake::getCurMS(void)
{
    uint64_t ulMS(H_INIT_NUMBER);
    struct timeval stTimeVal;

    H_GetTimeOfDay(stTimeVal);

    ulMS = static_cast<uint64_t>(stTimeVal.tv_usec) / 1000;//ȡ����
    ulMS += static_cast<uint64_t>(stTimeVal.tv_sec) * 1000;

    return ulMS;
}

uint64_t CSnowFlake::tilNextMillis(void)
{
    uint64_t ulCur(getCurMS());

    while (ulCur <= m_ulLastTime) 
    {
        ulCur = getCurMS();
    }

    return ulCur;
}

uint64_t CSnowFlake::getID(void)
{
    uint64_t uiCurTime(getCurMS());
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

    return ((uiCurTime - Epoch) << TimestampLeftShift) | 
        (m_uiCenterid << DatacenterIdShift) | 
        (m_uiWorkid << WorkerIdShift) | 
        m_lSequence;
}

H_ENAMSP;
