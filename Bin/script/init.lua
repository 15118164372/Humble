--任务初始化 每个任务都要require
require("macros")
local humble = require("humble")
local utile = require("utile")
local hotfix = require("hotfix")
local cjson = require("cjson")
local httpd = require("httpd")
local table = table
local Event = Event
local SockType = SockType
local m_strTaskName = g_taskName
local m_RPCTimeOut = 5000 --毫秒

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
	g_TimeEvent.Reg = false--事件是否注册
	g_TimeEvent.Frame = nil--帧函数
	g_TimeEvent.Delay = {}--延迟
	g_TimeEvent.TimeEvId = 0--id
	g_TimeEvent.MilSecond = humble.milSecond()--当前帧时间 毫秒
	g_TimeEvent.OrderedQu = CLOrderedQu(thisState)--超时队列
end
local m_TimeEvent = g_TimeEvent

function cacheMilSecond()
	return m_TimeEvent.MilSecond
end

--帧事件c++调用函数
function onFrame(uiTick, ulCount)
	m_TimeEvent.MilSecond = humble.milSecond()
	local timeOut = m_TimeEvent.OrderedQu:popNode(cacheMilSecond())
	if #timeOut > 0 then
		local funcInfo
		local delay = m_TimeEvent.Delay
		for _, val in pairs(timeOut) do
			funcInfo = delay[val]
			if funcInfo then
				utile.callFunc(funcInfo.Func, table.unpack(funcInfo.Param))
				delay[val] = nil
			end
		end
	end
	
	if m_TimeEvent.Frame then
		utile.callFunc(m_TimeEvent.Frame, uiTick, ulCount)
	end
end

local function regTimeEvent()
	if not m_TimeEvent.Reg then
		m_TimeEvent.Reg = true
		humble.regEvent(Event.TIME_FRAME, m_strTaskName, -1)
	end
end

--注册func(uiTick, ulCount)
function regFrameEv(func)
	regTimeEvent()
	m_TimeEvent.Frame = func
end
--iTime毫秒
function regDelayEv(iTime, Func, ...)
	regTimeEvent()
	m_TimeEvent.TimeEvId = m_TimeEvent.TimeEvId + 1
	
	local tParam = {}
	tParam.Func = Func
	tParam.Param = {...}
	
	m_TimeEvent.Delay[m_TimeEvent.TimeEvId] = tParam
	m_TimeEvent.OrderedQu:pushNode(cacheMilSecond() + iTime, m_TimeEvent.TimeEvId)
end
--strTime 格式(24小时制)：12:36:28
function regDelayAtEv(strTime, Func, ...)
	local strHour, strMin, strSec = string.match(strTime, "(%d+):(%d+):(%d+)")
    local iTime = (tonumber(strHour) * 60 * 60) + (tonumber(strMin) * 60) + tonumber(strSec)
    local tNow = os.date("*t", time)
    local iNowTime = (tonumber(tNow.hour) * 60 * 60) + (tonumber(tNow.min) * 60) + tonumber(tNow.sec)
    
    local iDelayTime = 0
    
    if iTime > iNowTime then
        iDelayTime = iTime - iNowTime
    else
        iDelayTime = (24 * 60 * 60) - (iNowTime - iTime)
    end
	
	regDelayEv(iDelayTime * 1000, Func, table.unpack({...}))
end
--移除帧事件
function unRegFrame()
	if not m_TimeEvent.Reg then
		return
	end
	
	m_TimeEvent.Reg = false
	humble.unRegFrame(m_strTaskName)
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
