
#ifndef H_BASE64_H_
#define H_BASE64_H_

#include "Macros.h"

H_BNAMSP

/*base64����*/
std::string H_B64Encode(const char *pszData, const size_t iLens);
/*base64����*/
std::string H_B64Decode(const char *pszData, const size_t iLens);

H_ENAMSP

#endif//H_BASE64_H_
