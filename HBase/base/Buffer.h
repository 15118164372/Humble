
#ifndef H_BUFFER_H_
#define H_BUFFER_H_

#include "HObject.h"

H_BNAMSP

//不拷贝 析构释放pBuf
class CBuffer : public CObject
{
public:
    CBuffer(const char *pBuf, const size_t &iLens) : m_pBuffer((char *)pBuf), m_pRealBuf(NULL), m_uiLens(iLens)
    {};
    CBuffer(void) : m_pBuffer(NULL), m_pRealBuf(NULL), m_uiLens(H_INIT_NUMBER)
    {};
    ~CBuffer(void)
    {
        if (NULL != m_pRealBuf)
        {
            H_SafeDelArray(m_pRealBuf);
            return;
        }

        H_SafeDelArray(m_pBuffer);
    };

    H_INLINE char *getBuffer(void)
    {
        return m_pBuffer;
    };
    H_INLINE const size_t &getLens(void)
    {
        return m_uiLens;
    };

protected:
    void setRealBuf(const char *pRealBuf)
    {
        m_pRealBuf = (char*)pRealBuf;
    };

    char *m_pBuffer;
    char *m_pRealBuf;
    size_t m_uiLens;
};

//拷贝
class CCopyBuffer : public CBuffer
{
public:
    CCopyBuffer(const char *pBuf, const size_t &iLens)
    {
        if (NULL == pBuf)
        {
            return;
        }

        m_pBuffer = new(std::nothrow) char[iLens];
        H_ASSERT(NULL != m_pBuffer, H_ERR_MEMORY);
        memcpy(m_pBuffer, pBuf, iLens);
        m_uiLens = iLens;
    };
    ~CCopyBuffer(void)
    {};

private:
    CCopyBuffer(void);
};

class CSkippedBuffer : public CBuffer
{
public:
    CSkippedBuffer(const char *pBuf, const size_t &iLens, const int &iSkipped) : CBuffer(pBuf, iLens)
    {
        setRealBuf(pBuf - iSkipped);
    };
    ~CSkippedBuffer(void)
    {};

private:
    CSkippedBuffer(void);
};

class CDynaBuffer : public CBuffer
{
public:
    CDynaBuffer(void) : m_pData(NULL), m_uiTotal(H_INIT_NUMBER)
    {};
    ~CDynaBuffer(void)
    {};

    H_INLINE void addLens(const size_t &iLens)
    {
        m_uiLens += iLens;
    };
    H_INLINE const size_t &getTotalLens(void)
    {
        return m_uiTotal;
    };
    void setData(void *pData)
    {
        m_pData = pData;
    };
    void *getData(void)
    {
        return m_pData;
    };

    H_INLINE const size_t getSurplusLens(void)
    {
        return m_uiTotal - m_uiLens;
    };
    H_INLINE bool Full(void)
    {
        return m_uiLens >= m_uiTotal;
    };
    H_INLINE void Reset(void)
    {
        m_pBuffer = NULL;
        m_uiLens = H_INIT_NUMBER;
        m_uiTotal = H_INIT_NUMBER;
    };
    bool Assign(const char *pBuf, const size_t &iLens)
    {
        m_uiLens = H_INIT_NUMBER;
        return Append(pBuf, iLens);
    };
    bool Append(const char *pBuf, const size_t &iLens)
    {
        if (H_INIT_NUMBER == iLens)
        {
            return true;
        }

        size_t uiNeedSize(m_uiLens + iLens);
        if (uiNeedSize > m_uiTotal)//长度不足
        {
            uiNeedSize = H_Max(m_uiTotal * 2, uiNeedSize);
            if (!New(uiNeedSize))
            {
                return false;
            }
        }

        memcpy(m_pBuffer + m_uiLens, pBuf, iLens);
        addLens(iLens);

        return true;
    };
    H_INLINE void Remove(const size_t &iLens)
    {
        if (H_INIT_NUMBER == iLens)
        {
            return;
        }

        H_ASSERT(m_uiLens >= iLens, "parsed lens error.");
        m_uiLens -= iLens;
        if (H_INIT_NUMBER == m_uiLens)
        {
            return;
        }

        (void)memmove(m_pBuffer, m_pBuffer + iLens, m_uiLens);
    };
    H_INLINE bool New(const size_t &iNewSize)
    {
        H_ASSERT(iNewSize > m_uiTotal, "param error.");
        char *pTmp = new(std::nothrow) char[iNewSize];
        if (NULL == pTmp)
        {
            return false;
        }

        if (NULL != m_pBuffer 
            && H_INIT_NUMBER != m_uiLens)
        {
            memcpy(pTmp, m_pBuffer, m_uiLens);
        }
        H_SafeDelete(m_pBuffer);

        m_pBuffer = pTmp;
        m_uiTotal = iNewSize;

        return true;
    };

private:
    void *m_pData;
    size_t m_uiTotal;//buffer总大小
};

H_ENAMSP

#endif//H_BUFFER_H_
