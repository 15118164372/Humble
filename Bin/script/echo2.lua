require("init")
local humble = require("humble")
local httpd = require("httpd")
local cjson = require("cjson")

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

local function add(rpcParam)
	local regInfo = cjson.decode(rpcParam)
	local sum = regInfo[1] + regInfo[2]
	local rtn = {}
	table.insert(rtn, sum)
	return cjson.encode(rtn)
end
regRPC("add", add)
