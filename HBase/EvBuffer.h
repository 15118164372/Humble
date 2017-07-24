
#ifndef H_EVENTBUFFER_H_
#define H_EVENTBUFFER_H_

#include "Macros.h"

H_BNAMSP

/*
bufferevent操作类
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
    /*获取读缓冲区中数据的长度*/
    size_t getTotalLens(void)
    {
        return evbuffer_get_length(m_pReadBuffer);
    };
    /*从读缓冲区中取出iLens个字节的数据*/
    char *readBuffer(const size_t &iLens)
    {
        return (char*)evbuffer_pullup(m_pReadBuffer, (ev_ssize_t)iLens);
    };
    /*删除读缓冲区的数据*/
    void delBuffer(const size_t &iLens)
    {
        if (H_INIT_NUMBER == iLens)
        {
            return;
        }

        (void)evbuffer_drain(m_pReadBuffer, iLens);
    };
    /*查找*/
    struct evbuffer_ptr Search(const char *pszWhat, size_t &iLens, const struct evbuffer_ptr *pStart = NULL)
    {
        return evbuffer_search(m_pReadBuffer, pszWhat, iLens, pStart);
    };

private:
    H_DISALLOWCOPY(CEvBuffer);

private:
    struct evbuffer *m_pReadBuffer;
};

H_ENAMSP

#endif//H_EVENTBUFFER_H_
