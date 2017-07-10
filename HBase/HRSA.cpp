
#include "HRSA.h"
#include "RSAEuro/rsa.h"

H_BNAMSP

CRSA::CRSA(void)
{
    m_pRandom = new(std::nothrow) R_RANDOM_STRUCT;
    H_ASSERT(NULL != m_pRandom, "malloc memory error.");

    m_pPubKey = new(std::nothrow)R_RSA_PUBLIC_KEY;
    H_ASSERT(NULL != m_pPubKey, "malloc memory error.");

    m_pPriKey = new(std::nothrow)R_RSA_PRIVATE_KEY;
    H_ASSERT(NULL != m_pPriKey, "malloc memory error.");
}

CRSA::~CRSA(void)
{   
    R_RANDOM_STRUCT *pRandom = (R_RANDOM_STRUCT *)m_pRandom;
    H_SafeDelete(pRandom);
    m_pRandom = NULL;

    R_RSA_PUBLIC_KEY *pPubKey = (R_RSA_PUBLIC_KEY *)m_pPubKey;
    H_SafeDelete(pPubKey);
    m_pPubKey = NULL;

    R_RSA_PRIVATE_KEY *pPriKey = (R_RSA_PRIVATE_KEY *)m_pPriKey;
    H_SafeDelete(pPriKey);
    m_pPriKey = NULL;
}

void CRSA::setKey(CRSAKey *pKey)
{
    memcpy(m_pRandom, pKey->m_pRandom, sizeof(R_RANDOM_STRUCT));
    memcpy(m_pPubKey, pKey->m_pPublicKey, sizeof(R_RSA_PUBLIC_KEY));
    memcpy(m_pPriKey, pKey->m_pPrivateKey, sizeof(R_RSA_PRIVATE_KEY));
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
        iStep = (((R_RSA_PUBLIC_KEY*)m_pPubKey)->bits + 7) / 8 - 11;
    }
    else
    {
        iStep = (((R_RSA_PRIVATE_KEY*)m_pPriKey)->bits + 7) / 8 - 11;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (unsigned int)(((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (EnType_Pub == emEnType)
        {
            iRtn = RSAPublicEncrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                (R_RSA_PUBLIC_KEY*)m_pPubKey, (R_RANDOM_STRUCT*)m_pRandom);
        }
        else
        {
            iRtn = RSAPrivateEncrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                (R_RSA_PRIVATE_KEY*)m_pPriKey);
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
        iStep = (((R_RSA_PUBLIC_KEY*)m_pPubKey)->bits + 7) / 8;
    }
    else
    {
        iStep = (((R_RSA_PRIVATE_KEY*)m_pPriKey)->bits + 7) / 8;
    } 

    for (size_t i = 0; i < iDataLens; i += iStep)
    {
        iBufLens = (unsigned int)(((i + iStep) > iDataLens) ? (iDataLens - i) : iStep);
        if (DeType_Pub == emEnType)
        {
            iRtn = RSAPublicDecrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                (R_RSA_PUBLIC_KEY*)m_pPubKey);
        }
        else
        {
            iRtn = RSAPrivateDecrypt(acOutBuf, &iTmpSize, pTmp + i, iBufLens,
                (R_RSA_PRIVATE_KEY*)m_pPriKey);
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
