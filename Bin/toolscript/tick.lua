--[[
tool 定时器服务
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
	utile.chanSend(tChan.tool, utile.Pack(msgtype.tick, nil, uiTick, uiCount))
    --1秒
    if 0 == ((uiTick * uiCount) % 1000) then 
        utile.chanSend(tChan.tool, utile.Pack(msgtype.onesec, nil))
    end
end
