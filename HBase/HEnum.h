
#ifndef H_ENUM_H_
#define H_ENUM_H_

#include "Macros.h"

H_BNAMSP

//事件
enum 
{
    MSG_NET_ACCEPT = 0,//接收到新连接
    MSG_NET_LINKED,    //连接成功
    MSG_NET_CLOSE,     //连接关闭
    //前面3个顺序不要变

    MSG_NET_READ,      //有新消息

    MSG_NET_CMD,       //命令

    MSG_NET_RPCCALL,   //网络rpc调用
    MSG_NET_RPCRTN,    //网络rpc返回

    MSG_TASK_RPCCALL,  //任务间rpc调用
    MSG_TASK_RPCRTN,   //任务间rpc返回

    MSG_TIME_FRAME,    //帧事件

    MSG_TASK_INIT,     //任务初始化
    MSG_TASK_DEL,      //任务删除

    MSG_MQTT_CONNECT,     //客户端请求连接服务端
    MSG_MQTT_PUBLISH,     //发布消息
    MSG_MQTT_PUBACK,      //QoS 1 消息发布收到确认
    MSG_MQTT_PUBREC,      //发布收到（保证交付第一步）
    MSG_MQTT_PUBREL,      //两个方向都允许 发布释放（保证交付第二步）
    MSG_MQTT_PUBCOMP,     //QoS 2 消息发布完成（保证交互第三步）
    MSG_MQTT_SUBSCRIBE,   //订阅请求
    MSG_MQTT_UNSUBSCRIBE, //取消订阅
    MSG_MQTT_PINGREQ,     //心跳
    MSG_MQTT_DISCONNECT,  //客户端断开连接

    MSG_COUNT,
};

//保留的连接类型
enum
{
    SOCKTYPE_CMD = 0,
    SOCKTYPE_RPC,
    SOCKTYPE_HTTP,
    SOCKTYPE_WS,
    SOCKTYPE_MQTT,
    SOCKTYPE_WSMQTT,
};

//MQTT 控制报文的类型
enum
{
    MQTT_CONNECT = 1,//客户端请求连接服务端 
    MQTT_CONNACK,    //连接报文确认 
    MQTT_PUBLISH,    //发布消息 
    MQTT_PUBACK,     //QoS 1 消息发布收到确认 
    MQTT_PUBREC,     //发布收到（保证交付第一步）  QoS 2消息发布返回
    MQTT_PUBREL,     //发布释放（保证交付第二步）
    MQTT_PUBCOMP,    //QoS 2 消息发布完成（保证交互第三步）
    MQTT_SUBSCRIBE,  //客户端订阅请求 
    MQTT_SUBACK,     //订阅请求报文确认 
    MQTT_UNSUBSCRIBE,//客户端取消订阅请求 
    MQTT_UNSUBACK,   //取消订阅报文确认 
    MQTT_PINGREQ,    //心跳请求 
    MQTT_PINGRESP,   //心跳响应
    MQTT_DISCONNECT, //客户端断开连接
};

//内部消息ID
enum
{
    PROTO_CMD = 0,
    PROTO_RPCCAL,
    PROTO_RPCRTN,
    PROTO_HTTP,
};

H_ENAMSP

#endif//H_ENUM_H_
