
#include "Library.h"

H_BNAMSP

CLibrary::CLibrary(const char *pszLib)
{
#ifdef H_OS_WIN
    m_pHandle = LoadLibrary(pszLib);
#else
    m_pHandle = dlopen(pszLib, RTLD_LAZY);
#endif

    H_ASSERT(NULL != m_pHandle, "open library error.");
}

CLibrary::~CLibrary(void)
{
    if (NULL != m_pHandle)
    {
#ifdef H_OS_WIN
        (void)FreeLibrary((HMODULE)m_pHandle);
#else
        (void)dlclose(m_pHandle);
#endif
        m_pHandle = NULL;
    }
}

void *CLibrary::getFuncAddr(const char *pSym)
{
#ifdef H_OS_WIN
    return GetProcAddress((HMODULE)m_pHandle, pSym);
#else
    return dlsym(m_pHandle, pSym);
#endif
}

H_ENAMSP
