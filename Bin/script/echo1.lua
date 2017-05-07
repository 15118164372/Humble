--[[
echo1
--]]

require("global")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")
local pChan = g_pChan

local taskName = g_taskName
local enevtDisp = g_enevtDisp
local timeWheel = g_timeWheel
local netDisp = g_netDisp
local svRPC = g_svRPC
local taskRPC = g_taskRPC

local function echo(uiSock, uiSession, tMsg)
	local tmsg = {}
	tmsg.rtn = 0
	tmsg.msg = "echo1 json return"
	local pWBinary = httpd.Response(200, tmsg)
    humble.sendB(uiSock, uiSession, pWBinary)
end
netDisp:regNetEvent("/echo1", echo)

local function add(iA, iB)
	return iA + iB
end
taskRPC:regRPC("echo1.add", add)
svRPC:regRPC("echo1.add", add)

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
