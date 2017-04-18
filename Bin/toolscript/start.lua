--[[
tool ����������
--]]

require("macros")
local msgtype = require("msgtype")
local humble = require("humble")
local utile = require("utile")
local httpd = require("httpd")
local websock = require("websock")
local mqtt = require("mqtt")
local table = table
local pairs = pairs
local pBuffer = g_pBuffer

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--��ʼ��
function onStart()
    humble.regTask("tool", 1024)
    tChan.tool = humble.getChan("tool")
end

--�˳�����Ҫ���������
function onStop()
    
end

function onTcpLinked(sock, uiSession, usSockType)
	utile.chanSend(tChan.tool, utile.Pack(msgtype.link, nil, sock, uiSession, usSockType))
end

function onTcpClose(sock, uiSession, usSockType)
    utile.chanSend(tChan.tool, utile.Pack(msgtype.close, nil, sock, uiSession, usSockType))
end

function onTcpRead(sock, uiSession, usSockType)
    local strName = humble.getParserNam(usSockType)
    if 0 == #strName then
        return
    end
    
    local buffer
    
    if "tcp1" == strName then
        buffer = pBuffer:getByte(pBuffer:getSurpLens())        
    elseif "tcp2" == strName then
        buffer = pBuffer:getByte(pBuffer:getSurpLens())
	elseif "tcp3" == strName then
        buffer = pBuffer:getByte(pBuffer:getSurpLens())	
    elseif "websock" == strName then
        buffer = websock.parsePack(pBuffer)
    elseif "mqtt" == strName then
        buffer = mqtt.parsePack(pBuffer)
    elseif "http" == strName then
        buffer = httpd.parsePack(pBuffer)
    elseif "default" == strName then
        buffer = pBuffer:getByte(pBuffer:getSurpLens())
    else
        assert(false)
    end
    
    if buffer then
        utile.chanSend(tChan.tool, utile.Pack(msgtype.read,  nil, strName, buffer))
    end
end

function onUdpRead(sock, pHost, usPort)
    
end
