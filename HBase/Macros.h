
#ifndef H_MACROS_H_
#define H_MACROS_H_

#include "Include.h"

#define H_FILEPATH_LENS              256        //路径最大长度
#define H_TIME_LENS                  30         //时间长度
#define H_ONEK                       1024       //1K
#define H_SERVICE_NAME_LEN           256        //windows 服务名称最大长度
#define H_UUIDLENS                   64         //UUID长度
#define H_SOCKKEEPALIVE_IDLE         60         //开始首次KeepAlive探测前的TCP空闭时间（秒）
#define H_SOCKKEEPALIVE_INTERVAL     3          //两次KeepAlive探测的时间间隔（秒）
#define H_IPLENS                     32
#define H_TASKNAMELENS               32
#define H_RPCNAMELENS                32
#define H_MAXPACK_LENS               H_ONEK * 10  //接收包最大长度

typedef unsigned short H_PROTOTYPE;
#define H_NTOH         ntohs

#define H_MAXRUNNUM                  10      
#define H_QULENS_LINKER              H_ONEK
#define H_QULENS_LOG                 H_ONEK * 10 * 5
#define H_QULENS_SENDER              H_ONEK * 10 * 10
#define H_MAXTASKNUM                 H_ONEK * 10 * 10

#define H_PARSER_HTTP                "http"
#define H_PARSER_TCP1                "tcp1"
#define H_PARSER_TCP2                "tcp2"
#define H_PARSER_WS                  "websocket"

#define H_INVALID_SOCK               -1
#define H_INVALID_ID                 -1
#define H_INIT_NUMBER                0

#define H_RTN_OK                     0
#define H_RTN_FAILE                  -1
#define H_RTN_ERROR                  1

#define H_MAJOR                      2
#define H_MINOR                      0
#define H_RELEASE                    0

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

/*数组大小*/
#define H_ArraySize(a) (sizeof(a)/sizeof(a[0]))

#define H_Max(a, b) ((a > b) ? (a) : (b))
#define H_Min(a, b) ((a < b) ? (a) : (b))

/*清空*/
#define H_Zero(name, len) memset(name, 0, len)

#define H_CONCAT2(a, b) a b
#define H_CONCAT3(a, b, c) a b c
#define H_Printf(fmt, ...) printf(H_CONCAT3("[%s %d] ", fmt, "\n"),  __FUNCTION__, __LINE__, ##__VA_ARGS__) 

#define H_DISALLOWCOPY(ClassName) \
    ClassName(const ClassName&);             \
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
    #define  H_TIMEB  _timeb
    #define  ftime    _ftime
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

#ifdef H_OS_WIN
typedef HANDLE  pthread_t;
typedef std::mutex pthread_mutex_t;
typedef std::condition_variable pthread_cond_t;

#define pthread_mutex_init(mutex, param)
#define pthread_mutex_destroy(mutex)

#define pthread_cond_init(cond, param)
#define pthread_cond_destroy(cond)
#define pthread_cond_wait(cond, mutex) (cond)->wait(mutex)
#define pthread_cond_signal(cond) (cond)->notify_one()
#define pthread_cond_broadcast(cond) (cond)->notify_all()
#endif

#define H_SafeDelete(v_para)\
do\
{\
    if (NULL != v_para)\
    {\
        delete v_para;\
        v_para = NULL;\
    }\
}while(0)

#define H_SafeDelArray(v_para)\
do\
{\
    if (NULL != v_para)\
    {\
        delete[] v_para;\
        v_para = NULL;\
    }\
}while(0)

#define H_SafeFree(v_para)\
do\
{\
    if (NULL != v_para)\
    {\
        free(v_para);\
        v_para = NULL;\
    }\
}while(0)

#endif//H_MACROS_H_
