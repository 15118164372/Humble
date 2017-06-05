require("init")
local humble = require("humble")
local httpd = require("httpd")
local tcp1 = require("tcp1")

function initTask()
	print("echo1 initTask")
end

function destroyTask()
	print("echo1 destroyTask")
end

local function echo(sock, sockType, httpInfo)
	httpd.Response(sock, 200, "echo1 return.")
end
regProto("/echo1", echo)

local function echonumber(sock, sockType, netMsg)
	print(netMsg)
	tcp1.Response(sock, 12, "echonumber return.")
end
regProto(10, echonumber)

local function echo1RPC(strNum)
	print("echo1RPC:"..strNum)
	strNum = tonumber(strNum) + 1
	
	callTaskRPC("echo2", "rpcOnTime", tostring(strNum))
	return tostring(strNum)
end
regRPC("echo1RPC", echo1RPC)
