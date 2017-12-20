
#include "Log.h"
#include "Funcs.h"

H_BNAMSP

SINGLETON_INIT(CLog)
CLog objLog;

CLog::CLog(void) : CTaskLazy<STLog>(H_QULENS_LOG),
    m_emLV(LOGLV_DEBUG), m_pFile(NULL)
{
    m_strCurDate = H_Date();
}

CLog::~CLog(void)
{
    if (NULL != m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

void CLog::setPriority(const unsigned short usLV)
{
    m_emLV = (LOG_LEVEL)usLV;
}
unsigned short CLog::getPriority(void)
{
    return (unsigned short)m_emLV;
}
void CLog::setLogFile(const char *pLogFile)
{    
    m_strLogFile = pLogFile;
}

void CLog::Open(void)
{
    m_pFile = fopen(m_strLogFile.c_str(), "a");
    if (NULL == m_pFile)
    {
        H_Printf("open log file %s error.", m_strLogFile.c_str());
    }
}

const char * CLog::getLV(LOG_LEVEL emInLogLv) const
{
    switch(emInLogLv)
    {
        case LOGLV_SYS:
            return "SYSTEM";
        case LOGLV_WARN:
            return "WARNING";
        case LOGLV_ERROR:
            return "ERROR";
        case LOGLV_INFO:
            return "INFO";
        case LOGLV_DEBUG:
            return "DEBUG";
        default:
            break;
    }

    return "UNKNOWN";
}

void CLog::renameLog(void)
{
    std::string strDate(H_Date());
    if (strDate == m_strCurDate)
    {
        return;
    }
    
    if (NULL != m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }

    std::string strPath;
    (void)H_DirName(m_strLogFile.c_str(), strPath);

    std::string strNewName(strPath + H_PATH_SEPARATOR + m_strCurDate + ".txt");
    int iCount(H_INIT_NUMBER);
    while (true)
    {
        if (H_RTN_OK != H_FileExist(strNewName.c_str()))
        {
            break;
        }

        iCount++;
        strNewName = strPath + H_PATH_SEPARATOR + m_strCurDate + "(" + H_ToString(iCount) + ").txt";
    }

    (void)rename(m_strLogFile.c_str(), strNewName.c_str());
    Open();

    m_strCurDate = strDate;
}

void CLog::runTask(STLog *pMsg)
{
    renameLog();

    std::string strMsg("[" + H_Now() + "]" + pMsg->strMsg);
    if (NULL != m_pFile)
    {
        fwrite(strMsg.c_str(), 1, strMsg.size(), m_pFile);
    }

    if (LOGLV_DEBUG != m_emLV)
    {
        return;
    }

    if (LOGLV_ERROR != pMsg->emInLogLv 
        && LOGLV_WARN != pMsg->emInLogLv)
    {
        printf("%s", strMsg.c_str());
        return;
    }

#ifdef H_OS_WIN
    CONSOLE_SCREEN_BUFFER_INFO stCsbi;
    HANDLE hOut(GetStdHandle(STD_OUTPUT_HANDLE));
    GetConsoleScreenBufferInfo(hOut, &stCsbi);

    switch (pMsg->emInLogLv)
    {
    case LOGLV_ERROR:
        SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
        break;
    case LOGLV_WARN:
        SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        break;
    default:
        break;
    }

    printf("%s", strMsg.c_str());

    SetConsoleTextAttribute(hOut, stCsbi.wAttributes);
#else
    switch (pMsg->emInLogLv)
    {
    case LOGLV_ERROR:
        printf("\033[1;31m[%s]\033[0m", strMsg.c_str());
        break;
    case LOGLV_WARN:
        printf("\033[1;33m[%s]\033[0m", strMsg.c_str());
        break;
    default:
        break;
    }
#endif
}

void CLog::writeLog(const LOG_LEVEL emInLogLv, const char *pFormat, ...)
{
    if (emInLogLv > m_emLV
        || NULL == m_pFile)
    {
        return;
    }

    STLog *pMsg(newT());
    H_ASSERT(NULL != pMsg, "malloc memory error.");

    pMsg->emInLogLv = emInLogLv;
    pMsg->strMsg.append("[");
    pMsg->strMsg.append(getLV(emInLogLv));
    pMsg->strMsg.append("]");

    va_list va;
    va_start(va, pFormat);
    pMsg->strMsg.append(H_FormatVa(pFormat, va));
    va_end(va);
    pMsg->strMsg.append("\n");

    if (!addTask(pMsg))
    {
        H_SafeDelete(pMsg);
        H_Printf("%s", "add message to CirQueue error.");
    }
}

H_ENAMSP
