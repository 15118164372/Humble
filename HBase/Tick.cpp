
#include "Tick.h"
#include "Funcs.h"
#include "Linker.h"
#include "MSGDispatch.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CTick)
CTick objTick;

CTick::CTick(void) : m_uiFrame(H_INIT_NUMBER)
{

}

CTick::~CTick(void)
{
    std::vector<TickEvent *>::iterator itEvent;
    for (itEvent = m_vcTickEvent.begin(); m_vcTickEvent.end() != itEvent; itEvent++)
    {
        event_free((*itEvent)->pEvent);
        H_SafeDelete(*itEvent);
    }
    m_vcTickEvent.clear();
}

void CTick::initTimeEv(const unsigned int uiMS, event_callback_fn func, TickEvent *pTickEvent)
{
    timeval tVal;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = uiMS / 1000;
        tVal.tv_usec = (uiMS % 1000) * 1000;
    }
    else
    {
        tVal.tv_usec = uiMS * 1000;
    }

    pTickEvent->pEvent = event_new(getBase(),
        -1, EV_PERSIST, func, pTickEvent);
    H_ASSERT(NULL != pTickEvent->pEvent, "event_new error.");
    (void)event_add(pTickEvent->pEvent, &tVal);
}

void CTick::timeCB(H_SOCK, short, void *arg)
{
    TickEvent *pTickEvent = (TickEvent *)arg;
    pTickEvent->stTick.uiCount++;

    if (MSG_TIME_SEC == pTickEvent->usType)
    {
        CLinker::getSingletonPtr()->reLink();
    }

    CMSGDispatch::getSingletonPtr()->sendEvent(pTickEvent->usType, (void*)&pTickEvent->stTick, sizeof(pTickEvent->stTick));
}

void CTick::onStart(void)
{
    if (H_INIT_NUMBER == m_uiFrame)
    {
        return;
    }

    TickEvent *pTickFrame = new(std::nothrow) TickEvent;
    H_ASSERT(NULL != pTickFrame, "malloc memory error.");
    pTickFrame->usType = MSG_TIME_FRAME;
    pTickFrame->stTick.uiCount = 0;
    pTickFrame->stTick.uiMS = m_uiFrame;
    initTimeEv(m_uiFrame, timeCB, pTickFrame);
    m_vcTickEvent.push_back(pTickFrame);

    TickEvent *pTickSec = new(std::nothrow) TickEvent;
    H_ASSERT(NULL != pTickSec, "malloc memory error.");
    pTickSec->usType = MSG_TIME_SEC;
    pTickSec->stTick.uiCount = 0;
    pTickSec->stTick.uiMS = 1000;
    initTimeEv(pTickSec->stTick.uiMS, timeCB, pTickSec);
    m_vcTickEvent.push_back(pTickSec);
}

H_ENAMSP
