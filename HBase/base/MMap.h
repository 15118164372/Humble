
#ifndef H_MMAP_H_
#define H_MMAP_H_

#include "HObject.h"

H_BNAMSP

/*
内存映射
*/
template <typename T>
class CMMap : public CObject
{
public:
    /*pszFileName --映射文件名 pszMapName --映射名  iNum --数量*/
    CMMap(const char *pszFileName, const char *pszMapName, const size_t &iNum);
    ~CMMap(void);

    /*返回T指针*/
    T *getData(void);
    /*返回T数量*/
    const size_t &getNumber(void);

    const char *getMapName(void);
    const char *getFile(void);

private:
    CMMap(void);
    size_t m_iNum;
    size_t m_iTotalSize;
    void *m_pDate;
    std::string m_strFile;
    std::string m_strMapName;
#ifdef H_OS_WIN
    HANDLE m_hMapFile;    
#endif
};

template <typename T>
CMMap<T>::CMMap(const char *pszFileName, const char *pszMapName, const size_t &iNum)
    : m_iNum(iNum), m_iTotalSize(sizeof(T) * iNum), m_pDate(NULL)
{
    if (NULL == pszFileName
        || NULL == pszMapName
        || 0 == iNum)
    {
        return;
    }

    m_strFile = pszFileName;
    m_strMapName = pszMapName;

#ifdef H_OS_WIN
    HANDLE hFile(CreateFile(pszFileName,
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, 
        NULL,
        OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL));
    if (INVALID_HANDLE_VALUE == hFile)  
    {
        H_Printf("CreateFile error, %s.", H_Error2Str(H_Error()));
        return;
    }

    m_hMapFile = CreateFileMapping((HANDLE)hFile, 
        NULL, 
        PAGE_READWRITE, 
        0,
        m_iTotalSize, 
        m_strMapName.c_str());
    if(NULL == m_hMapFile)
    {
        H_Printf("CreateFileMapping error, %s.", H_Error2Str(H_Error()));
        (void)CloseHandle(hFile);        
        return;
    }

    m_pDate = MapViewOfFile(m_hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        m_iTotalSize);
    if (NULL == m_pDate)
    {
        H_Printf("MapViewOfFile error, %s.", H_Error2Str(H_Error()));
        (void)CloseHandle(m_hMapFile);
        (void)CloseHandle(hFile);
        m_hMapFile = NULL;
        return;
    }
#else
    int iFd(-1);

    iFd = open(pszFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);  
    if(-1 == iFd)
    {
        H_Printf("open error, %s.", H_Error2Str(H_Error()));
        return;
    }

    if (0 != ftruncate(iFd, m_iTotalSize))
    {
        H_Printf("ftruncate error, %s.", H_Error2Str(H_Error()));
        close(iFd);
        return;
    }

    m_pDate = mmap(NULL, m_iTotalSize, PROT_READ|PROT_WRITE, MAP_SHARED, iFd, 0);
    close(iFd);
    if(MAP_FAILED == m_pDate)  
    {
        H_Printf("mmap error, %s.", H_Error2Str(H_Error()));
        return;
    }
#endif
}

template <typename T>
CMMap<T>::~CMMap(void)
{
#ifdef H_OS_WIN
    if (NULL != m_pDate)
    {
        (void)UnmapViewOfFile(m_pDate);
    }
    if (NULL != m_hMapFile)
    {
        (void)CloseHandle(m_hMapFile);
        m_hMapFile = NULL;
    }    
#else
    if (MAP_FAILED != m_pDate
        && NULL != m_pDate)
    {
        (void)munmap(m_pDate, m_iTotalSize);
        m_pDate = MAP_FAILED;
    }
#endif
}

template <typename T>
T * CMMap<T>::getData(void)
{
    return (T*)m_pDate;
}

template <typename T>
const size_t &CMMap<T>::getNumber(void)
{
    return m_iNum;
}

template <typename T>
const char *CMMap<T>::getMapName(void)
{
    return m_strMapName.c_str();
}

template <typename T>
const char *CMMap<T>::getFile(void)
{
    return m_strFile.c_str();
}

H_ENAMSP

#endif//H_MMAP_H_
