
#ifndef H_NETWORKERMGR_H_
#define H_NETWORKERMGR_H_

#include "Singleton.h"
#include "NetWorker.h"
#include "CRC.h"

H_BNAMSP

class CNetWorkerMgr : public CSingleton<CNetWorkerMgr>
{
public:
    CNetWorkerMgr(void);
    ~CNetWorkerMgr(void);

    void startWorker(const unsigned short usNum);
    void stopWorker(void);

    H_INLINE bool addLink(H_SOCK &sock, class CParser *pParser, const unsigned short &usType, const bool bAccept) 
    {
        if (H_INVALID_SOCK == sock)
        {
            return false;
        }

        return m_pNetWorker[getIndex(sock)].addLink(sock, pParser, usType, bAccept);
    };
    H_INLINE bool closeLink(H_SOCK &sock)
    {
        if (H_INVALID_SOCK == sock)
        {
            return false;
        }

        return m_pNetWorker[getIndex(sock)].closeLink(sock);
    };
    bool removeLink(H_SOCK &sock);

private:
    H_INLINE unsigned short getIndex(H_SOCK &sock)
    {
        return (1 == m_usWorkerNum) ? 0 : (H_CRC16((const char*)&sock, sizeof(sock)) % m_usWorkerNum);
    };

private:
    H_DISALLOWCOPY(CNetWorkerMgr);

private:
    unsigned short m_usWorkerNum;
    CNetWorker *m_pNetWorker;
};

H_ENAMSP

#endif//H_NETWORKERMGR_H_
