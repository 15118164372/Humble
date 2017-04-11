--[[
任务模板
--]]

require("global")
local utile = require("utile")
local humble = require("humble")
local pChan = g_pChan

g_taskName = "任务名"
initGlobal(g_taskName)
local taskName = g_taskName
local enevtDisp = g_enevtDisp
local timeWheel = g_timeWheel
local netDisp = g_netDisp
local svRPC = g_svRPC
local taskRPC = g_taskRPC

--[[ 可选
--accept成功
local function onNetAccept(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
enevtDisp:regEvent(EnevtType.NetAccept, onNetAccept)
--链接事件
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)
--连接断开
local function onNetCloseEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
enevtDisp:regEvent(EnevtType.NetClose, onNetCloseEvent)

--一帧
local function onFrameEvent(Proto, msgPack)
	local uiTick, uiCount = table.unpack(msgPack)	
end
enevtDisp:regEvent(EnevtType.Frame, onFrameEvent)--]]

--任务初始化
function initTask()
    
end

--有新任务执行
function runTask()
    local varRecv = pChan:Recv()
	if not varRecv then
		return
	end
		
	local evType, Proto, msgPakc = utile.unPack(varRecv)
	enevtDisp:onEvent(evType, Proto, msgPakc)
end

--任务销毁
function destroyTask()
	
end
