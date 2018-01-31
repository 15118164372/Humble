
#ifndef H_ENUM_H_
#define H_ENUM_H_

#include "Macros.h"

H_BNAMSP

//任务类型
enum TASK_MSG_TYPE
{
    MSG_TASK_INIT = 1, //任务初始化
    MSG_TASK_DEL,      //任务删除

    MSG_TIME_TIMEOUT,  //超时

    MSG_NET_ACCEPT,    //accept成功
    MSG_NET_CONNECT,   //connect成功
    MSG_NET_CLOSE,     //连接关闭
    MSG_NET_READ_I,    //网络可读(数字请求码)
    MSG_NET_READ_HTTPD,  //网络可读(http服务端)
    MSG_NET_READ_HTTPC,  //网络可读(http客户端)    
    MSG_NET_RPC,         //网络rpc

    MSG_TASK_RPCCALL,  //任务间rpc调用
    MSG_TASK_RPCRTN,   //任务间rpc返回

    MSG_TASK_DEBUG,    //debug


    MSG_NET_READ_RTNBUF,   //返回数据包
    MSG_NET_READ_CONTINUE, //继续解包
};

//日志级别
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
