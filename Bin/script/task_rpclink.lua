--[[
服务器间rpc链接管理
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

if not g_RPCLink then
	g_RPCLink = {}
end
local m_RPCLink = g_RPCLink

if not g_Register then
	g_Register = {}
end
local m_Register = g_Register

--获取服务器id的链接信息
local function getRPCLink(svId)
	return m_RPCLink[svId]
end
m_taskRPC:regRPC("getRPCLink", getRPCLink)

--注册超时
local function registerTimeOut(uiSock, uiSession)
	local curRegister = m_Register[uiSock]
	if not curRegister then
		return
	end
	
	if curRegister.session == uiSession then
		utile.Warnf("rpc register timeout.")
		humble.closeSock(uiSock, uiSession)
		m_Register[uiSock] = nil
	end
end

--注册服务器id
local function registerSVId(svId)
	local curSock, curSession = m_svRPC:getCurSock()
	local curRegister = m_Register[curSock]
	if not curRegister then
		utile.Warnf("not find rpc register info.")
		return -1
	end
	
	local mySVId = utile.getSVId()
	if mySVId == svId then
		utile.Errorf("repeat server id %s.", svId)
		return -1
	end
	
	curRegister.svId = svId	
	m_RPCLink[svId] = curRegister
	utile.Infof("register server, id %s", svId)
	
	m_Register[curSock] = nil
	
	return mySVId
end
m_svRPC:regRPC("registerSVId", registerSVId)

--注册服务器id返回
local function registerCB(bOk, rtnMsg, uiSock)
	if not bOk or -1 == rtnMsg then 
		m_Register[uiSock] = nil
		utile.Warnf("rpc register return error.")
		return
	end
	
	local curRegister = m_Register[uiSock]
	if not curRegister then
		utile.Warnf("not find rpc register info.")
		return 
	end
	
	curRegister.svId = rtnMsg
	m_RPCLink[rtnMsg] = curRegister
	utile.Infof("register server, id:%s", rtnMsg)
	
	m_Register[uiSock] = nil
end

--accept成功
local function onNetAccept(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	
	local rpcAccep = {}
	rpcAccep.sock = uiSock
	rpcAccep.session = uiSession
	rpcAccep.svId = 0
	
	m_Register[uiSock] = rpcAccep
	DEV_Reg(m_timeWheel, 5, registerTimeOut, uiSock, uiSession)
end
m_enevtDisp:regEvent(EnevtType.NetAccept, onNetAccept)

--链接事件
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	
	local rpcLink = {}
	rpcLink.sock = uiSock
	rpcLink.session = uiSession
	rpcLink.svId = 0
	
	m_Register[uiSock] = rpcLink
	
	--注册
	m_svRPC:callRPC(uiSock, uiSession, m_taskName, m_taskName, "registerSVId", 
		m_svRPC:createParam(utile.getSVId()), registerCB, uiSock)		
end
m_enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)

--连接断开
local function onNetCloseEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	
	if m_Register[uiSock] then
		m_Register[uiSock] = nil
		return
	end
	
	for key, val in pairs(m_RPCLink) do
		if val.sock == uiSock and val.session == uiSession then
			m_RPCLink[key] = nil
			return
		end
	end
end
m_enevtDisp:regEvent(EnevtType.NetClose, onNetCloseEvent)

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
