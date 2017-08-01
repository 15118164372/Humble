
#include "RPCLink.h"

H_BNAMSP

SINGLETON_INIT(CRPCLink)
CRPCLink objRPCLink;

CRPCLink::CRPCLink(void)
{
}

CRPCLink::~CRPCLink(void)
{
}

void CRPCLink::Register(const int &iSVId, const int &iSVType, H_SOCK &sock)
{
    m_objLock.wLock();
    m_mapSVId[iSVId] = sock;

    rpcsvtypeit itType = m_mapSVType.find(iSVType);
    if (m_mapSVType.end() != itType)
    {
        SVLink stLink;
        stLink.iSVId = iSVId;
        stLink.sock = sock;
        itType->second.push_back(stLink);
    }
    else
    {
        SVLink stLink;
        stLink.iSVId = iSVId;
        stLink.sock = sock;

        std::list<SVLink> lstTmp;
        lstTmp.push_back(stLink);

        m_mapSVType[iSVType] = lstTmp;
    }
    m_objLock.wunLock();
}

void CRPCLink::Unregister(const int &iSVId, const int &iSVType)
{
    m_objLock.wLock();
    rpcsvidit it = m_mapSVId.find(iSVId);
    if (m_mapSVId.end() != it)
    {
        m_mapSVId.erase(it);
    }

    rpcsvtypeit itType = m_mapSVType.find(iSVType);
    if (m_mapSVType.end() != itType)
    {
        for (std::list<SVLink>::iterator itLink = itType->second.begin(); itType->second.end() != itLink; ++itLink)
        {
            if (itLink->iSVId == iSVId)
            {
                itType->second.erase(itLink);
                break;
            }
        }
    }
    m_objLock.wunLock();
}

H_SOCK CRPCLink::getLinkById(const int &iSVId)
{
    H_SOCK sock(H_INVALID_SOCK);

    m_objLock.rLock();
    rpcsvidit it = m_mapSVId.find(iSVId);
    if (m_mapSVId.end() != it)
    {
        sock = it->second;
    }
    m_objLock.runLock();

    return sock;
}

std::vector<H_SOCK> CRPCLink::getLinkByType(const int &iSVType)
{
    std::vector<H_SOCK> vsSock;
    m_objLock.rLock();
    rpcsvtypeit itType = m_mapSVType.find(iSVType);
    if (m_mapSVType.end() != itType)
    {
        for (std::list<SVLink>::iterator itLink = itType->second.begin(); itType->second.end() != itLink; ++itLink)
        {
            vsSock.push_back(itLink->sock);
        }
    }
    m_objLock.runLock();

    return vsSock;
}

H_ENAMSP
