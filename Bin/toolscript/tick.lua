--[[
tool ��ʱ������
--]]

require("macros")
local msgtype = require("msgtype")
local humble = require("humble")
local utile = require("utile")

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function onStart()
    tChan.tool = humble.getChan("tool")
end

function onStop()
    
end

function onTimer(uiTick, uiCount)
    tChan.tool:Send(utile.Pack(msgtype.tick, uiTick, uiCount))
    --1��
    if 0 == ((uiTick * uiCount) % 1000) then 
        tChan.tool:Send(utile.Pack(msgtype.onesec))
    end
end
