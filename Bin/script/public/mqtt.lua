--[[
mqtt
--]]

require("macros")
local ws = require("ws")
local string = string
local table = table
local math = math
local sockSend = sockSend
local SockType = SockType

local mqtt = {}

--MQTT ���Ʊ��ĵ�����
local MsgType = {
	"CONNECT",--�ͻ����������ӷ����
	"CONNACK",--���ӱ���ȷ��
	"PUBLISH",--������Ϣ  QoS 0�޷���
	"PUBACK",--QoS 1 ��Ϣ�����յ�ȷ�� 
	"PUBREC",--�����յ�����֤������һ����  QoS 2��Ϣ��������
	"PUBREL",--�����ͷţ���֤�����ڶ�����
	"PUBCOMP",--QoS 2 ��Ϣ������ɣ���֤������������
	"SUBSCRIBE",--�ͻ��˶������� 
	"SUBACK",--����������ȷ�� 
	"UNSUBSCRIBE",--�ͻ���ȡ����������
	"UNSUBACK",--ȡ�����ı���ȷ��
	"PINGREQ",--��������
	"PINGRESP",--������Ӧ
	"DISCONNECT",--�ͻ��˶Ͽ�����
}
MsgType = table.enum(MsgType, 1)

local function createHead(msgtype, dup, qos, retain, lens)
    local cType = (msgtype << 4) | (dup << 3) | (qos << 1) | retain
    
    local head = string.pack("B", cType)
    local cDigit = 0
	
    repeat
       cDigit = lens % 128
       lens =  math.floor(lens / 128)
       if lens > 0 then
            cDigit = cDigit | 0x80
       end
       
	   head = head .. string.pack("B", cDigit)
    until(lens <= 0)
    
    return head
end

--[[
����ȷ��  
sessPresent ����ȷ�ϱ�־  rtnCode ���ӷ�����
CleanSession Ϊ1   rtnCode 0  sessPresent 0
CleanSession Ϊ0   �Ѿ�����ClientId��Ӧ�ͻ��˵ĻỰ״̬ 
						rtnCode 0  sessPresent 1
				   û������
						rtnCode 0  sessPresent 0 
rtnCode      ��0   sessPresent 0 
--]]
function mqtt.CONNACK(sock, sockType, sessPresent, rtnCode)
	if 0 ~= rtnCode then
		sessPresent = 0
	end
	
    --�ɱ�ͷ
	--����ȷ�ϱ�־
    local vHead = string.pack("B", sessPresent)
	--���ӷ�����
    vHead = vHead .. string.pack("B", rtnCode)
    
    --�̶�ͷ
    local fHead = createHead(MsgType.CONNACK, 0, 0, 0, #vHead)
	
    local sendMsg = fHead .. vHead
	if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
������Ϣ  
topic ����  msg ��Ϣ msgid ���ı�ʶ��  dup �ط���־  qos ���������ȼ�  retain ������־
dup: 0��ʾ��һ��������������� 1 ��ʾ��������ǰ����������ط�
qos : 0��෢һ�� 1 ���ٷ�һ�� 2ֻ��һ��
retain : 
--]]
function mqtt.PUBLISH(sock, sockType, topic, msg, msgid, dup, qos, retain)
	--�ɱ�ͷ
	--������
    local vHead = string.pack(">H", #topic)
    vHead = vHead .. topic
	--���ı�ʶ��
    if 1 == qos or 2 == qos then
        vHead = vHead .. string.pack(">H", msgid)
    end
    
	--��Ч�غ�
	vHead = vHead .. msg
	
	--�̶�ͷ
    local fHead = createHead(MsgType.PUBLISH, dup, qos, retain, #vHead)
	
    local sendMsg = fHead .. vHead	
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
QoS 1 ��Ϣ�����յ�ȷ��
msgid ���ı�ʶ��
--]]
function mqtt.PUBACK(sock, sockType, msgid)
	--�ɱ�ͷ
    local vHead = string.pack(">H", msgid)
	--�̶�ͷ
    local fHead = createHead(MsgType.PUBACK, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead	
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
�����յ�����֤������һ����  QoS 2��Ϣ��������
msgid ���ı�ʶ��
--]]
function mqtt.PUBREC(sock, sockType, msgid)
	--�ɱ�ͷ
    local vHead = string.pack(">H", msgid)
	--�̶�ͷ
    local fHead = createHead(MsgType.PUBREC, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead	
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
�����ͷţ���֤�����ڶ�����
msgid ���ı�ʶ��
--]]
function mqtt.PUBREL(sock, sockType, msgid)
	--�ɱ�ͷ
	local vHead = string.pack(">H", msgid)
	--�̶�ͷ
    local fHead = createHead(MsgType.PUBREL, 0, 1, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
--QoS 2 ��Ϣ������ɣ���֤������������
msgid ���ı�ʶ��
--]]
function mqtt.PUBCOMP(sock, sockType, msgid)
	--�ɱ�ͷ
	local vHead = string.pack(">H", msgid)
	--�̶�ͷ
    local fHead = createHead(MsgType.PUBCOMP, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
����ȷ��
msgid ���ı�ʶ��
tRtn  ������  ��Ӧ SUBSCRIBE �����еĶ������
ֵ�� 0x00 ���QoS 0
	 0x01 �ɹ� QoS 1
	 0x02 �ɹ� QoS 2
	 0x80 ʧ��
--]]
function mqtt.SUBACK(sock, sockType, msgid, tRtn)
	--�ɱ�ͷ
    local vHead = string.pack(">H", msgid)
    
	--��Ч�غ�
	for _, val in pairs(tRtn) do
        vHead = vHead .. string.pack("B", val)
    end
	
	--�̶�ͷ
	local fHead = createHead(MsgType.SUBACK, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
ȡ������ȷ��
msgid ���ı�ʶ��
--]]
function mqtt.UNSUBACK(sock, sockType, msgid)
	--�ɱ�ͷ
    local vHead = string.pack(">H", msgid)
    --�̶�ͷ
    local fHead = createHead(MsgType.UNSUBACK, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
������Ӧ
--]]
function mqtt.PINGRESP(sock, sockType)
	--�̶�ͷ
	local fHead = createHead(MsgType.PINGRESP, 0, 0, 0, 0)
	
	if SockType.WS == sockType then
		ws.resWithOutProto(sock, fHead)
	else
		sockSend(sock, fHead, #fHead)
	end
end

return mqtt
