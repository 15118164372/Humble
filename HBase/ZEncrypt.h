
#ifndef H_ZLIB_H_
#define H_ZLIB_H_

#include "Macros.h"

H_BNAMSP

//zib ѹ��
std::string H_ZEncode(const char *pszVal, const size_t iLens);
//zib ��ѹ
std::string H_ZDecode(const char *pszVal, const size_t iLens);

H_ENAMSP

#endif//H_ZLIB_H_
