require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 9, netMsg)
end
regIProto(9, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 10, netMsg)
end
regIProto(10, test2)
