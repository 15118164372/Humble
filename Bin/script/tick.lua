--[[
��ʱ������
--]]

require("logicmacro")
local humble = require("humble")
local utile = require("utile")

--��ʱ����ʼ��
function onStart()
	
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
		local objChan = humble.getChan("task_rpclink")
		if objChan then
			utile.chanSend(objChan, utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
		end
		
		objChan = humble.getChan("echo2")
		if objChan then
			utile.chanSend(objChan, utile.Pack(EnevtType.Second_1, nil, uiTick, uiCount))
		end
    end
end
