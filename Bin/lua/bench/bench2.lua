require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 3, netMsg)
end
regIProto(3, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 4, netMsg)
end
regIProto(4, test2)
