
#include "DebugShow.h"
#include "event2/event.h"

H_BNAMSP

void CDebugShow::addDebug(const H_SOCK &sock)
{
    CToSockAdjure *pAdjure = new(std::nothrow) CToSockAdjure(H_INIT_NUMBER, sock);
    if (NULL == pAdjure)
    {
        H_Printf("%s", H_ERR_MEMORY);
        return;
    }

    if (!Adjure(pAdjure))
    {
        H_SafeDelete(pAdjure);
        H_Printf("%s", H_ERR_ADDINQU);
        return;
    }
}

void CDebugShow::onCloseSock(H_SOCK &sock, CDynaBuffer *pSockBuf)
{
    H_Printf("socket %d closed.", sock);

    CUtils::closeSock(sock);
    event_free((struct event *)pSockBuf->getData());
    H_SafeDelete(pSockBuf);
}
void CDebugShow::socketEventCB(H_SOCK sock, short sEvent, void *pArg)
{
    unsigned short usType(H_INIT_NUMBER);
    CDynaBuffer *pSockBuf((CDynaBuffer *)pArg);
    READ_RETURN emRet(CEventService::sockRead(sock, pSockBuf));
    switch (emRet)
    {
        case READ_DATA_RECEIVED://处理数据
        {
            bool bClose(false);
            size_t uiParsed(H_INIT_NUMBER);
            size_t uiTotalParsed(H_INIT_NUMBER);
            CDebugAdjure *pDebug;
            CDebugParser objParser;
            while (true)
            {
                uiParsed = H_INIT_NUMBER;
                pDebug = (CDebugAdjure *)objParser.Parse(sock, usType, 
                    pSockBuf->getBuffer() + uiTotalParsed, pSockBuf->getLens() - uiTotalParsed, uiParsed, bClose);
                if (bClose)
                {
                    onCloseSock(sock, pSockBuf);
                    return;
                }
                if (NULL == pDebug)
                {
                    break;
                }

                uiTotalParsed += uiParsed;
                printf("==>%s\n", pDebug->getDebug().c_str());
            }

            pSockBuf->Remove(uiTotalParsed);
        }
        break;
        case READ_NO_DATA_RECEIVED://无数据
            break;
        case READ_ERROR:
        case READ_MEMORY_ERROR:
        {
            onCloseSock(sock, pSockBuf);
        }
        break;
        default:
            break;
    }
}
void CDebugShow::addInLoop(CToSockAdjure *pToSockAdjure)
{
    H_SOCK sock(pToSockAdjure->getSock());
    CDynaBuffer *pSockBuf = new(std::nothrow) CDynaBuffer;
    if (NULL == pSockBuf)
    {
        CUtils::closeSock(sock);
        H_Printf("%s", H_ERR_ADDINQU);
        return;
    }

    if (!pSockBuf->New(H_SOCKBUFSIZE))
    {
        CUtils::closeSock(sock);
        H_SafeDelete(pSockBuf);
        H_Printf("%s", H_ERR_ADDINQU);
        return;
    }

    (void)evutil_make_socket_nonblocking(sock);
    struct event *pEvent(event_new((struct event_base *)getLoop(), sock, EV_READ | EV_PERSIST, socketEventCB, pSockBuf));
    if (NULL == pEvent)
    {
        CUtils::closeSock(sock);
        H_SafeDelete(pSockBuf);
        H_Printf("%s", "event_new error.");
        return;
    }

    pSockBuf->setData(pEvent);
    event_add(pEvent, NULL);
}

H_ENAMSP
