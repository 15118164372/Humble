--[[
��ʱ������
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
--ʱ����
if not g_objTimeWheel then
	g_objTimeWheel = WheelMgr:new()
end
local objTimeWheel = g_objTimeWheel

--��ʱ����ʼ��
function onStart()
    tChan.timer = humble.getChan("test")
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
        DEV_OnTime(objTimeWheel)
    end
end

--�����ӳ�
local function onDEV()
	--print(string.format('onDEV: %d', os.time()))
	tChan.timer:Send(utile.Pack(EnevtType.Delay, nil, "�����ӳ�", "5��һ��."))
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
--�����ӳ� ����
