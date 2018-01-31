require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 23, netMsg)
end
regIProto(23, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 24, netMsg)
end
regIProto(24, test2)
