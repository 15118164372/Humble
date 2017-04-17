--[[
rpcģ��,�����rpcͨ��
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

function TaskRPC:new()
    local self = {}
    setmetatable(self, TaskRPC)
    
	local objId = CSnowFlake()
	objId:setWorkid(1)
	objId:setCenterid(1)
	
	self.Proto = {}	
	self.SnowFlake = objId
	self.RPCCash = {}

    return self
end

--rcpע��
function TaskRPC:regRPC(strRPCName, Func)
	assert("function" == type(Func))
	
	self.Proto[strRPCName] = Func
    utile.Debug("register task rpc protocol %s", strRPCName)
end

function TaskRPC:onRPC(tRPC)
	--rpc ����	
	if tRPC.Enevt == EnevtType.TaskCallRPC then
		local Func = self.Proto[tRPC.Method]
		if Func then
			tRPC.Rnt, tRPC.Param = utile.callFunc(Func, table.unpack(tRPC.Param))
			if not tRPC.Rnt then
				utile.Log(LogLV.Err, "%s call task rpc %s error,message: %s, .", 
					tRPC.RecvTask, tRPC.Method, tRPC.Param)
			end
		else
			tRPC.Rnt = false
			tRPC.Param = string.format("not find task rpc method %s", tRPC.Method)
			utile.Log(LogLV.Err, "%s", tRPC.Param)
		end		
		
		if 0 ~= tRPC.ID then
			tRPC.Enevt = EnevtType.TaskRPCRtn
			local objChan = humble.getChan(tRPC.RecvTask)
			if objChan then
				utile.chanSend(objChan, utile.Pack(EnevtType.TaskRPCRtn, nil, tRPC))
			else
				utile.Log(LogLV.Err, "task rpc %s, not find recv task %s.", tRPC.Method, tRPC.RecvTask)
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

--��ʱ��,��timewheel���
function TaskRPC:removeById(rpcId)
	if 0 == rpcId then
		return
	end
	
	local rpcCash = self.RPCCash[rpcId]
	if rpcCash then
		utile.Log(LogLV.Warn, "call task rpc %s timeout.", rpcCash.Method)
		self.RPCCash[rpcId] = nil
	end
end

function removeRPC(objRPC, rpcId)
	objRPC:removeById(rpcId)
end

function TaskRPC:createParam(...)
	return {...}
end

--���� Func(rpcOK, rpcMsg, ...) FuncΪnil��ʾ����Ҫ����
function TaskRPC:callRPC(strToTask, strRecvTask, strRPCName, tRPCParam, Func, ...)
	local objChan = humble.getChan(strToTask)
	if not objChan then
		utile.Log(LogLV.Err, "call task rpc %s, not find target task %s.", strRPCName, strToTask)
		return
	end
	
	local rpcId = 0
	if Func then
		assert("function" == type(Func))
		rpcId = self.SnowFlake:getID()
		local tRPCBC = {}
		tRPCBC.Func = Func
		tRPCBC.Method = strRPCName
		tRPCBC.Param = {...}
		self.RPCCash[rpcId] = tRPCBC
	end
	
	local tCallRPC = {}
	tCallRPC.Enevt = EnevtType.TaskCallRPC
	tCallRPC.ID = rpcId
	tCallRPC.Param = tRPCParam
	tCallRPC.RecvTask = strRecvTask
	tCallRPC.Method = strRPCName

	utile.chanSend(objChan, utile.Pack(EnevtType.TaskCallRPC, nil, tCallRPC))
	
	return rpcId
end

return TaskRPC
