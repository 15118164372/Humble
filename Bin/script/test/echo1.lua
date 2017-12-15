require("init")
local humble = require("humble")
local httpd = require("httpd")
local tcp1 = require("tcp1")

function initTask()
	--humble.rpcClient("127.0.0.1", 16001)
end

function destroyTask()
	
end

local function echo(sock, sockType, httpInfo)
	httpd.Response(sock, 200, "echo1 return.")
end
regProto("/echo1", echo)
