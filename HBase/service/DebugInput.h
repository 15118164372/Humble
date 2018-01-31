
#ifndef H_DEBUGINPUT_H_
#define H_DEBUGINPUT_H_

#include "Sender.h"
#include "DebugShow.h"

H_BNAMSP

class CDebugInput : public CObject
{
public:
    CDebugInput(const char *pszHost, unsigned short &usPort);
    ~CDebugInput(void) 
    {};

    int EnterInput(void);

private:
    CDebugInput(void);
    bool sendCmd(const char *pszCmd, const char *pszTo, const char *pszContent);

private:
    H_SOCK m_Sock;
    CSender m_objSender;
    CDebugShow m_objShow;
};

H_ENAMSP

#endif//H_DEBUGINPUT_H_
