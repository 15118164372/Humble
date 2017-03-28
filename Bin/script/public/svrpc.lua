--[[
rpc模块,服务器间rpc通信
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

--strTaskName 模块名
function SVRPC:new(strTaskName)
	assert(strTaskName and 0 ~= #strTaskName)
    local self = {}
    setmetatable(self, SVRPC)
    
	local objId = CSnowFlake()
	objId:setWorkid(1)
	objId:setCenterid(1)
	
    self.TaskName = strTaskName
	self.Proto = {}	
	self.SnowFlake = objId
	self.RPCCash = {}

    return self
end

--rcp注册
function SVRPC:regRPC(strRPCName, Func)
	assert("function" == type(Func))
	
	humble.regProto(strRPCName, self.TaskName)
	
	self.Proto[strRPCName] = Func
    utile.Debug("register service rpc protocol %s", strRPCName)
end

--rpc执行
function SVRPC:onRPC(uiSock, uiSession, tRPC)
	--rpc 调用
	if tRPC.Enevt == EnevtType.CallRPC then
		local Func = self.Proto[tRPC.Method]
		if Func then
			tRPC.Rnt, tRPC.Param = utile.callFunc(Func, table.unpack(tRPC.Param))
		else
			tRPC.Rnt = false
			tRPC.Param = string.format("not find service rpc method %s", tRPC.Method)
		end
		
		tRPC.Enevt = EnevtType.RPCRtn
		
		humble.sendB(uiSock, uiSession,
			tcp3.Response(cjson.encode(tRPC)))
		
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
function SVRPC:removeById(rpcId)
	local rpcCash = self.RPCCash[rpcId]
	if rpcCash then
		utile.Log(LogLV.Warn, "call service rpc %s timeout.", rpcCash.Method)
		self.RPCCash[rpcId] = nil
	end
end

function removeRPC(objRPC, rpcId)
	objRPC:removeById(rpcId)
end

function SVRPC:createParam(...)
	return {...}
end

--调用 Func(rpcOK, rpcMsg, ...)
function SVRPC:callRPC(uiSock, uiSession, strRecvTask, strRPCName, tRPCParam, Func, ...)
	local rpcId = self.SnowFlake:getID()
	local tRPCBC = {}
	tRPCBC.Func = Func
	tRPCBC.Method = strRPCName
	tRPCBC.Param = {...}
	self.RPCCash[rpcId] = tRPCBC
	
	local tCallRPC = {}
	tCallRPC.Enevt = EnevtType.CallRPC
	tCallRPC.ID = rpcId
	tCallRPC.Param = tRPCParam
	tCallRPC.RecvTask = strRecvTask
	tCallRPC.Method = strRPCName
	
	humble.sendB(uiSock, uiSession,
		tcp3.Response(cjson.encode(tCallRPC)))
	
	return rpcId
end

return SVRPC
