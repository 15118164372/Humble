--[[
网络服务
--]]

require("macros")
require("logicmacro")
local humble = require("humble")
local utile = require("utile")
local cjson = require("cjson")
local tcp3 = require("tcp3")
local table = table
local pairs = pairs
local EnevtType = EnevtType
local SockType = SockType
local pBuffer = g_pBuffer

--任务名与chan对应
if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--以下为测试
local httpd = require("httpd")

--初始化  这里注册任务
function onStart()
	--解析器，这个要先设置
	humble.setParser(SockType.CMD, "tcp3")
	humble.setParser(SockType.RPC, "tcp3")	
	--命令监听
	humble.addListener(SockType.CMD, "127.0.0.1", 15100)
	--rpc
	humble.addListener(SockType.RPC, "0.0.0.0", 15200)	
	--rpc链接管理
	humble.regTask("task_rpclink.lua", "task_rpclink", 1024 * 10)
	tChan.task_rpclink = humble.getChan("task_rpclink")
		
	--TODO
	--以下为测试
	humble.addTcpLink(SockType.RPC, "127.0.0.1", 15201)
	humble.addListener(SockType.HTTP, "0.0.0.0", 80)
	humble.setParser(SockType.HTTP, "http")
	humble.regTask("echo1.lua", "echo1", 1024 * 10)
	humble.regTask("echo2.lua", "echo2", 1024 * 10)
    tChan.echo1 = humble.getChan("echo1")
	tChan.echo2 = humble.getChan("echo2")	
end

--退出
function onStop()
	
end

--accept到新链接
function onTcpAccept(sock, uiSession, usSockType)
	if SockType.RPC == usSockType then
		utile.chanSend(tChan.task_rpclink, utile.Pack(EnevtType.NetAccept, nil, sock, uiSession, usSockType))
	else
	end
end

--链接成功
function onTcpLinked(sock, uiSession, usSockType)
	if SockType.RPC == usSockType then
		utile.chanSend(tChan.task_rpclink, utile.Pack(EnevtType.NetLinked, nil, sock, uiSession, usSockType))
	else
	end
end

--链接断开
function onTcpClose(sock, uiSession, usSockType)
	if SockType.RPC == usSockType then
		utile.chanSend(tChan.task_rpclink, utile.Pack(EnevtType.NetClose, nil, sock, uiSession, usSockType))
	else
	end
end

local function dispCMD(sock, uiSession)
	local strCmd = pBuffer:getString()
	local strTask = pBuffer:getString()
	local strMsg = pBuffer:getString()
	local objChan
	
	if "hotfix" == strCmd and "all" == strTask then
		for _, objChan in pairs(tChan) do
			utile.chanSend(objChan, utile.Pack(EnevtType.CMD, strCmd, -1, 0, strMsg))
		end
		
		humble.sendB(sock, uiSession, 
			tcp3.Response(cjson.encode({"ok", ""})))
		return	
	end
	
	objChan = tChan[strTask]
	if not objChan then
		humble.sendB(sock, uiSession, 
			tcp3.Response(cjson.encode({"fail", string.format("not find task %s.", strTask)})))
		return
	end
		
	utile.chanSend(objChan, utile.Pack(EnevtType.CMD, strCmd, sock, uiSession, strMsg))
end

local function dispRPC(sock, uiSession)
	local tRPC = cjson.decode(pBuffer:getByte(pBuffer:getSurpLens()))
	if tRPC.Enevt == EnevtType.CallRPC then --调用
		humble.netToTask(tChan, tRPC.Method, utile.Pack(tRPC.Enevt, tRPC.Method, sock, uiSession, tRPC))
	else --调用返回
		local objChan = tChan[tRPC.RecvTask]
		if objChan then
			utile.chanSend(objChan, utile.Pack(tRPC.Enevt, nil, sock, uiSession, tRPC))
		else
			utile.Errorf("not find task %s", tRPC.RecvTask)
		end
	end
end

local function dispHttp(sock, uiSession)
	local buffer = httpd.parsePack(pBuffer)
	humble.netToTask(tChan, buffer.url, utile.Pack(EnevtType.TcpRead, buffer.url, sock, uiSession, buffer))
end

--tcp读到一完整的包
function onTcpRead(sock, uiSession, usSockType)	
	if SockType.CMD == usSockType then --命令
		dispCMD(sock, uiSession)
	elseif SockType.RPC == usSockType then --服务器间RPC
		dispRPC(sock, uiSession)
	elseif SockType.HTTP == usSockType then
		dispHttp(sock, uiSession)
	else		
	end	
end

--udp可读
function onUdpRead(sock, strHost, usPort)
	--TODO
end
