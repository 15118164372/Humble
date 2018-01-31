require("init")
local tcp2 = require("deftcp")

local m_Socks = {}

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 11, netMsg)
end
regIProto(11, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 12, netMsg)
end
regIProto(12, test2)
