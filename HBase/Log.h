
#ifndef H_LOG_H_
#define H_LOG_H_

#include "TaskLazy.h"
#include "Singleton.h"

H_BNAMSP

enum LOG_LEVEL
{
    LOGLV_SYS = 0,
    LOGLV_ERROR, 
    LOGLV_WARN,
    LOGLV_INFO,
    LOGLV_DEBUG,
};

//文本日志
class CLog : public CTaskLazy<std::string>, public CSingleton<CLog>
{
public:
    CLog(void);
    ~CLog(void);

    void setPriority(const unsigned short usLV);
    unsigned short getPriority(void);
    void setLogFile(const char *pLogFile);
    void Open(void);
    void runTask(std::string *pMsg);
    void writeLog(const LOG_LEVEL emInLogLv, const char *pFormat, ...);

private:
    H_DISALLOWCOPY(CLog);
    const char *getLV(LOG_LEVEL emInLogLv) const;
    void renameLog(void);

private:
    LOG_LEVEL m_emLV;
    FILE *m_pFile;
    std::string m_strLogFile;
    std::string m_strCurDate;
};

#define H_LOG(emLogLV, acFormat, ...) \
    CLog::getSingletonPtr()->writeLog(emLogLV, H_CONCAT2("[%s %d] ", acFormat), __FUNCTION__, __LINE__, ##__VA_ARGS__)

H_ENAMSP

#endif//H_LOG_H_
