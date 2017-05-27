
#ifndef H_CIRQUEUE_H_
#define H_CIRQUEUE_H_

#include "Macros.h"

H_BNAMSP

//Ñ­»·¶ÓÁÐ
class CCirQueue
{
public:
    CCirQueue(void) : m_iSize(H_INIT_NUMBER), m_iNext(H_INIT_NUMBER),
        m_iCapacity(H_ONEK)
    {
        m_pData = (void**)malloc(m_iCapacity * sizeof(void*));
        H_ASSERT(NULL != m_pData, "malloc memory error.");
    };
    explicit CCirQueue(const int iCapacity) : m_iSize(H_INIT_NUMBER), m_iNext(H_INIT_NUMBER),
        m_iCapacity(iCapacity)
    {
        m_pData = (void**)malloc(m_iCapacity * sizeof(void*));
        H_ASSERT(NULL != m_pData, "malloc memory error.");
    };
    ~CCirQueue(void)
    {
        H_SafeFree(m_pData);
    };

    bool Empty(void)
    {
        return H_INIT_NUMBER == m_iSize;
    };
    bool Full(void)
    {
        return m_iSize >= m_iCapacity;
    };
    size_t Size(void)
    {
        return m_iSize;
    };
    size_t Capacity(void) 
    {
        return m_iCapacity;
    };

    bool Push(void *pValue)
    {
        if (Full())
        {
            H_Printf("%s", "cirqueue is full.");
            return false;
        }

        int iPos(m_iNext + m_iSize);
        if (iPos >= m_iCapacity)
        {
            iPos -= m_iCapacity;
        }

        m_pData[iPos] = pValue;

        ++m_iSize;

        return true;
    };
    void *Pop(void)
    {
        void *pValue(NULL);

        if (m_iSize > H_INIT_NUMBER)
        {
            pValue = m_pData[m_iNext];
            ++m_iNext;
            --m_iSize;
            if (m_iNext >= m_iCapacity)
            {
                m_iNext -= m_iCapacity;
            }
        }

        return pValue;
    };
    void* Peek(void)
    {
        return (m_iSize > H_INIT_NUMBER) ? m_pData[m_iNext] : NULL;
    };

private:
    H_DISALLOWCOPY(CCirQueue);    

private:
    int m_iSize;
    int m_iNext;
    int m_iCapacity;
    void **m_pData;
};

H_ENAMSP

#endif//H_CIRQUEUE_H_
