--[[
��ʱ������
--]]

require("logicmacro")
local humble = require("humble")
local utile = require("utile")

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--��ʱ����ʼ��
function onStart()
	
end

--��ʱ��ֹͣ
function onStop()
    
end

--��ʱ������
function onTimer(uiTick, uiCount)
	--ÿ֡
    --tChan.timer:Send(utile.Pack(EnevtType.Frame, nil, uiTick, uiCount))
    
    --1��
    if 0 == ((uiTick * uiCount) % 1000) then 
        --tChan.timer:Send(utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
    end
end
