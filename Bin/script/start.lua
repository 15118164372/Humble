--[[
网络器服务
--]]

require("macros")
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

if not g_tListener then
    g_tListener = {}
end
local tListener = g_tListener

if not g_tLinker then
    g_tLinker = {}
end
local tLinker = g_tLinker

--初始化
function onStart()
    tListener.test = humble.addListener(1, "0.0.0.0", 15000)
    --tLinker.test = humble.addTcpLink(1, "127.0.0.1", 15000)  
    humble.setParser(1, "websock")
    tListener.udp = humble.addUdp("0.0.0.0", 15001)
    
    humble.regTask("echo")
    humble.regTask("test")      
    
    tChan.echo = humble.getChan("echo")
end

--退出，主要清理掉连接
function onStop()
    humble.closeByType(1)
    humble.delListener(tListener.test)
    humble.delUdp(tListener.udp)
end

function onTcpLinked(sock, uiSession, usSockType)
    
end

function onTcpClose(sock, uiSession, usSockType)
    
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
        tChan.echo:Send(utile.Pack(sock, uiSession, strName, buffer))
    end
end

function onUdpRead(sock, pHost, usPort)
    local strBuf = pBuffer:getByte(pBuffer:getSurpLens())
    humble.sendU(sock, pHost, usPort, strBuf.."lua1")
    humble.broadCastU(sock, usPort, strBuf.."lua2")
end
