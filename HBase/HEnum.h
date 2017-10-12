
#ifndef H_ENUM_H_
#define H_ENUM_H_

#include "Macros.h"

H_BNAMSP

//�¼�
enum 
{
    MSG_NET_ACCEPT = 0,//���յ�������
    MSG_NET_LINKED,    //���ӳɹ�
    MSG_NET_CLOSE,     //���ӹر�
    //ǰ��3��˳��Ҫ��

    MSG_NET_READ,      //������Ϣ

    MSG_NET_CMD,       //����

    MSG_NET_RPCCALL,   //����rpc����
    MSG_NET_RPCRTN,    //����rpc����

    MSG_TASK_RPCCALL,  //�����rpc����
    MSG_TASK_RPCRTN,   //�����rpc����

    MSG_TIME_FRAME,    //֡�¼�

    MSG_TASK_INIT,     //�����ʼ��
    MSG_TASK_DEL,      //����ɾ��

    MSG_MQTT_CONNECT,     //�ͻ����������ӷ����
    MSG_MQTT_PUBLISH,     //������Ϣ
    MSG_MQTT_PUBACK,      //QoS 1 ��Ϣ�����յ�ȷ��
    MSG_MQTT_PUBREC,      //�����յ�����֤������һ����
    MSG_MQTT_PUBREL,      //������������ �����ͷţ���֤�����ڶ�����
    MSG_MQTT_PUBCOMP,     //QoS 2 ��Ϣ������ɣ���֤������������
    MSG_MQTT_SUBSCRIBE,   //��������
    MSG_MQTT_UNSUBSCRIBE, //ȡ������
    MSG_MQTT_PINGREQ,     //����
    MSG_MQTT_DISCONNECT,  //�ͻ��˶Ͽ�����

    MSG_COUNT,
};

//��������������
enum
{
    SOCKTYPE_CMD = 0,
    SOCKTYPE_RPC,
    SOCKTYPE_HTTP,
    SOCKTYPE_WS,
    SOCKTYPE_MQTT,
    SOCKTYPE_WSMQTT,
};

//MQTT ���Ʊ��ĵ�����
enum
{
    MQTT_CONNECT = 1,//�ͻ����������ӷ���� 
    MQTT_CONNACK,    //���ӱ���ȷ�� 
    MQTT_PUBLISH,    //������Ϣ 
    MQTT_PUBACK,     //QoS 1 ��Ϣ�����յ�ȷ�� 
    MQTT_PUBREC,     //�����յ�����֤������һ����  QoS 2��Ϣ��������
    MQTT_PUBREL,     //�����ͷţ���֤�����ڶ�����
    MQTT_PUBCOMP,    //QoS 2 ��Ϣ������ɣ���֤������������
    MQTT_SUBSCRIBE,  //�ͻ��˶������� 
    MQTT_SUBACK,     //����������ȷ�� 
    MQTT_UNSUBSCRIBE,//�ͻ���ȡ���������� 
    MQTT_UNSUBACK,   //ȡ�����ı���ȷ�� 
    MQTT_PINGREQ,    //�������� 
    MQTT_PINGRESP,   //������Ӧ
    MQTT_DISCONNECT, //�ͻ��˶Ͽ�����
};

//�ڲ���ϢID
enum
{
    PROTO_CMD = 0,
    PROTO_RPCCAL,
    PROTO_RPCRTN,
    PROTO_HTTP,
};

H_ENAMSP

#endif//H_ENUM_H_
