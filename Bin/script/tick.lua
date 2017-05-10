--[[
定时器服务
--]]

require("logicmacro")
local humble = require("humble")
local utile = require("utile")

if not g_tChan then
	g_tChan = {}
end
local m_tChan = g_tChan

--定时器初始化
function onStart()
	m_tChan.task_rpclink = humble.getChan("task_rpclink")
end

--定时器停止
function onStop()
    
end

--定时器触发
function onTimer(uiTick, uiCount)
	--每帧
	--utile.chanSend(objChan, utile.Pack(EnevtType.Frame, nil, uiTick, uiCount))
    
    --1秒
    if 0 == ((uiTick * uiCount) % 1000) then
		utile.chanSend(m_tChan.task_rpclink, utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
		
		objChan = humble.getChan("echo2")
		if objChan then
			utile.chanSend(objChan, utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
		end
    end
end
