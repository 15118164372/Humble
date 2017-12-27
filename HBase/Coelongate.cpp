
#include "Coelongate.h"

H_BNAMSP

std::string H_XorEncrypt(const char *pszKey, const size_t iCount, const char *pszBuf, const size_t iLens)
{
    H_ASSERT(strlen(pszKey) >= 4, "key lens error.");
    char cKey1 = pszKey[0];
    char cKey2 = pszKey[1];
    char cKey3 = pszKey[2];
    char cKey4 = pszKey[3];

    std::string strBuf(pszBuf, iLens);

    for (size_t i = 0; i < iCount; i++)
    {
        strBuf[0] = ((strBuf[0] + cKey2) ^ cKey3) ^ cKey4;
        for (size_t j = 1; j < iLens; j++)
        {
            strBuf[j] = (strBuf[j - 1] + strBuf[j]) ^ cKey1;
        }
    }

    return strBuf;
}

std::string H_XorDecrypt(const char *pszKey, const size_t iCount, const char *pszBuf, const size_t iLens)
{
    H_ASSERT(strlen(pszKey) >= 4, "key lens error.");
    char cKey1 = pszKey[0];
    char cKey2 = pszKey[1];
    char cKey3 = pszKey[2];
    char cKey4 = pszKey[3];

    std::string strBuf(pszBuf, iLens);

    for (size_t i = 0; i < iCount; i++)
    {
        for (size_t j = iLens - 1; j > 0; j--)
        {
            strBuf[j] = (strBuf[j] ^ cKey1) - strBuf[j - 1];
        }

        strBuf[0] = ((strBuf[0] ^ cKey4) ^ cKey3) - cKey2;
    }

    return strBuf;
}

H_ENAMSP
