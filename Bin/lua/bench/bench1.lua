require("init")
require("macros")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 1, netMsg)
end
regIProto(1, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 2, netMsg)
end
regIProto(2, test2)
