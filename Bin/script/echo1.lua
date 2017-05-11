--[[
echo1
--]]

require("global")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")

local m_pChan = g_pChan--消息chan
local m_taskName = g_taskName--任务名
local m_enevtDisp = g_enevtDisp--事件
local m_timeWheel = g_timeWheel--时间轮
local m_netDisp = g_netDisp--网络消息事件
local m_svRPC = g_svRPC--网络rpc
local m_taskRPC = g_taskRPC--任务间rpc

local function echo(uiSock, uiSession, tMsg)
	local tmsg = {}
	tmsg.rtn = 0
	tmsg.msg = "echo1 json return"
	local pWBinary = httpd.Response(200, tmsg)
    humble.sendB(uiSock, uiSession, pWBinary)
end
--m_netDisp:regNetEvent("/echo1", echo)

local function add(iA, iB)
	return iA + iB
end
m_taskRPC:regRPC("add", add)
m_svRPC:regRPC("add", add)

local function showTest(strName)
	print(string.format("%s call showTest", strName))
end
m_taskRPC:regRPC("showTest", showTest)

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
