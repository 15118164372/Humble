
#ifndef H_LIBRARY_H_
#define H_LIBRARY_H_

#include "Macros.h"

H_BNAMSP

/*��̬�������*/
class CLibrary
{
public:
    explicit CLibrary(const char *pszLib);
    ~CLibrary(void);

    /*��ȡdllָ��������ַ*/
    void *getFuncAddr(const char *pSym);

private:
    H_DISALLOWCOPY(CLibrary);
    CLibrary(void);

private:
    void *m_pHandle;
};

H_ENAMSP

#endif//H_LIBRARY_H_
