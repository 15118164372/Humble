
#ifndef H_ADJURE_MAIL_H_
#define H_ADJURE_MAIL_H_

#include "Adjure.h"

H_BNAMSP

//邮件内容
class CMailAdjure : public CAdjure
{
public:
    CMailAdjure(void) : CAdjure(H_INIT_NUMBER), m_usAuthType(1)
    {};
    ~CMailAdjure(void)
    {};

    //AuthType
    void setAuthLogin(void)
    {
        m_usAuthType = 1;
    };
    void setAuthPlain(void)
    {
        m_usAuthType = 2;
    };
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

    friend class CMailer;

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

H_ENAMSP

#endif//H_ADJURE_MAIL_H_
