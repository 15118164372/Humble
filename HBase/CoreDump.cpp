
#include "CoreDump.h"

H_BNAMSP

#ifdef H_OS_WIN
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <strsafe.h>
#pragma warning(disable:4091)
#include <dbghelp.h>

#pragma warning(disable:4995)

#define DBGHELP "dbghelp.dll"
CCoreDump* CCoreDump::s_pMiniDumper = NULL;
LPCRITICAL_SECTION CCoreDump::s_pCriticalSection = NULL;

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess,
    DWORD dwPid,
    HANDLE hFile,
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

CCoreDump::CCoreDump(void)
{
    s_pMiniDumper = this;
    s_pCriticalSection = new(std::nothrow)CRITICAL_SECTION;
    H_ASSERT(NULL != s_pCriticalSection, "malloc memory error.");    
    InitializeCriticalSection(s_pCriticalSection);
    ::SetUnhandledExceptionFilter(unhandledExceptionHandler);

    return;
}

CCoreDump::~CCoreDump(void)
{
    if(s_pCriticalSection)
    {
        DeleteCriticalSection(s_pCriticalSection);
        H_SafeDelete(s_pCriticalSection);
    }

    m_pExceptionInfo = NULL;

    return;
}

LONG CCoreDump::unhandledExceptionHandler(_EXCEPTION_POINTERS *pExceptionInfo)
{
    if(NULL == s_pMiniDumper)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    return s_pMiniDumper->writeMiniDump(pExceptionInfo);
}

void CCoreDump::setMiniDumpFileName(void)
{
    time_t currentTime(time(NULL));

    H_Snprintf(m_szMiniDumpPath, sizeof(m_szMiniDumpPath) - 1, "%s%s.%lld.dmp",
        m_szAppPath, m_szAppBaseName, currentTime);

    return;
}

bool CCoreDump::getImpersonationToken(HANDLE* phToken) const
{
    *phToken = NULL;

    if(!OpenThreadToken( GetCurrentThread(),
        TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
        TRUE,
        phToken))
    {
        if(ERROR_NO_TOKEN == GetLastError())
        {
            if(!OpenProcessToken( GetCurrentProcess(),
                TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
                phToken))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

BOOL CCoreDump::enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const
{
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    BOOL bOk(LookupPrivilegeValue(0, pszPriv, &tp.Privileges[0].Luid));

    if(bOk)
    {
        DWORD cbOld(sizeof(*ptpOld));
        bOk = AdjustTokenPrivileges(hToken, false, &tp, cbOld, ptpOld, &cbOld);
    }

    return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

BOOL CCoreDump::restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld) const
{
    BOOL bOk(AdjustTokenPrivileges(hToken, false, ptpOld, 0, NULL, NULL));

    return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}


LONG CCoreDump::writeMiniDump( _EXCEPTION_POINTERS *pExceptionInfo )
{
    bool bRetval(false);
    m_pExceptionInfo = pExceptionInfo;
    HANDLE hImpersonationToken(NULL);

    if(!getImpersonationToken( &hImpersonationToken ))
    {
        return false;
    }

    HMODULE hDll(NULL);
    TCHAR szDbgHelpPath[H_FILEPATH_LENS];

    if(GetModuleFileName( NULL, m_szAppPath, H_FILEPATH_LENS))
    {
        TCHAR *pSlash(strchr(m_szAppPath, H_FILEPATH_LENS));
        if(pSlash)
        {
            _tcscpy( m_szAppBaseName, pSlash + 1);
            *(pSlash+1) = 0;
        }

        strcpy(szDbgHelpPath, m_szAppPath);
        strcat(szDbgHelpPath, DBGHELP);

        hDll = ::LoadLibrary(szDbgHelpPath);
    }

    if(NULL == hDll)
    {
        hDll = ::LoadLibrary(DBGHELP);
    }

    if(NULL != hDll)
    {
        MINIDUMPWRITEDUMP MiniDumpWriteDump((MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" ));
        if(NULL != MiniDumpWriteDump)
        {
            setMiniDumpFileName();
            HANDLE hFile(::CreateFile(m_szMiniDumpPath, 
                GENERIC_WRITE, 
                FILE_SHARE_WRITE, 
                NULL, 
                CREATE_ALWAYS, 
                FILE_ATTRIBUTE_NORMAL, 
                NULL));
            if(INVALID_HANDLE_VALUE != hFile)
            {
                BOOL bOk;
                TOKEN_PRIVILEGES tp;                
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                ExInfo.ThreadId          = ::GetCurrentThreadId();
                ExInfo.ExceptionPointers = pExceptionInfo;
                ExInfo.ClientPointers    = NULL;
                
                BOOL bPrivilegeEnabled(enablePrivilege(SE_DEBUG_NAME, hImpersonationToken, &tp));

                EnterCriticalSection(s_pCriticalSection);
                {
                     bOk = MiniDumpWriteDump(GetCurrentProcess(),
                        GetCurrentProcessId(),
                        hFile,
                        MiniDumpNormal,
                        &ExInfo,
                        NULL,
                        NULL);
                }

                LeaveCriticalSection(s_pCriticalSection);

                if(bPrivilegeEnabled)
                {
                    (void)restorePrivilege(hImpersonationToken, &tp);
                }

                if(bOk)
                {
                    bRetval = true;
                }

                ::CloseHandle( hFile );
            }
        }        
    }

    TerminateProcess(GetCurrentProcess(), 0);

    return bRetval;
}

#else

CCoreDump::CCoreDump(void)
{
    (void)setRLimit(RLIMIT_CORE);
}

CCoreDump::~CCoreDump(void)
{
    
}

int CCoreDump::setRLimit(int iResource)
{
    struct rlimit stLimitOld;
    struct rlimit stLimNew;

    if (getrlimit(iResource, &stLimitOld) < 0)
    {
        H_Printf("getrlimit failed: %s, resource %d", strerror(errno), iResource);
        return H_RTN_ERROR;
    }

    stLimNew.rlim_cur = stLimitOld.rlim_max;
    stLimNew.rlim_max = stLimitOld.rlim_max;
    if (setrlimit(iResource, &stLimNew) < 0)
    {
        H_Printf("setrlimit failed: %s, resource %d", strerror(errno), iResource);
        (void)setrlimit(iResource, &stLimitOld);

        return H_RTN_ERROR;
    } 

    return H_RTN_OK;
}

#endif

H_ENAMSP
