
#ifndef H_COREDUMP_H_
#define H_COREDUMP_H_

#include "Macros.h"

H_BNAMSP

#ifdef H_OS_WIN
//dump¿‡
class CCoreDump
{
public:    
    CCoreDump(void);
    ~CCoreDump(void);

private:
    static LONG WINAPI unhandledExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo);
    void setMiniDumpFileName(void);
    bool getImpersonationToken(HANDLE* phToken) const;
    BOOL enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const;
    BOOL restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const;
    LONG writeMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );

    _EXCEPTION_POINTERS *m_pExceptionInfo;
    TCHAR m_szMiniDumpPath[H_FILEPATH_LENS];
    TCHAR m_szAppPath[H_FILEPATH_LENS];
    TCHAR m_szAppBaseName[H_FILEPATH_LENS];

    static CCoreDump* s_pMiniDumper;
    static LPCRITICAL_SECTION s_pCriticalSection;

private:
    H_DISALLOWCOPY(CCoreDump);
};

#else

class CCoreDump
{
public:    
    CCoreDump(void);
    ~CCoreDump(void);

private:
    H_DISALLOWCOPY(CCoreDump);
    int setRLimit(int iResource);
};
#endif

H_ENAMSP

#endif//H_COREDUMP_H_
