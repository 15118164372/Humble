require("init")
local humble = require("humble")
local httpd = require("httpd")

local function echo1RPCRtn(strNum)
	print("echo1RPCRtn:"..strNum)
end

local function onTime(strNum)
	callTaskRPC("echo1", "echo1RPC", tostring(tonumber(strNum) + 1), echo1RPCRtn)	
end

local function rpcOnTime(strNum)	
	regDelayEv(2, onTime, strNum)
end
regRPC("rpcOnTime", rpcOnTime)

function initTask()
	print("echo2 initTask")
	regDelayEv(2, rpcOnTime, "0")
end

function destroyTask()
	print("echo2 destroyTask")
end

local function echo(sock, sockType, httpInfo)
	httpd.Response(sock, 200, "echo2 return.")
end
regProto("/echo2", echo)
