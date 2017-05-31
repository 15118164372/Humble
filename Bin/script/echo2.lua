require("init")
require("proto")
local humble = require("humble")
local httpd = require("httpd")
local ErrCode = ErrCode
local SockType = SockType
local Proto = Proto

function initTask()
	print("echo2 initTask")
end

function destroyTask()
	print("echo2 destroyTask")
end

local function echo(sock, sockType, httpInfo)
	httpd.Response(sock, 200, "echo2 return.")
end
regProto("/echo2", echo)
