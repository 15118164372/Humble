--[[
echo2
--]]

require("global")
local utile = require("utile")
local httpd = require("httpd")
local humble = require("humble")
local AOI = require("aoi")

local m_pChan = g_pChan--消息chan
local m_taskName = g_taskName--任务名
local m_enevtDisp = g_enevtDisp--事件
local m_timeWheel = g_timeWheel--时间轮
local m_netDisp = g_netDisp--网络消息事件
local m_svRPC = g_svRPC--网络rpc
local m_taskRPC = g_taskRPC--任务间rpc

local function rcpBack(bOk, rtnMsg, strType)
	print(string.format("%d, strType : %s val %s", os.time(), strType, tostring(rtnMsg)))
end

local function rpcGetLink(bOk, rtnMsg)
	if not rtnMsg then
		return
	end
	
	local rpcId = m_svRPC:callRPC(rtnMsg.sock, rtnMsg.session, "echo1", m_taskName, "add", m_svRPC:createParam(5, 5), rcpBack, "server rpc")
	DEV_Reg(m_timeWheel, 5, removeRPC, m_svRPC, rpcId)
end

local function testAoi()
	local aoi = AOI:new(10, 10)
	print("---------Enter-----------")
	table.print(aoi:Enter(1, 0, 0, 2, 2))
	print("-----------------------------")
	table.print(aoi:Enter(2, 0, 3, 2, 2))
	print("-----------------------------")
	table.print(aoi:Enter(3, 0, 5, 2, 2))
	print("-----------------------------")
	table.print(aoi:Enter(4, 1, 1, 2, 2))
	print("-----------------------------")
	table.print(aoi:Enter(5, 2, 2, 2, 2))
	print("-----------------------------")
	table.print(aoi:Enter(6, 3, 3, 2, 2))	
	
	print("---------Move-----------")
	local inArea, outArea, newArea = aoi:Move(6, 0, 5, 2, 2)
	table.print(inArea)
	print("-----------------------------")
	table.print(outArea)
	print("-----------------------------")
	table.print(newArea)
	print("-----------------------------")
	--aoi:Print()
	
	print("---------getAOIArea-----------")
	table.print(aoi:getAOIArea(6))
	
	print("---------Leave-----------")
	table.print(aoi:Leave(6))	
	aoi:Print()
end

--测试
local function echo(uiSock, uiSession, tMsg)
	local tmsg = {}
	tmsg.rtn = 0
	tmsg.msg = "echo2 json return"
	local pWBinary = httpd.Response(200, tmsg)
    humble.sendB(uiSock, uiSession, pWBinary)	
	
	local rpcId = m_taskRPC:callRPC("echo1", m_taskName, "add", m_taskRPC:createParam(4, 8), rcpBack, "task rpc")
	DEV_Reg(m_timeWheel, 5, removeRPC, m_taskRPC, rpcId)
	
	rpcId = m_taskRPC:callRPC("task_rpclink", m_taskName, "getRPCLink", m_taskRPC:createParam("2"), rpcGetLink)
	DEV_Reg(m_timeWheel, 5, removeRPC, m_taskRPC, rpcId)
end
m_netDisp:regNetEvent("/echo2", echo)

local function removeRepeatTask(strName)
	humble.unregTask(strName)
	--print(strName)
end

local iTaskName = 0
local function testRepeatTask()
	for i = 1, 20 do
		iTaskName = iTaskName + 1
		humble.regTask("echo1.lua", tostring(iTaskName), 50)
		--humble.unregTask(tostring(iTaskName))
		m_taskRPC:callRPC("echo1", tostring(iTaskName), "showTest", m_taskRPC:createParam(tostring(iTaskName)))
		DEV_Reg(m_timeWheel, 1, removeRepeatTask, tostring(iTaskName))
	end
	
	--collectgarbage("collect") 
	
	DEV_Reg(m_timeWheel, 5, testRepeatTask)
end
--DEV_Reg(m_timeWheel, 5, testRepeatTask)

--任务初始化
function initTask()
    --testAoi()
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
