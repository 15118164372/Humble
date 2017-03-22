--[[
定时器服务
--]]

require("macros")
require("logicmacro")
require("timewheel")
local humble = require("humble")
local utile = require("utile")

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan
--时间轮
if not g_objTimeWheel then
	g_objTimeWheel = WheelMgr:new()
end
local objTimeWheel = g_objTimeWheel

--定时器初始化
function onStart()
    tChan.timer = humble.getChan("test")
end

--定时器停止
function onStop()
    
end

--定时器触发
function onTimer(uiTick, uiCount)
	--每帧
    --tChan.timer:Send(utile.Pack(EnevtType.Frame, nil, uiTick, uiCount))
    
    --1秒
    if 0 == ((uiTick * uiCount) % 1000) then 
        DEV_OnTime(objTimeWheel)
    end
end

--测试延迟
local function onDEV()
	--print(string.format('onDEV: %d', os.time()))
	tChan.timer:Send(utile.Pack(EnevtType.Delay, nil, "测试延迟", "5秒一次."))
	DEV_Reg(objTimeWheel, 5, onDEV)
end
DEV_Reg(objTimeWheel, 5, onDEV)
local function onDEV2()
	--print(string.format('onDEV2: %d', os.time()))
	DEV_Reg(objTimeWheel, 1, onDEV2)
end
DEV_Reg(objTimeWheel, 1, onDEV2)
local function onAtTime()
	print(string.format('onAtTime: %s', os.date("%Y-%m-%d %H:%M:%S")))
end
DEV_AtTime(objTimeWheel, "16:15:28", onAtTime)
--测试延迟 结束
