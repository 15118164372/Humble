
#ifndef H_RSA_H_
#define H_RSA_H_

#include "RSAKey.h"

H_BNAMSP

//RSA�ӽ���
class CRSA
{
public:
    CRSA(void);
    ~CRSA(void);

    /*��Կ����,��ҪR_RANDOM_STRUCT�ṹ��*/
    void setKey(R_RSA_PUBLIC_KEY *pPubKey, R_RSA_PRIVATE_KEY *pPriKey, R_RANDOM_STRUCT *pRandom = NULL);

    /*��Կ����*/
    std::string pubEncrypt(const char* pszData, const size_t iLens);
    /*˽Կ����*/
    std::string priDecrypt(const char* pszData, const size_t iLens);

    /*˽Կ����*/
    std::string priEncrypt(const char* pszData, const size_t iLens);
    /*��Կ����*/
    std::string pubDecrypt(const char* pszData, const size_t iLens);

private:
    H_DISALLOWCOPY(CRSA);
    /*���ܷ�ʽ*/
    enum RSAEnType
    {
        EnType_Pub = 0,//��Կ����
        EnType_Pri   //˽Կ����
    };
    /*���ܷ�ʽ*/
    enum RSADeType
    {
        DeType_Pub = 0,//��Կ����
        DeType_Pri   //˽Կ����
    };

private:
    std::string RSAEncrypt(RSAEnType emEnType, const char* pszData, 
        const size_t &iDataLens);
    std::string RSADecrypt(RSADeType emEnType, const char* pszData, 
        const size_t &iDataLens);

private:
    R_RSA_PUBLIC_KEY *m_pPubKey;
    R_RSA_PRIVATE_KEY *m_pPriKey;
    R_RANDOM_STRUCT *m_pRandom;
};

H_ENAMSP

#endif//H_RSA_H_
