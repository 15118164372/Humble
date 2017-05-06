--[[
��������rpc���ӹ���
--]]

require("global")
local utile = require("utile")
local humble = require("humble")
local pChan = g_pChan

g_taskName = "task_rpclink"
initGlobal(g_taskName)
local taskName = g_taskName
local enevtDisp = g_enevtDisp
local timeWheel = g_timeWheel
local netDisp = g_netDisp
local svRPC = g_svRPC
local taskRPC = g_taskRPC

if not g_RPCLink then
	g_RPCLink = {}
end
local m_RPCLink = g_RPCLink

if not g_Register then
	g_Register = {}
end
local m_Register = g_Register

--��ȡ������id��������Ϣ
local function getRPCLink(svId)
	return m_RPCLink[svId]
end
taskRPC:regRPC("task_rpclink.getRPCLink", getRPCLink)

--ע�ᳬʱ
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

--ע�������id
local function registerSVId(svId)
	local curSock, curSession = svRPC:getCurSock()
	local curRegister = m_Register[curSock]
	if not curRegister then
		utile.Warnf("not find rpc register info.")
		return -1
	end
	
	curRegister.svId = svId	
	m_RPCLink[svId] = curRegister
	
	m_Register[curSock] = nil
	
	return getSVId()
end
svRPC:regRPC("task_rpclink.registerSVId", registerSVId)

--ע�������id����
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
	
	m_Register[uiSock] = nil
end

--accept�ɹ�
local function onNetAccept(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	
	local rpcAccep = {}
	rpcAccep.sock = uiSock
	rpcAccep.session = uiSession
	rpcAccep.svId = 0
	
	m_Register[uiSock] = rpcAccep
	DEV_Reg(timeWheel, 5, registerTimeOut, uiSock, uiSession)
end
enevtDisp:regEvent(EnevtType.NetAccept, onNetAccept)

--�����¼�
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	
	local rpcLink = {}
	rpcLink.sock = uiSock
	rpcLink.session = uiSession
	rpcLink.svId = 0
	
	m_Register[uiSock] = rpcLink
	
	--ע��
	svRPC:callRPC(uiSock, uiSession, taskName, "task_rpclink.registerSVId", 
		svRPC:createParam(getSVId()), registerCB, uiSock)		
end
enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)

--���ӶϿ�
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
enevtDisp:regEvent(EnevtType.NetClose, onNetCloseEvent)

--�����ʼ��
function initTask()
    
end

--��������ִ��
function runTask()
    local varRecv = pChan:Recv()
	if not varRecv then
		return
	end
		
	local evType, Proto, msgPakc = utile.unPack(varRecv)
	enevtDisp:onEvent(evType, Proto, msgPakc)
end

--��������
function destroyTask()
	
end
