
#ifndef H_CHAN_H_
#define H_CHAN_H_

#include "Atomic.h"

H_BNAMSP

//��Ϣͨ��
class CChan
{
public:
    CChan(void);
    ~CChan(void);

    void Send(void *pszVal);
    void *Recv(void);

    void setTaskNam(std::string *pszName);

    size_t getSize(void);

private:
    H_DISALLOWCOPY(CChan);

private:
    std::string *m_pstrName;
    CAtomic m_objQuLck;
    std::queue<void *> m_quData;   
};

H_ENAMSP

#endif//H_CHAN_H_
