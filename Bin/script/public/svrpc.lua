--[[
rpcģ��,��������rpcͨ��
--]]

require("macros")
require("logicmacro")
local utile = require("utile")
local humble = require("humble")
local cjson = require("cjson")
local tcp3 = require("tcp3")
local EnevtType = EnevtType
local table = table
local string = string
local tostring = tostring

local SVRPC = {}
SVRPC.__index = SVRPC

--strTaskName ģ����
function SVRPC:new(strTaskName)
	assert(strTaskName and 0 ~= #strTaskName)
    local self = {}
    setmetatable(self, SVRPC)
	
    self.TaskName = strTaskName
	self.Proto = {}	
	self.Id = 0
	self.RPCCash = {}
	self.curSock = -1
	self.curSession = -1

    return self
end

function SVRPC:creatRPCName(strTask, strRPCName)
	return strTask.."."..strRPCName
end

--rcpע��
function SVRPC:regRPC(strRPCName, Func)
	assert("function" == type(Func))
	
	self.Proto[self:creatRPCName(self.TaskName, strRPCName)] = Func
    utile.Debugf("register service rpc protocol %s", strRPCName)
end

--��ȡ��ǰִ��rpc��Ϣ��sock session ��Ϣ
function SVRPC:getCurSock()
	return self.curSock, self.curSession
end

--rpcִ��
function SVRPC:onRPC(uiSock, uiSession, tRPC)
	self.curSock = uiSock
	self.curSession = uiSession
	
	--rpc ����
	if tRPC.Enevt == EnevtType.CallRPC then
		local Func = self.Proto[tRPC.Method]
		if Func then
			tRPC.Rnt, tRPC.Param = utile.callFunc(Func, table.unpack(tRPC.Param))
			if not tRPC.Rnt then
				utile.Errorf("%s call sv rpc %s error,message: %s.", 
					tRPC.RecvTask, tRPC.Method, tRPC.Param)
			end
		else
			tRPC.Rnt = false
			tRPC.Param = string.format("not find service rpc method %s", tRPC.Method)
			utile.Errorf("%s", tRPC.Param)
		end
		
		if 0 ~= tRPC.ID then
			tRPC.Enevt = EnevtType.RPCRtn			
			humble.sendB(uiSock, uiSession,
				tcp3.Response(cjson.encode(tRPC)))
		end	
		
		return
	else
		local rpcCash = self.RPCCash[tRPC.ID]
		if rpcCash then
			utile.callFunc(rpcCash.Func, tRPC.Rnt, tRPC.Param, table.unpack(rpcCash.Param))
			self.RPCCash[tRPC.ID] = nil
		end
	end
end

--��ʱ��,��timewheel���
function SVRPC:removeById(rpcId)
	if 0 == rpcId then
		return
	end
	
	local rpcCash = self.RPCCash[rpcId]
	if rpcCash then
		utile.Warnf("call service rpc %s timeout.", rpcCash.Method)
		self.RPCCash[rpcId] = nil
	end
end

function removeRPC(objRPC, rpcId)
	objRPC:removeById(rpcId)
end

function SVRPC:createParam(...)
	return {...}
end

function SVRPC:getID()
	self.Id = self.Id + 1
	return self.Id
end

--���� Func(rpcOK, rpcMsg, ...)  FuncΪnil��ʾ����Ҫ����
function SVRPC:callRPC(uiSock, uiSession, strToTask, strRecvTask, strRPCName, tRPCParam, Func, ...)
	local rpcId = 0
	local realName = self:creatRPCName(strToTask, strRPCName)
	if Func then
		assert("function" == type(Func))
		rpcId = self:getID()
		local tRPCBC = {}
		tRPCBC.Func = Func
		tRPCBC.Method = realName
		tRPCBC.Param = {...}
		self.RPCCash[rpcId] = tRPCBC
	end
	
	local tCallRPC = {}
	tCallRPC.Enevt = EnevtType.CallRPC
	tCallRPC.ID = rpcId
	tCallRPC.Param = tRPCParam
	tCallRPC.ToTask = strToTask
	tCallRPC.RecvTask = strRecvTask
	tCallRPC.Method = realName
	
	humble.sendB(uiSock, uiSession,
		tcp3.Response(cjson.encode(tCallRPC)))
	
	return rpcId
end

return SVRPC
