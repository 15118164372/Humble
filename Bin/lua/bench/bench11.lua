require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 21, netMsg)
end
regIProto(21, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 22, netMsg)
end
regIProto(22, test2)
