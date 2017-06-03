
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

void CRPCLink::Register(H_SOCK sock, int iSVId)
{
    m_objLock.wLock();
    m_mapSV[iSVId] = sock;
    m_objLock.unLock();
}

void CRPCLink::Unregister(int iSVId)
{
    m_objLock.wLock();
    rpcsvit it = m_mapSV.find(iSVId);
    if (m_mapSV.end() != it)
    {
        m_mapSV.erase(it);
    }
    m_objLock.unLock();
}

H_SOCK CRPCLink::getSVLink(int iSVId)
{
    H_SOCK sock(H_INVALID_SOCK);

    m_objLock.rLock();
    rpcsvit it = m_mapSV.find(iSVId);
    if (m_mapSV.end() != it)
    {
        sock = it->second;
    }
    m_objLock.unLock();

    return sock;
}

H_ENAMSP
