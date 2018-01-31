
#include "RSAKey.h"
#include "Utils.h"
#include "EncryptUtils.h"
#include "RSAEuro/rsa.h"

H_BNAMSP

CRSAKey::CRSAKey(void)
{
    m_pRandom = new(std::nothrow) R_RANDOM_STRUCT;
    H_ASSERT(NULL != m_pRandom, "malloc memory error.");

    m_pPublicKey = new(std::nothrow)R_RSA_PUBLIC_KEY;
    H_ASSERT(NULL != m_pPublicKey, "malloc memory error.");

    m_pPrivateKey = new(std::nothrow)R_RSA_PRIVATE_KEY;
    H_ASSERT(NULL != m_pPrivateKey, "malloc memory error.");
}

CRSAKey::~CRSAKey(void)
{
    R_RANDOM_STRUCT *pRandom = (R_RANDOM_STRUCT *)m_pRandom;
    H_SafeDelete(pRandom);
    m_pRandom = NULL;

    R_RSA_PUBLIC_KEY *pPubKey = (R_RSA_PUBLIC_KEY *)m_pPublicKey;
    H_SafeDelete(pPubKey);
    m_pPublicKey = NULL;

    R_RSA_PRIVATE_KEY *pPriKey = (R_RSA_PRIVATE_KEY *)m_pPrivateKey;
    H_SafeDelete(pPriKey);
    m_pPrivateKey = NULL;
}

void CRSAKey::creatKey(const unsigned short usKey)
{
    R_RSA_PROTO_KEY stProtoKey;
    stProtoKey.bits = usKey;
    stProtoKey.useFermat4 = 1;

    R_RandomCreate((R_RANDOM_STRUCT*)m_pRandom);
    H_ASSERT(H_RTN_OK == R_GeneratePEMKeys((R_RSA_PUBLIC_KEY*)m_pPublicKey, (R_RSA_PRIVATE_KEY*)m_pPrivateKey, 
        &stProtoKey, (R_RANDOM_STRUCT*)m_pRandom),
        "generate keys error.") ;
}

int CRSAKey::fileWrite(const char *pszFile, const void *pVal, const size_t iLens)
{
    FILE *pFile = fopen(pszFile, "wb");
    if (NULL == pFile)
    {
        H_Printf("open file %s error.", pszFile);
        return H_RTN_FAILE;
    }

    fwrite(pVal, 1, iLens, pFile);
    fclose(pFile);

    return H_RTN_OK;
}

int CRSAKey::saveRandom(const char *pszFile)
{   
    std::string strVal(CEnUtils::b64Encode((const char*)m_pRandom, sizeof(R_RANDOM_STRUCT)));
    return fileWrite(pszFile, strVal.c_str(), strVal.size());
}

int CRSAKey::savePubKey(const char *pszFile)
{
    std::string strVal(CEnUtils::b64Encode((const char*)m_pPublicKey, sizeof(R_RSA_PUBLIC_KEY)));
    return fileWrite(pszFile, strVal.c_str(), strVal.size());
}

int CRSAKey::savePriKey(const char *pszFile)
{
    std::string strVal(CEnUtils::b64Encode((const char*)m_pPrivateKey, sizeof(R_RSA_PRIVATE_KEY)));
    return fileWrite(pszFile, strVal.c_str(), strVal.size());
}

char *CRSAKey::fileRead(const char *pszFile)
{
    unsigned long ulFileSize(H_INIT_NUMBER);
    int iRtn(CUtils::fileSize(pszFile, ulFileSize));
    if (H_RTN_OK != iRtn)
    {
        H_Printf("get file %s size error.", pszFile);
        return NULL;
    }

    FILE *pFile = fopen(pszFile, "rb");
    if (NULL == pFile)
    {
        return NULL;
    }

    char *pBuf = new(std::nothrow) char[ulFileSize];
    H_ASSERT(NULL != pBuf, "malloc memory error.");
    fread(pBuf, 1, ulFileSize, pFile);
    fclose(pFile);

    return pBuf;
}

int CRSAKey::loadPubKey(const char *pszFile)
{
    char *pBuf(fileRead(pszFile));
    if (NULL == pBuf)
    {
        return H_RTN_FAILE;
    }

    std::string strVal(CEnUtils::b64Decode(pBuf, strlen(pBuf)));
    if (strVal.empty())
    {
        H_SafeDelete(pBuf);
        return H_RTN_FAILE;
    }
    memcpy(m_pPublicKey, strVal.c_str(), sizeof(R_RSA_PUBLIC_KEY));
    H_SafeDelete(pBuf);

    return H_RTN_OK;
}

int CRSAKey::loadPriKey(const char *pszFile)
{
    char *pBuf(fileRead(pszFile));
    if (NULL == pBuf)
    {
        return H_RTN_FAILE;
    }

    std::string strVal(CEnUtils::b64Decode(pBuf, strlen(pBuf)));
    if (strVal.empty())
    {
        H_SafeDelete(pBuf);
        return H_RTN_FAILE;
    }
    memcpy(m_pPrivateKey, strVal.c_str(), sizeof(R_RSA_PRIVATE_KEY));
    H_SafeDelete(pBuf);

    return H_RTN_OK;
}

int CRSAKey::loadRandom(const char *pszFile)
{
    char *pBuf(fileRead(pszFile));
    if (NULL == pBuf)
    {
        return H_RTN_FAILE;
    }

    std::string strVal(CEnUtils::b64Decode(pBuf, strlen(pBuf)));
    if (strVal.empty())
    {
        H_SafeDelete(pBuf);
        return H_RTN_FAILE;
    }
    memcpy(&m_pRandom, strVal.c_str(), sizeof(R_RANDOM_STRUCT));
    H_SafeDelete(pBuf);

    return H_RTN_OK;
}

H_ENAMSP
