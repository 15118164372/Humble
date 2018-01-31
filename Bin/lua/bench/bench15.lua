require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 29, netMsg)
end
regIProto(29, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 30, netMsg)
end
regIProto(30, test2)
