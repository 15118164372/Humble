
require("macros")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")
local websock = require("websock")
local mqtt = require("mqtt")
local def = require("def")
local tcp1 = require("tcp1")
local tcp2 = require("tcp2")
local table = table
local string = string
local pChan = g_pChan

if not g_pBinary then
    g_pBinary = CBinary()
end
local pBinary = g_pBinary

function initTask()
    
end

function runTask()
    local varRecv = pChan:Recv()
    local sock, uiSession, strName, strMsg = utile.unPack(varRecv)
    
    if "tcp1" == strName then
        local pWBinary = tcp1.Response(strMsg)
        humble.sendB(sock, uiSession, pWBinary)       
    elseif "tcp2" == strName then
        local pWBinary = tcp2.Response(strMsg)
        humble.sendB(sock, uiSession, pWBinary)  
    elseif "websock" == strName then
        table.print(strMsg)
		--控制帧处理
		--分片帧处理等
        local pWBinary = websock.Text(strMsg.info)
        humble.sendB(sock, uiSession, pWBinary)
    elseif "mqtt" == strName then
        table.print(strMsg)
        local pWBinary
        if strMsg.fhead.type == mqtt.Type.CONNECT then
            pWBinary = mqtt.CONNACK(0)
        end
        
        if strMsg.fhead.type == mqtt.Type.SUBSCRIBE then
            local tTopic = strMsg.payload
            local tQos = {}
            for _, val in pairs(tTopic) do
                table.insert(tQos, val)
            end
            
            pWBinary = mqtt.SUBACK(strMsg.vhead.msgid, tQos)
        end
        humble.sendB(sock, uiSession, pWBinary)
    elseif "http" == strName then
        table.print(strMsg)
        local pWBinary = httpd.Response(200, "hello word")
        humble.sendB(sock, uiSession, pWBinary)
    elseif "default" == strName then
        local pWBinary = def.Response(strMsg)
        humble.sendB(sock, uiSession, pWBinary)  
    else
        assert(false)
    end
end

function destroyTask()

end
