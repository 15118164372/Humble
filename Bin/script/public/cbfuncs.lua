--[[
����ע��
--]]

require("timewheel")
local utile = require("utile")

local table = table
local type = type
local string = string
local tonumber = tonumber
local callFunc = utile.callFunc
local m_iOneDay = 24 * 60 * 60

local cbfunc = {}

if not g_RegFuncs then
    g_RegFuncs = {}
    
    --������Ϣ
    g_RegFuncs.NetEvent = {}
    --��Ϸ�¼�
    g_RegFuncs.GameEvent = {}
    --�ӳ��¼�
    g_RegFuncs.DelayEvent = WheelMgr:new()
end

local RegFuncs = g_RegFuncs

--[[
�������ӳ��¼�ִ��
������
����ֵ����
--]]
function cbfunc.onDelayEvent()   
    RegFuncs.DelayEvent:onTime()
end

--[[
�������ӳ��¼�ע��
������iTime --�Ӻ�ʱ�䣨�룩
����ֵ��TvecBase
--]]
function cbfunc.regDelayEvent(iTime, Func, ...)
    return RegFuncs.DelayEvent:addTimer(Func, iTime, table.unpack({...}))
end
function cbfunc.regDelayEventByBase(objTvecBase)
    return RegFuncs.DelayEvent:Add(objTvecBase:getTime(), objTvecBase)
end
--strTime ��ʽ(24Сʱ��)��12:36:28
function cbfunc.regDelayEventAtTime(strTime, Func, ...)
    local strHour, strMin, strSec = string.match(strTime, "(%d+):(%d+):(%d+)")
    local iTime = (tonumber(strHour) * 60 * 60) + (tonumber(strMin) * 60) + tonumber(strSec)
    local tNow = os.date("*t", time)
    local iNowTime = (tonumber(tNow.hour) * 60 * 60) + (tonumber(tNow.min) * 60) + tonumber(tNow.sec)
    
    local iDelayTime = 0
    
    if iTime >= iNowTime then
        iDelayTime = iTime - iNowTime
    else
        iDelayTime = m_iOneDay - (iNowTime - iTime)
    end
    
    return cbfunc.regDelayEvent(iDelayTime, Func, table.unpack({...}))
end

--[[
�������ӳ��¼��Ƴ�
������objTvecBase
����ֵ����
--]]
function cbfunc.removDelayEvent(objTvecBase)
    RegFuncs.DelayEvent:Remove(objTvecBase)
end

--[[
�������ӳ��¼��ܹ��߹�����ʱ��
��������
����ֵ����
--]]
function cbfunc.getDelayEventTick()
    return RegFuncs.DelayEvent:getTick()
end

--[[
��������Ϸ�¼��ص�
������iEvent --�¼����
����ֵ����
--]]
function cbfunc.onGameEvent(iEvent, ...)
    if not RegFuncs.GameEvent[iEvent] then
        return
    end
    
    for _, val in pairs(RegFuncs.GameEvent[iEvent]) do     
        if val then
            callFunc(val, table.unpack{...})
        end
    end
end

--[[
��������Ϸ�¼�ע��
������
����ֵ����
--]]
function cbfunc.regGameEvent(iEvent, Func)
    if "function" ~= type(Func) then
        return
    end
    
    if not RegFuncs.GameEvent[iEvent] then
        RegFuncs.GameEvent[iEvent] = {}
    end
    
    table.insert(RegFuncs.GameEvent[iEvent], Func)
end

--[[
����������ɶ��¼��ص�
������
����ֵ����
--]]
function cbfunc.onNetEvent(usSockType, iProtocol, ...)
    if not RegFuncs.NetEvent[usSockType] then
        return
    end
    
    local Func = RegFuncs.NetEvent[usSockType][iProtocol]
    if Func then
        callFunc(Func, table.unpack{...})
    end
end

--[[
����������ɶ��¼�ע��
������
����ֵ����
--]]
function cbfunc.regNetEvent(usSockType, iProtocol, Func)
    if "function" ~= type(Func) then
        return
    end
    
    if not RegFuncs.NetEvent[usSockType] then
        RegFuncs.NetEvent[usSockType] = {}
    end
    
    RegFuncs.NetEvent[usSockType][iProtocol] = Func
    utile.Debug("register protocol %d", iProtocol)
end

return cbfunc
