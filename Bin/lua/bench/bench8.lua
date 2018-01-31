require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 15, netMsg)
end
regIProto(15, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 16, netMsg)
end
regIProto(16, test2)
