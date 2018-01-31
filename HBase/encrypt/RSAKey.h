
#ifndef H_RSAKEY_H_
#define H_RSAKEY_H_

#include "HObject.h"

H_BNAMSP

enum
{
    RSAKEY_512 = 512,
    RSAKEY_1024 = 1024
};

//RSA秘钥
class CRSAKey : public CObject
{
public:
    CRSAKey(void);
    ~CRSAKey(void);

    /*生成key RSAKEY_...*/
    void creatKey(const unsigned short usKey);

    /*保存R_RANDOM_STRUCT 公钥加密需要使用  base64编码*/
    int saveRandom(const char *pszFile);
    /*保存公钥*/
    int savePubKey(const char *pszFile);
    /*保存私钥*/
    int savePriKey(const char *pszFile);

    /*读取Key*/
    int loadPubKey(const char *pszFile);
    int loadPriKey(const char *pszFile);
    int loadRandom(const char *pszFile);

    friend class CRSA;
private:    
    int fileWrite(const char *pszFile, const void *pVal, const size_t iLens);
    char *fileRead(const char *pszFile);

protected:
    void *m_pRandom;
    void *m_pPublicKey;    /* RSA public key */
    void *m_pPrivateKey;  /* RSA private key */
};

H_ENAMSP

#endif//H_RSAKEY_H_
