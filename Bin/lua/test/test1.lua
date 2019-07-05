require("init")
require("macros")
local ws = require("ws")
local httpd = require("httpd")
local humble = require("humble")

function initTask()
	humble.rpcClient("127.0.0.1", 17000)
end

function destroyTask()
	
end

local function onAccept(sock, sockType)
    print("--------------onAccept event--------------")
	print(sock .. " onAccept.")
end
regAcceptEv(SockType.RPC, onAccept)
local function onConnected(sock, sockType)
    print("--------------onConnected event--------------")
	print(sock .. " onConnected.")
end
regConnectEv(SockType.RPC, onConnected)
local function onClosed(sock, sockType)
    print("--------------onClosed event--------------")
	print(sock .. " onClosed.")
end
regCloseEv(SockType.RPC, onClosed)

--websocket
local function wsEcho1(sock, sockType, netMsg)
    print("--------------websocket echo--------------")
	ws.binaryFrame(sock, 1000, "wsEcho1 return.")
end
regIProto(1000, wsEcho1)

--websocket
local function wsEcho3(sock, sockType, netMsg)
    print("--------------websocket echo 分片--------------")
	ws.bgContinuation(sock, 3000, "123456")
	ws.Continuation(sock, "7890")
	ws.endContinuation(sock, "aaaaaaa!")
end
regIProto(3000, wsEcho3)

--httpd
local function echo1(sock, sockType, strMethod, tUrl, tHead, pszBody)
    print("--------------http echo-------------")
	print(strMethod)	
	table.print(tUrl)
	table.print(tHead)
	print(pszBody)
	
	httpd.Response(sock, 200, nil, "echo1 server return")
end
regHttpdProto("/echo1", echo1)

--rpc
local function AddRtn(rpcRtn)
    print("--------------RPC Add 函数返回-------------")
	table.print(rpcRtn)
end
local function StringRtn(rpcRtn)
    print("--------------RPC String 函数返回-------------")
	table.print(rpcRtn)
end
local function test_TaskRPC()
	taskRPC("test2", "SetAttr", {"attr set"})
	taskRPC("test2", "Add", {1, 2}, 20, AddRtn)
	taskRPC("test2", "String", {}, 20, StringRtn)	
end
regDelayEv(1000, test_TaskRPC)

local function test_NetRPC()
	local sock = humble.getALinkById(humble.getServiceId())
	if Invalid_Sock == sock then
		print("no rpc linked.")
		regDelayEv(1000, test_NetRPC)
		return
	end
    
	netRPC(sock, "test2", "SetAttr", {"attr set"})
	netRPC(sock, "test2", "Add", {1, 2}, 200, AddRtn)
	netRPC(sock, "test2", "String", {}, 200, StringRtn)
end
regDelayEv(3000, test_NetRPC)

--httpc
local function onResponse(sock, sockType, strStatus, tHead, pszBody, param)
	print(CUtils.nowMilSecond())
	print("--------------httpClient onResponse param--------------")
	print(param)
	
	print("--------------httpClient onResponse--------------")
	print(strStatus)
	table.print(tHead)
	print(pszBody)
end
local function httpClient_Conn(sock, sockType, param)
	print("--------------httpClient_Conn param--------------")
	print(param)
	httpd.Get(sock, "/echo1?a=1&n=2")
end
local function test_Httpc()
	print("--------------test_Httpc--------------")
	print(CUtils.nowMilSecond())
	--httpClient(pszHost, usPort, sockType, onConnectFunc, tConnParam, onResponse, ...)
	httpClient("127.0.0.1", "8080", 10, 
		httpClient_Conn, {"httpClient_Conn param"}, onResponse, "onResponse param")
end
regDelayEv(5000, test_Httpc)
