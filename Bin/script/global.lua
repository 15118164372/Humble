--[[
������ͬȫ�ֱ�����ʼ��
--]]

require("cmd")
require("timewheel")
local SVRPC = require("svrpc")
local netdisp = require("netdisp")
local enevtdisp = require("enevtdisp")
local SVRPC = require("svrpc")
local TaskRPC = require("taskrpc")
local table = table

g_taskName = ""
g_netDisp = nil
local netDisp = nil
g_enevtDisp = nil
local enevtDisp = nil
g_svRPC = nil
local svRPC = nil
g_taskRPC = nil
local taskRPC = nil
g_timeWheel = nil
local timeWheel = nil

--cmd
local function onCMDEvent(Proto, msgPack)
	local uiSock, uiSession, strMsg = table.unpack(msgPack)
	doCmd(Proto, uiSock, uiSession, strMsg)
end

--��������rpc
local function onSVRPCEvent(Proto, msgPack)
	local uiSock, uiSession, tMsg = table.unpack(msgPack)
	svRPC:onRPC(uiSock, uiSession, tMsg)
end

--�����rpc
local function onTaskRPCEvent(Proto, msgPack)
	taskRPC:onRPC(table.unpack(msgPack))
end

--tcp�ɶ��¼�
local function onTcpReadEvent(Proto, msgPack)
	local uiSock, uiSession, tMsg = table.unpack(msgPack)
	netDisp:onNetEvent(Proto, uiSock, uiSession, tMsg)
end

--udp�ɶ�
local function onUdpReadEvent(Proto, msgPack)
	
end

--һ��
local function onSecond_1Enevt(Proto, msgPack)
	DEV_OnTime(timeWheel)
end

local function initEnevt()
	--cmd
	enevtDisp:regEvent(EnevtType.CMD, onCMDEvent)
	--��������rpc
	enevtDisp:regEvent(EnevtType.CallRPC, onSVRPCEvent)
	enevtDisp:regEvent(EnevtType.RPCRtn, onSVRPCEvent)
	--�����rpc
	enevtDisp:regEvent(EnevtType.TaskCallRPC, onTaskRPCEvent)
	enevtDisp:regEvent(EnevtType.TaskRPCRtn, onTaskRPCEvent)
	--tcp�ɶ��¼�
	enevtDisp:regEvent(EnevtType.TcpRead, onTcpReadEvent)	
	--udp�ɶ�
	enevtDisp:regEvent(EnevtType.UdpRead, onUdpReadEvent)	
	--һ��
	enevtDisp:regEvent(EnevtType.Second_1, onSecond_1Enevt)
end

function initGlobal(strTaskName)
	--��ֵ
	if not g_enevtDisp then
		g_enevtDisp = enevtdisp:new()
	end
	enevtDisp = g_enevtDisp
	
	if not g_timeWheel then
		g_timeWheel = WheelMgr:new()
	end
	timeWheel = g_timeWheel
	
	if not g_netDisp then
		g_netDisp = netdisp:new(strTaskName)
	end
	netDisp = g_netDisp
	
	if not g_svRPC then
		g_svRPC = SVRPC:new(strTaskName)
	end
	svRPC = g_svRPC
	
	if not g_taskRPC then
		g_taskRPC = TaskRPC:new(strTaskName)
	end
	taskRPC = g_taskRPC
	
	initEnevt()
end
