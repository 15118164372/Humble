require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 17, netMsg)
end
regIProto(17, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 18, netMsg)
end
regIProto(18, test2)
