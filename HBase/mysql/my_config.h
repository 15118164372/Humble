#ifndef MY_CONFIG_H
#define MY_CONFIG_H

#ifdef  WIN32
    #include "my_config_win.h"
#else
    #if __x86_64__ || __ppc64__ || __x86_64 || __amd64__  || __amd64
        #include "my_config_linux_x64.h"
    #else
        #include "my_config_linux_x86.h"
    #endif
#endif

#endif
