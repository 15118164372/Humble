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
