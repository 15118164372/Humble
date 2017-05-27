
#ifndef H_LIBRARY_H_
#define H_LIBRARY_H_

#include "Macros.h"

H_BNAMSP

/*动态库操作类*/
class CLibrary
{
public:
    explicit CLibrary(const char *pszLib);
    ~CLibrary(void);

    /*获取dll指定函数地址*/
    void *getFuncAddr(const char *pSym);

private:
    H_DISALLOWCOPY(CLibrary);
    CLibrary(void);

private:
    void *m_pHandle;
};

H_ENAMSP

#endif//H_LIBRARY_H_
