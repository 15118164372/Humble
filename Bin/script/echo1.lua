--[[
echo1
--]]

require("global")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")

local pChan = g_pChan--消息chan
local taskName = g_taskName--任务名
local enevtDisp = g_enevtDisp--事件
local timeWheel = g_timeWheel--时间轮
local netDisp = g_netDisp--网络消息事件
local svRPC = g_svRPC--网络rpc
local taskRPC = g_taskRPC--任务间rpc

local function echo(uiSock, uiSession, tMsg)
	local tmsg = {}
	tmsg.rtn = 0
	tmsg.msg = "echo1 json return"
	local pWBinary = httpd.Response(200, tmsg)
    humble.sendB(uiSock, uiSession, pWBinary)
end
--netDisp:regNetEvent("/echo1", echo)

local function add(iA, iB)
	return iA + iB
end
taskRPC:regRPC("add", add)
svRPC:regRPC("add", add)

local function showTest(strName)
	print(string.format("%s call showTest", strName))
end
taskRPC:regRPC("showTest", showTest)

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
