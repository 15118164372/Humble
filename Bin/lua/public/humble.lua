--c++导出函数
require("macros")
local string = string
local debug = debug
local regTask = regTask
local broadCast = broadCast
local H_LOG = H_LOG
local m_Humble = g_Humble
local m_curWorker = g_curWorker
local SockType = SockType

local humble = {}

--日志
--文本日志级别 与c++中的对应
local LogLV = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
}
LogLV = table.enum(LogLV, 1)

local function getFileName(strFile)
	local strName = string.match(strFile, ".+/([^/]*%.%w+)$")
	if nil == strName or 0 == #strName then
		strName = string.match(strFile, ".+\\([^\\]*%.%w+)$")
	end
	
	return strName
end
--设置日志级别
function humble.setLogPriority(logLevel)
	m_Humble:setLogPriority(logLevel)
end
function humble.Debugf(fmt, ...)	
	if LogLV.DEBUG > m_Humble:getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt,table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.DEBUG, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Infof(fmt, ...)
	if LogLV.INFO > m_Humble:getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.INFO, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Warnf(fmt, ...)
	if LogLV.WARN > m_Humble:getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.WARN, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Errorf(fmt, ...)
	if LogLV.ERROR > m_Humble:getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.ERROR, getFileName(stack.short_src), stack.currentline, strMsg)
end

--任务注册
function humble.regTask(pszFile, pszName, uiCapacity)
	regTask(pszFile, pszName, uiCapacity)
end
--任务反注册
function humble.unRegTask(pszName)
	m_Humble:unRegTask(pszName)
end
--获取任务对象
function humble.getTask(pszName)
	return m_Humble:getTask(pszName)
end
--获取所有任务名
function humble.getAllName()
	return m_Humble:getAllName(m_curWorker)
end

--网络RPC
function humble.netRPC(uiSock, pszFrom, pszTo, pszContent)
	m_Humble:netRPC(uiSock, pszFrom, pszTo, pszContent, #pszContent)
end
--服务间RPC
function humble.taskRPC(toTask, rpcName, pszParam, ulId)
	local pTo = humble.getTask(toTask)
	if not pTo then
		return
	end
	
	m_Humble:taskRPC(m_curWorker, pTo, rpcName, pszParam, #pszParam, ulId)
end
function humble.taskRPCRtn(pTo, pszRtn, ulId)
	m_Humble:taskRPCRtn(pTo, pszRtn, #pszRtn, ulId)
end

--超时注册
function humble.regTimeOut(uiTime, ulId)
	m_Humble:regTimeOut(m_curWorker, uiTime, ulId)
end

--socket accept成功 注册
function humble.regAcceptEvent(usType)
	m_Humble:regAcceptEvent(m_curWorker, usType)
end
--socket连接成功 注册
function humble.regConnectEvent(usType)
	m_Humble:regConnectEvent(m_curWorker, usType)
end
--socket关闭  注册
function humble.regCloseEvent(usType)
	m_Humble:regCloseEvent(m_curWorker, usType)
end

--网络可读
function humble.regIProto(iProto)
	m_Humble:regIProto(m_curWorker, iProto)
end
function humble.regHttpdProto(pszProto)
	m_Humble:regHttpdProto(m_curWorker, pszProto)
end

--关闭连接
function humble.closeLink(sock)
	m_Humble:closeLink(sock)
end

--新加监听
function humble.addListener(pszParser, usType, pszHost, usPort)
	m_Humble:addListener(pszParser, usType, pszHost, usPort)
end
--default
function humble.defServer(pszHost, usPort)
	m_Humble:addListener("default", SockType.Default, pszHost, usPort)
end
--debug
function humble.debugServer(pszHost, usPort)
	m_Humble:addListener("debug", SockType.Debug, pszHost, usPort)
end
--rpc
function humble.rpcServer(pszHost, usPort)
	m_Humble:addListener("rpc", SockType.RPC, pszHost, usPort)
end
--http
function humble.httpServer(pszHost, usPort)
	m_Humble:addListener("httpd", SockType.Httpd, pszHost, usPort)
end
--websocket
function humble.wsServer(pszHost, usPort)
	m_Humble:addListener("ws", SockType.WS, pszHost, usPort)
end

--添加主动连接 bKeepAlive 断开后是否重连
function humble.addLinker(pszParser, usType, pszHost, usPort, bKeepAlive)
	m_Humble:addLinker(nil, pszParser, usType, pszHost, usPort, 0, bKeepAlive)
end
--绑定到任务的 connet close消息走私有的
function humble.addBindLinker(pszParser, usType, pszHost, usPort, ulId, bKeepAlive)
	m_Humble:addLinker(m_curWorker, pszParser, usType, pszHost, usPort, ulId, bKeepAlive)
end
--rpc客户端
function humble.rpcClient(pszHost, usPort)
	humble.addLinker("rpc", SockType.RPC, pszHost, usPort, true)
end

--绑定socket消息到任务 close消息走通用的
function humble.bindWorker(sock)
	m_Humble:bindWorker(m_curWorker, sock)
end
function humble.unBindWorker(sock)
	m_Humble:unBindWorker(sock)
end

--发送消息
function humble.sendMsg(sock, pszBuf)
	m_Humble:sendMsg(sock, pszBuf, #pszBuf)
end
function humble.broadCast(socks, pszBuf)
	broadCast(socks, pszBuf, #pszBuf)
end

--获取rpc连接的socket
--根据服务器Id获取
function humble.getALinkById(svId)
	return m_Humble:getALinkById(svId)
end
function humble.getLinkById(svId)
	return m_Humble:getLinkById(m_curWorker, svId)
end
--根据服务器类型获取
function humble.getALinkByType(svType)
	return m_Humble:getALinkByType(svType)
end
function humble.getLinkByType(svType)
	return m_Humble:getLinkByType(m_curWorker, svType)
end

--服务器id
function humble.getServiceId()
	return m_Humble:getServiceId()
end
--服务器类型
function humble.getServiceType()
	return m_Humble:getServiceType()
end
--获取当前程序所在路径
function humble.getProPath()
	return m_Humble:getProPath()
end
--获取脚本所在路径
function humble.getScriptPath()
	return m_Humble:getScriptPath()
end
--服务器间连接key
function humble.getRPCKey()
	return m_Humble:getRPCKey()
end

--邮件 只能发送一次,调用Send后会销毁该对象
function humble.newMail()
	return newMail()
end
function humble.sendMail(pMail)
	m_Humble:sendMail(pMail)
end

return humble
