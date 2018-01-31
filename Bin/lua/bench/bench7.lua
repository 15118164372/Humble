require("init")
local tcp2 = require("deftcp")

local m_Socks = {}

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 13, netMsg)
end
regIProto(13, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 14, netMsg)
end
regIProto(14, test2)
