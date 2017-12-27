#ifndef H_COELONGATE_H_
#define H_COELONGATE_H_

#include "Macros.h"

//等长编解码

H_BNAMSP

//异或  iCount 编码次数
std::string H_XorEncrypt(const char *pszKey, const size_t iCount, const char *pszBuf, const size_t iLens);
std::string H_XorDecrypt(const char *pszKey, const size_t iCount, const char *pszBuf, const size_t iLens);

H_ENAMSP

#endif//H_COELONGATE_H_