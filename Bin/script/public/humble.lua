--c++��������
require("macros")
require("proto")
local string = string
local debug = debug
local getSVId = getSVId
local getSVType = getSVType
local getLinkById = getLinkById
local getLinkByType = getLinkByType
local getLogPriority = getLogPriority
local setLogPriority = setLogPriority
local H_LOG = H_LOG
local newMail = newMail
local CRC16 = CRC16
local CRC32 = CRC32
local urlEncode = urlEncode
local urlDecode = urlDecode
local zlibEncode = zlibEncode
local zlibDecode = zlibDecode
local b64Encode = b64Encode
local b64Decode = b64Decode
local md5Str = md5Str
local md5File = md5File
local regEvent = regEvent
local regIProto = regIProto
local regStrProto = regStrProto
local addListener = addListener
local linkTo = linkTo
local closeLink = closeLink
local removeLink = removeLink
local rpcCall = rpcCall
local taskRPCCall = taskRPCCall
local regTask = regTask
local unregTask = unregTask
local SockType = SockType

local humble = {}

--��־
--�ı���־���� ��c++�еĶ�Ӧ
local LogLV = {
    "Err",
    "Warn",
    "Info",
    "Debug",
}
LogLV = table.enum(LogLV, 1)

local function getFileName(strFile)
	local strName = string.match(strFile, ".+/([^/]*%.%w+)$")
	if nil == strName or 0 == #strName then
		strName = string.match(strFile, ".+\\([^\\]*%.%w+)$")
	end
	
	return strName
end
function humble.setLogPriority(logLevel)
	setLogPriority(logLevel)
end
function humble.Debugf(fmt, ...)	
	if LogLV.Debug > getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt,table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Debug, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Infof(fmt, ...)
	if LogLV.Info > getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Info, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Warnf(fmt, ...)
	if LogLV.Warn > getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Warn, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Errorf(fmt, ...)
	if LogLV.Err > getLogPriority() then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Err, getFileName(stack.short_src), stack.currentline, strMsg)
end

--�ʼ� ֻ�ܷ���һ��,����Send������ٸö���
function humble.newMail()
	return newMail()
end

--������id
function humble.getSVId()
	return getSVId()
end
--����������
function humble.getSVType()
	return getSVType()
end

--��ȡ���������ӵ�socket
function humble.getLinkById(svId)
	return getLinkById(svId)
end
function humble.getLinkByType(svType)
	return getLinkByType(this, svType)
end

--��ȡ��ǰ��������·��
function humble.getProPath()
	return getProPath()
end

--һЩ�����
function humble.CRC16(strval)
    return CRC16(strval, #strval)
end
function humble.CRC32(strval)
    return CRC32(strval, #strval)
end

function humble.urlEncode(strval)
    return urlEncode(strval, #strval)
end
function humble.urlDecode(strval)
    return urlDecode(strval, #strval)
end

function humble.zlibEncode(strval)
    return zlibEncode(strval, #strval)
end
function humble.zlibDecode(strval)
    return zlibDecode(strval, #strval)
end

function humble.b64Encode(strval)
    return b64Encode(strval, #strval)
end
function humble.b64Decode(strval)
    return b64Decode(strval, #strval)
end

function humble.md5Str(strval)
    return md5Str(strval, #strval)
end
function humble.md5File(strFile)
    return md5File(strFile)
end

--�¼�ע�� Accept, Linked, Closed, Frame, Sec 
function humble.regEvent(usEvent, strTask, sockType)
	regEvent(usEvent, strTask, sockType)
end
--����Э��ע��
function humble.regIProto(iProto, strTask)
	regIProto(iProto, strTask)
end
function humble.regStrProto(strProto, strTask)
	regStrProto(strProto, strTask)
end

--����
function humble.addListener(strParser, sockType, strHost, usPort)
	addListener(strParser, sockType, strHost, usPort)
end
--����
function humble.linkTo(strParser, sockType, strHost, usPort)
	linkTo(strParser, sockType, strHost, usPort)
end

--http�����
function humble.httpServer(strHost, usPort)
    addListener("http", SockType.HTTP, strHost, usPort)
end
--cmd�����
function humble.cmdServer(usPort)
    addListener("tcp2", SockType.CMD, "127.0.0.1", usPort)
end
--rpc�����
function humble.rpcServer(strHost, usPort)
    addListener("tcp2", SockType.RPC, strHost, usPort)
end
--rpc�ͻ���
function humble.rpcClient(strHost, usPort)
    linkTo("tcp2", SockType.RPC, strHost, usPort)
end

--���ӹر�
function humble.closeLink(sock)
	closeLink(sock)
end
--�Ƴ�������������
function humble.removeLink(sock)
	removeLink(sock)
end

--����Զ��rpc
function humble.rpcCall(sock, uiId, strRPCName, toTask, srcTask, buf, lens)
	rpcCall(sock, uiId, strRPCName, toTask, srcTask, buf, lens)
end
--���ñ�������rpc
function humble.taskRPCCall(uiId, strRPCName, toTask, srcTask, buf, lens)
	taskRPCCall(uiId, strRPCName, toTask, srcTask, buf, lens)
end

--ע�����
function humble.regTask(strFile, strTask, iCapacity)
	regTask(strFile, strTask, iCapacity)
end
--ȡ��ע��
function humble.unregTask(strTask)
	unregTask(strTask)
end

return humble
