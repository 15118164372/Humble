
#ifndef H_ENUM_H_
#define H_ENUM_H_

#include "Macros.h"

H_BNAMSP

//�¼�
enum 
{
    MSG_NET_ACCEPT = 0,
    MSG_NET_LINKED,    
    MSG_NET_CLOSE,
    //ǰ��3��˳��Ҫ��

    MSG_NET_READ,

    MSG_NET_CMD,

    MSG_NET_RPCCALL,
    MSG_NET_RPCRTN,

    MSG_TASK_RPCCALL,
    MSG_TASK_RPCRTN,

    MSG_TIME_FRAME,
    MSG_TIME_SEC,

    MSG_COUNT,
};

//��������Ϣid
enum
{
    PROTO_CMD = 0,
    PROTO_RPCCAL,
    PROTO_RPCRTN,
    PROTO_HTTP,
};

//��������������
enum
{
    SOCKTYPE_CMD = 0,
    SOCKTYPE_RPC,
    SOCKTYPE_HTTP,
};

H_ENAMSP

#endif//H_ENUM_H_
