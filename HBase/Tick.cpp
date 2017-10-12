
#include "Tick.h"
#include "Funcs.h"
#include "Linker.h"
#include "MSGDispatch.h"
#include "TaskMgr.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CTick)
CTick objTick;

CTick::CTick(void) : m_uiFrame(H_INIT_NUMBER), m_uiLoad(H_INIT_NUMBER)
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
    TickEvent *pTickEvent((TickEvent *)arg);
    pTickEvent->stTick.uiCount++;
    if (MSG_TIME_FRAME == pTickEvent->usType)
    {
        CMSGDispatch::getSingletonPtr()->sendEvent(pTickEvent->usType, (void*)&pTickEvent->stTick, sizeof(pTickEvent->stTick));
        return;
    }

    CLinker::getSingletonPtr()->reLink();
    if (H_INIT_NUMBER == pTickEvent->stTick.uiCount % pTickEvent->uiLoad)
    {
        CTaskMgr::getSingletonPtr()->adjustLoad(pTickEvent->uiLoad);
    }
}

void CTick::onStart(void)
{
    H_ASSERT((H_INIT_NUMBER != m_uiFrame && H_INIT_NUMBER != m_uiLoad), "time counfig error.");

    TickEvent *pTickFrame = new(std::nothrow) TickEvent;
    H_ASSERT(NULL != pTickFrame, "malloc memory error.");
    pTickFrame->usType = MSG_TIME_FRAME;
    pTickFrame->stTick.uiCount = H_INIT_NUMBER;
    pTickFrame->uiLoad = H_INIT_NUMBER;
    pTickFrame->stTick.uiMS = m_uiFrame;
    initTimeEv(m_uiFrame, timeCB, pTickFrame);
    m_vcTickEvent.push_back(pTickFrame);

    TickEvent *pTickSec = new(std::nothrow) TickEvent;
    H_ASSERT(NULL != pTickSec, "malloc memory error.");
    pTickSec->usType = H_INIT_NUMBER;
    pTickSec->uiLoad = m_uiLoad;
    pTickSec->stTick.uiCount = H_INIT_NUMBER;
    pTickSec->stTick.uiMS = H_SECOND;
    initTimeEv(pTickSec->stTick.uiMS, timeCB, pTickSec);
    m_vcTickEvent.push_back(pTickSec);
}

H_ENAMSP
