--[[
echo2
--]]

require("global")
local utile = require("utile")
local httpd = require("httpd")
local humble = require("humble")
local pChan = g_pChan

g_taskName = "echo2"
local taskName = g_taskName
initGlobal(taskName)
local enevtDisp = g_enevtDisp
local timeWheel = g_timeWheel
local netDisp = g_netDisp
local svRPC = g_svRPC
local taskRPC = g_taskRPC

local function rcpBack(bOk, rtnMsg, strType)
	print(string.format("strType : %s val %s", strType, tostring(rtnMsg)))
end

local function svRPCTimeOut(rpcId)
	svRPC:removeById(rpcId)
end

local function rpcGetLink(bOk, rtnMsg)
	local rpcId = svRPC:callRPC(rtnMsg[1], rtnMsg[2], taskName, "echo1.add", svRPC:createParam(5, 5), rcpBack, "server rpc")
	DEV_Reg(timeWheel, 5, svRPCTimeOut, rpcId)
end

local function rpcTimeOut(rpcId)
	taskRPC:removeById(rpcId)
end

--测试
local function echo2(uiSock, uiSession, tMsg)
	local tmsg = {}
	tmsg.rtn = 0
	tmsg.msg = "echo2 json return"
	local pWBinary = httpd.Response(200, tmsg)
    humble.sendB(uiSock, uiSession, pWBinary)
	
	local rpcId = taskRPC:callRPC("echo1", taskName, "echo1.add", taskRPC:createParam(4, 8), rcpBack, "task rpc")
	DEV_Reg(timeWheel, 5, rpcTimeOut, rpcId)
	
	rpcId = taskRPC:callRPC("task_link", taskName, "task_link.getRandLink", taskRPC:createParam(SockType.RPCCLIENT), rpcGetLink)
	DEV_Reg(timeWheel, 5, rpcTimeOut, rpcId)
end
netDisp:regNetEvent("/echo2", echo2)

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
