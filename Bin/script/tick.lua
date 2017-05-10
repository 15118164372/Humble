--[[
��ʱ������
--]]

require("logicmacro")
local humble = require("humble")
local utile = require("utile")

if not g_tChan then
	g_tChan = {}
end
local m_tChan = g_tChan

--��ʱ����ʼ��
function onStart()
	m_tChan.task_rpclink = humble.getChan("task_rpclink")
end

--��ʱ��ֹͣ
function onStop()
    
end

--��ʱ������
function onTimer(uiTick, uiCount)
	--ÿ֡
	--utile.chanSend(objChan, utile.Pack(EnevtType.Frame, nil, uiTick, uiCount))
    
    --1��
    if 0 == ((uiTick * uiCount) % 1000) then
		utile.chanSend(m_tChan.task_rpclink, utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
		
		objChan = humble.getChan("echo2")
		if objChan then
			utile.chanSend(objChan, utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
		end
    end
end
