require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 37, netMsg)
end
regIProto(37, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 38, netMsg)
end
regIProto(38, test2)
