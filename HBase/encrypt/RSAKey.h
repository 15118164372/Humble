
#ifndef H_RSAKEY_H_
#define H_RSAKEY_H_

#include "HObject.h"

H_BNAMSP

enum
{
    RSAKEY_512 = 512,
    RSAKEY_1024 = 1024
};

//RSA��Կ
class CRSAKey : public CObject
{
public:
    CRSAKey(void);
    ~CRSAKey(void);

    /*����key RSAKEY_...*/
    void creatKey(const unsigned short usKey);

    /*����R_RANDOM_STRUCT ��Կ������Ҫʹ��  base64����*/
    int saveRandom(const char *pszFile);
    /*���湫Կ*/
    int savePubKey(const char *pszFile);
    /*����˽Կ*/
    int savePriKey(const char *pszFile);

    /*��ȡKey*/
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
