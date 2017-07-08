
#include "RSAKey.h"
#include "Funcs.h"
#include "Base64.h"

H_BNAMSP

CRSAKey::CRSAKey(void)
{

}

CRSAKey::~CRSAKey(void)
{

}

void CRSAKey::creatKey(const unsigned short usKeyLens)
{
    R_RSA_PROTO_KEY stProtoKey;
    stProtoKey.bits = usKeyLens;
    stProtoKey.useFermat4 = 1;

    R_RandomCreate(&m_stRandom);
    H_ASSERT(H_RTN_OK == R_GeneratePEMKeys(&m_stPublicKey, &m_stPrivateKey, &stProtoKey, &m_stRandom), 
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
    std::string strVal = H_B64Encode((const char*)&m_stRandom, sizeof(m_stRandom));
    return fileWrite(pszFile, strVal.c_str(), strVal.size());
}

int CRSAKey::savePubKey(const char *pszFile)
{
    std::string strVal = H_B64Encode((const char*)&m_stPublicKey, sizeof(m_stPublicKey));
    return fileWrite(pszFile, strVal.c_str(), strVal.size());
}

int CRSAKey::savePriKey(const char *pszFile)
{
    std::string strVal = H_B64Encode((const char*)&m_stPrivateKey, sizeof(m_stPrivateKey));
    return fileWrite(pszFile, strVal.c_str(), strVal.size());
}

char *CRSAKey::fileRead(const char *pszFile)
{
    int iRtn(H_RTN_OK);
    unsigned long ulFileSize(H_INIT_NUMBER);
    iRtn = H_FileSize(pszFile, ulFileSize);
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
    char *pBuf = fileRead(pszFile);
    if (NULL == pBuf)
    {
        return H_RTN_FAILE;
    }

    std::string strVal = H_B64Decode(pBuf, strlen(pBuf));
    if (strVal.empty())
    {
        H_SafeDelete(pBuf);
        return H_RTN_FAILE;
    }
    memcpy(&m_stPublicKey, strVal.c_str(), sizeof(m_stPublicKey));
    H_SafeDelete(pBuf);

    return H_RTN_OK;
}

int CRSAKey::loadPriKey(const char *pszFile)
{
    char *pBuf = fileRead(pszFile);
    if (NULL == pBuf)
    {
        return H_RTN_FAILE;
    }

    std::string strVal = H_B64Decode(pBuf, strlen(pBuf));
    if (strVal.empty())
    {
        H_SafeDelete(pBuf);
        return H_RTN_FAILE;
    }
    memcpy(&m_stPrivateKey, strVal.c_str(), sizeof(m_stPrivateKey));
    H_SafeDelete(pBuf);

    return H_RTN_OK;
}

int CRSAKey::loadRandom(const char *pszFile)
{
    char *pBuf = fileRead(pszFile);
    if (NULL == pBuf)
    {
        return H_RTN_FAILE;
    }

    std::string strVal = H_B64Decode(pBuf, strlen(pBuf));
    if (strVal.empty())
    {
        H_SafeDelete(pBuf);
        return H_RTN_FAILE;
    }
    memcpy(&m_stRandom, strVal.c_str(), sizeof(m_stRandom));
    H_SafeDelete(pBuf);

    return H_RTN_OK;
}

H_ENAMSP
