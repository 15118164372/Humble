--任务初始化 每个任务都要require
require("macros")
require("timewheel")
local humble = require("humble")
local utile = require("utile")
local hotfix = require("hotfix")
local cjson = require("cjson")
local httpd = require("httpd")
local table = table
local Event = Event
local SockType = SockType
local m_strTaskName = g_taskName
local m_RPCTimeOut = 5

--命令执行c++调用函数
function onCMD(cmd ,info)
	if "hotfix" == cmd then
		local bRtn = utile.callFunc(hotfix.hotfix_module, info)
		if bRtn then
			return cjson.encode({"ok"})
		end
		
		return cjson.encode({"fail"})		
	end
	
	local Func = load(info)
    if Func then
        local bRtn, rtnMsg = utile.callFunc(Func)
        if bRtn then
			return cjson.encode({"ok", rtnMsg})
        end
		
		return cjson.encode({"fail"})
    end
	
	return cjson.encode({"fail"})
end


--网络事件 NET_ACCEPT  NET_LINKED NET_CLOSE
if not g_NetEvent then
	g_NetEvent = {}
end
local m_NetEvent = g_NetEvent

--网络事件c++调用函数
function onNetEvent(usEvent, sock, sockType)
	if m_NetEvent[usEvent] then
		local func = m_NetEvent[usEvent][sockType]
		if func then
			utile.callFunc(func, sock, sockType)
		end
	end
end
local function regNetEvent(usEvent, sockType, func)
	if not m_NetEvent[usEvent] then
		m_NetEvent[usEvent] = {}
	end
	
	if not m_NetEvent[usEvent][sockType] then
		m_NetEvent[usEvent][sockType] = func
		humble.regEvent(usEvent, m_strTaskName, sockType)
	end
end
--注册
--func  func(sock, sockType)
function regAcceptEv(sockType, func)
	regNetEvent(Event.NET_ACCEPT, sockType, func)
end
function regLinkedEv(sockType, func)
	regNetEvent(Event.NET_LINKED, sockType, func)
end
function regClosedEv(sockType, func)
	regNetEvent(Event.NET_CLOSE, sockType, func)
end


--时间事件
if not g_TimeEvent then
	g_TimeEvent = {}
end
local m_TimeEvent = g_TimeEvent

--时间事件c++调用函数
function onTimeEvent(usEvent, uiTick, ulCount)
	local func = m_TimeEvent[usEvent]
	if func then
		utile.callFunc(func, uiTick, ulCount)
	end
end
local function regTimeEvent(usEvent, func)	
	if not m_TimeEvent[usEvent] then
		m_TimeEvent[usEvent] = func
		humble.regEvent(usEvent, m_strTaskName, -1)
	end
end
local function onSec(uiTick, ulCount)
	DEV_OnTime(g_WheelMgr)
end
local function regSecEvent()
	if not g_WheelMgr then
		g_WheelMgr = WheelMgr:new()
		regTimeEvent(Event.TIME_SEC, onSec)
	end
end
--注册func(uiTick, ulCount)
function regFrameEv(func)
	regTimeEvent(Event.TIME_FRAME, func)
end
function regDelayEv(iTime, Func, ...)
	regSecEvent()
	DEV_Reg(g_WheelMgr, iTime, Func, table.unpack({...}))
end
--strTime 格式(24小时制)：12:36:28
function regDelayAtEv(strTime, Func, ...)
	regSecEvent()
	DEV_AtTime(g_WheelMgr, strTime, Func, table.unpack({...}))
end
--移除帧事件
function unRegFrame()
	humble.unRegFrame(m_strTaskName)
end
--移除秒事件
function unRegDelay()
	humble.unRegSec(m_strTaskName)
end


--rpc server端
if not g_RPCSV then
	g_RPCSV = {}
end
local m_RPCSV = g_RPCSV

--c++调用函数
function onRPCCall(rpcName, rpcParam)
	local func = m_RPCSV[rpcName]
	if func then
		local bRtn, rtnMsg = utile.callFunc(func, rpcParam)
		if bRtn then
			return rtnMsg
		else
			humble.Errorf("call rpc %s error", rpcName)
		end
	else
		humble.Errorf("get rpc name %s error", rpcName)		
	end
end
--string func(param)
function regRPC(rpcName, func)
	if not m_RPCSV[rpcName] then
		m_RPCSV[rpcName] = func
	end
end


--rpc client端
if not g_RPCId then
	g_RPCId = 0
end
if not g_RPCClient then
	g_RPCClient = {}
end
local m_RPCClient = g_RPCClient

local function rpcTimeOut(rpcId)
	local param = m_RPCClient[rpcId]
	if param then
		humble.Warnf("call server rpc method %s time out", param.RPCName)
		m_RPCClient[rpcId] = nil
	end
end
--c++调用函数
function onRPCRtn(rpcId, rtnMsg)
	local param = m_RPCClient[rpcId]
	if not param then
		return
	end
	
	utile.callFunc(param.Func, rtnMsg, table.unpack(param.Param))
	m_RPCClient[rpcId] = nil
end
--func(rpcRtn, ...)
function callNetRPC(sock, toTask, rpcName, rpcParam, func, ...)
	g_RPCId = g_RPCId + 1
	local rpcId = 0
	
	if func then
		rpcId = g_RPCId
		local param = {}
		param.RPCName = rpcName
		param.Func = func		
		param.Param = {...}
		m_RPCClient[rpcId] = param
		
		regDelayEv(m_RPCTimeOut, rpcTimeOut, rpcId)
	end
	
	if rpcParam then
		humble.rpcCall(sock, rpcId, rpcName, toTask, m_strTaskName, rpcParam, #rpcParam)
	else
		humble.rpcCall(sock, rpcId, rpcName, toTask, m_strTaskName, rpcParam, 0)
	end
end
--func(rpcRtn, ...)
function callTaskRPC(toTask, rpcName, rpcParam, func, ...)
	g_RPCId = g_RPCId + 1
	local rpcId = 0
	
	if func then
		rpcId = g_RPCId
		local param = {}
		param.RPCName = rpcName
		param.Func = func		
		param.Param = {...}
		m_RPCClient[rpcId] = param
		
		regDelayEv(m_RPCTimeOut, rpcTimeOut, rpcId)
	end
	
	if rpcParam then
		humble.taskRPCCall(rpcId, rpcName, toTask, m_strTaskName, rpcParam, #rpcParam)
	else
		humble.taskRPCCall(rpcId, rpcName, toTask, m_strTaskName, rpcParam, 0)
	end
end


--网络消息
if not g_NetMsg then
	g_NetMsg = {}
end
local m_NetMsg = g_NetMsg

local function onHttp(sock, sockType, netMsg)
	local httpInfo = httpd.parsePack(netMsg)
	local func = m_NetMsg[httpInfo.url]
	if func then
		utile.callFunc(func, sock, sockType, httpInfo)
	end
end
--c++调用函数
function onNetRead(sock, sockType, iProto, netMsg)
	--http特殊处理
	if SockType.HTTP == sockType then		
		onHttp(sock, sockType, netMsg)
		return
	end
	
	local func = m_NetMsg[iProto]
	if func then
		utile.callFunc(func, sock, sockType, netMsg)
	end
end

--func(sock, sockType, msg) http msg == tabel
function regProto(Proto, func)
	m_NetMsg[Proto] = func
	
	if "string" == type(Proto) then
		humble.regStrProto(Proto, m_strTaskName)
	else
		humble.regIProto(Proto, m_strTaskName)
	end		
end

--mqtt
if not g_MQTTEvent then
	g_MQTTEvent = {}
end
local m_MQTTEvent = g_MQTTEvent

--客户端请求连接服务端 
function onMQTTCONNECT(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_CONNECT]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTCONNECT(func)
	m_MQTTEvent[Event.MQTT_CONNECT] = func
	humble.regEvent(Event.MQTT_CONNECT, m_strTaskName, -1)
end

--发布消息
function onMQTTPUBLISH(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_PUBLISH]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTPUBLISH(func)
	m_MQTTEvent[Event.MQTT_PUBLISH] = func
	humble.regEvent(Event.MQTT_PUBLISH, m_strTaskName, -1)
end

--oS 1 消息发布收到确认 
function onMQTTPUBACK(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_PUBACK]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTPUBACK(func)
	m_MQTTEvent[Event.MQTT_PUBACK] = func
	humble.regEvent(Event.MQTT_PUBACK, m_strTaskName, -1)
end

--发布收到（保证交付第一步） 
function onMQTTPUBREC(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_PUBREC]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTPUBREC(func)
	m_MQTTEvent[Event.MQTT_PUBREC] = func
	humble.regEvent(Event.MQTT_PUBREC, m_strTaskName, -1)
end

--发布释放（保证交付第二步）  
function onMQTTPUBREL(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_PUBREL]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTPUBREL(func)
	m_MQTTEvent[Event.MQTT_PUBREL] = func
	humble.regEvent(Event.MQTT_PUBREL, m_strTaskName, -1)
end

--QoS 2 消息发布完成（保证交互第三步） 
function onMQTTPUBCOMP(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_PUBCOMP]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTPUBCOMP(func)
	m_MQTTEvent[Event.MQTT_PUBCOMP] = func
	humble.regEvent(Event.MQTT_PUBCOMP, m_strTaskName, -1)
end

--客户端订阅请求 
function onMQTTSUBSCRIBE(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_SUBSCRIBE]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTSUBSCRIBE(func)
	m_MQTTEvent[Event.MQTT_SUBSCRIBE] = func
	humble.regEvent(Event.MQTT_SUBSCRIBE, m_strTaskName, -1)
end

--客户端取消订阅请求 
function onMQTTUNSUBSCRIBE(sock, sockType, fixedHead, info)
	local func = m_MQTTEvent[Event.MQTT_UNSUBSCRIBE]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead, info)
	end
end
function regMQTTUNSUBSCRIBE(func)
	m_MQTTEvent[Event.MQTT_UNSUBSCRIBE] = func
	humble.regEvent(Event.MQTT_UNSUBSCRIBE, m_strTaskName, -1)
end

--心跳请求 
function onMQTTPINGREQ(sock, sockType, fixedHead)
	local func = m_MQTTEvent[Event.MQTT_PINGREQ]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead)
	end
end
function regMQTTPINGREQ(func)
	m_MQTTEvent[Event.MQTT_PINGREQ] = func
	humble.regEvent(Event.MQTT_PINGREQ, m_strTaskName, -1)
end

--客户端断开连接
function onMQTTDISCONNECT(sock, sockType, fixedHead)
	local func = m_MQTTEvent[Event.MQTT_DISCONNECT]
	if func then
		utile.callFunc(func, sock, sockType, fixedHead)
	end
end
function regMQTTDISCONNECT(func)
	m_MQTTEvent[Event.MQTT_DISCONNECT] = func
	humble.regEvent(Event.MQTT_DISCONNECT, m_strTaskName, -1)
end
