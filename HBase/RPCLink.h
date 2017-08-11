
#ifndef H_RPCLINK_H_
#define H_RPCLINK_H_

#include "Singleton.h"
#include "RWLock.h"

H_BNAMSP

class CRPCLink : public CSingleton<CRPCLink>
{
public:
    CRPCLink(void);
    ~CRPCLink(void);

    void Register(const int &iSVId, const int &iSVType, H_SOCK &sock);
    void Unregister(const int &iSVId, const int &iSVType, H_SOCK &sock);
    void getLinkById(const int &iSVId, std::list<H_SOCK> &lstSock);
    void getLinkByType(const int &iSVType, std::list<H_SOCK> &lstSock);

private:
#ifdef H_OS_WIN
    #define idit std::unordered_map<int , std::list<H_SOCK> >::iterator
    #define id_map std::unordered_map<int , std::list<H_SOCK> >

    #define typeit std::unordered_map<int , std::list<H_SOCK> >::iterator
    #define type_map std::unordered_map<int , std::list<H_SOCK> >
#else
    #define idit std::tr1::unordered_map<int , std::list<H_SOCK> >::iterator
    #define id_map std::tr1::unordered_map<int , std::list<H_SOCK> >

    #define typeit std::tr1::unordered_map<int , std::list<H_SOCK> >::iterator
    #define type_map std::tr1::unordered_map<int , std::list<H_SOCK> >
#endif

    void removeSock(std::list<H_SOCK> *lstSock, H_SOCK &sock);

private:
    id_map m_mapId;
    type_map m_mapType;
    CRWLock m_objLockId;
    CRWLock m_objLockType;
};

H_ENAMSP

#endif//H_RPCLINK_H_
