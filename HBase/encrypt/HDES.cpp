
#include "HDES.h"
extern "C" {
    #include "d3des/d3des.h"
}

H_BNAMSP

/*
最后一块的剩余字节中全部填上多余的字节数，比如最后一块大小为8字节，5字节有效，
那在最后三字节填充3(8-5),表示有3个字节无效，这样解密后减去这部分就好了。
如果原始数据长度刚好是8的整数倍，则多加密一个block,内容为8,
表示该block全部为无效数据。
*/

enum
{
    DES_BlockSize = 8,
    D2ES_BlockSize = 16,
    D3ES_BlockSize = 24,
};

CDESEncrypt::CDESEncrypt(void)
{
    m_pContext = getcontext();
    H_ASSERT(NULL != m_pContext, "malloc memory error.");
}

CDESEncrypt::~CDESEncrypt()
{
    if (NULL != m_pContext)
    {
        freecontext((DESContext *)m_pContext);
        m_pContext = NULL;
    }
}

void CDESEncrypt::setKey(const char *pszKey, const unsigned short usType, const unsigned short usMode)
{
    m_usMode = usMode;
    m_usType = usType;
    switch (m_usType)
    {
        case DESTYPE_DES:
            deskey((DESContext *)m_pContext, (unsigned char*)pszKey, usMode);
            break;
        case DESTYPE_D2DES:
            des2key((DESContext *)m_pContext, (unsigned char*)pszKey, usMode);
            break;
        case DESTYPE_D3DES:
            des3key((DESContext *)m_pContext, (unsigned char*)pszKey, usMode);
            break;
    }
}

std::string CDESEncrypt::DEncode(const char *pszData, const size_t &iLens)
{
    size_t iCopyLen(H_INIT_NUMBER);
    bool bFill(false);
    unsigned char uacPlain[DES_BlockSize];
    unsigned char uacCipher[DES_BlockSize];
    std::string strRtn;

    if (0 == iLens % DES_BlockSize)
    {
        bFill = true;
    }

    unsigned char *pPlain;
    for (size_t i = 0; i < iLens; i += DES_BlockSize)
    {
        iCopyLen = ((iLens - i) >= DES_BlockSize) ? (DES_BlockSize) : (iLens - i);
        if (iCopyLen < DES_BlockSize)//不足一块的以差的字节数填充
        {
            memcpy(uacPlain, pszData + i, iCopyLen);
            memset(uacPlain + iCopyLen, (int)(DES_BlockSize - iCopyLen), DES_BlockSize - iCopyLen);
            pPlain = uacPlain;
        }
        else
        {
            pPlain = (unsigned char*)(pszData + i);
        }
        des((DESContext *)m_pContext, pPlain, uacCipher);

        strRtn.append((const char*)uacCipher, DES_BlockSize);

    }

    if (bFill)
    {
        memset(uacPlain, DES_BlockSize, DES_BlockSize);
        des((DESContext *)m_pContext, uacPlain, uacCipher);
        strRtn.append((const char*)uacCipher, DES_BlockSize);
    }

    return strRtn;
}

std::string CDESEncrypt::D2Encode(const char *pszData, const size_t &iLens)
{
    size_t iCopyLen(H_INIT_NUMBER);
    bool bFill(false);
    unsigned char uacPlain[D2ES_BlockSize];
    unsigned char uacCipher[D2ES_BlockSize];
    std::string strRtn;

    if (0 == iLens % D2ES_BlockSize)
    {
        bFill = true;
    }

    unsigned char *pPlain;
    for (size_t i = 0; i < iLens; i += D2ES_BlockSize)
    {
        iCopyLen = ((iLens - i) >= D2ES_BlockSize) ? (D2ES_BlockSize) : (iLens - i);
        if (iCopyLen < D2ES_BlockSize)//不足一块的以差的字节数填充
        {
            memcpy(uacPlain, pszData + i, iCopyLen);
            memset(uacPlain + iCopyLen, (int)(D2ES_BlockSize - iCopyLen), D2ES_BlockSize - iCopyLen);
            pPlain = uacPlain;
        }
        else
        {
            pPlain = (unsigned char*)(pszData + i);
        }
        D2des((DESContext *)m_pContext, pPlain, uacCipher);

        strRtn.append((const char*)uacCipher, D2ES_BlockSize);

    }

    if (bFill)
    {
        memset(uacPlain, D2ES_BlockSize, D2ES_BlockSize);
        D2des((DESContext *)m_pContext, uacPlain, uacCipher);
        strRtn.append((const char*)uacCipher, D2ES_BlockSize);
    }

    return strRtn;
}

std::string CDESEncrypt::D3Encode(const char *pszData, const size_t &iLens)
{
    size_t iCopyLen(H_INIT_NUMBER);
    bool bFill(false);
    unsigned char uacPlain[D3ES_BlockSize];
    unsigned char uacCipher[D3ES_BlockSize];
    std::string strRtn;

    if (0 == iLens % D3ES_BlockSize)
    {
        bFill = true;
    }

    unsigned char *pPlain;
    for (size_t i = 0; i < iLens; i += D3ES_BlockSize)
    {
        iCopyLen = ((iLens - i) >= D3ES_BlockSize) ? (D3ES_BlockSize) : (iLens - i);
        if (iCopyLen < D3ES_BlockSize)//不足一块的以差的字节数填充
        {
            memcpy(uacPlain, pszData + i, iCopyLen);
            memset(uacPlain + iCopyLen, (int)(D3ES_BlockSize - iCopyLen), D3ES_BlockSize - iCopyLen);
            pPlain = uacPlain;
        }
        else
        {
            pPlain = (unsigned char*)(pszData + i);
        }
        D3des((DESContext *)m_pContext, pPlain, uacCipher);

        strRtn.append((const char*)uacCipher, D3ES_BlockSize);

    }

    if (bFill)
    {
        memset(uacPlain, D3ES_BlockSize, D3ES_BlockSize);
        D3des((DESContext *)m_pContext, uacPlain, uacCipher);
        strRtn.append((const char*)uacCipher, D3ES_BlockSize);
    }

    return strRtn;
}

std::string CDESEncrypt::Encode(const char *pszData, const size_t &iLens)
{
    switch (m_usType)
    {
        case DESTYPE_DES:
            return DEncode(pszData, iLens);
        case DESTYPE_D2DES:
            return D2Encode(pszData, iLens);
        case DESTYPE_D3DES:
            return D3Encode(pszData, iLens);
    }

    return "";
}

std::string CDESEncrypt::DDecode(const char *pszData, const size_t &iLens)
{
    unsigned char m_uacPlain[DES_BlockSize] = {0};
    std::string strRtn;
    if (0 != (iLens % DES_BlockSize))
    {
        return "";
    }

    for (size_t i = 0; i < iLens; i += DES_BlockSize)
    {
        des((DESContext *)m_pContext, (unsigned char*)(pszData + i), m_uacPlain);
        strRtn.append((const char*)m_uacPlain, DES_BlockSize);
    }

    strRtn.erase(strRtn.size() - (size_t)m_uacPlain[DES_BlockSize - 1], strRtn.size());

    return strRtn;
}

std::string CDESEncrypt::D2Decode(const char *pszData, const size_t &iLens)
{
    unsigned char m_uacPlain[D2ES_BlockSize] = {0};
    std::string strRtn;
    if (0 != (iLens % D2ES_BlockSize))
    {
        return "";
    }

    for (size_t i = 0; i < iLens; i += D2ES_BlockSize)
    {
        D2des((DESContext *)m_pContext, (unsigned char*)(pszData + i), m_uacPlain);
        strRtn.append((const char*)m_uacPlain, D2ES_BlockSize);
    }

    strRtn.erase(strRtn.size() - (size_t)m_uacPlain[D2ES_BlockSize - 1], strRtn.size());

    return strRtn;
}

std::string CDESEncrypt::D3Decode(const char *pszData, const size_t &iLens)
{
    unsigned char m_uacPlain[D3ES_BlockSize] = {0};
    std::string strRtn;
    if (0 != (iLens % D3ES_BlockSize))
    {
        return "";
    }

    for (size_t i = 0; i < iLens; i += D3ES_BlockSize)
    {
        D3des((DESContext *)m_pContext, (unsigned char*)(pszData + i), m_uacPlain);
        strRtn.append((const char*)m_uacPlain, D3ES_BlockSize);
    }

    strRtn.erase(strRtn.size() - (size_t)m_uacPlain[D3ES_BlockSize - 1], strRtn.size());

    return strRtn;
}

std::string CDESEncrypt::Decode(const char *pszData, const size_t &iLens)
{
    switch (m_usType)
    {
        case DESTYPE_DES:
            return DDecode(pszData, iLens);
        case DESTYPE_D2DES:
            return D2Decode(pszData, iLens);
        case DESTYPE_D3DES:
            return D3Decode(pszData, iLens);
    }

    return "";
}

std::string CDESEncrypt::Encrypt(const char *pszData, const size_t iLens)
{
    switch (m_usMode)
    {
        case DES_ENCODE:
            return Encode(pszData, iLens);
        case DES_DECODE:
            return Decode(pszData, iLens);
    }

    return "";
}

H_ENAMSP
