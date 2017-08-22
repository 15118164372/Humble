
require("init")
local ErrCode = ErrCode
local SockType = SockType
local Proto = Proto
local m_strTaskName = g_taskName

function initTask()
	consoleWrite("init task:"..m_strTaskName)
end

function destroyTask()
	
end

local function onLinked(sock, sockType)
    consoleWrite("sock linked " .. sock)
end
regLinkedEv(getLinkType(), onLinked)

local function test(sock, sockType, netMsg)
    consoleWrite(netMsg)
end
for i=1, 1000 do
	regProto(i, test)
end
