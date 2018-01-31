
#ifndef H_HDES_H_
#define H_HDES_H_

#include "HObject.h"

H_BNAMSP

enum 
{
    DESTYPE_DES = 0,
    DESTYPE_D2DES,
    DESTYPE_D3DES
};

enum
{
    DES_ENCODE = 0,
    DES_DECODE
};

class CDESEncrypt : public CObject
{
public:
    CDESEncrypt(void);
    ~CDESEncrypt(void);

    //usType DESTYPE_DES...  usMode DES_ENCODE...
    void setKey(const char *pszKey, const unsigned short usType, const unsigned short usMode);
    std::string Encrypt(const char *pszData, const size_t iLens);

private:
    std::string Encode(const char *pszData, const size_t &iLens);
    std::string DEncode(const char *pszData, const size_t &iLens);
    std::string D2Encode(const char *pszData, const size_t &iLens);
    std::string D3Encode(const char *pszData, const size_t &iLens);

    std::string Decode(const char *pszData, const size_t &iLens);
    std::string DDecode(const char *pszData, const size_t &iLens);
    std::string D2Decode(const char *pszData, const size_t &iLens);
    std::string D3Decode(const char *pszData, const size_t &iLens);

private:
    unsigned short m_usMode;
    unsigned short m_usType;
    void *m_pContext;
};

H_ENAMSP

#endif//H_HDES_H_
