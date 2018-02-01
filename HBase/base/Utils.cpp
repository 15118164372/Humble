
#include "Utils.h"

#ifdef H_OS_WIN
std::string H_StrError(DWORD error)
{
    char *pError(NULL);
    if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR)&pError,
        0,
        NULL))
    {
        return "failed in translating the error.";
    }

    std::string strErr(pError);
    LocalFree(pError);

    return strErr;
}
#endif

H_BNAMSP

uint64_t CUtils::ntohl64(const uint64_t &ulVal)
{
    //大小端
    static union
    {
        char a[4];
        unsigned int ul;
    }endian = { { 'L', '?', '?', 'B' } };
    #define H_ENDIAN ((char)endian.ul) 

    if ('L' == H_ENDIAN)
    {
        uint64_t uiRet(H_INIT_NUMBER);
        unsigned long ulHigh, ulLow;

        ulLow = ulVal & 0xFFFFFFFF;
        ulHigh = (ulVal >> 32) & 0xFFFFFFFF;

        ulLow = ntohl(ulLow);
        ulHigh = ntohl(ulHigh);

        uiRet = ulLow;
        uiRet <<= 32;
        uiRet |= ulHigh;

        return uiRet;
    }

    return ulVal;
}

unsigned int CUtils::threadId(void)
{
#ifdef H_OS_WIN
    return (unsigned int)GetCurrentThreadId();
#else
    return (unsigned int)pthread_self();
#endif
}

unsigned short CUtils::coreCount(void)
{
    unsigned short usCount(1);
#ifdef H_OS_WIN
    SYSTEM_INFO stInfo;
    GetSystemInfo(&stInfo);
    usCount = (unsigned short)stInfo.dwNumberOfProcessors;
#else
    usCount = sysconf(_SC_NPROCESSORS_CONF);
#endif

    return usCount;
}

std::string CUtils::formatVa(const char *pcFormat, va_list args)
{
    if (NULL == pcFormat)
    {
        return "";
    }
    int iNum(H_INIT_NUMBER);
    unsigned int uiSize(H_ONEK / 2);
    char *pcBuff(NULL);
    std::string strRtn;

    pcBuff = new(std::nothrow) char[uiSize];
    if (NULL == pcBuff)
    {
        return strRtn;
    }
    while (true)
    {        H_Zero(pcBuff, uiSize);
        iNum = vsnprintf(pcBuff, uiSize, pcFormat, args);
        if ((iNum > -1)
            && (iNum < (int)uiSize))
        {
            strRtn = pcBuff;
            H_SafeDelArray(pcBuff);

            return strRtn;
        }
        //分配更大空间
        uiSize = (size_t)((iNum > -1) ? (iNum + 1) : uiSize * 2);
        H_SafeDelArray(pcBuff);
        pcBuff = new(std::nothrow) char[uiSize];
        if (NULL == pcBuff)
        {
            return "";
        }
    }
    H_SafeDelArray(pcBuff);

    return "";
}

std::string CUtils::formatStr(const char *pcFormat, ...)
{
    va_list va;
    std::string strVa;

    va_start(va, pcFormat);
    strVa = formatVa(pcFormat, va);
    va_end(va);

    return strVa;
}

void CUtils::Split(const std::string &strSource, const char *pszFlag,
    std::list<std::string> &lstRst)
{
    lstRst.clear();

    if ((NULL == pszFlag)
        || (0 == strlen(pszFlag))
        || strSource.empty())
    {
        return;
    }

    size_t iFlagLens(strlen(pszFlag));
    std::string::size_type loc(strSource.find(pszFlag, 0));
    if (std::string::npos == loc)
    {
        lstRst.push_back(strSource);
        return;
    }

    std::string strTmp;
    std::string strRst;

    strTmp = strSource;

    while (std::string::npos != loc)
    {
        strRst.clear();
        strRst = strTmp.substr(0, loc);
        lstRst.push_back(strRst);
        strTmp = strTmp.substr(loc + iFlagLens);

        loc = strTmp.find(pszFlag, 0);
        if (std::string::npos == loc)
        {
            strRst = strTmp;
            lstRst.push_back(strRst);
        }
    }
}

std::string &CUtils::trimLeft(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    std::string::iterator itSrc;
    for (itSrc = strSource.begin(); strSource.end() != itSrc;)
    {
        if ((' ' == *itSrc)
            || ('\n' == *itSrc)
            || ('\r' == *itSrc))
        {
            itSrc = strSource.erase(itSrc);
            continue;
        }

        break;
    }

    return strSource;
}

std::string &CUtils::trimRight(std::string &strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    size_t iFlag(H_INIT_NUMBER);
    std::string::reverse_iterator reitSrc;
    size_t iSize(strSource.size());
    for (reitSrc = strSource.rbegin(); strSource.rend() != reitSrc; reitSrc++, iFlag++)
    {
        if ((' ' != *reitSrc)
            && ('\n' != *reitSrc)
            && ('\r' != *reitSrc))
        {
            break;
        }
    }

    if (0 == iFlag)
    {
        return strSource;
    }
    if (iSize == iFlag)
    {
        strSource.clear();

        return strSource;
    }

    std::string::iterator itSrc((strSource.begin() + (iSize - iFlag)));
    while (strSource.end() != itSrc)
    {
        itSrc = strSource.erase(itSrc);
    }

    return strSource;
}

std::string CUtils::Trim(std::string strSource)
{
    return trimLeft(trimRight(strSource));
}

std::string CUtils::frontOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos(strSource.find_first_of(pszFlag));
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(0, iPos);
}

std::string CUtils::lastOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos(strSource.find_last_of(pszFlag));
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(iPos + 1, strSource.size());
}

std::string CUtils::Now(void)
{
    char acTimeStr[H_TIME_LENS] = { 0 };
    time_t t(time(NULL));

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));

    return std::string(acTimeStr);
}

std::string CUtils::nowDate(void)
{
    char acTimeStr[H_TIME_LENS] = { 0 };
    time_t t(time(NULL));

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d", localtime(&t));

    return std::string(acTimeStr);
}

struct timeval CUtils::timeOfDay(void)
{
    struct timeval tv;

#ifdef H_OS_WIN
    #define U64_LITERAL(n) n##ui64
    #define EPOCH_BIAS U64_LITERAL(116444736000000000)
    #define UNITS_PER_SEC U64_LITERAL(10000000)
    #define USEC_PER_SEC U64_LITERAL(1000000)
    #define UNITS_PER_USEC U64_LITERAL(10)
    union 
    {
        FILETIME ft_ft;
        uint64_t ft_64;
    } ft;

    GetSystemTimeAsFileTime(&ft.ft_ft);
    ft.ft_64 -= EPOCH_BIAS;
    tv.tv_sec = (long)(ft.ft_64 / UNITS_PER_SEC);
    tv.tv_usec = (long)((ft.ft_64 / UNITS_PER_USEC) % USEC_PER_SEC);
#else
    (void)gettimeofday(&tv, NULL);
#endif
    return tv;
}

uint64_t CUtils::nowMilSecond(void)
{
    uint64_t ulMS(H_INIT_NUMBER);
    struct timeval tv(timeOfDay());

    ulMS = static_cast<uint64_t>(tv.tv_usec) / 1000;//取毫秒
    ulMS += static_cast<uint64_t>(tv.tv_sec) * 1000;

    return ulMS;
}

std::string CUtils::nowStrMilSecond(void)
{
    char acTimeStr[H_TIME_LENS] = { 0 };
    struct timeval tv(timeOfDay());
    time_t t(tv.tv_sec);

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));
    size_t uiLens(strlen(acTimeStr));
    H_Snprintf(acTimeStr + uiLens, sizeof(acTimeStr) - uiLens - 1, " %d", int(tv.tv_usec / 1000));

    return acTimeStr;
}

bool CUtils::IsIpV6(const char *pszIp)
{
    std::string::size_type loc(std::string(pszIp).find(":", 0));
    if (std::string::npos == loc)
    {
        return false;
    }

    return true;
}

void CUtils::closeSock(const H_SOCK &sock)
{
#ifdef H_OS_WIN
    (void)closesocket(sock);
#else
    (void)close(sock);
#endif
}

void CUtils::shutDown(const H_SOCK &uiSock)
{
#ifdef H_OS_WIN
    (void)shutdown(uiSock, SD_BOTH);
#else
    (void)shutdown(uiSock, SHUT_RDWR);
#endif
}

int CUtils::sockDataLens(H_SOCK &fd)
{
#ifdef H_OS_WIN
    unsigned long ulNRead(H_INIT_NUMBER);
    if (ioctlsocket(fd, FIONREAD, &ulNRead) < 0)
    {
        return -1;
    }

    return (int)ulNRead;
#else
    int iNRead(H_INIT_NUMBER);
    if (ioctl(fd, FIONREAD, &iNRead) < 0)
    {
        return -1;
    }

    return iNRead;
#endif
}

int CUtils::fileExist(const char *pszFileName)
{
    return H_ACCESS(pszFileName, 0);
}

int CUtils::fileSize(const char *pszFileName, unsigned long &ulSize)
{
    int iRtn(H_RTN_OK);
    struct H_STAT stBuffer;

    ulSize = H_INIT_NUMBER;
    iRtn = H_STAT(pszFileName, &stBuffer);
    if (H_RTN_OK != iRtn)
    {
        return iRtn;
    }

    ulSize = (unsigned long)stBuffer.st_size;

    return H_RTN_OK;
}

int CUtils::dirName(const char *pszPath, std::string &strPath)
{
    if (NULL == pszPath)
    {
        return H_RTN_FAILE;
    }

    size_t iLens(strlen(pszPath));
    if (iLens < 2
        || iLens >= H_FILEPATH_LENS)
    {
        return H_RTN_FAILE;
    }

    strPath.clear();

#ifdef H_OS_WIN
    size_t iPos(H_INIT_NUMBER);

    strPath = pszPath;
    iPos = strPath.find_last_of(H_PATH_SEPARATOR);
    if (std::string::npos != iPos)
    {
        strPath = strPath.substr(0, iPos);
    }
#else
    char acTmp[H_FILEPATH_LENS] = { 0 };
    memcpy(acTmp, pszPath, iLens);
    strPath = dirname(acTmp);
#endif 

    return H_RTN_OK;
}

std::string CUtils::getProPath(void)
{
    int iSize(H_INIT_NUMBER);
    char acPath[H_FILEPATH_LENS] = { 0 };

#ifdef H_OS_WIN 
    iSize = (int)GetModuleFileName(NULL, acPath, sizeof(acPath) - 1);
#else
    iSize = readlink("/proc/self/exe", acPath, sizeof(acPath) - 1);
#endif
    if (0 >= iSize
        || sizeof(acPath) <= (size_t)iSize)
    {
        return "";
    }

    acPath[iSize] = '\0';

    std::string strPath;
    if (H_RTN_OK != dirName(acPath, strPath))
    {
        return "";
    }

    strPath += std::string(H_PATH_SEPARATOR);

    return strPath;
}

void CUtils::subDirName(const char *pszParentPathName, std::list<std::string> &lstDirName)
{
#ifdef H_OS_WIN
    WIN32_FIND_DATA fd = { 0 };
    std::string strFilePathName(pszParentPathName + std::string("\\*"));

    HANDLE hSearch(FindFirstFile(strFilePathName.c_str(), &fd));
    if (INVALID_HANDLE_VALUE == hSearch)
    {
        return;
    }

    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
    {
        if (strcmp(fd.cFileName, ".")
            && strcmp(fd.cFileName, ".."))
        {
            lstDirName.push_back(fd.cFileName);
        }
    }

    for (;;)
    {
        memset(&fd, 0, sizeof(fd));
        if (!FindNextFile(hSearch, &fd))
        {
            if (ERROR_NO_MORE_FILES == GetLastError())
            {
                break;
            }

            FindClose(hSearch);

            return;
        }

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
        {
            if (strcmp(fd.cFileName, ".")
                && strcmp(fd.cFileName, ".."))
            {
                lstDirName.push_back(fd.cFileName);
            }
        }
    }

    FindClose(hSearch);

    return;
#else
    DIR *dir;
    struct dirent *ptr;
    struct stat strFileInfo = { 0 };
    char acFullName[H_FILEPATH_LENS] = { 0 };
    dir = opendir(pszParentPathName);
    if (NULL == dir)
    {
        return;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        H_Snprintf(acFullName, sizeof(acFullName) - 1, "%s/%s", pszParentPathName, ptr->d_name);
        if (lstat(acFullName, &strFileInfo) < 0)
        {
            closedir(dir);

            return;
        }

        if (S_ISDIR(strFileInfo.st_mode))
        {
            if (strcmp(ptr->d_name, ".")
                && strcmp(ptr->d_name, ".."))
            {
                lstDirName.push_back(ptr->d_name);
            }
        }
    }

    closedir(dir);

    return;
#endif
}

#define IBS(n) (0x01<<(n-1))
int CUtils::setNTo1(int iVal, int iN)
{
    return iVal |= IBS(iN);
}

int CUtils::setNTo0(int iVal, int iN)
{
    return iVal &= ~IBS(iN);
}

long CUtils::hashStr(const char *pszStr)
{
    unsigned int uiSeed(131);
    unsigned int uiHash(H_INIT_NUMBER);
    while (*pszStr)
    {
        uiHash = uiHash * uiSeed + (*pszStr++);
    }

    return (long)(uiHash & 0x7FFFFFFF);
}

H_ENAMSP
