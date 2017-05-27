
#include "Log.h"
#include "Funcs.h"

H_BNAMSP

SINGLETON_INIT(CLog)
CLog objLog;

CLog::CLog(void) : CTaskLazy<std::string>(H_QULENS_LOG),
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
    std::string strDate = H_Date();
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

    std::string strNewName = strPath + H_PATH_SEPARATOR + m_strCurDate + ".txt";
    int iCount = H_INIT_NUMBER;
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

void CLog::runTask(std::string *pMsg)
{
    renameLog();

    std::string strMsg = "[" + H_Now() + "]" + *pMsg;
    if (NULL != m_pFile)
    {
        fwrite(strMsg.c_str(), 1, strMsg.size(), m_pFile);
    }

    if (LOGLV_DEBUG == m_emLV)
    {
        printf("%s", strMsg.c_str());
    }
}

void CLog::writeLog(const LOG_LEVEL emInLogLv, const char *pFormat, ...)
{
    if (emInLogLv > m_emLV
        || NULL == m_pFile)
    {
        return;
    }

    std::string *pstrVa = newT();
    H_ASSERT(NULL != pstrVa, "malloc memory error.");

    pstrVa->append("[");
    pstrVa->append(getLV(emInLogLv));
    pstrVa->append("]");

    va_list va;
    va_start(va, pFormat);
    pstrVa->append(H_FormatVa(pFormat, va));
    va_end(va);
    pstrVa->append("\n");

    if (!addTask(pstrVa))
    {
        H_SafeDelete(pstrVa);
        H_Printf("%s", "add message to CirQueue error.");
    }
}

H_ENAMSP
