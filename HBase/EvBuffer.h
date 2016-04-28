
#ifndef H_EVENTBUFFER_H_
#define H_EVENTBUFFER_H_

#include "Binary.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"

H_BNAMSP

/*
bufferevent������
*/
class CEvBuffer
{
public:
    CEvBuffer(void) : m_pReadBuffer(NULL)
    {};
    ~CEvBuffer(void) 
    {};

    void setEvBuf(struct bufferevent *pBev)
    {
        m_pReadBuffer = bufferevent_get_input(pBev);
    };
    /*��ȡ�������������ݵĳ���*/
    size_t getTotalLens(void)
    {
        return evbuffer_get_length(m_pReadBuffer);
    };
    /*�Ӷ���������ȡ��iLens���ֽڵ�����*/
    CBinary *readBuffer(const size_t iLens)
    {
        const char *pBuffer = (const char*)evbuffer_pullup(m_pReadBuffer, (ev_ssize_t)iLens);
        if (NULL == pBuffer)
        {
            return NULL;
        }

        m_objBinary.setReadBuffer(pBuffer, iLens);

        return &m_objBinary;
    };
    /*ɾ����������������*/
    void delBuffer(const size_t iLens)
    {
        (void)evbuffer_drain(m_pReadBuffer, iLens);
    };
    /*����*/
    struct evbuffer_ptr Search(const char *pszWhat, size_t iLens, const struct evbuffer_ptr *pStart = NULL)
    {
        return evbuffer_search(m_pReadBuffer, pszWhat, iLens, pStart);
    };

private:
    H_DISALLOWCOPY(CEvBuffer);

private:
    struct evbuffer *m_pReadBuffer;
    CBinary m_objBinary;
};

H_ENAMSP

#endif//H_EVENTBUFFER_H_
