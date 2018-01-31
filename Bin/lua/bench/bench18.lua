require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 35, netMsg)
end
regIProto(35, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 36, netMsg)
end
regIProto(36, test2)
