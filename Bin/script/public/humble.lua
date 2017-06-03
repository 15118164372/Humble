--c++导出函数
require("macros")
local string = string
local debug = debug
local getSVId = getSVId
local getRPCLink = getRPCLink
local logPriority = getLogPriority()
local H_LOG = H_LOG
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
local rpcCall = rpcCall
local taskRPCCall = taskRPCCall
local regTask = regTask
local unregTask = unregTask

local humble = {}

--日志
--文本日志级别 与c++中的对应
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
function humble.Debugf(fmt, ...)	
	if LogLV.Debug > logPriority then
		return
	end
	
    local strMsg = string.format(fmt,table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Debug, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Infof(fmt, ...)
	if LogLV.Info > logPriority then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Info, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Warnf(fmt, ...)
	if LogLV.Warn > logPriority then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Warn, getFileName(stack.short_src), stack.currentline, strMsg)
end
function humble.Errorf(fmt, ...)
	if LogLV.Err > logPriority then
		return
	end
	
    local strMsg = string.format(fmt, table.unpack({...}))
	local stack = debug.getinfo(2)
	H_LOG(LogLV.Err, getFileName(stack.short_src), stack.currentline, strMsg)
end

--服务器id
function humble.getSVId()
	return getSVId()
end

--获取服务器连接的socket
function humble.getRPCLink(svId)
	return getRPCLink(svId)
end

--获取当前程序所在路径
function humble.getProPath()
	return getProPath()
end

--一些编解码
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

--事件注册 Accept, Linked, Closed, Frame, Sec 
function humble.regEvent(usEvent, strTask, sockType)
	regEvent(usEvent, strTask, sockType)
end
--网络协议注册
function humble.regIProto(iProto, strTask)
	regIProto(iProto, strTask)
end
function humble.regStrProto(strProto, strTask)
	regStrProto(strProto, strTask)
end
--监听
function humble.addListener(strParser, sockType, strHost, usPort)
	addListener(strParser, sockType, strHost, usPort)
end
--连接
function humble.linkTo(strParser, sockType, strHost, usPort)
	linkTo(strParser, sockType, strHost, usPort)
end
--调用远端rpc
function humble.rpcCall(sock, uiId, strRPCName, toTask, srcTask, buf, lens)
	rpcCall(sock, uiId, strRPCName, toTask, srcTask, buf, lens)
end
--调用本服务器rpc
function humble.taskRPCCall(uiId, strRPCName, toTask, srcTask, buf, lens)
	taskRPCCall(uiId, strRPCName, toTask, srcTask, buf, lens)
end
--注册服务
function humble.regTask(strFile, strTask, iCapacity)
	regTask(strFile, strTask, iCapacity)
end
--取消注册
function humble.unregTask(strTask)
	unregTask(strTask)
end

return humble
