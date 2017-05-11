--[[
��������rpc���ӹ���
--]]

require("global")
local utile = require("utile")
local humble = require("humble")

local m_pChan = g_pChan--��Ϣchan
local m_taskName = g_taskName--������
local m_enevtDisp = g_enevtDisp--�¼�
local m_timeWheel = g_timeWheel--ʱ����
local m_netDisp = g_netDisp--������Ϣ�¼�
local m_svRPC = g_svRPC--����rpc
local m_taskRPC = g_taskRPC--�����rpc

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
m_taskRPC:regRPC("getRPCLink", getRPCLink)

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
	utile.Infof("register server, id:%s", rtnMsg)
	
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
	DEV_Reg(m_timeWheel, 5, registerTimeOut, uiSock, uiSession)
end
m_enevtDisp:regEvent(EnevtType.NetAccept, onNetAccept)

--�����¼�
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	
	local rpcLink = {}
	rpcLink.sock = uiSock
	rpcLink.session = uiSession
	rpcLink.svId = 0
	
	m_Register[uiSock] = rpcLink
	
	--ע��
	m_svRPC:callRPC(uiSock, uiSession, m_taskName, m_taskName, "registerSVId", 
		m_svRPC:createParam(utile.getSVId()), registerCB, uiSock)		
end
m_enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)

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
m_enevtDisp:regEvent(EnevtType.NetClose, onNetCloseEvent)

--�����ʼ��
function initTask()
    
end

--��������ִ��
function runTask()
    local varRecv = m_pChan:Recv()
	if not varRecv then
		return
	end
		
	local evType, Proto, msgPakc = utile.unPack(varRecv)
	m_enevtDisp:onEvent(evType, Proto, msgPakc)
end

--��������
function destroyTask()
	
end
