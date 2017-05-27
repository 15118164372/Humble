
#ifndef H_BASE64_H_
#define H_BASE64_H_

#include "Macros.h"

H_BNAMSP

/*base64±àÂë*/
std::string H_B64Encode(const unsigned char *pszData, const size_t iLens);
/*base64½âÂë*/
std::string H_B64Decode(const unsigned char *pszData, const size_t iLens);

H_ENAMSP

#endif//H_BASE64_H_
