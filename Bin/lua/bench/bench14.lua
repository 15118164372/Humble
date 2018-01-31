require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 27, netMsg)
end
regIProto(27, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 28, netMsg)
end
regIProto(28, test2)
