--[[
����ģ��
--]]

require("global")
local utile = require("utile")
local humble = require("humble")

local pChan = g_pChan--��Ϣchan
local taskName = g_taskName--������
local enevtDisp = g_enevtDisp--�¼�
local timeWheel = g_timeWheel--ʱ����
local netDisp = g_netDisp--������Ϣ�¼�
local svRPC = g_svRPC--����rpc
local taskRPC = g_taskRPC--�����rpc

--[[ ��ѡ
--accept�ɹ�
local function onNetAccept(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
enevtDisp:regEvent(EnevtType.NetAccept, onNetAccept)
--�����¼�
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)
--���ӶϿ�
local function onNetCloseEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
end
enevtDisp:regEvent(EnevtType.NetClose, onNetCloseEvent)

--һ֡
local function onFrameEvent(Proto, msgPack)
	local uiTick, uiCount = table.unpack(msgPack)	
end
enevtDisp:regEvent(EnevtType.Frame, onFrameEvent)--]]

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
