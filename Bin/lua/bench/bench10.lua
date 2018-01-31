require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 19, netMsg)
end
regIProto(19, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 20, netMsg)
end
regIProto(20, test2)
