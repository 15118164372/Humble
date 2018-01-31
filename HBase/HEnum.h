
#ifndef H_ENUM_H_
#define H_ENUM_H_

#include "Macros.h"

H_BNAMSP

//��������
enum TASK_MSG_TYPE
{
    MSG_TASK_INIT = 1, //�����ʼ��
    MSG_TASK_DEL,      //����ɾ��

    MSG_TIME_TIMEOUT,  //��ʱ

    MSG_NET_ACCEPT,    //accept�ɹ�
    MSG_NET_CONNECT,   //connect�ɹ�
    MSG_NET_CLOSE,     //���ӹر�
    MSG_NET_READ_I,    //����ɶ�(����������)
    MSG_NET_READ_HTTPD,  //����ɶ�(http�����)
    MSG_NET_READ_HTTPC,  //����ɶ�(http�ͻ���)    
    MSG_NET_RPC,         //����rpc

    MSG_TASK_RPCCALL,  //�����rpc����
    MSG_TASK_RPCRTN,   //�����rpc����

    MSG_TASK_DEBUG,    //debug


    MSG_NET_READ_RTNBUF,   //�������ݰ�
    MSG_NET_READ_CONTINUE, //�������
};

//��־����
enum LOG_LEVEL
{
    LOGLV_SYS = 0,
    LOGLV_ERROR,
    LOGLV_WARN,
    LOGLV_INFO,
    LOGLV_DEBUG,
};

H_ENAMSP

#endif//H_ENUM_H_
