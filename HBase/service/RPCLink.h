
#ifndef H_RPCLINK_H_
#define H_RPCLINK_H_

#include "RWLock.h"

H_BNAMSP

class CRPCLink : public CObject
{
public:
    CRPCLink(void);
    ~CRPCLink(void);

    void Register(const int &iSVId, const int &iSVType, const H_SOCK &sock);
    void Unregister(const int &iSVId, const int &iSVType, const H_SOCK &sock);
    void getLinkById(const int &iSVId, std::list<H_SOCK> *plstSock);
    H_SOCK getALinkById(const int &iSVId);
    void getLinkByType(const int &iSVType, std::list<H_SOCK> *plstSock);
    H_SOCK getALinkByType(const int &iSVType);

private:
#ifdef H_OS_WIN
    typedef std::unordered_map<int, std::list<H_SOCK> >::iterator idit;
    typedef std::unordered_map<int, std::list<H_SOCK> > id_map;

    typedef std::unordered_map<int, std::list<H_SOCK> >::iterator typeit;
    typedef std::unordered_map<int, std::list<H_SOCK> > type_map;
#else
    typedef std::tr1::unordered_map<int, std::list<H_SOCK> >::iterator idit;
    typedef std::tr1::unordered_map<int, std::list<H_SOCK> > id_map;

    typedef std::tr1::unordered_map<int, std::list<H_SOCK> >::iterator typeit;
    typedef std::tr1::unordered_map<int, std::list<H_SOCK> > type_map;
#endif

    void removeSock(std::list<H_SOCK> *lstSock, const H_SOCK &sock);

private:
    id_map m_mapId;
    type_map m_mapType;
    CRWLock m_objLockId;
    CRWLock m_objLockType;
};

H_ENAMSP

#endif//H_RPCLINK_H_
