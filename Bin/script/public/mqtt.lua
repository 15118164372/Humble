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

--MQTT 控制报文的类型
local MsgType = {
	"CONNECT",--客户端请求连接服务端
	"CONNACK",--连接报文确认
	"PUBLISH",--发布消息  QoS 0无返回
	"PUBACK",--QoS 1 消息发布收到确认 
	"PUBREC",--发布收到（保证交付第一步）  QoS 2消息发布返回
	"PUBREL",--发布释放（保证交付第二步）
	"PUBCOMP",--QoS 2 消息发布完成（保证交互第三步）
	"SUBSCRIBE",--客户端订阅请求 
	"SUBACK",--订阅请求报文确认 
	"UNSUBSCRIBE",--客户端取消订阅请求
	"UNSUBACK",--取消订阅报文确认
	"PINGREQ",--心跳请求
	"PINGRESP",--心跳响应
	"DISCONNECT",--客户端断开连接
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
连接确认  
sessPresent 连接确认标志  rtnCode 连接返回码
CleanSession 为1   rtnCode 0  sessPresent 0
CleanSession 为0   已经保存ClientId对应客户端的会话状态 
						rtnCode 0  sessPresent 1
				   没保存则
						rtnCode 0  sessPresent 0 
rtnCode      非0   sessPresent 0 
--]]
function mqtt.CONNACK(sock, sockType, sessPresent, rtnCode)
	if 0 ~= rtnCode then
		sessPresent = 0
	end
	
    --可变头
	--连接确认标志
    local vHead = string.pack("B", sessPresent)
	--连接返回码
    vHead = vHead .. string.pack("B", rtnCode)
    
    --固定头
    local fHead = createHead(MsgType.CONNACK, 0, 0, 0, #vHead)
	
    local sendMsg = fHead .. vHead
	if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
发布消息  
topic 主题  msg 消息 msgid 报文标识符  dup 重发标志  qos 服务质量等级  retain 保留标志
dup: 0表示第一次请求发送这个报文 1 表示可能是早前报文请求的重发
qos : 0最多发一次 1 至少发一次 2只发一次
retain : 
--]]
function mqtt.PUBLISH(sock, sockType, topic, msg, msgid, dup, qos, retain)
	--可变头
	--主题名
    local vHead = string.pack(">H", #topic)
    vHead = vHead .. topic
	--报文标识符
    if 1 == qos or 2 == qos then
        vHead = vHead .. string.pack(">H", msgid)
    end
    
	--有效载荷
	vHead = vHead .. msg
	
	--固定头
    local fHead = createHead(MsgType.PUBLISH, dup, qos, retain, #vHead)
	
    local sendMsg = fHead .. vHead	
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
QoS 1 消息发布收到确认
msgid 报文标识符
--]]
function mqtt.PUBACK(sock, sockType, msgid)
	--可变头
    local vHead = string.pack(">H", msgid)
	--固定头
    local fHead = createHead(MsgType.PUBACK, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead	
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
发布收到（保证交付第一步）  QoS 2消息发布返回
msgid 报文标识符
--]]
function mqtt.PUBREC(sock, sockType, msgid)
	--可变头
    local vHead = string.pack(">H", msgid)
	--固定头
    local fHead = createHead(MsgType.PUBREC, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead	
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
发布释放（保证交付第二步）
msgid 报文标识符
--]]
function mqtt.PUBREL(sock, sockType, msgid)
	--可变头
	local vHead = string.pack(">H", msgid)
	--固定头
    local fHead = createHead(MsgType.PUBREL, 0, 1, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
--QoS 2 消息发布完成（保证交互第三步）
msgid 报文标识符
--]]
function mqtt.PUBCOMP(sock, sockType, msgid)
	--可变头
	local vHead = string.pack(">H", msgid)
	--固定头
    local fHead = createHead(MsgType.PUBCOMP, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
订阅确认
msgid 报文标识符
tRtn  返回码  对应 SUBSCRIBE 请求中的多个主题
值： 0x00 最大QoS 0
	 0x01 成功 QoS 1
	 0x02 成功 QoS 2
	 0x80 失败
--]]
function mqtt.SUBACK(sock, sockType, msgid, tRtn)
	--可变头
    local vHead = string.pack(">H", msgid)
    
	--有效载荷
	for _, val in pairs(tRtn) do
        vHead = vHead .. string.pack("B", val)
    end
	
	--固定头
	local fHead = createHead(MsgType.SUBACK, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
取消订阅确认
msgid 报文标识符
--]]
function mqtt.UNSUBACK(sock, sockType, msgid)
	--可变头
    local vHead = string.pack(">H", msgid)
    --固定头
    local fHead = createHead(MsgType.UNSUBACK, 0, 0, 0, #vHead)
	
	local sendMsg = fHead .. vHead
    if SockType.WS == sockType then
		ws.resWithOutProto(sock, sendMsg)
	else
		sockSend(sock, sendMsg, #sendMsg)
	end
end

--[[
心跳响应
--]]
function mqtt.PINGRESP(sock, sockType)
	--固定头
	local fHead = createHead(MsgType.PINGRESP, 0, 0, 0, 0)
	
	if SockType.WS == sockType then
		ws.resWithOutProto(sock, fHead)
	else
		sockSend(sock, fHead, #fHead)
	end
end

return mqtt
