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
	tChan.echo2 = humble.getChan("echo2")
end

--��ʱ��ֹͣ
function onStop()
    
end

--��ʱ������
function onTimer(uiTick, uiCount)
	--ÿ֡
	--utile.chanSend(tChan.timer, utile.Pack(EnevtType.Frame, nil, uiTick, uiCount))
    
    --1��
    if 0 == ((uiTick * uiCount) % 1000) then 
		utile.chanSend(tChan.echo2, utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
    end
end
