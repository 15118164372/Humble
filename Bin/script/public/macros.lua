--ȫ�ֶ���
local table = table

function table.enum(tMsg, iBegin) 
    assert("table" == type(tMsg))    
    local tEnum = {} 
    local iEnumIndex = iBegin or 0 
    for key, val in pairs(tMsg) do 
        tEnum[val] = iEnumIndex + key - 1
    end 
    
    return tEnum 
end 

--��ЧID
Invalid_ID = -1
--��Чsocket
Invalid_Sock = -1

--������
ErrCode = {
    "Fail",
    "Ok",
    "Error",
}
ErrCode = table.enum(ErrCode, -1)

--aes key����
AES = {
    Key128 = 128,
    Key192 = 192,
    Key256 = 256,
}

--rsa key����
RSA = {
    Key512 = 512,
    Key1024 = 1024,
}

--des ����
DES = {
    "Des",
    "D2Des",
    "D3Des",
}
DES = table.enum(DES, 0)
--des�������ܻ��ǽ���
DESMode = {
    "Encode",
    "Decode",
}
DESMode = table.enum(DESMode, 0)

--��������
SockType = {
	"CMD",
	"RPC",
	"HTTP",
	"WS",
	"MQTT",
	"WSMQTT",
	--������C++��Ӧ ��������������
	
	
}
SockType = table.enum(SockType, 0)

--�¼� ��c++�еĶ�Ӧ
Event = {
	"NET_ACCEPT",--���յ�������
	"NET_LINKED",--���ӳɹ�
	"NET_CLOSE",--���ӹر�
	
	"NET_READ",--������Ϣ
	
	"NET_CMD",--����
	
	"NET_RPCCALL",--����rpc����
	"NET_RPCRTN",--����rpc����
	
	"TASK_RPCCALL",--�����rpc����
	"TASK_RPCRTN",--�����rpc����
	
	"TIME_FRAME",--֡�¼�
	"TIME_SEC",--���¼�
	
	"TASK_INIT",--�����ʼ��
	"TASK_DEL",--����ɾ��
	
	"MQTT_CONNECT",     --�ͻ����������ӷ����
    "MQTT_PUBLISH",     --������Ϣ
    "MQTT_PUBACK",      --QoS 1 ��Ϣ�����յ�ȷ��
    "MQTT_PUBREC",      --�����յ�����֤������һ����
    "MQTT_PUBREL",      --������������ �����ͷţ���֤�����ڶ�����
    "MQTT_PUBCOMP",     --QoS 2 ��Ϣ������ɣ���֤������������
    "MQTT_SUBSCRIBE",   --��������
    "MQTT_UNSUBSCRIBE", --ȡ������
    "MQTT_PINGREQ",     --����
    "MQTT_DISCONNECT",  --�ͻ��˶Ͽ�����
}
Event = table.enum(Event, 0)
