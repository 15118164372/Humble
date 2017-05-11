--[[
����ģ��
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

--[[ ��ѡ
--accept�ɹ�
local function onNetAccept(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
m_enevtDisp:regEvent(EnevtType.NetAccept, onNetAccept)
--�����¼�
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
m_enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)
--���ӶϿ�
local function onNetCloseEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
m_enevtDisp:regEvent(EnevtType.NetClose, onNetCloseEvent)

--һ֡
local function onFrameEvent(Proto, msgPack)
	local uiTick, uiCount = table.unpack(msgPack)	
end
m_enevtDisp:regEvent(EnevtType.Frame, onFrameEvent)--]]

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
