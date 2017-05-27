
#ifndef H_SENDER_H_
#define H_SENDER_H_

#include "SendWorker.h"
#include "Singleton.h"
#include "CRC.h"

H_BNAMSP

//ÍøÂç·¢ËÍ·þÎñ
class CSender : public CSingleton<CSender>
{
public:
    CSender(void);
    ~CSender(void);

    void startSender(const unsigned short usNum);
    void stopSender(void);

    void Send(H_SOCK &sock, const char *pszBuf, const size_t &iLens, const bool bCopy = true)
    {
        m_pWorker[getIndex(sock)].addSend(sock, pszBuf, iLens, bCopy);
    };
    void sendCMDRtn(H_SOCK &sock, const char *pszBuf, const size_t &iLens);
    void sendRPCCall(H_SOCK &sock, unsigned int &uiId, const char *pszRPCName, const char *pszToTask, const char *pszSrcTask, 
        const char *pMsg, const size_t &iLens);
    void sendRPCRtn(H_SOCK &sock, H_RPC *pRPC, const char *pszMsg, const size_t &iLens);

private:
    H_DISALLOWCOPY(CSender);
    H_INLINE unsigned short getIndex(H_SOCK &sock)
    {
        return (1 == m_usWorkerNum) ? 0 : (H_CRC16((const char*)&sock, sizeof(sock)) % m_usWorkerNum);
    };

private:
    unsigned short m_usWorkerNum;
    CSendWorker *m_pWorker;
};

H_ENAMSP

#endif//H_SENDER_H_
