
#include "Linker.h"
#include "NETAddr.h"
#include "Funcs.h"
#include "NetParser.h"
#include "NetWorkerMgr.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CLinker)
CLinker objLinker;

CLinker::CLinker(void) : CTaskLazy<H_LinkCMD>(H_QULENS_LINKER), m_uiReLinking(RLStatus_None)
{
}

CLinker::~CLinker(void)
{
    std::vector<H_LinkCMD *>::iterator itLink;
    for (itLink = m_vcLink.begin(); m_vcLink.end() != itLink; itLink++)
    {
        H_SafeDelete(*itLink);
    }
    m_vcLink.clear();
}

void CLinker::addLink(H_LinkCMD *pMsg)
{
    H_LinkCMD *pLink(newT());
    H_ASSERT(NULL != pLink, "malloc memory error.");

    pLink->bLinked = false;
    pLink->pParser = pMsg->pParser;
    pLink->usPort = pMsg->usPort;
    pLink->stLink.usType = pMsg->stLink.usType;
    H_Zero(pLink->acHost, sizeof(pLink->acHost));
    memcpy(pLink->acHost, pMsg->acHost, strlen(pMsg->acHost));

    pLink->stLink.sock = H_ClientSock(pLink->acHost, pLink->usPort);
    if (H_INVALID_SOCK != pLink->stLink.sock)
    {
        pLink->bLinked = true;
        if (!CNetWorkerMgr::getSingletonPtr()->addLink(pLink->stLink.sock, pLink->pParser, pLink->stLink.usType, false))
        {
            pLink->bLinked = false;
            evutil_closesocket(pLink->stLink.sock);
            pLink->stLink.sock = H_INVALID_SOCK;
        }  
    }
    else
    {
        H_LOG(LOGLV_ERROR, "link to host %s on port %d error.", pLink->acHost, pLink->usPort);
    }

    m_vcLink.push_back(pLink); 
}

void CLinker::reLink(H_LinkCMD *)
{
    H_AtomicSet(&m_uiReLinking, RLStatus_Linking);

    H_LinkCMD *pLink;
    std::vector<H_LinkCMD *>::iterator itLink;
    for (itLink = m_vcLink.begin(); m_vcLink.end() != itLink; ++itLink)
    {
        pLink = *itLink;
        if (pLink->bLinked)
        {
            continue;
        }

        pLink->stLink.sock = H_ClientSock(pLink->acHost, pLink->usPort);
        if (H_INVALID_SOCK != pLink->stLink.sock)
        {
            pLink->bLinked = true;
            if (!CNetWorkerMgr::getSingletonPtr()->addLink(pLink->stLink.sock, pLink->pParser, pLink->stLink.usType, false))
            {
                pLink->bLinked = false;
                evutil_closesocket(pLink->stLink.sock);
                pLink->stLink.sock = H_INVALID_SOCK;
            }
        }
        else
        {
            H_LOG(LOGLV_ERROR, "link to host %s on port %d error.", pLink->acHost, pLink->usPort);
        }
    }

    H_AtomicSet(&m_uiReLinking, RLStatus_None);
}

void CLinker::linkClosed(H_LinkCMD *pMsg)
{
    H_LinkCMD *pLink;
    std::vector<H_LinkCMD *>::iterator itLink;
    for (itLink = m_vcLink.begin(); m_vcLink.end() != itLink; ++itLink)
    {
        pLink = *itLink;
        if (pLink->stLink.sock == pMsg->stLink.sock)
        {
            pLink->bLinked = false;
            pLink->stLink.sock = H_INVALID_SOCK;
            H_LOG(LOGLV_ERROR, "link to host %s on port %d closed.", pLink->acHost, pLink->usPort);
            break;
        }
    }
}

void CLinker::removeLink(H_LinkCMD *pMsg)
{
    H_LinkCMD *pLink;
    std::vector<H_LinkCMD *>::iterator itLink;
    for (itLink = m_vcLink.begin(); m_vcLink.end() != itLink; ++itLink)
    {
        pLink = *itLink;
        if (pLink->stLink.sock == pMsg->stLink.sock)
        {
            H_LOG(LOGLV_SYS, "remove link to host %s on port %d.", pLink->acHost, pLink->usPort);
            H_SafeDelete(pLink);
            m_vcLink.erase(itLink);            
            break;
        }
    }
}

void CLinker::runTask(H_LinkCMD *pMsg)
{    
    switch (pMsg->cCmd)
    {
        case CMD_ADDLINK:
        {
            addLink(pMsg);
        }
        break;
        case CMD_RELINK:
        {
            reLink(pMsg);
        }
        break;
        case CMD_CLOSED:
        {
            linkClosed(pMsg);
        }
        break;
        case CMD_REMOVE:
        {
            removeLink(pMsg);
        }
        break;
        default:
            break;
    }    
}

void CLinker::addLink(const char *pszParser, const unsigned short &usType, const char *pszHost, const unsigned short &usPort)
{
    H_ASSERT(NULL != pszParser && NULL != pszHost, "got null pointer.");
    CParser *pParser = CParserMgr::getSingletonPtr()->getParser(pszParser);
    H_ASSERT(NULL != pParser, "get parser  error.");
    H_LinkCMD *pLinker = newT();
    H_ASSERT(NULL != pLinker, "malloc memory error.");

    pLinker->cCmd = CMD_ADDLINK;
    pLinker->pParser = pParser;
    pLinker->usPort = usPort;
    pLinker->stLink.usType = usType;
    H_Zero(pLinker->acHost, sizeof(pLinker->acHost));
    memcpy(pLinker->acHost, pszHost, strlen(pszHost));

    if (!addTask(pLinker))
    {
        H_SafeDelete(pLinker);
        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
    }    
}

void CLinker::reLink(void)
{
    if (RLStatus_None != H_AtomicGet(&m_uiReLinking))
    {
        return;
    }

    H_LinkCMD *pLinker(newT());
    H_ASSERT(NULL != pLinker, "malloc memory error.");

    pLinker->cCmd = CMD_RELINK;

    if (!addTask(pLinker))
    {
        H_SafeDelete(pLinker);
        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
    }
}

void CLinker::linkClosed(H_SOCK &sock)
{
    H_LinkCMD *pLinker(newT());
    H_ASSERT(NULL != pLinker, "malloc memory error.");

    pLinker->cCmd = CMD_CLOSED;
    pLinker->stLink.sock = sock;

    if (!addTask(pLinker))
    {
        H_SafeDelete(pLinker);
        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
    }
}

void CLinker::removeLink(H_SOCK &sock)
{
    H_LinkCMD *pLinker(newT());
    H_ASSERT(NULL != pLinker, "malloc memory error.");

    pLinker->cCmd = CMD_REMOVE;
    pLinker->stLink.sock = sock;

    if (!addTask(pLinker))
    {
        H_SafeDelete(pLinker);
        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
    }
}

H_ENAMSP
