--�����ʼ�� ÿ������Ҫrequire
require("macros")
require("proto")
require("timewheel")
local humble = require("humble")
local utile = require("utile")
local hotfix = require("hotfix")
local cjson = require("cjson")
local httpd = require("httpd")
local table = table
local Event = Event
local Proto = Proto
local m_strTaskName = g_taskName
local m_RPCTimeOut = 5

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

--�����¼� Accept  Linked Closed
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
	regNetEvent(Event.Accept, sockType, func)
end
function regLinkedEv(sockType, func)
	regNetEvent(Event.Linked, sockType, func)
end
function regClosedEv(sockType, func)
	regNetEvent(Event.Closed, sockType, func)
end

--ʱ���¼�
if not g_TimeEvent then
	g_TimeEvent = {}
end
local m_TimeEvent = g_TimeEvent

--ʱ���¼�c++���ú���
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
		regTimeEvent(Event.Sec, onSec)
	end
end
--ע��func(uiTick, ulCount)
function regFrameEv(func)
	regTimeEvent(Event.Frame, func)
end
function regDelayEv(iTime, Func, ...)
	regSecEvent()
	DEV_Reg(g_WheelMgr, iTime, Func, table.unpack({...}))
end
--strTime ��ʽ(24Сʱ��)��12:36:28
function regDelayAtEv(strTime, Func, ...)
	regSecEvent()
	DEV_AtTime(g_WheelMgr, strTime, Func, table.unpack({...}))
end

--rpc server��
if not g_RPCSV then
	g_RPCSV = {}
end
local m_RPCSV = g_RPCSV

--c++���ú���
function onRPCCall(rpcName, rpcParam, sock)
	local func = m_RPCSV[rpcName]
	if func then
		if sock then			
			local bRtn, rtnMsg = utile.callFunc(func, sock, rpcParam)
			if bRtn then
				return rtnMsg
			else
				humble.Errorf("call rpc %s error", rpcName)
			end
		else
			local bRtn, rtnMsg = utile.callFunc(func, rpcParam)
			if bRtn then
				return rtnMsg
			else
				humble.Errorf("call rpc %s error", rpcName)
			end
		end
	else
		humble.Errorf("get rpc name %s error", rpcName)		
	end
	
	return ""
end
--string task: func(param) net:func(sock, param)
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
	if Proto.HTTP == iProto then		
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
