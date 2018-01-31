require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 31, netMsg)
end
regIProto(31, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 32, netMsg)
end
regIProto(32, test2)
