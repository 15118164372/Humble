
#ifndef H_RPCLINK_H_
#define H_RPCLINK_H_

#include "Singleton.h"
#include "RWLock.h"

H_BNAMSP

class CRPCLink : public CSingleton<CRPCLink>
{
    struct SVLink
    {
        int iSVId;
        H_SOCK sock;
    };
public:
    CRPCLink(void);
    ~CRPCLink(void);

    void Register(const int &iSVId, const int &iSVType, H_SOCK &sock);
    void Unregister(const int &iSVId, const int &iSVType);
    H_SOCK getLinkById(const int &iSVId);
    std::vector<H_SOCK> getLinkByType(const int &iSVType);

private:
#ifdef H_OS_WIN
    #define rpcsvidit std::unordered_map<int , H_SOCK>::iterator
    #define rpcsvid_map std::unordered_map<int , H_SOCK>

    #define rpcsvtypeit std::unordered_map<int , std::list<SVLink> >::iterator
    #define rpcsvtype_map std::unordered_map<int , std::list<SVLink> >
#else
    #define rpcsvidit std::tr1::unordered_map<int , H_SOCK>::iterator
    #define rpcsvid_map std::tr1::unordered_map<int , H_SOCK>

    #define rpcsvtypeit std::tr1::unordered_map<int , std::list<SVLink> >::iterator
    #define rpcsvtype_map std::tr1::unordered_map<int , std::list<SVLink> >
#endif

private:
    rpcsvid_map m_mapSVId;
    rpcsvtype_map m_mapSVType;
    CRWLock m_objLock;
};

H_ENAMSP

#endif//H_RPCLINK_H_
