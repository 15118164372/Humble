
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

    void Register(H_SOCK sock, int iSVId);
    void Unregister(int iSVId);
    H_SOCK getSVLink(int iSVId);

private:
#ifdef H_OS_WIN
    #define rpcsvit std::unordered_map<int , H_SOCK>::iterator
    #define rpcsv_map std::unordered_map<int , H_SOCK>
#else
    #define rpcsvit std::tr1::unordered_map<int , H_SOCK>::iterator
    #define rpcsv_map std::tr1::unordered_map<int , H_SOCK>
#endif

private:
    rpcsv_map m_mapSV;
    CRWLock m_objLock;
};

H_ENAMSP

#endif//H_RPCLINK_H_
