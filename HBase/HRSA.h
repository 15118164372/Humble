
#ifndef H_RSA_H_
#define H_RSA_H_

#include "RSAKey.h"

H_BNAMSP

//RSA加解密
class CRSA
{
public:
    CRSA(void);
    ~CRSA(void);

    /*公钥加密,需要R_RANDOM_STRUCT结构体*/
    void setKey(CRSAKey *pKey);

    /*公钥加密*/
    std::string pubEncrypt(const char* pszData, const size_t iLens);
    /*私钥解密*/
    std::string priDecrypt(const char* pszData, const size_t iLens);

    /*私钥加密*/
    std::string priEncrypt(const char* pszData, const size_t iLens);
    /*公钥解密*/
    std::string pubDecrypt(const char* pszData, const size_t iLens);

private:
    H_DISALLOWCOPY(CRSA);
    /*加密方式*/
    enum RSAEnType
    {
        EnType_Pub = 0,//公钥加密
        EnType_Pri   //私钥加密
    };
    /*解密方式*/
    enum RSADeType
    {
        DeType_Pub = 0,//公钥解密
        DeType_Pri   //私钥解密
    };

private:
    std::string RSAEncrypt(RSAEnType emEnType, const char* pszData, 
        const size_t &iDataLens);
    std::string RSADecrypt(RSADeType emEnType, const char* pszData, 
        const size_t &iDataLens);

private:
    R_RSA_PUBLIC_KEY m_stPubKey;
    R_RSA_PRIVATE_KEY m_stPriKey;
    R_RANDOM_STRUCT m_stRandom;
};

H_ENAMSP

#endif//H_RSA_H_
