
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

    void setTime(const unsigned int uiMS) 
    {
        m_uiTick = uiMS;
    };
    void addCount(void) 
    {
        H_AtomicAdd(&m_uiCount, 1);
    };

    void onStart(void);
    void onTime(void);
    static void timeCB(H_SOCK, short, void *arg);

private:
    struct event *initTimeEv(const unsigned int uiMS, event_callback_fn func);

private:
    H_DISALLOWCOPY(CTick);

private:
    struct event *m_pTickEvent;
    unsigned int m_uiTick;
    unsigned int m_uiCount;
};

H_ENAMSP

#endif//H_TICK_H_
