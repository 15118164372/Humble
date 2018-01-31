
#ifndef H_SENDER_H_
#define H_SENDER_H_

#include "Log.h"
#include "Adjure_NetWorker.h"

H_BNAMSP

//·¢ËÍ
class CSender : public CService
{
public:
    CSender(void) : CService(H_QULENS_SENDER)
    {
        setSVName(H_SERVICE_SENDER);
    };
    ~CSender(void)
    {};

    void Send(const H_SOCK &fd, const char *pBuf, const size_t &iLens)
    {
        CCopySendAdjure *pSendAdjur = new(std::nothrow) CCopySendAdjure(fd, pBuf, iLens);
        if (NULL == pSendAdjur)
        {
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            return;
        }

        if (!Adjure(pSendAdjur))
        {
            H_SafeDelete(pSendAdjur);
            H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
        }
    };
    void Send(const H_SOCK &fd, CBuffer *pBuffer)
    {
        CNoCopySendAdjure *pSendAdjur = new(std::nothrow) CNoCopySendAdjure(fd, pBuffer);
        if (NULL == pSendAdjur)
        {
            H_SafeDelete(pBuffer);
            H_LOG(LOGLV_ERROR, "%s", H_ERR_MEMORY);
            return;
        }

        if (!Adjure(pSendAdjur))
        {
            H_SafeDelete(pBuffer);
            H_SafeDelete(pSendAdjur);
            H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
        }
    };
    void broadCast(CBroadCastAdjure *pAdjure)
    {
        if (!Adjure(pAdjure))
        {
            H_SafeDelete(pAdjure);
            H_LOG(LOGLV_ERROR, "%s", H_ERR_ADDINQU);
        }
    };
    void onAdjure(CAdjure *pAdjure) 
    {
        switch (pAdjure->getAdjure())
        {
            case NETWORKERADJ_COPYSEND:
            {
                CCopySendAdjure *pSendAdjur((CCopySendAdjure *)pAdjure);
                CBuffer *pBuffer(pSendAdjur->getPack());

                (void)sockSend(pSendAdjur->getSock(), pBuffer->getBuffer(), pBuffer->getLens());
            }
            break;
            case NETWORKERADJ_NOCOPYSEND:
            {
                CNoCopySendAdjure *pSendAdjur((CNoCopySendAdjure *)pAdjure);
                CBuffer *pBuffer(pSendAdjur->getPack());

                (void)sockSend(pSendAdjur->getSock(), pBuffer->getBuffer(), pBuffer->getLens());
                H_SafeDelete(pBuffer);
            }
            break;
            case NETWORKERADJ_BROADCAST:
            {
                CBroadCastAdjure *pBroadCastAdjure((CBroadCastAdjure *)pAdjure);
                std::vector<H_SOCK> *pvsSocks(pBroadCastAdjure->getSocks());
                CBuffer *pBuffer(pBroadCastAdjure->getPack());

                for (std::vector<H_SOCK>::iterator it = pvsSocks->begin(); pvsSocks->end() != it; ++it)
                {
                    (void)sockSend(*it, pBuffer->getBuffer(), pBuffer->getLens());
                }
            }
            break;
            default:
                break;
        }
    };
    void afterAdjure(CAdjure *pAdjure) 
    {
        H_SafeDelete(pAdjure);
    };

private:
    int sockSend(const H_SOCK &fd, const char *pBuf, const size_t &iLens)
    {
        int iRtn(H_RTN_OK);
        int iSendSize(H_INIT_NUMBER);
        int iSendTotalSize(H_INIT_NUMBER);

        do
        {
            iSendSize = send(fd, pBuf + iSendTotalSize, (int)iLens - iSendTotalSize, 0);
            if (iSendSize <= 0)
            {
                iRtn = H_SockError();
                if (IS_EAGAIN(iRtn))
                {
                    continue;
                }

                return iRtn;
            }

            iSendTotalSize += iSendSize;

        } while ((int)iLens > iSendTotalSize);

        return H_RTN_OK;
    };
};

H_ENAMSP

#endif//H_SENDER_H_
