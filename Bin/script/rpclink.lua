--rpc¡¨Ω”
require("init")
require("proto")
local humble = require("humble")
local cjson = require("cjson")
local rpcLinkRegister = rpcLinkRegister
local rpcLinkUnregister = rpcLinkUnregister
local SockType = SockType
local m_curRPCLink = g_curRPCLink
local m_strTaskName = g_taskName

if not g_RPCReg then
	g_RPCReg = {}
end
local m_RPCReg = g_RPCReg

function initTask()
	
end

function destroyTask()
	
end

local function register(rpcParam)
	local regInfo = cjson.decode(rpcParam)
	local sock = m_curRPCLink:getSock()
	m_RPCReg[sock] = table.copy(regInfo)
	rpcLinkRegister(regInfo.svId, regInfo.svType, sock)
	humble.Infof("register server %d, type %d, sock %d", regInfo.svId, regInfo.svType, sock)
	
	regInfo.svId = humble.getSVId()
	regInfo.svType = humble.getSVType()
	
	return cjson.encode(regInfo)
end
regRPC("register", register)

local function registerRtn(rpcParam, sock)
	if 0 == #rpcParam then
		humble.Warnf("%s", "register service error")
		return
	end
	
	local regInfo = cjson.decode(rpcParam)
	m_RPCReg[sock] = regInfo
	rpcLinkRegister(regInfo.svId, regInfo.svType, sock)
	humble.Infof("register server %d, type %d, sock %d", regInfo.svId, regInfo.svType, sock)
end

local function rpcLinked(sock, sockType)
	local regInfo = {}
	regInfo.svId = humble.getSVId()
	regInfo.svType = humble.getSVType()
	
	callNetRPC(sock, m_strTaskName, "register", cjson.encode(regInfo), registerRtn, sock)
end
regLinkedEv(SockType.RPC, rpcLinked)

local function rpcClosed(sock, sockType)
	if not m_RPCReg[sock] then
		return
	end
	
	humble.Infof("unregister server %d, type %d, sock %d", m_RPCReg[sock].svId, m_RPCReg[sock].svType, sock)
	rpcLinkUnregister(m_RPCReg[sock].svId, m_RPCReg[sock].svType)
	m_RPCReg[sock] = nil
end
regClosedEv(SockType.RPC, rpcClosed)
