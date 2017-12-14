--rpc¡¨Ω”
require("init")
local humble = require("humble")
local cjson = require("cjson")
local rpcLinkRegister = rpcLinkRegister
local rpcLinkUnregister = rpcLinkUnregister
local SockType = SockType
local m_curRPCLink = g_curRPCLink
local m_strTaskName = g_taskName
local regRPC = regRPC
local regClosedEv = regClosedEv
local callNetRPC = callNetRPC
local regLinkedEv = regLinkedEv

if not g_RPCReg then
	g_RPCReg = {}
end
local m_RPCReg = g_RPCReg

if not g_WaitRegister then
	g_WaitRegister = {}
end
local m_WaitRegister = g_WaitRegister

local m_RPCKey = humble.getRPCKey()
local m_RegTimeOut = 500

function initTask()
	
end

function destroyTask()
	
end

local function removeWaitReg(sock)
	if m_WaitRegister[sock] then
		m_WaitRegister[sock] = nil
	end
end

local function getToken(regInfo)
	return humble.md5Str(regInfo.svId..regInfo.svType..regInfo.ts..m_RPCKey)
end

local function checkToken(regInfo)
	if not regInfo.svId or not regInfo.svType or not regInfo.ts then
		return false
	end
	
	if getToken(regInfo) ~= regInfo.token then
		return false
	end
	
	return true
end

local function register(rpcParam)
	local sock = m_curRPCLink:getSock()
	local regInfo = cjson.decode(rpcParam)
	if not checkToken(regInfo) then
		humble.Warnf("register server %d, type %d token error", regInfo.svId, regInfo.svType)
		removeWaitReg(sock)
		humble.closeLink(sock)
		return
	end
	
	m_RPCReg[sock] = table.copy(regInfo)
	rpcLinkRegister(regInfo.svId, regInfo.svType, sock)
	humble.Infof("register server %d, type %d, sock %d", regInfo.svId, regInfo.svType, sock)
	
	regInfo.svId = humble.getSVId()
	regInfo.svType = humble.getSVType()
	regInfo.ts = humble.milSecond()
	regInfo.token = getToken(regInfo)
	
	removeWaitReg(sock)
	
	return cjson.encode(regInfo)
end
regRPC("register", register)

local function registerRtn(rpcParam, sock)
	if 0 == #rpcParam then
		humble.Warnf("%s", "register service error")
		return
	end
	
	local regInfo = cjson.decode(rpcParam)
	if not checkToken(regInfo) then
		humble.Warnf("register server %d, type %d token error", regInfo.svId, regInfo.svType)
		humble.closeLink(sock)
		return
	end
	
	m_RPCReg[sock] = regInfo
	rpcLinkRegister(regInfo.svId, regInfo.svType, sock)
	humble.Infof("register server %d, type %d, sock %d", regInfo.svId, regInfo.svType, sock)
end

local function rpcLinked(sock, sockType)
	local regInfo = {}
	regInfo.svId = humble.getSVId()
	regInfo.svType = humble.getSVType()
	regInfo.ts = humble.milSecond()
	regInfo.token = getToken(regInfo)
	
	callNetRPC(sock, m_strTaskName, "register", cjson.encode(regInfo), registerRtn, sock)
end
regLinkedEv(SockType.RPC, rpcLinked)

local function regTimeOut(sock)
	if m_WaitRegister[sock] then
		humble.Warnf("rpc register time out")
		humble.closeLink(sock)
		m_WaitRegister[sock] = nil
	end
end

local function onRPCAccept(sock, sockType)
	m_WaitRegister[sock] = true
	regDelayEv(m_RegTimeOut, regTimeOut, sock)
end
regAcceptEv(SockType.RPC, onRPCAccept)

local function rpcClosed(sock, sockType)
	removeWaitReg(sock)
	if not m_RPCReg[sock] then
		return
	end
	
	humble.Infof("unregister server %d, type %d, sock %d", m_RPCReg[sock].svId, m_RPCReg[sock].svType, sock)
	rpcLinkUnregister(m_RPCReg[sock].svId, m_RPCReg[sock].svType, sock)
	m_RPCReg[sock] = nil
end
regClosedEv(SockType.RPC, rpcClosed)
