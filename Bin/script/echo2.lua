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
	print(string.format("%d, strType : %s val %s", os.time(), strType, tostring(rtnMsg)))
end

local function rpcGetLink(bOk, rtnMsg)
	if not rtnMsg then
		return
	end
	
	local rpcId = svRPC:callRPC(rtnMsg.sock, rtnMsg.session, taskName, "echo1.add", svRPC:createParam(5, 5), rcpBack, "server rpc")
	DEV_Reg(timeWheel, 5, removeRPC, svRPC, rpcId)
end

--测试
local function echo(uiSock, uiSession, tMsg)
	local tmsg = {}
	tmsg.rtn = 0
	tmsg.msg = "echo2 json return"
	local pWBinary = httpd.Response(200, tmsg)
    humble.sendB(uiSock, uiSession, pWBinary)
	
	local rpcId = taskRPC:callRPC("echo1", taskName, "echo1.add", taskRPC:createParam(4, 8), rcpBack, "task rpc")
	DEV_Reg(timeWheel, 5, removeRPC, taskRPC, rpcId)
	
	rpcId = taskRPC:callRPC("task_rpclink", taskName, "task_rpclink.getRPCLink", taskRPC:createParam("2"), rpcGetLink)
	DEV_Reg(timeWheel, 5, removeRPC, taskRPC, rpcId)
end
netDisp:regNetEvent("/echo2", echo)

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
