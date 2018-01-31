
#ifndef H_MEMORY_H_
#define H_MEMORY_H_

#include "Config.h"

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

#endif//H_MEMORY_H_
