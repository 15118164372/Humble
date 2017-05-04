
#ifndef H_FUNCS_H_
#define H_FUNCS_H_

#include "Macros.h"

H_BNAMSP

uint64_t ntohl64(uint64_t host);

//字符串相关
std::string H_FormatVa(const char *pcFormat, va_list args);
std::string H_FormatStr(const char *pcFormat, ...);
void H_Split(const std::string &strSource, const char *pszFlag,
    std::list<std::string> &lstRst);
std::string H_TrimLeft(std::string strSource);
std::string H_TrimRight(std::string strSource);
std::string H_Trim(std::string strSource);
std::string H_GetFrontOfFlag(const std::string &strSource, const char *pszFlag);
std::string H_GetLastOfFlag(const std::string &strSource, const char *pszFlag);

//时间相关
std::string H_Now(void);
std::string H_Date(void);
void H_GetTimeOfDay(struct timeval &stTime);

//文件相关
int H_FileExist(const char *pszFileName);
int H_FileSize(const char *pszFileName, unsigned long &ulSize);
int H_DirName(const char *pszPath, std::string &strPath);
std::string H_GetProPath(void);
void H_GetSubDirName(const char *pszParentPathName, std::list<std::string> &lstDirName);

//系统
unsigned int H_GetThreadID(void);
unsigned short H_GetCoreCount(void);

//socket相关
void H_KeepAlive(H_SOCK &fd, const unsigned int iKeepIdle, const unsigned int iKeepInterval);
int H_SockPair(H_SOCK acSock[2]);
bool H_SockWrite(H_SOCK &fd, const char *pBuf, const size_t &iLens);
int H_GetSockDataLens(H_SOCK &fd);
H_SOCK H_ClientSock(const char *pszHost, const unsigned short &usPort);

/*转字符串*/
template<typename T>
std::string H_ToString(T number)
{
    std::stringstream objStream;

    objStream << number;

    return objStream.str();
};

/*转数字*/
template<typename T>
T H_ToNumber(const char *pszNumber)
{
    T  iValue(H_INIT_NUMBER);
    std::stringstream objStream;

    objStream << pszNumber;
    objStream >> iValue;

    return iValue;
};

H_ENAMSP

#endif//H_FUNCS_H_
