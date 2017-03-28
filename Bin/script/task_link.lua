--[[
���ӹ���
--]]

require("global")
local utile = require("utile")
local humble = require("humble")
local pChan = g_pChan

g_taskName = "task_link"
local taskName = g_taskName
initGlobal(taskName)
local enevtDisp = g_enevtDisp
local timeWheel = g_timeWheel
local netDisp = g_netDisp
local svRPC = g_svRPC
local taskRPC = g_taskRPC

if not g_allLink then
	g_allLink = {}
end
local allLink = g_allLink

--��ȡĳһ���͵���������
local function getAllLink(sockeType)
	return allLink[sockeType]
end
taskRPC:regRPC("task_link.getAllLink", getAllLink)

--�����ȡĳһ���͵�һ������
local function getRandLink(sockeType)
	local tSocks = allLink[sockeType]
	if not tSocks then
		return nil
	end
	
	return tSocks[math.random(1, #tSocks)]
end
taskRPC:regRPC("task_link.getRandLink", getRandLink)

--�����¼�
local function onNetLinkedEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	if not allLink[uiType] then
		allLink[uiType] = {}
	end
	
	table.insert(allLink[uiType], {uiSock, uiSession})
end
enevtDisp:regEvent(EnevtType.NetLinked, onNetLinkedEvent)

--���ӶϿ�
local function onNetCloseEvent(Proto, msgPack)
	local uiSock, uiSession, uiType = table.unpack(msgPack)
	if not allLink[uiType] then
		return
	end
	
	for key, val in pairs(allLink[uiType]) do
		if val[1] == uiSock and val[2] == uiSession then
			table.remove(allLink[uiType], key)
			break
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

