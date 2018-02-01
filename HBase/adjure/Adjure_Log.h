
#ifndef H_ADJURE_LOG_H_
#define H_ADJURE_LOG_H_

#include "Adjure.h"
#include "HEnum.h"

H_BNAMSP

//»’÷æ
class CLogAdjure : public CAdjure
{
public:
    CLogAdjure(const LOG_LEVEL &emLevel, const char *pszMsg) : CAdjure(H_INIT_NUMBER),
        m_emInLogLv(emLevel), m_strMsg(pszMsg)
    {};
    ~CLogAdjure(void) 
    {};

    const LOG_LEVEL &getLevel(void)
    {
        return m_emInLogLv;
    };
    const std::string *getMsg(void)
    {
        return &m_strMsg;
    };

private:
    LOG_LEVEL m_emInLogLv;
    std::string m_strMsg;
};

H_ENAMSP

#endif//H_ADJURE_LOG_H_
