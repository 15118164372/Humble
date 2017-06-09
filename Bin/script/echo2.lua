require("init")
local humble = require("humble")
local httpd = require("httpd")

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
