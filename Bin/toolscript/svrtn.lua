
require("init")
require("proto")
local ErrCode = ErrCode
local SockType = SockType
local Proto = Proto
local m_strTaskName = g_taskName

function initTask()
	consoleWrite("init task:"..m_strTaskName)
end

function destroyTask()
	
end

local function test(sock, sockType, netMsg)
    consoleWrite(netMsg)
end
regProto(12, test)
