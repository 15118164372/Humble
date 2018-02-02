
#ifndef H_MAILER_H_
#define H_MAILER_H_

#include "Service.h"
#include "Log.h"
#include "Adjure_Mail.h"
#include "jwsmtp.h"

H_BNAMSP

//SMTP 邮件
class CMailer : public CService
{
public:
    CMailer(void) : CService(H_QULENS_MAIL)
    {
        setSVName(H_SERVICE_MAIL);
    };
    ~CMailer(void) 
    {};

    //发送邮件请求
    void sendMail(CMailAdjure *pMail)
    {
        Adjure(pMail);
    };

    //执行发送
    void onAdjure(CAdjure *pAdjure) 
    {
        CMailAdjure *pMail((CMailAdjure *)pAdjure);
        jwsmtp::mailer stMailer;

        stMailer.authtype((enum jwsmtp::mailer::authtype)(pMail->m_usAuthType));
        (void)stMailer.setserver(pMail->m_strSMTPSV);
        (void)stMailer.setsender(pMail->m_strFromAddr);
        stMailer.username(pMail->m_strUserName);
        stMailer.password(pMail->m_strPSW);
        (void)stMailer.setsubject(pMail->m_strSubject);
        (void)stMailer.setmessage(pMail->m_strMsg);
        (void)stMailer.setmessageHTML(pMail->m_strHtml);
        (void)stMailer.setmessageHTMLfile(pMail->m_strHtmlFile);

        std::vector<std::string>::iterator it;
        for (it = pMail->m_vcToAddr.begin(); pMail->m_vcToAddr.end() != it; ++it)
        {
            (void)stMailer.addrecipient(*it);
        }
        for (it = pMail->m_vcAttach.begin(); pMail->m_vcAttach.end() != it; ++it)
        {
            (void)stMailer.attach(*it);
        }

        stMailer.send();
        H_LOG(LOGLV_SYS, "send mail(%s) return: %s", pMail->m_strSubject.c_str(), stMailer.response().c_str());        
    };
    
    void afterAdjure(CAdjure *pAdjure)
    {
        H_SafeDelete(pAdjure);
    }
};

H_ENAMSP

#endif//H_MAILER_H_
