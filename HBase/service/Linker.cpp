
#include "Linker.h"
#include "Parser.h"
#include "NetMgr.H"
#include "Utils.h"
#include "NETAddr.h"
#include "Adjure_NetWorker.h"

H_BNAMSP

CLinker::CLinker(class CParserMgr *pParserMgr, class CNetMgr *pNetMgr) : CService(H_QULENS_LINKER), m_bRunning(false),
    m_pParserMgr(pParserMgr), m_pNetMgr(pNetMgr)
{
    setSVName(H_SERVICE_LINKER);
}
CLinker::~CLinker(void)
{
    for (std::list<class CLinkInfo *>::iterator it = m_lstAllLink.begin(); m_lstAllLink.end() != it; ++it)
    {
        H_SafeDelete((*it));
    }
}

void CLinker::addLinker(class CWorker *pWorker, const char *pszParser, const unsigned short &usType,
    const char *pszHost, const unsigned short &usPort, const uint64_t &ulId, const bool &bKeepAlive)
{
    CParser *pParser(m_pParserMgr->getParser(pszParser));
    H_ASSERT(NULL != pParser, "get parser error.");

    CLinkInfo *pLinkInfo = new(std::nothrow) CLinkInfo(NULL, pParser, usType, pszHost, usPort);
    if (NULL == pLinkInfo)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    pLinkInfo->setBind(pWorker, ulId);
    pLinkInfo->setKeepAlive(bKeepAlive);
    CAddLinkerAdjure *pAddLinker = new(std::nothrow) CAddLinkerAdjure(pLinkInfo);
    if (NULL == pAddLinker)
    {
        H_SafeDelete(pLinkInfo);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    if (!Adjure(pAddLinker))
    {
        H_SafeDelete(pLinkInfo);
        H_SafeDelete(pAddLinker);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
    }
}
void CLinker::reLink(void)
{
    if (m_bRunning)
    {
        return;
    }

    CAdjure *pReLink = new(std::nothrow) CAdjure(NETWORKERADJ_RELINK);
    if (NULL == pReLink)
    {
        H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
        return;
    }

    if (!Adjure(pReLink))
    {
        H_SafeDelete(pReLink);
        H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
    }
}
void CLinker::onAdjure(CAdjure *pAdjure)
{
    switch (pAdjure->getAdjure())
    {
        case NETWORKERADJ_ADDLINKER:
        {
            CLinkInfo *pLinkInfo(((CAddLinkerAdjure *)pAdjure)->getLinkInfo());
            m_lstAllLink.push_back(pLinkInfo);
            Link(pLinkInfo);
        }
        break;
        case NETWORKERADJ_RELINK:
        {
            m_bRunning = true;
            CLinkInfo *pLinkInfo;
            for (std::list<class CLinkInfo *>::iterator it = m_lstAllLink.begin(); m_lstAllLink.end() != it;)
            {
                pLinkInfo = *it;
                switch (pLinkInfo->getLinkState())
                {
                    case LS_WAITLINK:
                    {
                        Link(pLinkInfo);
                        ++it;
                    }
                    break;
                    case LS_LINKED:
                    {
                        ++it;
                    }
                    break;
                    case LS_CLOSED:
                    {
                        if (pLinkInfo->getKeepAlive())
                        {
                            Link(pLinkInfo);
                            ++it;
                        }
                        else
                        {
                            H_LOG(LOGLV_SYS, "remove link, host %s, port %d, type %d", 
                                pLinkInfo->getHost(), pLinkInfo->getPort(), pLinkInfo->getType());
                            H_SafeDelete(pLinkInfo);
                            it = m_lstAllLink.erase(it);
                        }
                    }
                    break;
                    default:
                    {
                        ++it;
                    }
                    break;
                }
            }
            m_bRunning = false;
        }
        break;
        default:
            break;
    }
}
void CLinker::afterAdjure(CAdjure *pAdjure)
{
    H_SafeDelete(pAdjure);
}

void CLinker::Link(class CLinkInfo *pInfo)
{
    CNETAddr objAddr;
    if (H_RTN_OK != objAddr.setAddr(pInfo->getHost(), pInfo->getPort(), pInfo->isIpV6()))
    {
        H_LOG(LOGLV_ERROR, "parse host %s port %d error.", pInfo->getHost(), pInfo->getPort());
        return;
    }

    //´´½¨socket
    H_SOCK sock(socket(AF_INET, SOCK_STREAM, 0));
    if (H_INVALID_SOCK == sock)
    {
        H_LOG(LOGLV_ERROR, "%s", "create socket error.");
        return;
    }
    if (H_RTN_OK != connect(sock, objAddr.getAddr(), (int)objAddr.getAddrSize()))
    {
        CUtils::closeSock(sock);
        H_LOG(LOGLV_WARN, "link to host %s on port %d error.", pInfo->getHost(), pInfo->getPort());
        return;
    }

    pInfo->setLinkState(LS_LINKED);
    m_pNetMgr->getNetWorker(sock)->addSock(pInfo, pInfo->getParser(), sock, pInfo->getType());
    H_LOG(LOGLV_SYS, "link to host %s on port %d successfully.", pInfo->getHost(), pInfo->getPort());
}

H_ENAMSP
