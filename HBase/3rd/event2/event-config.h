#ifndef EVENT_CONFIG_H_ 
#define EVENT_CONFIG_H_

#ifdef WIN32
    #include "event-config-win.h"
#else
    #if __x86_64__ || __ppc64__ || __x86_64 || __amd64__  || __amd64
        #include "event_config_linux_x64.h"
    #else
        #include "event_config_linux_x86.h"
    #endif
#endif

#endif//EVENT_CONFIG_H_
