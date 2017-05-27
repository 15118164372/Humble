
#ifndef H_CONFIG_H_
#define H_CONFIG_H_

/*check system*/
#if defined _WIN32
    #define H_OS_WIN
#elif defined __sun
    #define H_OS_SOLARIS
#elif defined __FreeBSD__
    #define H_OS_FREEBASD
#elif defined __NetBSD__
    #define H_OS_NETBSD
#elif defined __linux__
    #define H_OS_LINUX
#elif defined HP_UX_C
    #define H_OS_HPUX
#elif defined AIX
    #define H_OS_AIX
#else
    #error unknown os system!
#endif

/*check x64 or x86*/
#ifdef H_OS_WIN
    #ifdef _WIN64
        #define H_X64
    #else
        #define H_X86
    #endif
#else
    #ifdef __GNUC__
        #if __x86_64__ || __ppc64__ || __x86_64 || __amd64__  || __amd64
            #define H_X64
        #else
            #define H_X86
        #endif
    #else
        #error unknown compile!
    #endif
#endif

#ifdef __GNUC__
    /*check atomic support*/
    #if (__GNUC__ < 4) || \
            ((__GNUC__ == 4) && ((__GNUC_MINOR__ < 1) || \
            ((__GNUC_MINOR__ == 1) && \
            (__GNUC_PATCHLEVEL__ < 2))))
        #pragma error "gcc 4.1.2 or new requested!"
    #endif

    #if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 8)  && (__GNUC_PATCHLEVEL__ >= 1))
        #define H_CPP11
    #endif
#endif

//#define H_UUID
#define H_MYSQL

#ifdef H_OS_WIN
    #define H_CPP11
#endif

#endif//H_CONFIG_H_
