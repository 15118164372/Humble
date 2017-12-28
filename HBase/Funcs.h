
#ifndef H_FUNCS_H_
#define H_FUNCS_H_

#include "Macros.h"

H_BNAMSP

uint64_t ntohl64(uint64_t host);

//�ַ������
std::string H_FormatVa(const char *pcFormat, va_list args);
std::string H_FormatStr(const char *pcFormat, ...);
void H_Split(const std::string &strSource, const char *pszFlag,
    std::list<std::string> &lstRst);
std::string H_TrimLeft(std::string strSource);
std::string H_TrimRight(std::string strSource);
std::string H_Trim(std::string strSource);
std::string H_GetFrontOfFlag(const std::string &strSource, const char *pszFlag);
std::string H_GetLastOfFlag(const std::string &strSource, const char *pszFlag);

//ʱ�����
std::string H_Now(void);
std::string H_Date(void);
void H_GetTimeOfDay(struct timeval &stTime);
uint64_t H_MilSecond(void);

//�ļ����
int H_FileExist(const char *pszFileName);
int H_FileSize(const char *pszFileName, unsigned long &ulSize);
int H_DirName(const char *pszPath, std::string &strPath);
std::string H_GetProPath(void);
void H_GetSubDirName(const char *pszParentPathName, std::list<std::string> &lstDirName);

//ϵͳ
unsigned int H_GetThreadID(void);
unsigned short H_GetCoreCount(void);

//socket���
void H_KeepAlive(H_SOCK &fd, const unsigned int iKeepIdle, const unsigned int iKeepInterval);
int H_SockPair(H_SOCK acSock[2]);
int H_SockWrite(H_SOCK &fd, const char *pBuf, const size_t &iLens);
int H_GetSockDataLens(H_SOCK &fd);
H_SOCK H_ClientSock(const char *pszHost, const unsigned short &usPort);

//����iVal�����Ƶ�NλΪ1 �� 0
int H_SetNTo1(int iVal, int iN);
int H_SetNTo0(int iVal, int iN);

/*ת�ַ���*/
template<typename T>
std::string H_ToString(T number)
{
    std::stringstream objStream;

    objStream << number;

    return objStream.str();
};

/*ת����*/
template<typename T>
T H_ToNumber(const char *pszNumber)
{
    T  iValue(H_INIT_NUMBER);
    std::stringstream objStream;

    objStream << pszNumber;
    objStream >> iValue;

    return iValue;
};

long H_HashStr(const char *pszStr);

template<typename T>
size_t H_HashNumber(T iVal)
{
#if defined(H_X64)
    #define OFFSET_BASIS 14695981039346656037ULL
    #define PRIME 1099511628211ULL
#else
    #define OFFSET_BASIS 2166136261U
    #define PRIME 16777619U
#endif
    size_t iHash(OFFSET_BASIS);
    unsigned char *pAddr((unsigned char *)&iVal);
    for (size_t i = 0; i < sizeof(iVal); ++i)
    {
        iHash ^= (size_t)pAddr[i];
        iHash *= PRIME;
    }

    return iHash;
};

H_ENAMSP

#endif//H_FUNCS_H_
