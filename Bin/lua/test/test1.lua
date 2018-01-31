require("init")
require("macros")
local ws = require("ws")
local httpd = require("httpd")
local humble = require("humble")

function initTask()
	
end

function destroyTask()
	
end

local function onAccept(sock, sockType)
	print(sock .. " onAccept.")
end
regAcceptEv(SockType.RPC, onAccept)
local function onConnected(sock, sockType)
	print(sock .. " onConnected.")
end
regConnectEv(SockType.RPC, onConnected)
local function onClosed(sock, sockType)
	print(sock .. " onClosed.")
end
regCloseEv(SockType.RPC, onClosed)

--websocket
local function wsEcho1(sock, sockType, netMsg)
	ws.binaryFrame(sock, 1000, "wsEcho1 return.")
end
regIProto(1000, wsEcho1)

--websocket
local function wsEcho3(sock, sockType, netMsg)
	ws.bgContinuation(sock, 3000, "123456")
	ws.Continuation(sock, "7890")
	ws.endContinuation(sock, "aaaaaaa!")
end
regIProto(3000, wsEcho3)

--httpd 测试
local function echo1(sock, sockType, strMethod, tUrl, tHead, pszBody)
	print(strMethod)	
	table.print(tUrl)
	table.print(tHead)
	print(pszBody)
	
	httpd.Response(sock, 200, "echo1 server return")
end
regHttpdProto("/echo1", echo1)

--rpc
local function AddRtn(rpcRtn)
	table.print(rpcRtn)
end
local function StringRtn(rpcRtn)
	table.print(rpcRtn)
end
local function test_TaskRPC()
	print("--------------test_TaskRPC--------------")
	taskRPC("test2", "SetAttr", {"attr set"})
	taskRPC("test2", "Add", {1, 2}, 20, AddRtn)
	taskRPC("test2", "String", {}, 20, StringRtn)
	
	humble.rpcClient("127.0.0.1", 17000)
end
regDelayEv(1000, test_TaskRPC)

local function test_NetRPC()
	local sock = humble.getALinkByType(3)
	if Invalid_Sock == sock then
		print("no rpc linked.")
		regDelayEv(1000, test_NetRPC)
		return
	end
	
	print("--------------test_NetRPC--------------")
	netRPC(sock, "test2", "SetAttr", {"attr set"})
	netRPC(sock, "test2", "Add", {1, 2}, 200, AddRtn)
	netRPC(sock, "test2", "String", {}, 200, StringRtn)
end
regDelayEv(2000, test_NetRPC)

--httpc
local function onResponse(sock, sockType, strStatus, tHead, pszBody)
	print(strStatus)
	table.print(tHead)
	print(pszBody)
end
local function httpClient_Conn(sock, sockType)
	httpd.Get(sock, "/echo1?a=1&n=2")
end
local function test_Httpc()
	print("--------------test_Httpc--------------")
	httpClient("127.0.0.1", "8080", 10, httpClient_Conn, onResponse)
end
regDelayEv(3000, test_Httpc)
