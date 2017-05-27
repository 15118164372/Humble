
#ifndef H_CRC_H_
#define H_CRC_H_

#include "Macros.h"

H_BNAMSP

//crc16
unsigned short H_CRC16(const char *pszKey, const size_t iLens);
//crc32
unsigned int H_CRC32(const char *pszKey, const size_t iLens);

H_ENAMSP

#endif//H_CRC_H_
