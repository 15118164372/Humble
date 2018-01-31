
#ifndef H_ENCRYPTUTILS_H_
#define H_ENCRYPTUTILS_H_

#include "HObject.h"

H_BNAMSP

class CEnUtils : public CObject
{
public:
    CEnUtils(void) 
    {};
    ~CEnUtils(void)
    {};

    //zib —πÀı
    static std::string zEncode(const char *pszVal, const size_t iLens);
    //zib Ω‚—π
    static std::string zDecode(const char *pszVal, const size_t iLens);

    //url
    static std::string uEncode(const char *pszVal, const size_t iLens);
    static std::string uDecode(const char *pszVal, const size_t iLens);

    //crc16
    static unsigned short crc16(const char *pszKey, const size_t iLens);
    //crc32
    static unsigned int crc32(const char *pszKey, const size_t iLens);

    //“ÏªÚ  iCount ±‡¬Î¥Œ ˝
    static std::string xorEncode(const char *pszKey, const size_t iCount, const char *pszBuf, const size_t iLens);
    static std::string xorDecode(const char *pszKey, const size_t iCount, const char *pszBuf, const size_t iLens);

    /*base64±‡¬Î*/
    static std::string b64Encode(const char *pszData, const size_t iLens);
    /*base64Ω‚¬Î*/
    static std::string b64Decode(const char *pszData, const size_t iLens);

    static std::string md5Str(const char *pszVal, const size_t iLens);
};

H_ENAMSP

#endif//H_ENCRYPTUTILS_H_
