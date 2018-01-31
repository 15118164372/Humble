require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 5, netMsg)
end
regIProto(5, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 6, netMsg)
end
regIProto(6, test2)
