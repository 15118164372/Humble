
#ifndef H_DEBUGSHOW_H_
#define H_DEBUGSHOW_H_

#include "EventService.h"
#include "Parser_Debug.h"
#include "Adjure_NetWorker.h"
#include "Adjure_Task.h"

H_BNAMSP

class CDebugShow : public CEventService
{
public:
    CDebugShow(void) : CEventService(H_QULENS_DEBUGSHOW)
    {
        setSVName(H_SERVICE_DEBUGSHOW);
    };
    ~CDebugShow(void)
    {};

    void addDebug(const H_SOCK &sock);
    void onAdjure(CAdjure *pAdjure)
    {
        addInLoop((CToSockAdjure *)pAdjure);
    };
    void afterAdjure(CAdjure *pAdjure)
    {
        H_SafeDelete(pAdjure);
    };

private:
    static void onCloseSock(H_SOCK &sock, CDynaBuffer *pSockBuf);
    static void socketEventCB(H_SOCK sock, short sEvent, void *pArg);
    void addInLoop(CToSockAdjure *pToSockAdjure);
};

H_ENAMSP

#endif//H_DEBUGSHOW_H_
