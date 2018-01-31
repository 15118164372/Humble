--c++��������
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

--��־
--�ı���־���� ��c++�еĶ�Ӧ
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
--������־����
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

--����ע��
function humble.regTask(pszFile, pszName, uiCapacity)
	regTask(pszFile, pszName, uiCapacity)
end
--����ע��
function humble.unregTask(pszName)
	m_Humble:unregTask(pszName)
end
--��ȡ�������
function humble.getTask(pszName)
	return m_Humble:getTask(pszName)
end
--��ȡ����������
function humble.getAllName()
	return m_Humble:getAllName(m_curWorker)
end
--����RPC
function humble.netRPC(uiSock, pszFrom, pszTo, pszContent)
	m_Humble:netRPC(uiSock, pszFrom, pszTo, pszContent, #pszContent)
end
--�����RPC
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
--��ʱע��
function humble.regTimeOut(uiTime, ulId)
	m_Humble:regTimeOut(m_curWorker, uiTime, ulId)
end
--socket accept�ɹ� ע��
function humble.regAcceptEvent(usType)
	m_Humble:regAcceptEvent(m_curWorker, usType)
end
--socket���ӳɹ� ע��
function humble.regConnectEvent(usType)
	m_Humble:regConnectEvent(m_curWorker, usType)
end
--socket�ر�  ע��
function humble.regCloseEvent(usType)
	m_Humble:regCloseEvent(m_curWorker, usType)
end

--����ɶ�
function humble.regIProto(iProto)
	m_Humble:regIProto(m_curWorker, iProto)
end
function humble.regHttpdProto(pszProto)
	m_Humble:regHttpdProto(m_curWorker, pszProto)
end

--�¼Ӽ���
function humble.addListener(pszParser, usType, pszHost, usPort)
	m_Humble:addListener(pszParser, usType, pszHost, usPort)
end
--debug
function humble.debugServer(pszHost, usPort)
	humble.addListener("debug", SockType.Debug, pszHost, usPort)
end
--rpc
function humble.rpcServer(pszHost, usPort)
	humble.addListener("rpc", SockType.RPC, pszHost, usPort)
end
--http
function humble.httpServer(pszHost, usPort)
	humble.addListener("httpd", SockType.Httpd, pszHost, usPort)
end
function humble.wsServer(pszHost, usPort)
	humble.addListener("ws", SockType.WS, pszHost, usPort)
end

--�����������
function humble.addLinker(pszParser, usType, pszHost, usPort, bKeepAlive)
	m_Humble:addLinker(nil, pszParser, usType, pszHost, usPort, bKeepAlive)
end
--�󶨵������
function humble.addBindLinker(pszParser, usType, pszHost, usPort, bKeepAlive)
	m_Humble:addLinker(m_curWorker, pszParser, usType, pszHost, usPort, bKeepAlive)
end
--rpc�ͻ���
function humble.rpcClient(pszHost, usPort)
	m_Humble:addLinker(nil, "rpc", SockType.RPC, pszHost, usPort, true)
end

--�ر�����
function humble.closeLink(sock)
	m_Humble:closeLink(sock)
end

--��socket��Ϣ������
function humble.bindWorker(sock)
	m_Humble:bindWorker(sock, m_curWorker)
end
function humble.unBindWorker(sock)
	m_Humble:unBindWorker(sock)
end

--������Ϣ
function humble.sendMsg(sock, pszBuf)
	m_Humble:sendMsg(sock, pszBuf, #pszBuf)
end
function humble.broadCast(socks, pszBuf)
	broadCast(socks, pszBuf, #pszBuf)
end

--��ȡrpc���ӵ�socket
function humble.getALinkById(svId)
	return m_Humble:getALinkById(svId)
end
function humble.getLinkById(svId)
	return m_Humble:getLinkById(m_curWorker, svId)
end
function humble.getALinkByType(svType)
	return m_Humble:getALinkByType(svType)
end
function humble.getLinkByType(svType)
	return m_Humble:getLinkByType(m_curWorker, svType)
end

--������id
function humble.getServiceId()
	return m_Humble:getServiceId()
end
--����������
function humble.getServiceType()
	return m_Humble:getServiceType()
end
--��ȡ��ǰ��������·��
function humble.getProPath()
	return m_Humble:getProPath()
end
--��ȡ�ű�����·��
function humble.getScriptPath()
	return m_Humble:getScriptPath()
end
--������������key
function humble.getRPCKey()
	return m_Humble:getRPCKey()
end

--�ʼ� ֻ�ܷ���һ��,����Send������ٸö���
function humble.newMail()
	return newMail()
end
function humble.sendMail(pMail)
	m_Humble:sendMail(pMail)
end

return humble
