require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 33, netMsg)
end
regIProto(33, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 34, netMsg)
end
regIProto(34, test2)
