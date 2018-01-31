
#include "Log.h"
#include "Utils.h"

H_BNAMSP

CLog *g_pLog(NULL);

CLog::CLog(void) : CService(H_QULENS_LOG), m_emLV(LOGLV_DEBUG), m_pFile(NULL)
{
    setSVName(H_SERVICE_LOG);

    m_strLogFile = CUtils::getProPath() + "log/" + CUtils::nowDate() + ".txt";
    m_pFile = fopen(m_strLogFile.c_str(), "a");
    H_ASSERT(NULL != m_pFile, "open log file error.");
}

CLog::~CLog(void)
{
    if (NULL != m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

void CLog::setPriority(const unsigned short &usLV)
{
    m_emLV = (LOG_LEVEL)usLV;
}
unsigned short CLog::getPriority(void)
{
    return m_emLV;
}

const char * CLog::getLV(const LOG_LEVEL &emInLogLv) const
{
    switch(emInLogLv)
    {
        case LOGLV_SYS:
            return "SYS";
        case LOGLV_WARN:
            return "WARN";
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

void CLog::onAdjure(CAdjure *pAdjure)
{
    CLogAdjure *pMsg((CLogAdjure *)pAdjure);
    LOG_LEVEL emLevel(pMsg->getLevel());
    const std::string *pStrMsg(pMsg->getMsg());
    if (NULL != m_pFile)
    {
        fwrite(pStrMsg->c_str(), 1, pStrMsg->size(), m_pFile);
    }
    if (LOGLV_DEBUG != m_emLV)
    {
        return;
    }
    if (LOGLV_ERROR != emLevel
        && LOGLV_WARN != emLevel)
    {
        printf("%s", pStrMsg->c_str());
        return;
    }

#ifdef H_OS_WIN
    CONSOLE_SCREEN_BUFFER_INFO stCsbi;
    HANDLE hOut(GetStdHandle(STD_OUTPUT_HANDLE));
    GetConsoleScreenBufferInfo(hOut, &stCsbi);

    switch (emLevel)
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

    printf("%s", pStrMsg->c_str());

    SetConsoleTextAttribute(hOut, stCsbi.wAttributes);
#else
    switch (emLevel)
    {
        case LOGLV_ERROR:
            printf("\033[1;31m[%s]\033[0m", pStrMsg->c_str());
            break;
        case LOGLV_WARN:
            printf("\033[1;33m[%s]\033[0m", pStrMsg->c_str());
            break;
        default:
            break;
    }
#endif
    
}
void CLog::afterAdjure(CAdjure *pAdjure)
{
    H_SafeDelete(pAdjure);
}

void CLog::writeLog(const LOG_LEVEL emInLogLv, const char *pFormat, ...)
{
    if (emInLogLv > m_emLV
        || NULL == m_pFile)
    {
        return;
    }

    std::string strMsg("[" + CUtils::nowStrMilSecond() + "][" + getLV(emInLogLv) + "]");
    va_list va;
    va_start(va, pFormat);
    strMsg.append(CUtils::formatVa(pFormat, va));
    va_end(va);
    strMsg.append("\n");

    CLogAdjure *pMsg = new(std::nothrow) CLogAdjure(emInLogLv, strMsg.c_str());
    if (NULL == pMsg)
    {
        H_Printf("%s", H_ERR_MEMORY);
        return;
    }

    if (!Adjure(pMsg))
    {
        H_SafeDelete(pMsg);
        H_Printf("%s", H_ERR_ADDINQU);
    }
}

H_ENAMSP
