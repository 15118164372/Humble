
#include "Parser_Default.h"
#include "Adjure_Task.h"
#include "Session.h"

H_BNAMSP

CAdjure *CDefaultParser::Parse(class CSession *pSession, const char *pBuf, const size_t &iLens, size_t &iParsed, bool &bClose)
{
    if (m_uiHeadLens > iLens)
    {
        return NULL;
    }

    //��ȡ��ͷ
    H_HEAD_TYPE iPackLens(H_HEAD_NTOH(*((H_HEAD_TYPE *)pBuf)));
    iParsed = iPackLens + sizeof(H_HEAD_TYPE);
    if (iParsed > iLens)//���Ȳ���
    {
        return NULL;
    }

    H_PROTO_TYPE iProto(H_PROTO_NTOH(*((H_PROTO_TYPE *)(pBuf + sizeof(H_HEAD_TYPE)))));
    size_t iMsgLens(iPackLens - sizeof(H_PROTO_TYPE));
    if (H_INIT_NUMBER == iMsgLens)//����Ϣ��
    {
        CCopyINetReadAdjure *pINetRead = new(std::nothrow) CCopyINetReadAdjure(pSession->getSock(), pSession->getType(),
            iProto, NULL, iMsgLens);
        if (NULL == pINetRead)
        {
            bClose = true;
            H_Printf("%s", H_ERR_MEMORY);
            return NULL;
        }

        return pINetRead;
    }

    CCopyINetReadAdjure *pINetRead = new(std::nothrow) CCopyINetReadAdjure(pSession->getSock(), pSession->getType(),
        iProto, pBuf + m_uiHeadLens, iMsgLens);
    if (NULL == pINetRead)
    {
        bClose = true;
        H_Printf("%s", H_ERR_MEMORY);
        return NULL;
    }

    return pINetRead;
}

H_ENAMSP
