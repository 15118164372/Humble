--mqtt–≠“È≤‚ ‘
require("public.init")
local mqtt = require("mqtt")
local ws = require("ws")

function initTask()
	
end

function destroyTask()
	
end

local function mqttCONNECT(sock, sockType, fixedHead, info)
	print("CONNECT:")
	table.print(info)
	mqtt.CONNACK(sock, sockType, 0, 0)
end
regMQTTCONNECT(mqttCONNECT)

local function mqttPUBLISH(sock, sockType, fixedHead, info)
	print("PUBLISH:")
	table.print(info)
	if 1 == fixedHead.QoS then
		mqtt.PUBACK(sock, sockType, info.MsgId)
	end
	if 2 == fixedHead.QoS then
		mqtt.PUBREC(sock, sockType, info.MsgId)
	end
end
regMQTTPUBLISH(mqttPUBLISH)

local function mqttPUBACK(sock, sockType, fixedHead, info)
	print("PUBACK:")
	table.print(info)
end
regMQTTPUBACK(mqttPUBACK)

local function mqttPUBREC(sock, sockType, fixedHead, info)
	print("PUBREC:")
	table.print(info)
	mqtt.PUBREL(sock, sockType, info.MsgId)
end
regMQTTPUBREC(mqttPUBREC)

local function mqttPUBREL(sock, sockType, fixedHead, info)
	print("PUBREL:")
	table.print(info)
	mqtt.PUBCOMP(sock, sockType, info.MsgId)
end
regMQTTPUBREL(mqttPUBREL)

local function mqttPUBCOMP(sock, sockType, fixedHead, info)
	print("PUBCOMP:")
	table.print(info)
end
regMQTTPUBCOMP(mqttPUBCOMP)

local function mqttSUBSCRIBE(sock, sockType, fixedHead, info)
	print("SUBSCRIBE:")
	table.print(info)
	mqtt.SUBACK(sock, sockType, info.MsgId, {1})
	--sock, topic, msg, msgid, dup, qos, retain
    qos = math.random(1, 2)
	mqtt.PUBLISH(sock, sockType, "a/b", "message test", 1, 0, qos, 0)
end
regMQTTSUBSCRIBE(mqttSUBSCRIBE)

local function mqttUNSUBSCRIBE(sock, sockType, fixedHead, info)
	print("UNSUBSCRIBE:")
	table.print(info)
	mqtt.UNSUBACK(sock, sockType, info.MsgId)
end
regMQTTUNSUBSCRIBE(mqttUNSUBSCRIBE)

local function mqttPINGREQ(sock, sockType, fixedHead)
	print("PINGREQ:")
	mqtt.PINGRESP(sock, sockType)
end
regMQTTPINGREQ(mqttPINGREQ)

local function mqttDISCONNECT(sock, sockType, fixedHead)
	print("DISCONNECT:")
end
regMQTTDISCONNECT(mqttDISCONNECT)

local function testws(sock, sockType, netMsg)
	ws.Response(sock, 201, "this is server return:"..netMsg)
end
regProto(200, testws)
