
#ifndef H_LOG_H_
#define H_LOG_H_

#include "Service.h"
#include "Adjure_Log.h"

H_BNAMSP

//文本日志
class CLog : public CService
{    
public:
    CLog(void);
    ~CLog(void);
    //日志级别
    void setPriority(const unsigned short &usLV);
    unsigned short getPriority(void);
    //执行写日志
    void onAdjure(CAdjure *pAdjure);
    void afterAdjure(CAdjure *pAdjure);
    //写日志请求
    void writeLog(const LOG_LEVEL &emInLogLv, const char *pFormat, ...);

private:
    const char *getLV(const LOG_LEVEL &emInLogLv) const;

private:
    LOG_LEVEL m_emLV;
    FILE *m_pFile;
    std::string m_strLogFile;
};

extern CLog *g_pLog;

#define H_LOG(emLogLV, acFormat, ...) \
    g_pLog->writeLog(emLogLV, H_CONCAT2("[%s %d] ", acFormat), __FUNCTION__, __LINE__, ##__VA_ARGS__)

H_ENAMSP

#endif//H_LOG_H_
