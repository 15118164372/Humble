
#ifndef H_CONFIG_H_
#define H_CONFIG_H_

/*check system*/
#if defined (_WIN32)
    #define H_OS_WIN
#elif defined (__linux__)
    #define H_OS_LINUX
#elif defined (_AIX)
    #define H_OS_AIX
#elif defined(__sun) || defined(sun)
    #define H_OS_SUN
##elif defined(__DragonFly__)      || \
       defined(__FreeBSD__)        || \
       defined(__FreeBSD_kernel__) || \
       defined(__OpenBSD__)        || \
       defined(__NetBSD__)
    #define H_OS_BSD
#elif defined _hpux
    #define H_OS_HPUX
#else
    #pragma error "unknown os system!"
#endif

/*check version x64 x86*/
#ifdef H_OS_WIN
    #if WINVER < _WIN32_WINNT_VISTA
        #pragma error "vista or new requested!"
    #endif
    #ifdef _WIN64
        #define H_X64
    #else
        #define H_X86
    #endif
#else
    #ifdef __GNUC__
        /*check atomic support*/
        #if (__GNUC__ < 4) || \
                    ((__GNUC__ == 4) && ((__GNUC_MINOR__ < 1) || \
                    ((__GNUC_MINOR__ == 1) && \
                    (__GNUC_PATCHLEVEL__ < 2))))
            #pragma error "gcc 4.1.2 or new requested!"
        #endif
        #if __x86_64__ || __ppc64__ || __x86_64 || __amd64__  || __amd64
            #define H_X64
        #else
            #define H_X86
        #endif
    #else
        #pragma error "unknown compile!"
    #endif
#endif

//uuid
//#define H_UUID
//mysql
#define H_MYSQL

#endif//H_CONFIG_H_
