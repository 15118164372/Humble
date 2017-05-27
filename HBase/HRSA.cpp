
#include "HRSA.h"

H_BNAMSP

CRSA::CRSA(void)
{

}

CRSA::~CRSA(void)
{

}

void CRSA::setKey(CRSAKey *pKey)
{
    memcpy(&m_stPubKey, pKey->getPubKey(), sizeof(m_stPubKey));
    memcpy(&m_stPriKey, pKey->getPriKey(), sizeof(m_stPriKey));
    memcpy(&m_stRandom, pKey->getRandom(), sizeof(m_stRandom));
}

std::string CRSA::RSAEncrypt(RSAEnType emEnType, const char* pszData, 
    const size_t &iDataLens)
{
    int iRtn(H_RTN_OK);
    unsigned int iStep(H_INIT_NUMBER);
    unsigned int iBufLens(H_INIT_NUMBER);
    unsigned int iTmpSize(H_INIT_NUMBER);
    unsigned char *pTmp = NULL;
    unsigned char acOutBuf[MAX_RSA_MODULUS_LEN];
    std::string strRtn;

    pTmp = (unsigned char*)(pszData);
    if (EnType_Pub == emEnType)
    {
        iStep = (m_stPubKey.bits + 7) / 8 - 11;
    }
    else
    {
        iStep = (m_stPriKey.bits + 7) / 8 - 11;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (unsigned int)(((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (EnType_Pub == emEnType)
        {
            iRtn = RSAPublicEncrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                &m_stPubKey, &m_stRandom);
        }
        else
        {
            iRtn = RSAPrivateEncrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                &m_stPriKey);
        }

        if (H_RTN_OK != iRtn)
        {
            H_Printf("%s", "RSAPublicEncrypt error.");

            return "";
        }

        strRtn.append((const char*)acOutBuf, iTmpSize);
    }

    return strRtn;
}

std::string CRSA::RSADecrypt(RSADeType emEnType, const char* pszData, 
    const size_t &iDataLens)
{
    int iRtn(H_RTN_OK);
    unsigned int iStep(H_INIT_NUMBER);
    unsigned int iBufLens(H_INIT_NUMBER);
    unsigned int iTmpSize(H_INIT_NUMBER);
    unsigned char *pTmp = NULL;
    unsigned char acOutBuf[MAX_RSA_MODULUS_LEN];
    std::string strRtn;

    pTmp = (unsigned char*)(pszData);
    if (DeType_Pub == emEnType)
    {
        iStep = (m_stPubKey.bits + 7) / 8;
    }
    else
    {
        iStep = (m_stPriKey.bits + 7) / 8;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (unsigned int)(((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (DeType_Pub == emEnType)
        {
            iRtn = RSAPublicDecrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                &m_stPubKey);
        }
        else
        {
            iRtn = RSAPrivateDecrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                &m_stPriKey);
        }

        if (H_RTN_OK != iRtn)
        {
            H_Printf("%s", "RSAPrivateDecrypt error.");

            return "";
        }

        strRtn.append((const char*)acOutBuf, iTmpSize);
    }

    return strRtn;
}

std::string CRSA::pubEncrypt(const char* pszData, const size_t iLens)
{
    return RSAEncrypt(EnType_Pub, pszData, iLens);
}

std::string CRSA::priDecrypt(const char* pszData, const size_t iLens)
{
    return RSADecrypt(DeType_Pri, pszData, iLens);
}

std::string CRSA::priEncrypt(const char* pszData, const size_t iLens)
{
    return RSAEncrypt(EnType_Pri, pszData, iLens);
}

std::string CRSA::pubDecrypt(const char* pszData, const size_t iLens)
{
    return RSADecrypt(DeType_Pub, pszData, iLens);
}

H_ENAMSP
