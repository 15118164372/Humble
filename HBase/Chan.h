
#ifndef H_CHAN_H_
#define H_CHAN_H_

#include "Atomic.h"
#include "CirQueue.h"

H_BNAMSP

//��Ϣͨ��
class CChan
{
public:
    CChan(const int iCapacity);
    ~CChan(void);

    bool Send(void *pszVal);
    void *Recv(void);

    void setTaskNam(std::string *pszName);
    const char *getTaskName(void);

    size_t getSize(void);
    size_t getCapacity(void);

private:
    CChan(void);
    H_DISALLOWCOPY(CChan);

private:
    std::string *m_pstrName;
    CAtomic m_objQuLck;
    CCirQueue m_quData;
};

H_ENAMSP

#endif//H_CHAN_H_
