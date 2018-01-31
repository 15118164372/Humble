
#include "RPCLink.h"

H_BNAMSP

CRPCLink::CRPCLink(void)
{
    srand((unsigned int)time(NULL));
}

CRPCLink::~CRPCLink(void)
{
}

void CRPCLink::Register(const int &iSVId, const int &iSVType, const H_SOCK &sock)
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

void CRPCLink::removeSock(std::list<H_SOCK> *lstSock, const H_SOCK &sock)
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

void CRPCLink::Unregister(const int &iSVId, const int &iSVType, const H_SOCK &sock)
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

void CRPCLink::getLinkById(const int &iSVId, std::list<H_SOCK> *plstSock)
{
    m_objLockId.rLock();
    idit itId(m_mapId.find(iSVId));
    if (m_mapId.end() != itId)
    {
        *plstSock = itId->second;
    }
    m_objLockId.runLock();
}

H_SOCK CRPCLink::getALinkById(const int &iSVId)
{
    H_SOCK sock(H_INVALID_SOCK);

    m_objLockId.rLock();
    idit itId(m_mapId.find(iSVId));
    if (m_mapId.end() != itId)
    {
        if (!itId->second.empty())
        {
            if (1 == itId->second.size())
            {
                sock = *(itId->second.begin());
            }
            else
            {
                unsigned int uiIndex(rand() % itId->second.size());
                std::list<H_SOCK>::iterator itSock(itId->second.begin());
                std::advance(itSock, uiIndex);
                sock = *itSock;
            }
        }
    }
    m_objLockId.runLock();

    return sock;
}

void CRPCLink::getLinkByType(const int &iSVType, std::list<H_SOCK> *plstSock)
{
    m_objLockType.rLock();
    typeit itType(m_mapType.find(iSVType));
    if (m_mapType.end() != itType)
    {
        *plstSock = itType->second;
    }
    m_objLockType.runLock();
}

H_SOCK CRPCLink::getALinkByType(const int &iSVType)
{
    H_SOCK sock(H_INVALID_SOCK);

    m_objLockType.rLock();
    typeit itType(m_mapType.find(iSVType));
    if (m_mapType.end() != itType)
    {
        if (!itType->second.empty())
        {
            if (1 == itType->second.size())
            {
                sock = *(itType->second.begin());
            }
            else
            {
                unsigned int uiIndex(rand() % itType->second.size());
                std::list<H_SOCK>::iterator itSock(itType->second.begin());
                std::advance(itSock, uiIndex);
                sock = *itSock;
            }
        }
    }
    m_objLockType.runLock();

    return sock;
}

H_ENAMSP
