
#include "Tick.h"
#include "Funcs.h"
#include "Linker.h"
#include "MSGDispatch.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CTick)
CTick objTick;

CTick::CTick(void) : m_pTickEvent(NULL), m_uiTick(H_INIT_NUMBER), m_uiCount(H_INIT_NUMBER)
{

}

CTick::~CTick(void)
{
    if (NULL != m_pTickEvent)
    {
        event_free(m_pTickEvent);
        m_pTickEvent = NULL;
    }
}

struct event *CTick::initTimeEv(const unsigned int uiMS, event_callback_fn func)
{
    timeval tVal;
    evutil_timerclear(&tVal);
    if (uiMS >= 1000)
    {
        tVal.tv_sec = uiMS / 1000;
        tVal.tv_usec = (uiMS % 1000) * (1000);
    }
    else
    {
        tVal.tv_usec = (uiMS * 1000);
    }

    struct event *pEvent = event_new(getBase(),
        -1, EV_PERSIST, func, this);
    H_ASSERT(NULL != pEvent, "event_new error.");
    (void)event_add(pEvent, &tVal);

    return pEvent;
}

void CTick::onTime(void)
{
    addCount();

    H_TICK stTick;
    stTick.uiMS = m_uiTick;
    stTick.uiCount = H_AtomicGet(&m_uiCount);
    CMSGDispatch::getSingletonPtr()->sendEvent(MSG_TIME_FRAME, (void*)&stTick, sizeof(stTick));

    if (H_INIT_NUMBER == ((stTick.uiMS * stTick.uiCount) % 1000))
    {
        CLinker::getSingletonPtr()->reLink();
        CMSGDispatch::getSingletonPtr()->sendEvent(MSG_TIME_SEC, (void*)&stTick, sizeof(stTick));
    }
}

void CTick::timeCB(H_SOCK, short, void *arg)
{
    CTick *pTick = (CTick *)arg;
    pTick->onTime();
}

void CTick::onStart(void)
{
    if (H_INIT_NUMBER == m_uiTick)
    {
        return;
    }

    m_pTickEvent = initTimeEv(m_uiTick, timeCB);
}

H_ENAMSP
