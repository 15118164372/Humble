
#ifndef H_UTILS_H_
#define H_UTILS_H_

#include "HObject.h"

H_BNAMSP

//一些常用函数
class CUtils : public CObject
{
public:
    CUtils(void) {};
    ~CUtils(void) {};

    static uint64_t ntohl64(const uint64_t &ulVal);
    static unsigned int threadId(void);
    static unsigned short coreCount(void);

    static std::string formatVa(const char *pcFormat, va_list args);
    static std::string formatStr(const char *pcFormat, ...);
    static void Split(const std::string &strSource, const char *pszFlag,
        std::list<std::string> &lstRst);
    static std::string &trimLeft(std::string &strSource);
    static std::string &trimRight(std::string &strSource);
    static std::string Trim(std::string strSource);
    static std::string frontOfFlag(const std::string &strSource, const char *pszFlag);
    static std::string lastOfFlag(const std::string &strSource, const char *pszFlag);

    static std::string Now(void);
    static std::string nowDate(void);
    static struct timeval timeOfDay(void);
    static uint64_t nowMilSecond(void);
    static std::string nowStrMilSecond(void);

    static bool IsIpV6(const char *pszIp);
    static void closeSock(const H_SOCK &sock);
    static void shutDown(const H_SOCK &uiSock);
    static int sockDataLens(H_SOCK &fd);

    static int fileExist(const char *pszFileName);
    static int fileSize(const char *pszFileName, unsigned long &ulSize);
    static int dirName(const char *pszPath, std::string &strPath);
    static std::string getProPath(void);
    static void subDirName(const char *pszParentPathName, std::list<std::string> &lstDirName);

    //设置iVal二进制第N位为1 或 0
    static int setNTo1(int iVal, int iN);
    static int setNTo0(int iVal, int iN);

    static long hashStr(const char *pszStr);
    template<typename T>
    static size_t hashNumber(T iVal)
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
    /*转数字*/
    template<typename T>
    static T toNumber(const char *pszNumber)
    {
        T  iValue(H_INIT_NUMBER);
        std::stringstream objStream;

        objStream << pszNumber;
        objStream >> iValue;

        return iValue;
    };
    /*转字符串*/
    template<typename T>
    static std::string toString(T number)
    {
        std::stringstream objStream;

        objStream << number;

        return objStream.str();
    };
};

H_ENAMSP

#endif//H_UTILS_H_
