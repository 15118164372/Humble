
#ifndef H_URL_H_
#define H_URL_H_

#include "Macros.h"

H_BNAMSP

//url
std::string H_UEncode(const char *pszVal, const size_t iLens);
std::string H_UDecode(const char *pszVal, const size_t iLens);

H_ENAMSP

#endif//H_URL_H_
