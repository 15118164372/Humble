--[[
����ģ��
--]]

require("global")
local utile = require("utile")
local humble = require("humble")
local pChan = g_pChan

g_taskName = "������"
initGlobal(g_taskName)
local taskName = g_taskName
local enevtDisp = g_enevtDisp
local timeWheel = g_timeWheel
local netDisp = g_netDisp
local svRPC = g_svRPC
local taskRPC = g_taskRPC

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
