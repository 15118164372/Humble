
require("macros")
local msgtype = require("msgtype")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")
local websock = require("websock")
local mqtt = require("mqtt")
local def = require("def")
local tcp1 = require("tcp1")
local tcp2 = require("tcp2")
local tcp3 = require("tcp3")
local cjson = require("cjson")
local table = table
local string = string
local pChan = g_pChan

if not g_tSock then
    g_tSock = {}
end
local tSock = g_tSock

function initTask()
    
end

local function callStr(strComm)
    local Rtn, strMsg = load(strComm)    
    if Rtn then
        Rtn, strMsg = pcall(Rtn)
        if Rtn then            
            return strMsg
        end
    end
    
    showMsg(strMsg, #strMsg)
    
    return nil
end

local function sendMsg(strParser, strComm)
    if not tSock.sock then
        local strMsg = "not linked."
        showMsg(strMsg, #strMsg)
        
        return
    end
    
    local pBinary = callStr(strComm)
    if not pBinary then
        return
    end
    
    humble.sendB(tSock.sock, tSock.session, pBinary)    
end

local function readMsg(strName, buffer)
    if "tcp1" == strName or "tcp2" == strName or "default" == strName then
        showMsg(buffer, #buffer)
        return
    end
    
    local strMsg = cjson.encode(buffer)
    showMsg(strMsg, #strMsg)
end

function runTask()
    local varRecv = pChan:Recv()
    local msgType, _, msg = utile.unPack(varRecv)
    if msgtype.tick == msgType then
        
    elseif msgtype.onesec == msgType then
        
    elseif msgtype.link == msgType then
	    tSock.sock = msg[1]
        tSock.session = msg[2]
        local strMsg = string.format("tcp linked, sock: %d, session %d", tSock.sock, tSock.session)
        showMsg(strMsg, #strMsg)
    elseif msgtype.close == msgType then
          local strMsg = string.format("tcp closed, sock: %d, session %d", msg[1], msg[2])
        showMsg(strMsg, #strMsg)
    elseif msgtype.send == msgType then
        sendMsg(msg[1], msg[2])
    elseif msgtype.read == msgType then
        readMsg(msg[1], msg[2])
    else
        local strMsg = string.format("unknown message type: %s", tostring(msgType))
        showMsg(strMsg, #strMsg)
    end
end

function destroyTask()

end
