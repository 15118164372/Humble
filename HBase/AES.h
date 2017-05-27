
#ifndef H_AES_H_
#define H_AES_H_

#include "Macros.h"

H_BNAMSP

/*aes key类型*/
enum
{
    AESKEY_128 = 128,
    AESKEY_192 = 192,
    AESKEY_256 = 256
};

//AES加解密
class CAES
{
public:
    CAES(void);
    ~CAES(void);

    /*设置密码 pszKey 密码， emKeyType 密码类型*/
    void setKey(const char *pszKey, const unsigned int uiKeyType);

    std::string Encode(const char *pszPlaint, const size_t iLens);
    std::string Decode(const char *pszCipher, const size_t iLens);

private:
    H_DISALLOWCOPY(CAES);
    enum
    {
        AES_BlockSize = 16
    };
private:
    unsigned long *m_pEncodeRK;
    unsigned long *m_pDecodeRK;  
    int m_iEncodeRounds;
    int m_iDecodeRounds;
};

H_ENAMSP

#endif//H_AES_H_
