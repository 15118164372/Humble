--[[
��ʱ������
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local humble = require("humble")
local utile = require("utile")
local pTick = g_pTick

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function onStart()
    tChan.timer = humble.getChan("test")
end

function onStop()
    
end

--pTick ��Ч{tick, count}
function onTimer()
    tChan.timer:Send(utile.Pack(pTick))
    
    --1��
    if 0 == ((pTick[1] * pTick[2]) % 1000) then 
        --print("1 sec")
        --print(string.format("cur load %d.", humble.getCurLoad()))
    end
end
