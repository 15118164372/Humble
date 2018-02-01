
#ifndef H_MACROS_H_
#define H_MACROS_H_

#include "Include.h"
#include "HMemory.h"

#define H_MAJOR                      2
#define H_MINOR                      0
#define H_RELEASE                    0

#define H_FILEPATH_LENS              256        //路径最大长度
#define H_TIME_LENS                  32         //时间长度
#define H_ONEK                       1024       //1K
#define H_UUIDLENS                   64         //UUID长度
#define H_TASKNAMELENS               64

#define H_MAXRUNNUM                  10         //每个任务每次最多执行多少个请求
#define H_SOCKBUFSIZE                H_ONEK * 4

typedef unsigned short H_HEAD_TYPE;
#define H_HEAD_NTOH          ntohs

typedef unsigned short H_PROTO_TYPE;
#define H_PROTO_NTOH         ntohs

#define H_PARSER_DEFAULT      "default"
#define H_PARSER_HTTPD        "httpd"
#define H_PARSER_HTTPC        "httpc"
#define H_PARSER_DEBUG        "debug"
#define H_PARSER_RPC          "rpc"
#define H_PARSER_WS           "ws"

#define H_SERVICE_MAIL        "mailer"
#define H_SERVICE_LOG         "logger"
#define H_SERVICE_RUNNER      "runner"
#define H_SERVICE_LISTENER    "listener"
#define H_SERVICE_NETWORKER   "networker"
#define H_SERVICE_SENDER      "sender"
#define H_SERVICE_LINKER      "linker"
#define H_SERVICE_DEBUGSHOW   "debugshow"

#define H_QULENS_MAIL                H_ONEK
#define H_QULENS_LOG                 H_ONEK * 10 * 10
#define H_QULENS_RUNNER              H_ONEK * 10
#define H_QULENS_LISTENER            H_ONEK
#define H_QULENS_ADDSOCK             H_ONEK * 10 * 5
#define H_QULENS_SENDER              H_ONEK * 10 * 10
#define H_QULENS_LINKER              H_ONEK
#define H_QULENS_WORKERPOOL          H_ONEK * 10 * 5
#define H_QULENS_DEBUGSHOW           10

#define H_TIMEOUT_RPCHS              2000
#define H_TIMEOUT_WSHS               2000

#define H_SECOND                     1000 //ms

#define H_INVALID_SOCK              -1
#define H_INVALID_ID                -1
#define H_INVALID_STATUS            -1
#define H_OK_STATUS                  0
#define H_INIT_NUMBER                0

#define H_RTN_OK                     0
#define H_RTN_FAILE                  -1
#define H_RTN_TIMEDOUT               -2
#define H_RTN_ERROR                  1

#define H_ERR_MEMORY  "malloc memory error."
#define H_ERR_ADDINQU "add message into queue error."

#define H_INLINE   inline
#define H_BNAMSP   namespace Humble {
#define H_ENAMSP   }

#ifndef H_OS_WIN
    #define H_SIGNAL_EXIT    SIGRTMIN + 10
#endif

#ifdef H_OS_WIN
    #define H_SOCK intptr_t
    #define socket_t int
#else
    #define H_SOCK int
#endif

#ifdef H_OS_WIN
    #define H_PATH_SEPARATOR "\\"
#else
    #define H_PATH_SEPARATOR "/"
#endif

#ifdef H_OS_WIN
    #define IS_EAGAIN(e) (WSAEWOULDBLOCK == (e) || EAGAIN == (e))
#else
    #if EAGAIN == EWOULDBLOCK
        #define IS_EAGAIN(e) (EAGAIN == (e))
    #else
        #define IS_EAGAIN(e) (EAGAIN == (e) || EWOULDBLOCK == (e))
    #endif
#endif

#ifdef H_OS_WIN
    typedef HANDLE sem_t;
    typedef HANDLE pthread_t;
    typedef std::mutex pthread_mutex_t;
    typedef std::condition_variable pthread_cond_t;
#endif

#define H_Max(a, b) (((a) > (b)) ? (a) : (b))
#define H_Min(a, b) (((a) < (b)) ? (a) : (b))

/*清空*/
#define H_Zero(name, len) memset(name, 0, len)

#define H_CONCAT2(a, b) a b
#define H_CONCAT3(a, b, c) a b c
#define H_Printf(fmt, ...) printf(H_CONCAT3("[%s %d] ", fmt, "\n"),  __FUNCTION__, __LINE__, ##__VA_ARGS__) 

//动态变量名
#define __ANONYMOUS(type, name, line)  type  name##line
#define _ANONYMOUS(type, line)  __ANONYMOUS(type, _anonymous, line)
#define H_ANONYMOUS(type)  _ANONYMOUS(type, __LINE__)

#define H_DISALLOWCOPY(ClassName)   \
    ClassName(const ClassName&);    \
    void operator=(const ClassName&)

#define H_ASSERT(Exp, strMsg) \
if (!(Exp))\
{\
    H_Printf("%s", strMsg);\
    abort();\
}

#ifdef H_OS_WIN
    std::string H_StrError(DWORD error);
    #define H_Strcasecmp _stricmp
    #define H_Strncasecmp _strnicmp
    #define H_StrTok strtok_s
    #define H_Snprintf _snprintf
    #define H_Swprintf swprintf
    #define H_Strncpy strncpy_s
    #define H_ITOA _itoa_s
    #define H_WCSICMP(pszFst, pwsiScd) _wcsicmp(pszFst, pwsiScd)
    #define H_SockError() WSAGetLastError()
    #define H_SockError2Str(errcode) H_StrError(errcode).c_str()
    #define H_Error() GetLastError()
    #define H_Error2Str(errcode) H_StrError(errcode).c_str()
    #define H_STAT _stat
    #define H_Sleep(uiTime)  Sleep(uiTime)
    #define H_TIMEB  _timeb
    #define ftime    _ftime
    #define H_ACCESS _access
    #define H_AtomicAdd InterlockedExchangeAdd
    #define H_AtomicSet InterlockedExchange
#else
    #define H_Strcasecmp strcasecmp
    #define H_Strncasecmp strncasecmp
    #define H_StrTok strtok_r
    #define H_Snprintf snprintf
    #define H_Swprintf swprintf
    #define H_Strncpy strncpy
    #define H_ITOA itoa
    #define H_WCSICMP(pszFst, pwsiScd) \
        wmemcpy(pszFst, pwsiScd,\
        wcslen(pszFst) >= wcslen(pwsiScd) ? wcslen(pszFst) : wcslen(pwsiScd))
    #define H_SockError() (errno)
    #define H_SockError2Str(errcode) strerror(errcode)
    #define H_Error() (errno)
    #define H_Error2Str(errcode) strerror(errcode)
    #define H_STAT stat
    #define H_Sleep(uiTime) usleep(uiTime * 1000)
    #define H_TIMEB timeb
    #define H_ACCESS access
    #define H_AtomicAdd __sync_fetch_and_add
    #define H_AtomicSet __sync_lock_test_and_set
#endif

#define H_AtomicGet(ppSrc) H_AtomicAdd(ppSrc, 0)

#endif//H_MACROS_H_
