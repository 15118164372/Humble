
#ifndef H_MAILER_H_
#define H_MAILER_H_

#include "TaskLazy.h"
#include "Singleton.h"

H_BNAMSP

class CMail
{
public:
    CMail(void);
    ~CMail(void);

    void setAuthLogin(void);
    void setAuthPlain(void);
    //SMTP server
    void setSMTPSV(const char *pszSV)
    {
        m_strSMTPSV = pszSV;
    };

    //发送者
    void setFromAddr(const char *pszFromAddr)
    {
        m_strFromAddr = pszFromAddr;
    };
    void setUserName(const char *pszUserName) 
    {
        m_strUserName = pszUserName;
    };
    void setPSW(const char *pszPSW)
    {
        m_strPSW = pszPSW;
    };

    //主题
    void setSubject(const char *pszSubject)
    {
        m_strSubject = pszSubject;
    };
    //信息
    void setMsg(const char *pszMsg)
    {
        m_strMsg = pszMsg;
    };

    //html
    void setHtml(const char *pszHtml)
    {
        m_strHtml = pszHtml;
    };
    void setHtmlFile(const char *pszFile)
    {
        m_strHtmlFile = pszFile;
    };

    //接收者
    void addToAddr(const char *pszToAddr)
    {
        m_vcToAddr.push_back(std::string(pszToAddr));
    };

    //附件
    void addAttach(const char *pszFile) 
    {
        m_vcAttach.push_back(std::string(pszFile));
    };

    //发送
    void Send(void);

    friend class CMailer;
private:
    H_DISALLOWCOPY(CMail);

protected:
    unsigned short m_usAuthType;
    std::string m_strSMTPSV;
    std::string m_strFromAddr;
    std::string m_strUserName;
    std::string m_strPSW;
    std::string m_strSubject;
    std::string m_strMsg;
    std::string m_strHtml;
    std::string m_strHtmlFile;
    std::vector<std::string> m_vcToAddr;
    std::vector<std::string> m_vcAttach;
};

class CMailer : public CTaskLazy<CMail>, public CSingleton<CMailer>
{
public:
    CMailer(void);
    ~CMailer(void);

    void runTask(CMail *pMail);
    void sendMail(CMail *pMail);

private:
    H_DISALLOWCOPY(CMailer);
    void *m_pMailer;
};

H_ENAMSP

#endif//H_MAILER_H_
