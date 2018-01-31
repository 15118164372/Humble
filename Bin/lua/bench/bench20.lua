require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 39, netMsg)
end
regIProto(39, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 40, netMsg)
end
regIProto(40, test2)
