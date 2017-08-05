
#ifndef H_TICK_H_
#define H_TICK_H_

#include "HStruct.h"
#include "NetBase.h"
#include "Singleton.h"

H_BNAMSP

class CTick : public CNetBase, public CSingleton<CTick>
{
public:
    CTick(void);
    ~CTick(void);

    void setTime(const unsigned int uiFrame, const unsigned int uiLoad)
    {
        m_uiFrame = uiFrame;
        m_uiLoad = uiLoad;
    };

    void onStart(void);
    static void timeCB(H_SOCK, short, void *arg);

private:
    struct TickEvent
    {
        unsigned short usType;
        struct event *pEvent;
        unsigned int uiLoad;
        H_TICK stTick;
    };

private:
    void initTimeEv(const unsigned int uiMS, event_callback_fn func, TickEvent *pTickEvent);

private:
    H_DISALLOWCOPY(CTick);

private:
    unsigned int m_uiFrame;
    unsigned int m_uiLoad;
    std::vector<TickEvent *> m_vcTickEvent;
};

H_ENAMSP

#endif//H_TICK_H_
