
#include "Log.h"
#include "Funcs.h"

H_BNAMSP

#define H_DefLogNam "log.txt"
SINGLETON_INIT(CLog)
CLog objLog;

CLog::CLog(void) : CRecvTask<std::string>(H_QULENS_LOG),
    m_emLV(LOGLV_DEBUG), m_pFile(NULL), m_strLogFile(H_DefLogNam)
{

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

void CLog::runTask(std::string *pMsg)
{    
    if (NULL != m_pFile)
    {
        fwrite(pMsg->c_str(), 1, pMsg->size(), m_pFile);
    }
    if (LOGLV_DEBUG == m_emLV)
    {
        printf("%s", pMsg->c_str());
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

    pstrVa->append("[");
    pstrVa->append(H_Now());
    pstrVa->append("][");
    pstrVa->append(getLV(emInLogLv));
    pstrVa->append("]");

    va_list va;
    va_start(va, pFormat);
    pstrVa->append(H_FormatVa(pFormat, va));
    va_end(va);
    pstrVa->append("\n");

    addTask(pstrVa);
}

H_ENAMSP
