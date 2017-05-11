--[[
任务模板
--]]

require("global")
local utile = require("utile")
local humble = require("humble")

local m_pChan = g_pChan--消息chan
local m_taskName = g_taskName--任务名
local m_enevtDisp = g_enevtDisp--事件
local m_timeWheel = g_timeWheel--时间轮
local m_netDisp = g_netDisp--网络消息事件
local m_svRPC = g_svRPC--网络rpc
local m_taskRPC = g_taskRPC--任务间rpc

--[[ 可选
--accept成功
local function onNetAccept(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
m_enevtDisp:regEvent(EnevtType.NetAccept, onNetAccept)
--链接事件
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
m_enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)
--连接断开
local function onNetCloseEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
m_enevtDisp:regEvent(EnevtType.NetClose, onNetCloseEvent)

--一帧
local function onFrameEvent(Proto, msgPack)
	local uiTick, uiCount = table.unpack(msgPack)	
end
m_enevtDisp:regEvent(EnevtType.Frame, onFrameEvent)--]]

--任务初始化
function initTask()
    
end

--有新任务执行
function runTask()
    local varRecv = m_pChan:Recv()
	if not varRecv then
		return
	end
		
	local evType, Proto, msgPakc = utile.unPack(varRecv)
	m_enevtDisp:onEvent(evType, Proto, msgPakc)
end

--任务销毁
function destroyTask()
	
end
