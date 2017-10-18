--�����ʼ�� ÿ������Ҫrequire
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
local m_RPCTimeOut = 5000 --����

--����ִ��c++���ú���
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


--�����¼� NET_ACCEPT  NET_LINKED NET_CLOSE
if not g_NetEvent then
	g_NetEvent = {}
end
local m_NetEvent = g_NetEvent

--�����¼�c++���ú���
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
--ע��
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


--ʱ���¼�
if not g_TimeEvent then
	g_TimeEvent = {}
	g_TimeEvent.Reg = false--�¼��Ƿ�ע��
	g_TimeEvent.Frame = nil--֡����
	g_TimeEvent.Delay = {}--�ӳ�
	g_TimeEvent.TimeEvId = 0--id
	g_TimeEvent.MilSecond = humble.milSecond()--��ǰ֡ʱ�� ����
	g_TimeEvent.OrderedQu = CLOrderedQu(thisState)--��ʱ����
end
local m_TimeEvent = g_TimeEvent

function cacheMilSecond()
	return m_TimeEvent.MilSecond
end

--֡�¼�c++���ú���
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

--ע��func(uiTick, ulCount)
function regFrameEv(func)
	regTimeEvent()
	m_TimeEvent.Frame = func
end
--iTime����
function regDelayEv(iTime, Func, ...)
	regTimeEvent()
	m_TimeEvent.TimeEvId = m_TimeEvent.TimeEvId + 1
	
	local tParam = {}
	tParam.Func = Func
	tParam.Param = {...}
	
	m_TimeEvent.Delay[m_TimeEvent.TimeEvId] = tParam
	m_TimeEvent.OrderedQu:pushNode(cacheMilSecond() + iTime, m_TimeEvent.TimeEvId)
end
--strTime ��ʽ(24Сʱ��)��12:36:28
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
--�Ƴ�֡�¼�
function unRegFrame()
	if not m_TimeEvent.Reg then
		return
	end
	
	m_TimeEvent.Reg = false
	humble.unRegFrame(m_strTaskName)
end


--rpc server��
if not g_RPCSV then
	g_RPCSV = {}
end
local m_RPCSV = g_RPCSV

--c++���ú���
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


--rpc client��
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
--c++���ú���
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


--������Ϣ
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
--c++���ú���
function onNetRead(sock, sockType, iProto, netMsg)
	--http���⴦��
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

--�ͻ����������ӷ���� 
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

--������Ϣ
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

--oS 1 ��Ϣ�����յ�ȷ�� 
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

--�����յ�����֤������һ���� 
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

--�����ͷţ���֤�����ڶ�����  
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

--QoS 2 ��Ϣ������ɣ���֤������������ 
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

--�ͻ��˶������� 
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

--�ͻ���ȡ���������� 
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

--�������� 
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

--�ͻ��˶Ͽ�����
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
