require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 7, netMsg)
end
regIProto(7, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 8, netMsg)
end
regIProto(8, test2)
