--任务初始化 每个任务都要require
require("macros")
local humble = require("humble")
local hotfix = require("hotfix")
local debugtcp = require("debugtcp")
local utile = require("utile")
local cjson = require("cjson")
local string = string
local table = table
local load = load
local m_taskName = g_taskName

--全局变量
if not g_Global then
	g_Global = {}
	g_Global.Id = 0	
	g_Global.TimeOut = {}--超时
	g_Global.AcceptEvent = {} --accept成功
	g_Global.ConnectEvent = {}--连接成功
	g_Global.CloseEvent = {}--连接关闭
	g_Global.BAcceptEvent = {} --accept成功
	g_Global.BConnectEvent = {}--连接成功
	g_Global.BCloseEvent = {}--连接关闭
	g_Global.NetIMsg = {}--网络消息
	g_Global.NetBindIMsg = {}
	g_Global.NetHttpdMsg = {}
	g_Global.NetHttpcMsg = {}
	g_Global.RPCFunc = {}--RPC
	g_Global.RPCRtn = {}
end
local m_Global = g_Global

local function getId()
	m_Global.Id = m_Global.Id + 1
	return m_Global.Id
end

--debug
function CCALL_DEBUG(sock, sockType, strDebug)
	local tDebug = cjson.decode(strDebug)
	if not tDebug then
		return
	end
	
	local tRtn = {}
	tRtn.cmd = tDebug.cmd
	tRtn.task = m_taskName
	tRtn.content = {}
	
	if "hotfix" == tDebug.cmd then
		--Modules require 过的，任务直接执行luaL_dofile更新会失败
		local tModules = string.split(tDebug.content, ",")
		for _, val in pairs(tModules) do
			local hotfixRtn = {}
			if nil == hotfix.hotfix_module(val) then
				hotfixRtn.ok = false
			else
				hotfixRtn.ok = true
			end
			
			hotfixRtn.msg = val
			table.insert(tRtn.content, hotfixRtn)
		end
	elseif "do" == 	tDebug.cmd then
		local doRtn = {}
		local func = load(tDebug.content)
		if func then
			doRtn.ok, doRtn.msg = utile.callFunc(func)
		else
			doRtn.ok = false
			doRtn.msg = "load string error."
		end
		
		table.insert(tRtn.content, doRtn)
	else
		
	end
	
	debugtcp.Response(sock, tRtn)
end

--超时
function CCALL_TIMEOUT(ulId)
	local cbFun = m_Global.TimeOut[ulId]
	if not cbFun then
		return
	end
	
	m_Global.TimeOut[ulId] = nil
	utile.callFunc(cbFun.func, table.unpack(cbFun.param))
end
--iTime毫秒
function regDelayEv(iTime, Func, ...)
	local Id = getId()
	local tParam = {}
	tParam.func = Func
	tParam.param = {...}
	
	m_Global.TimeOut[Id] = tParam
	humble.regTimeOut(iTime, Id)
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

--网络事件
function CCALL_ACCEPT(sock, sockType, ulId)
	if 0 == ulId then
		local func = m_Global.AcceptEvent[sockType]
		if not func then
			return
		end
		
		utile.callFunc(func, sock, sockType)
		return
	end
	
	local cbFun = m_Global.BAcceptEvent[ulId]
	if not cbFun then
		return
	end
	utile.callFunc(cbFun.func, sock, sockType, table.unpack(cbFun.param))
end
--func(sock, sockType)
function regAcceptEv(sockType, func)
	m_Global.AcceptEvent[sockType] = func
	humble.regAcceptEvent(sockType)
end
--func(sock, sockType, ...)
function regBAcceptEv(sockType, ulId, func, ...)
	local tParam = {}
	tParam.func = func
	tParam.param = {...}
	
	m_Global.BAcceptEvent[ulId] = tParam
	humble.regAcceptEvent(sockType)
end

function CCALL_CONNECT(sock, sockType, ulId)
	if 0 == ulId then
		local func = m_Global.ConnectEvent[sockType]
		if not func then
			return
		end
		
		utile.callFunc(func, sock, sockType)
		return
	end
	
	local cbFun = m_Global.BConnectEvent[ulId]
	if not cbFun then
		return
	end
	utile.callFunc(cbFun.func, sock, sockType, table.unpack(cbFun.param))
end
function regConnectEv(sockType, func)
	m_Global.ConnectEvent[sockType] = func
	humble.regConnectEvent(sockType)
end
function regBConnectEv(sockType, ulId, func, ...)
	local tParam = {}
	tParam.func = func
	tParam.param = {...}
	
	m_Global.BConnectEvent[ulId] = tParam
	humble.regConnectEvent(sockType)
end

function CCALL_CLOSED(sock, sockType, ulId)
	if 0 == ulId then
		local func = m_Global.CloseEvent[sockType]
		if not func then
			return
		end
		
		utile.callFunc(func, sock, sockType)
		return
	end
	
	local cbFun = m_Global.BCloseEvent[ulId]
	if not cbFun then
		return
	end
	if cbFun.func then
		utile.callFunc(cbFun.func, sock, sockType, table.unpack(cbFun.param))
	end
	
	if not cbFun.keep then
		m_Global.BCloseEvent[ulId] = nil
		m_Global.BAcceptEvent[ulId] = nil
		m_Global.BConnectEvent[ulId] = nil
	end
end
function regCloseEv(sockType, func)
	m_Global.CloseEvent[sockType] = func
	humble.regCloseEvent(sockType)
end
function regBCloseEv(sockType, ulId, bKeepAlive, func, ...)
	local tParam = {}
	tParam.func = func
	tParam.keep = bKeepAlive
	tParam.param = {...}
	
	m_Global.BCloseEvent[ulId] = tParam
	humble.regCloseEvent(sockType)
end

--网络消息
function CCALL_NETREADI(sock, sockType, iProto, pszBuf)
	local func = m_Global.NetIMsg[iProto]
	if not func then
		return
	end
	
	utile.callFunc(func, sock, sockType, pszBuf)
end
--function func(sock, sockType, pszBuf)
function regIProto(iProto, func)	
	m_Global.NetIMsg[iProto] = func
	humble.regIProto(iProto)
end

--绑定到任务的
function CCALL_BIND_I(sock, sockType, iProto, pszBuf)
	local func = m_Global.NetBindIMsg[iProto]
	if not func then
		return
	end
	
	utile.callFunc(func, sock, sockType, pszBuf)
end
--function func(sock, sockType, pszBuf)
function regBindIProto(iProto, func)
	m_Global.NetBindIMsg[iProto] = func
end

--httpd
function CCALL_NETREADHTTPD(sock, sockType, strProto, pHttpdAdjure)
	local func = m_Global.NetHttpdMsg[strProto]
	if not func then
		return
	end
	
	utile.callFunc(func, sock, sockType, 
		pHttpdAdjure:getMethod(), pHttpdAdjure:getUrl(), 
		pHttpdAdjure:getHead(), pHttpdAdjure:getBody())
end
--function func(sock, sockType, strMethod, tUrl, tHead, pszBody)
function regHttpdProto(strProto, func)
	m_Global.NetHttpdMsg[strProto] = func
	humble.regHttpdProto(strProto)	
end

--http client
function CCALL_BIND_HTTPC(sock, sockType, ulId, strStatus, tHead, pszBody)
	local cbFun = m_Global.NetHttpcMsg[ulId]
	if not cbFun then
		return
	end

	utile.callFunc(cbFun.func, sock, sockType, strStatus, tHead, pszBody, 
		table.unpack(cbFun.param))
	humble.closeLink(sock)
end
--http 客户端 
--onConnectFunc(sock, sockType, ...) 
--onResponse(sock, sockType, strStatus, tHead, pszBody,...)
function httpClient(pszHost, usPort, sockType, onConnectFunc, tConnParam, onResponse, ...)
	local Id = getId()
	local tParam = {}
	tParam.func = onResponse
	tParam.param = {...}
	
	regBConnectEv(sockType, Id, onConnectFunc, table.unpack(tConnParam))
	regBCloseEv(sockType, Id, false)
	m_Global.NetHttpcMsg[Id] = tParam
	
	humble.addBindLinker("httpc", sockType, pszHost, usPort, Id, false)	
end

--任务间RPC
function CCALL_RPCCALL(pFrom, strRPCName, pszBuf, ulId)
	local tRtn = {}
	local func = m_Global.RPCFunc[strRPCName]
	if not func then
		tRtn.ok = false
		tRtn.msg = "not find rpc func "..strRPCName
	else
		tRtn.ok, tRtn.msg = utile.callFunc(func, table.unpack(cjson.decode(pszBuf)))
	end
	
	if 0 == ulId then
		return
	end
	
	humble.taskRPCRtn(pFrom, cjson.encode(tRtn), ulId)
end
function CCALL_RPCRTN(pszBuf, ulId)
	local param = m_Global.RPCRtn[ulId]
	if not param then
		return
	end
	
	m_Global.RPCRtn[ulId] = nil
	utile.callFunc(param.func, cjson.decode(pszBuf), table.unpack(param.param))
end
--注册rpc函数
function regRPC(strName, func)
	if m_Global.RPCFunc[strName] then
		humble.Warnf("rpc %s repeat register.", strName)
	end
	
	m_Global.RPCFunc[strName] = func
end
local function rpcTimeOut(rpcId)
	local param = m_Global.RPCRtn[rpcId]
	if param then
		m_Global.RPCRtn[rpcId] = nil
		humble.Warnf("call rpc %s time out", param.name)
	end
end
--本地rpc调用 func(rpcRtn, ...)
function taskRPC(toTask, rpcName, tRPCParam, iTimeOut, func, ...)
	local rpcId = 0
	if func then
		rpcId = getId()
		
		local param = {}
		param.name = rpcName
		param.func = func
		param.param = {...}
		
		m_Global.RPCRtn[rpcId] = param		
		regDelayEv(iTimeOut, rpcTimeOut, rpcId)
	end
	
	humble.taskRPC(toTask, rpcName, cjson.encode(tRPCParam), rpcId)
end

--网络RPC
function CCALL_NETRPC(sock, sockType, strFrom, pszBuf)
	local tRPCInfo = cjson.decode(pszBuf)
	
	local strRPCName = tRPCInfo.name	
	if strRPCName and 0 ~= #strRPCName then
		local tRtn = {}
		local func = m_Global.RPCFunc[strRPCName]
		if not func then
			tRtn.ok = false
			tRtn.msg = "not find rpc func "..strRPCName
		else
			tRtn.ok, tRtn.msg = utile.callFunc(func, table.unpack(tRPCInfo.param))
		end
		
		if 0 == tRPCInfo.id then
			return
		end
		
		tRtn.id = tRPCInfo.id
		humble.netRPC(sock, m_taskName, strFrom, cjson.encode(tRtn))
		return
	end
	
	local ulId = tRPCInfo.id
	local param = m_Global.RPCRtn[ulId]
	if not param then
		return
	end
	
	m_Global.RPCRtn[ulId] = nil
	tRPCInfo.id = nil
	utile.callFunc(param.func, tRPCInfo, table.unpack(param.param))
end
--网络rpc调用 func(rpcRtn, ...)
function netRPC(sock, toTask, rpcName, tRPCParam, iTimeOut, func, ...)
	local rpcId = 0
	if func then
		rpcId = getId()
		
		local param = {}
		param.name = rpcName
		param.func = func		
		param.param = {...}
		
		m_Global.RPCRtn[rpcId] = param		
		regDelayEv(iTimeOut, rpcTimeOut, rpcId)
	end
	
	local tNetRPC = {}
	tNetRPC.name = rpcName
	tNetRPC.param = tRPCParam
	tNetRPC.id = rpcId
	humble.netRPC(sock, m_taskName, toTask, cjson.encode(tNetRPC))
end
