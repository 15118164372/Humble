--[[
rpc模块,任务间rpc通信
--]]

require("macros")
require("logicmacro")
local utile = require("utile")
local humble = require("humble")
local cjson = require("cjson")
local EnevtType = EnevtType
local table = table
local string = string
local tostring = tostring

local TaskRPC = {}
TaskRPC.__index = TaskRPC

function TaskRPC:new(strTaskName)
	assert(strTaskName and 0 ~= #strTaskName)
    local self = {}
    setmetatable(self, TaskRPC)
    
	self.TaskName = strTaskName
	self.Proto = {}	
	self.Id = 0
	self.RPCCash = {}

    return self
end

function TaskRPC:creatRPCName(strTask, strRPCName)
	return strTask.."."..strRPCName
end

--rcp注册
function TaskRPC:regRPC(strRPCName, Func)
	assert("function" == type(Func))
	
	self.Proto[self:creatRPCName(self.TaskName, strRPCName)] = Func
    utile.Debugf("register task rpc protocol %s", strRPCName)
end

function TaskRPC:onRPC(tRPC)
	--rpc 调用	
	if tRPC.Enevt == EnevtType.TaskCallRPC then
		local Func = self.Proto[tRPC.Method]
		if Func then
			tRPC.Rnt, tRPC.Param = utile.callFunc(Func, table.unpack(tRPC.Param))
			if not tRPC.Rnt then
				utile.Errorf("%s call task rpc %s error,message: %s, .", 
					tRPC.RecvTask, tRPC.Method, tRPC.Param)
			end
		else
			tRPC.Rnt = false
			tRPC.Param = string.format("not find task rpc method %s", tRPC.Method)
			utile.Errorf("%s", tRPC.Param)
		end		
		
		if 0 ~= tRPC.ID then
			tRPC.Enevt = EnevtType.TaskRPCRtn
			local objChan = humble.getChan(tRPC.RecvTask)
			if objChan then
				utile.chanSend(objChan, utile.Pack(EnevtType.TaskRPCRtn, nil, tRPC))
			else
				utile.Errorf("task rpc %s, not find recv task %s.", tRPC.Method, tRPC.RecvTask)
			end
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

--超时用,与timewheel配合
function TaskRPC:removeById(rpcId)
	if 0 == rpcId then
		return
	end
	
	local rpcCash = self.RPCCash[rpcId]
	if rpcCash then
		utile.Warnf("call task rpc %s timeout.", rpcCash.Method)
		self.RPCCash[rpcId] = nil
	end
end

function removeRPC(objRPC, rpcId)
	objRPC:removeById(rpcId)
end

function TaskRPC:createParam(...)
	return {...}
end

function TaskRPC:getID()
	self.Id = self.Id + 1
	return self.Id
end

--调用 Func(rpcOK, rpcMsg, ...) Func为nil表示不需要返回
function TaskRPC:callRPC(strToTask, strRecvTask, strRPCName, tRPCParam, Func, ...)
	local objChan = humble.getChan(strToTask)
	if not objChan then
		utile.Errorf("call task rpc %s, not find target task %s.", strRPCName, strToTask)
		return
	end
	
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
	tCallRPC.Enevt = EnevtType.TaskCallRPC
	tCallRPC.ID = rpcId
	tCallRPC.Param = tRPCParam
	tCallRPC.RecvTask = strRecvTask
	tCallRPC.Method = realName
	
	utile.chanSend(objChan, utile.Pack(EnevtType.TaskCallRPC, nil, tCallRPC))
	
	return rpcId
end

return TaskRPC
