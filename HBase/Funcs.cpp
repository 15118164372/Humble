
#include "Funcs.h"
#include "NETAddr.h"
#include "event2/util.h"

H_BNAMSP

#define H_TCPKEEPCOUNT    5

#ifdef H_OS_WIN
    #define IS_EAGAIN(e) (WSAEWOULDBLOCK == (e) || EAGAIN == (e))
#else
    #if EAGAIN == EWOULDBLOCK
        #define IS_EAGAIN(e) (EAGAIN == (e))
    #else
        #define IS_EAGAIN(e) (EAGAIN == (e) || EWOULDBLOCK == (e))
    #endif
#endif

//大小端
static union  
{
    char a[4];
    unsigned int ul;
}endian = {{'L', '?', '?', 'B'}}; 
#define Q_ENDIAN ((char)endian.ul) 

uint64_t ntohl64(uint64_t host)
{
    if ('L' == Q_ENDIAN)
    {
        uint64_t uiRet(H_INIT_NUMBER);
        unsigned long ulHigh, ulLow;

        ulLow = host & 0xFFFFFFFF;
        ulHigh = (host >> 32) & 0xFFFFFFFF;

        ulLow = ntohl(ulLow); 
        ulHigh = ntohl(ulHigh);

        uiRet = ulLow;
        uiRet <<= 32;
        uiRet |= ulHigh;

        return uiRet;
    }
    else
    {
        return host;
    }
}

std::string H_FormatVa(const char *pcFormat, va_list args)
{
    if (NULL == pcFormat)
    {
        return "";
    }
    int iNum(H_INIT_NUMBER);
    unsigned int uiSize(H_ONEK / 2);
    char *pcBuff = NULL;
    std::string strRtn;

    pcBuff = new(std::nothrow) char[uiSize];
    if (NULL == pcBuff)
    {
        return strRtn;
    }
    while(true)
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
        uiSize = (size_t)((iNum > -1) ? (iNum + 1) : uiSize*2);
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

std::string H_FormatStr(const char *pcFormat, ...)
{
    if (NULL == pcFormat)
    {
        return "";
    }

    va_list va;
    std::string strVa;

    va_start(va, pcFormat);
    strVa = H_FormatVa(pcFormat, va);
    va_end(va);

    return strVa;
}

std::string H_Now(void)
{
    char acTimeStr[H_TIME_LENS] = {0};
    time_t t(time(NULL));	

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));

    return std::string(acTimeStr);
}

std::string H_Date(void)
{
    char acTimeStr[H_TIME_LENS] = { 0 };
    time_t t(time(NULL));

    strftime(acTimeStr, sizeof(acTimeStr) - 1, "%Y-%m-%d", localtime(&t));

    return std::string(acTimeStr);
}

int H_FileExist(const char *pszFileName)
{
    H_ASSERT(NULL != pszFileName, "pointer is null.");

    return H_ACCESS(pszFileName, 0);
}

int H_FileSize(const char *pszFileName, unsigned long &ulSize)
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

unsigned int H_GetThreadID(void)
{
#ifdef H_OS_WIN
    return (unsigned int)GetCurrentThreadId();
#else
    return (unsigned int)pthread_self();
#endif
}

unsigned short H_GetCoreCount(void)
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

int H_DirName(const char *pszPath, std::string &strPath)
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

std::string H_GetProPath(void)
{
    int iSize(H_INIT_NUMBER);
    char acPath[H_FILEPATH_LENS] = {0};

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
    if (H_RTN_OK != H_DirName(acPath, strPath))
    {
        return "";
    }

    strPath += std::string(H_PATH_SEPARATOR);

    return strPath;
}

void H_GetSubDirName(const char *pszParentPathName, std::list<std::string> &lstDirName)
{
#ifdef H_OS_WIN
    WIN32_FIND_DATA fd = { 0 };
    HANDLE hSearch;
    std::string strFilePathName;
    strFilePathName = pszParentPathName + std::string("\\*");

    hSearch = FindFirstFile(strFilePathName.c_str(), &fd);
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

void H_Split(const std::string &strSource, const char *pszFlag,
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
    std::string::size_type loc;
    loc = strSource.find(pszFlag, 0);
    if(std::string::npos == loc)
    {
        lstRst.push_back(strSource);

        return;
    }

    std::string strTmp;
    std::string strRst;

    strTmp = strSource;

    while(std::string::npos != loc)
    {
        strRst.clear();
        strRst = strTmp.substr(0, loc);
        lstRst.push_back(strRst);
        strTmp = strTmp.substr(loc + iFlagLens);

        loc = strTmp.find(pszFlag, 0);
        if(std::string::npos == loc)
        {
            strRst = strTmp;
            lstRst.push_back(strRst);
        }
    }

    return;
}

std::string H_TrimLeft(std::string strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    std::string::iterator itSrc;
    for (itSrc = strSource.begin(); strSource.end() != itSrc;)
    {
        if((' ' == *itSrc)
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

std::string H_TrimRight(std::string strSource)
{
    if (strSource.empty())
    {
        return strSource;
    }

    size_t iFlag(H_INIT_NUMBER);
    size_t iSize(H_INIT_NUMBER);
    std::string::reverse_iterator reitSrc;

    iSize = strSource.size();

    for (reitSrc = strSource.rbegin(); strSource.rend() != reitSrc; reitSrc++, iFlag++)
    {
        if((' ' != *reitSrc)
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

    std::string::iterator itSrc;
    itSrc = (strSource.begin() + (iSize - iFlag));
    while(strSource.end() != itSrc)
    {
        itSrc = strSource.erase(itSrc);
    }

    return strSource;
}

std::string H_Trim(std::string strSource)
{
    return H_TrimLeft(H_TrimRight(strSource));
}

std::string H_GetFrontOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = H_INIT_NUMBER;
    iPos = strSource.find_first_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(0, iPos);
}

std::string H_GetLastOfFlag(const std::string &strSource, const char *pszFlag)
{
    std::string::size_type iPos = H_INIT_NUMBER;
    iPos = strSource.find_last_of(pszFlag);
    if (std::string::npos == iPos)
    {
        return strSource;
    }

    return strSource.substr(iPos + strlen(pszFlag), strSource.size());
}

void H_GetTimeOfDay(struct timeval &stTime)
{
    evutil_timerclear(&stTime);
    (void)evutil_gettimeofday(&stTime, NULL);
}

void H_KeepAlive(H_SOCK &fd, const unsigned int iKeepIdle, const unsigned int iKeepInterval)
{
    int iKeepAlive = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive)) < 0)
    {
        H_Printf("%s", "setsockopt SO_KEEPALIVE error!");
    }

#ifdef H_OS_WIN
    unsigned long ulBytesReturn = H_INIT_NUMBER;
    tcp_keepalive stAliveIn = { 0 };
    tcp_keepalive stAliveOut = { 0 };

    stAliveIn.onoff = 1;
    stAliveIn.keepalivetime = iKeepIdle * 1000;
    stAliveIn.keepaliveinterval = iKeepInterval * 1000;
    if (SOCKET_ERROR == WSAIoctl(fd, SIO_KEEPALIVE_VALS, &stAliveIn, sizeof(stAliveIn),
        &stAliveOut, sizeof(stAliveOut), &ulBytesReturn, NULL, NULL))
    {
        H_Printf("%s", "WSAIoctl set error!");
    }
#else 
    int ikeepCount = H_TCPKEEPCOUNT;//判定断开前的KeepAlive探测次数

    if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void *)&iKeepIdle, sizeof(iKeepIdle)) < 0)
    {
        H_Printf("%s", "setsockopt TCP_KEEPIDLE error!");
    }
    if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&iKeepInterval, sizeof(iKeepInterval)) < 0)
    {
        H_Printf("%s", "setsockopt TCP_KEEPINTVL error!");
    }
    if (setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&ikeepCount, sizeof(ikeepCount)) < 0)
    {
        H_Printf("%s", "setsockopt TCP_KEEPCNT error!");
    }
#endif
}

static int creatListener(H_SOCK &fdListener)
{
    CNETAddr objListen_addr;
    int iRtn = H_RTN_OK;

    iRtn = objListen_addr.setAddr("127.0.0.1", 0);
    if (H_RTN_OK != iRtn)
    {
        H_Printf("set socket addr error. error code %d.", iRtn);

        return iRtn;
    }

    fdListener = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == fdListener)
    {
        iRtn = H_SockError();
        H_Printf("create socket error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));

        return H_RTN_FAILE;
    }

    if (H_RTN_FAILE == bind(fdListener, objListen_addr.getAddr(), (int)objListen_addr.getAddrSize()))
    {
        iRtn = H_SockError();
        H_Printf("bind port error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));

        evutil_closesocket(fdListener);

        return H_RTN_FAILE;
    }

    if (H_RTN_FAILE == listen(fdListener, 1))
    {
        iRtn = H_SockError();
        H_Printf("listen error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));

        evutil_closesocket(fdListener);

        return H_RTN_FAILE;
    }

    return H_RTN_OK;
}
int H_SockPair(H_SOCK acSock[2])
{
    H_SOCK fdListener = H_INVALID_SOCK;
    H_SOCK fdConnector = H_INVALID_SOCK;
    H_SOCK fdAcceptor = H_INVALID_SOCK;
    ev_socklen_t iSize = H_INIT_NUMBER;
    struct sockaddr_in connect_addr;
    struct sockaddr_in listen_addr;
    int iKeepAlive = 1;
    int iRtn = H_RTN_OK;

    if (H_RTN_OK != creatListener(fdListener))
    {
        return H_RTN_FAILE;
    }

    fdConnector = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == fdConnector)
    {
        iRtn = H_SockError();
        H_Printf("create socket error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));
        evutil_closesocket(fdListener);

        return H_RTN_FAILE;
    }

    iSize = sizeof(connect_addr);
    if (H_RTN_FAILE == getsockname(fdListener, (struct sockaddr *) &connect_addr, &iSize))
    {
        iRtn = H_SockError();
        H_Printf("getsockname error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));

        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);

        return H_RTN_FAILE;
    }

    if (iSize != sizeof(connect_addr))
    {
        H_Printf("%s", "addr size not equ.");

        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);

        return H_RTN_FAILE;
    }

    if (H_RTN_FAILE == connect(fdConnector, (struct sockaddr *) &connect_addr, sizeof(connect_addr)))
    {
        iRtn = H_SockError();
        H_Printf("connect error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));

        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);

        return H_RTN_FAILE;
    }

    iSize = sizeof(listen_addr);
    fdAcceptor = accept(fdListener, (struct sockaddr *) &listen_addr, &iSize);
    if (H_INVALID_SOCK == fdAcceptor)
    {
        iRtn = H_SockError();
        H_Printf("accept error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));

        evutil_closesocket(fdListener);
        evutil_closesocket(fdConnector);

        return H_RTN_FAILE;
    }

    evutil_closesocket(fdListener);

    if (H_RTN_FAILE == getsockname(fdConnector, (struct sockaddr *) &connect_addr, &iSize))
    {
        iRtn = H_SockError();
        H_Printf("getsockname error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));

        evutil_closesocket(fdAcceptor);
        evutil_closesocket(fdConnector);

        return H_RTN_FAILE;
    }

    if (iSize != sizeof(connect_addr)
        || listen_addr.sin_family != connect_addr.sin_family
        || listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
        || listen_addr.sin_port != connect_addr.sin_port)
    {
        H_Printf("%s", "addr not equ.");

        evutil_closesocket(fdAcceptor);
        evutil_closesocket(fdConnector);

        return H_RTN_FAILE;
    }

    int iFlag = 1;
    if (setsockopt(fdConnector, IPPROTO_TCP, TCP_NODELAY, (char *)&iFlag, sizeof(iFlag)) < 0)
    {
        H_Printf("%s", "setsockopt TCP_NODELAY error!");
    }
    if (setsockopt(fdAcceptor, IPPROTO_TCP, TCP_NODELAY, (char *)&iFlag, sizeof(iFlag)) < 0)
    {
        H_Printf("%s", "setsockopt TCP_NODELAY error!");
    }
    if (setsockopt(fdConnector, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive)))
    {
        H_Printf("%s", "setsockopt SO_KEEPALIVE error!");
    }
    if (setsockopt(fdAcceptor, SOL_SOCKET, SO_KEEPALIVE, (char *)&iKeepAlive, (int)sizeof(iKeepAlive)))
    {
        H_Printf("%s", "setsockopt SO_KEEPALIVE error!");
    }
    (void)evutil_make_socket_nonblocking(fdAcceptor);
    (void)evutil_make_socket_nonblocking(fdConnector);

    acSock[0] = fdAcceptor;
    acSock[1] = fdConnector;

    return H_RTN_OK;
}

static bool selectWrite(H_SOCK &fd)
{
    int iRtn(H_RTN_OK);
    struct timeval stTime;    
    fd_set fdWrite;

#ifdef H_OS_WIN
#pragma warning(disable:4244)
#endif // H_OS_WIN
    FD_ZERO(&fdWrite);
    FD_SET(fd, &fdWrite);
    stTime.tv_sec = 3;
    stTime.tv_usec = 0;

    iRtn = select(fd + 1, NULL, &fdWrite, NULL, &stTime);
    if (iRtn <= 0)
    {
        return false;
    }

    return ((H_INIT_NUMBER == FD_ISSET(fd, &fdWrite)) ? false : true);
}

bool H_SockWrite(H_SOCK &fd, const char *pBuf, const size_t &iLens)
{
    int iSendSize(H_INIT_NUMBER);
    size_t iSendTotalSize(H_INIT_NUMBER);

    do
    {
        iSendSize = send(fd, pBuf + iSendTotalSize, (int)(iLens - iSendTotalSize), 0);
        if (iSendSize <= 0)
        {
            int iRtn = H_SockError();
            if (IS_EAGAIN(iRtn))
            {
                if (selectWrite(fd))
                {
                    continue;
                }
            }

            H_Printf("send error. error code %d, message %s ", iRtn, H_SockError2Str(iRtn));
            return false;
        }

        iSendTotalSize += (size_t)iSendSize;

    } while (iLens > iSendTotalSize);

    return true;
}

int H_GetSockDataLens(H_SOCK &fd)
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

H_SOCK H_ClientSock(const char *pszHost, const unsigned short &usPort)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pszHost, usPort))
    {
        H_Printf("%s", "setAddr error.");
        return H_INVALID_SOCK;
    }

    //创建socket
    H_SOCK sock = socket(AF_INET, SOCK_STREAM, 0);
    if (H_INVALID_SOCK == sock)
    {
        H_Printf("%s", "creat socket error.");
        return H_INVALID_SOCK;
    }

    if (0 != connect(sock, objAddr.getAddr(), (int)objAddr.getAddrSize()))
    {
        H_Printf("connect %s on port %d error.", pszHost, usPort);
        evutil_closesocket(sock);

        return H_INVALID_SOCK;
    }

    return sock;
}

H_ENAMSP
