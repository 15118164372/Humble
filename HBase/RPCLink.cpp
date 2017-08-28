
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
    m_objLockId.wLock();
    idit itId(m_mapId.find(iSVId));
    if (m_mapId.end() != itId)
    {
        itId->second.push_back(sock);
    }
    else
    {
        std::list<H_SOCK> lstSock;
        lstSock.push_back(sock);
        m_mapId[iSVId] = lstSock;
    }
    m_objLockId.wunLock();

    m_objLockType.wLock();
    typeit itType(m_mapType.find(iSVType));
    if (m_mapType.end() != itType)
    {
        itType->second.push_back(sock);
    }
    else
    {
        std::list<H_SOCK> lstSock;
        lstSock.push_back(sock);
        m_mapType[iSVType] = lstSock;
    }
    m_objLockType.wunLock();
}

void CRPCLink::removeSock(std::list<H_SOCK> *lstSock, H_SOCK &sock)
{
    std::list<H_SOCK>::iterator itSock;
    for (itSock = lstSock->begin(); lstSock->end() != itSock; ++itSock)
    {
        if (*itSock == sock)
        {
            lstSock->erase(itSock);
            break;
        }
    }
}

void CRPCLink::Unregister(const int &iSVId, const int &iSVType, H_SOCK &sock)
{
    m_objLockId.wLock();
    idit itId(m_mapId.find(iSVId));
    if (m_mapId.end() != itId)
    {
        removeSock(&(itId->second), sock);
        if (itId->second.empty())
        {
            m_mapId.erase(itId);
        }
    }
    m_objLockId.wunLock();

    m_objLockType.wLock();
    typeit itType(m_mapType.find(iSVType));
    if (m_mapType.end() != itType)
    {
        removeSock(&(itType->second), sock);
        if (itType->second.empty())
        {
            m_mapType.erase(itType);
        }
    }
    m_objLockType.wunLock();
}

void CRPCLink::getLinkById(const int &iSVId, std::list<H_SOCK> &lstSock)
{
    m_objLockId.rLock();
    idit itId(m_mapId.find(iSVId));
    if (m_mapId.end() != itId)
    {
        lstSock = itId->second;
    }
    m_objLockId.runLock();
}

void CRPCLink::getLinkByType(const int &iSVType, std::list<H_SOCK> &lstSock)
{
    m_objLockType.rLock();
    typeit itType(m_mapType.find(iSVType));
    if (m_mapType.end() != itType)
    {
        lstSock = itType->second;
    }
    m_objLockType.runLock();
}

H_ENAMSP
