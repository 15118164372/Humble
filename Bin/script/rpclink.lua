--rpc¡¨Ω”
require("init")
require("proto")
local humble = require("humble")
local rpcLinkRegister = rpcLinkRegister
local rpcLinkUnregister = rpcLinkUnregister
local ErrCode = ErrCode
local SockType = SockType
local Proto = Proto
local m_strTaskName = g_taskName
local m_regTimeOut = 5

if not g_RPCSV then
	g_RPCSV = {}
end
local m_RPCSV = g_RPCSV

function initTask()
	
end

function destroyTask()
	
end

local function register(sock, svId)
	svId = tonumber(svId)
	m_RPCSV[svId] = sock
	rpcLinkRegister(svId, sock)
	humble.Infof("register service %d", svId)

	return tostring(humble.getSVId())
end
regRPC("register", register)

local function registerRtn(svId, sock)
	if 0 == #svId then
		humble.Warnf("%s", "register service error")
		return
	end
	
	svId = tonumber(svId)
	m_RPCSV[svId] = sock
	rpcLinkRegister(svId, sock)
	humble.Infof("register service %d", svId)
end

local function rpcLinked(sock, sockType)
	callNetRPC(sock, m_strTaskName, "register", tostring(humble.getSVId()), registerRtn, sock)
end
regLinkedEv(SockType.RPC, rpcLinked)

local function rpcClosed(sock, sockType)
	for key, val in pairs(m_RPCSV) do
		if val == sock then
			m_RPCSV[key] = nil
			rpcLinkUnregister(key)
			humble.Infof("unregister service %s", key)
			break
		end
	end
end
regClosedEv(SockType.RPC, rpcClosed)
