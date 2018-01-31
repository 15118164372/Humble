
#ifndef H_LINKER_H_
#define H_LINKER_H_

#include "Log.h"

H_BNAMSP

//建立连接
class CLinker : public CService
{
public:
    CLinker(class CParserMgr *pParserMgr, class CNetMgr *pNetMgr);
    ~CLinker(void);

    //新加连接
    void addLinker(class CWorker *pWorker, const char *pszParser, const unsigned short &usType,
        const char *pszHost, const unsigned short &usPort, const bool &bKeepAlive);
    void reLink(void);

    //执行请求
    void onAdjure(CAdjure *pAdjure);
    void afterAdjure(CAdjure *pAdjure);

private:
    CLinker(void);
    void Link(class CLinkInfo *pInfo);

private:
    bool m_bRunning;
    class CParserMgr *m_pParserMgr;
    class CNetMgr *m_pNetMgr;
    std::list<class CLinkInfo *> m_lstAllLink;
};

H_ENAMSP

#endif//H_LINKER_H_
