
#ifndef H_RSAKEY_H_
#define H_RSAKEY_H_

#include "Macros.h"
#include "RSAEuro/rsa.h"

H_BNAMSP

//RSA秘钥
class CRSAKey
{
public:
    CRSAKey(void);
    ~CRSAKey(void);

    /*生成key(长度512 - 1024)*/
    void creatKey(const unsigned short usKeyLens);

    R_RANDOM_STRUCT *getRandom(void)
    {
        return &m_stRandom;
    };
    R_RSA_PUBLIC_KEY *getPubKey(void)
    {
        return &m_stPublicKey;
    };
    R_RSA_PRIVATE_KEY *getPriKey(void)
    {
        return &m_stPrivateKey;
    };

    /*保存R_RANDOM_STRUCT 公钥加密需要使用*/
    int saveRandom(const char *pszFile);
    /*保存公钥*/
    int savePubKey(const char *pszFile);
    /*保存私钥*/
    int savePriKey(const char *pszFile);

    /*读取Key*/
    int loadPubKey(const char *pszFile);
    int loadPriKey(const char *pszFile);
    int loadRandom(const char *pszFile);

private:    
    int fileWrite(const char *pszFile, const void *pVal, const size_t iLens);
    char *fileRead(const char *pszFile);

private:
    H_DISALLOWCOPY(CRSAKey);
    enum RSAKeyLen
    {
        RSAKeyMinLen = 512,
        RSAKeyMaxLen = 1024
    };

private:
    R_RANDOM_STRUCT m_stRandom;
    R_RSA_PUBLIC_KEY m_stPublicKey;    /* RSA public key */   
    R_RSA_PRIVATE_KEY m_stPrivateKey;  /* RSA private key */
};

H_ENAMSP

#endif//H_RSAKEY_H_
