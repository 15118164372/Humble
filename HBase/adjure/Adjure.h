
#ifndef H_ADJURE_H_
#define H_ADJURE_H_

#include "Buffer.h"

H_BNAMSP

//命令基类
class CAdjure : public CObject
{
public:
    CAdjure(const unsigned short usAdjure) : m_bNorProc(true), m_usAdjure(usAdjure)
    {};
    ~CAdjure(void) 
    {};

    //命令
    const unsigned short &getAdjure(void)
    {
        return m_usAdjure;
    };
    //是否正常流程
    void setNorProc(const bool bNorProc)
    {
        m_bNorProc = bNorProc;
    };
    const bool &getNorProc(void)
    {
        return m_bNorProc;
    };

private:
    CAdjure(void);
    bool m_bNorProc;
    unsigned short m_usAdjure;
};

H_ENAMSP

#endif//H_ADJURE_H_
