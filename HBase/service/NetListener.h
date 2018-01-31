
#ifndef H_NETLISTENER_H_
#define H_NETLISTENER_H_

#include "EventService.h"

H_BNAMSP

//监听
class CNetListener : public CEventService
{
public:
    CNetListener(class CParserMgr *pMgr , class CNetMgr *pNetMgr);
    ~CNetListener(void);

    //新加监听
    void addListener(const char *pszParser, const unsigned short &usType,
        const char *pszHost, const unsigned short &usPort);

    void onAdjure(CAdjure *PAdjure);
    void afterAdjure(CAdjure *pAdjure);

    class CNetMgr *getNetMgr(void)
    {
        return m_pNetMgr;
    };

private:
    CNetListener(void);
    class CParserMgr *m_pParserMgr;
    class CNetMgr *m_pNetMgr;
    std::vector<void *> m_vcListener;
    std::vector<class CNetInfo *> m_vcNetInfo;
};

H_ENAMSP

#endif//H_NETLISTENER_H_
