
#include "HMailer.h"
#include "Log.h"
#include "jwsmtp/jwsmtp.h"

H_BNAMSP

SINGLETON_INIT(CMailer)
CMailer objMailer;

CMail::CMail(void) : m_usAuthType(1)
{
}
CMail::~CMail(void)
{
}

void CMail::setAuthLogin(void)
{
    m_usAuthType = 1;
}
void CMail::setAuthPlain(void)
{
    m_usAuthType = 2;
}

void CMail::Send(void)
{
    CMailer::getSingletonPtr()->sendMail(this);
}


CMailer::CMailer(void) : CTaskLazy<CMail>(H_ONEK)
{
    m_pMailer = new(std::nothrow) jwsmtp::mailer();
    H_ASSERT(NULL != m_pMailer, "malloc memory error.");
}

CMailer::~CMailer(void)
{
    jwsmtp::mailer *pMailer((jwsmtp::mailer *)m_pMailer);
    H_SafeDelete(pMailer);
    m_pMailer = NULL;
}

void CMailer::sendMail(CMail *pMail)
{
    if (!addTask(pMail))
    {
        H_SafeDelete(pMail);
        H_LOG(LOGLV_ERROR, "%s", "add message to CirQueue error.");
    }
}

void CMailer::runTask(CMail *pMail)
{
    std::vector<std::string>::iterator it;
    jwsmtp::mailer *pMailer((jwsmtp::mailer *)m_pMailer);

    pMailer->reset();

    pMailer->authtype((enum jwsmtp::mailer::authtype)(pMail->m_usAuthType));
    (void)pMailer->setserver(pMail->m_strSMTPSV);
    (void)pMailer->setsender(pMail->m_strFromAddr);
    pMailer->username(pMail->m_strUserName);
    pMailer->password(pMail->m_strPSW);
    (void)pMailer->setsubject(pMail->m_strSubject);
    (void)pMailer->setmessage(pMail->m_strMsg);
    (void)pMailer->setmessageHTML(pMail->m_strHtml);
    (void)pMailer->setmessageHTMLfile(pMail->m_strHtmlFile);
    for (it = pMail->m_vcToAddr.begin(); pMail->m_vcToAddr.end() != it; ++it)
    {
        (void)pMailer->addrecipient(*it);
    }
    for (it = pMail->m_vcAttach.begin(); pMail->m_vcAttach.end() != it; ++it)
    {
        (void)pMailer->attach(*it);
    }

    pMailer->send();

    H_LOG(LOGLV_INFO, "send mail(%s) return: %s", pMail->m_strSubject.c_str(), pMailer->response().c_str());
}

H_ENAMSP
