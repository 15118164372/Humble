--[[
时间轮
--]]

local type = type
local math = math
local table = table
local assert = assert
local setmetatable = setmetatable
local utile = require("utile")
local callFunc = utile.callFunc

--基础属性
TvecBase = {}
TvecBase.__index = TvecBase

function TvecBase:new(Func, tParam, iTimeOut)
    assert("function" == type(Func))
    assert("table" == type(tParam))
    assert("number" == type(iTimeOut))
    
    local self = {}
    setmetatable(self, TvecBase)
    
    self.Func = Func
    self.Param = tParam
    self.TimeOut = iTimeOut
    self.CompTime = 0
    self.Run = false
    
    --该定时器在轮中的信息
    self.RegWheel = nil
    self.RegSlot = 0
    self.RegIndex = 0 
    
    self.RegTick = 0
	
    return self
end

--注册在那个轮
function TvecBase:setRegWheel(objWheel)
    self.RegWheel = objWheel
end
function TvecBase:getRegWheel()
    return self.RegWheel
end

--注册在轮的那个槽
function TvecBase:setRegSlot(iSlot)
    self.RegSlot = iSlot
end
function TvecBase:getRegSlot()
    return self.RegSlot
end

--注册在槽的那个位置
function TvecBase:setRegIndex(iIndex)
    self.RegIndex = iIndex
end
function TvecBase:getRegIndex()
    return self.RegIndex
end

--注册时的时间tick
function TvecBase:setRegTick(iTick)
    self.RegTick = iTick
end
--对外接口
function TvecBase:getRegTick()
    return self.RegTick
end

--是否执行
function TvecBase:setRun(bRun)
    self.Run = bRun
end
--对外接口
function TvecBase:getRun()
    return self.Run
end

--超时间
--对外接口
function TvecBase:setTime(iTime)
    self.TimeOut = iTime
end
--对外接口
function TvecBase:getTime()
    return self.TimeOut
end
function TvecBase:subTime(iVal)
    self.TimeOut = self.TimeOut - iVal
end

--下次移动的补偿时间
function TvecBase:setCompTime(iCompTime)
    self.CompTime = iCompTime
end
function TvecBase:getCompTime()
    return self.CompTime
end

--执行回调函数
function TvecBase:Call()
    self:setRun(true)
    callFunc(self.Func, table.unpack(self.Param))
end

--轮
Wheel = {}
Wheel.__index = Wheel

function Wheel:new(iCount, iParam)
    assert("number" == type(iCount))
    
    local self = {}
    setmetatable(self, Wheel)
    
    self.Slot = {}
    for i = 1, iCount do
        table.insert(self.Slot, {})
    end
    
    self.Count = iCount
    self.CurWheel = 0
    self.Param = iParam
    self.Range = iCount * iParam
	
    return self
end

--获取标识范围
function Wheel:getRange()
    return self.Range
end
--获取时间跨度参数
function Wheel:getParam()
    return self.Param
end

--获取轮槽数
function Wheel:getCount()
    return self.Count
end

--增加timer
function Wheel:Add(iIndex, objTvecBase)
    assert(iIndex <= self.Count)
    table.insert(self.Slot[iIndex], objTvecBase)
end

--获取槽
function Wheel:Get(iIndex)
    assert(iIndex <= self.Count)
    return self.Slot[iIndex]
end

--获取槽中元素个数
function Wheel:getSlotNum(iIndex)
    assert(iIndex <= self.Count)
    return #(self.Slot[iIndex])
end

--当前槽数
function Wheel:getCurWheel()
    return self.CurWheel
end
function Wheel:addCurWheel()
    self.CurWheel = self.CurWheel + 1
end
function Wheel:resetCurWheel()
    self.CurWheel = 0
end

--轮管理
WheelMgr = {}
WheelMgr.__index = WheelMgr

function WheelMgr:new()
    local self = {}
    setmetatable(self, WheelMgr)
    self.Sec = Wheel:new(60, 1)--秒
    self.Min = Wheel:new(60, 60)--分
    self.Hor = Wheel:new(24, 60 * 60)--时
    self.Day = Wheel:new(30, 60 * 60 * 24)--天
    self.Mon = Wheel:new(12, 60 * 60 * 24 * 30)--月
    self.Tick = 0
    
    return self
end

--获取执行了的时间
--对外接口
function WheelMgr:getTick()
    return self.Tick
end

--获取当前槽与要插入的槽间隔的槽数
function WheelMgr:getCurCompTime(objWheel, iIndex)
    local iCount = 0
    local iCurWheel = objWheel:getCurWheel()
    
    if iIndex > iCurWheel then
        iCount = iIndex - iCurWheel
    else
        iCount = objWheel:getCount() - (iCurWheel - iIndex)
    end
    
    return iCount
end

--获取补偿时间
function WheelMgr:getMinCompTime(iIndex)
    return self:getCurCompTime(self.Min, iIndex) * self.Min:getParam() - 
        self.Sec:getCurWheel() * self.Sec:getParam()
end
function WheelMgr:getHorCompTime(iIndex)
    return self:getCurCompTime(self.Hor, iIndex) * self.Hor:getParam() -
        self.Min:getCurWheel() * self.Min:getParam() -
        self.Sec:getCurWheel() * self.Sec:getParam()
end
function WheelMgr:getDayCompTime(iIndex)
    return self:getCurCompTime(self.Day, iIndex) * self.Day:getParam() -
        self.Hor:getCurWheel() * self.Hor:getParam() -
        self.Min:getCurWheel() * self.Min:getParam() -
        self.Sec:getCurWheel() * self.Sec:getParam()
end
function WheelMgr:getMonCompTime(iIndex)
    return self:getCurCompTime(self.Mon, iIndex) * self.Mon:getParam() -
        self.Day:getCurWheel() * self.Day:getParam() -
        self.Hor:getCurWheel() * self.Hor:getParam() -
        self.Min:getCurWheel() * self.Min:getParam() -
        self.Sec:getCurWheel() * self.Sec:getParam()
end

--清除注册时的信息
function WheelMgr:clearRegInfo(objTvecBase)
    objTvecBase:setRegWheel(nil)
    objTvecBase:setRegSlot(0)
    objTvecBase:setRegIndex(0)
end

--移除一个事件
--对外接口
function WheelMgr:Remove(objTvecBase)
    local objWheel = objTvecBase:getRegWheel()
    local iSlot = objTvecBase:getRegSlot()
    local iIndex = objTvecBase:getRegIndex()
    
    if ((not objWheel)
        or (0 == iSlot)
        or (0 == iIndex)) then
       return
    end
    
    local tSlot = objWheel:Get(iSlot)
    local iSlotSize = #tSlot
    
    table.remove(tSlot, iIndex)
    self:clearRegInfo(objTvecBase)
    
    if iIndex >= iSlotSize then        
        return
    end
    
    --后续标识前移
    for i = iIndex, #tSlot do
        tSlot[i]:setRegIndex(i)
    end
end

--设置注册时的信息,主要用于移除
function WheelMgr:setRegInfo(objTvecBase, objWheel, iSlot)
    objTvecBase:setRegWheel(objWheel)
    objTvecBase:setRegSlot(iSlot)
    objTvecBase:setRegIndex(objWheel:getSlotNum(iSlot) + 1)
    objTvecBase:setRegTick(self.Tick)
end

--对外接口
function WheelMgr:Add(iTimeOut, objTvecBase)
    assert(iTimeOut <= self.Mon:getRange())
    
    objTvecBase:setRun(false)
    
    --已经到期
    if iTimeOut <= 0 then
        objTvecBase:Call()
        return objTvecBase
    end
    
    local iIndex = 0
    if iTimeOut <= self.Sec:getRange() then
        --秒
        --计算放入位置
        iIndex = ((iTimeOut + self.Sec:getCurWheel() - 1) % self.Sec:getCount()) + 1
        --设置注册时的信息
        self:setRegInfo(objTvecBase, self.Sec, iIndex)
        --注册
        self.Sec:Add(iIndex, objTvecBase)
    elseif iTimeOut <= self.Min:getRange() then
        --分
        --计算多少分
        local iMin = math.floor(iTimeOut / self.Sec:getRange())
        --计算放入位置
        iIndex = ((iMin + self.Min:getCurWheel() - 1) % self.Min:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getMinCompTime(iIndex))
        --设置注册时的信息
        self:setRegInfo(objTvecBase, self.Min, iIndex)
        --注册
        self.Min:Add(iIndex, objTvecBase)
    elseif iTimeOut <= self.Hor:getRange() then
        --小时
        --计算多少小时
        local iHor = math.floor(iTimeOut / self.Min:getRange())
        --计算放入位置
        iIndex = ((iHor + self.Hor:getCurWheel() - 1) % self.Hor:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getHorCompTime(iIndex))
        --设置注册时的信息
        self:setRegInfo(objTvecBase, self.Hor, iIndex)
        --注册
        self.Hor:Add(iIndex, objTvecBase)
    elseif iTimeOut <= self.Day:getRange() then
        --天
        --计算多少天
        local iDay = math.floor(iTimeOut / self.Hor:getRange())
        --计算放入位置
        iIndex = ((iDay + self.Day:getCurWheel() - 1) % self.Day:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getDayCompTime(iIndex))
        --设置注册时的信息
        self:setRegInfo(objTvecBase, self.Day, iIndex)
        --注册
        self.Day:Add(iIndex, objTvecBase)
    else
        --月
        --计算多少月
        local iMon = math.floor(iTimeOut / self.Day:getRange())
        --计算放入位置
        iIndex = ((iMon + self.Mon:getCurWheel() - 1) % self.Mon:getCount()) + 1
        --设置执行时减去的时间差
        objTvecBase:setCompTime(self:getMonCompTime(iIndex))
        --设置注册时的信息
        self:setRegInfo(objTvecBase, self.Mon, iIndex)
        --注册
        self.Mon:Add(iIndex, objTvecBase)
    end
    
    return objTvecBase
end

--增加timer
--对外接口
function WheelMgr:addTimer(Func, iTimeOut, ...)
    local objTvecBase = TvecBase:new(Func, {...}, iTimeOut)
    
    return self:Add(iTimeOut, objTvecBase)
end

--数据移动
function WheelMgr:cashData(objWheel)
    local tRunKey = {}  
    local tWheel = objWheel:Get(objWheel:getCurWheel())    
    for key, _ in pairs(tWheel) do
        table.insert(tRunKey, key)
    end
    
    for i = #tRunKey, 1, -1 do
        tWheel[i]:subTime(tWheel[i]:getCompTime())
        self:Add(tWheel[i]:getTime(), tWheel[i])
        
        table.remove(tWheel, i)
    end
end

--数据移动
function WheelMgr:cashAll()
    local iCurWheel = self.Sec:getCurWheel()
    
    --秒满一圈
    if self.Sec:getCount() == iCurWheel then
        --秒槽数重置
        self.Sec:resetCurWheel()
        
        --分 处理
        self.Min:addCurWheel()
        self:cashData(self.Min)
        
        iCurWheel = self.Min:getCurWheel()
        --分满一圈
        if self.Min:getCount() == iCurWheel then
            --分槽数重置
            self.Min:resetCurWheel()
            
            --小时 处理
            self.Hor:addCurWheel()
            self:cashData(self.Hor)
            
            iCurWheel = self.Hor:getCurWheel()
            --小时满一圈
            if self.Hor:getCount() == iCurWheel then
                --小时槽数重置
                self.Hor:resetCurWheel()
                
                --天 处理
                self.Day:addCurWheel()
                self:cashData(self.Day)
                
                iCurWheel = self.Day:getCurWheel()
                --天满一圈
                if self.Day:getCount() == iCurWheel then
                    --天槽数重置
                    self.Day:resetCurWheel()
                    
                    --月 处理
                    self.Mon:addCurWheel()
                    self:cashData(self.Mon)
                    
                    iCurWheel = self.Mon:getCurWheel()
                    --月满一圈
                    if self.Mon:getCount() == iCurWheel then
                        self.Mon:resetCurWheel()
                    end
                end
            end
        end
    end
end

--对外接口
function WheelMgr:onTime()
    --流逝的时间+1
    self.Tick = self.Tick + 1
    
    --槽+1
    self.Sec:addCurWheel()
    
    --秒执行当前槽
    local tRunKey = {}    
    local tCurWheel = self.Sec:Get(self.Sec:getCurWheel())    
    for key, _ in pairs(tCurWheel) do
        table.insert(tRunKey, key)
    end
    for i = #tRunKey, 1, -1 do
        --执行
        if tCurWheel[i] then
            tCurWheel[i]:Call()
            table.remove(tCurWheel, i)
        end
    end
    
    self:cashAll()
end

--触发   objWheelMgr = WheelMgr:new()
function DEV_OnTime(objWheelMgr)
	objWheelMgr:onTime()
end

--延迟制定时间
function DEV_Reg(objWheelMgr, iTime, Func, ...)
	objWheelMgr:addTimer(Func, iTime, table.unpack({...}))
end

--strTime 格式(24小时制)：12:36:28
function DEV_AtTime(objWheelMgr, strTime, Func, ...)
	local strHour, strMin, strSec = string.match(strTime, "(%d+):(%d+):(%d+)")
    local iTime = (tonumber(strHour) * 60 * 60) + (tonumber(strMin) * 60) + tonumber(strSec)
    local tNow = os.date("*t", time)
    local iNowTime = (tonumber(tNow.hour) * 60 * 60) + (tonumber(tNow.min) * 60) + tonumber(tNow.sec)
    
    local iDelayTime = 0
    
    if iTime >= iNowTime then
        iDelayTime = iTime - iNowTime
    else
        iDelayTime = (24 * 60 * 60) - (iNowTime - iTime)
    end
    
    DEV_Reg(objWheelMgr, iDelayTime, Func, table.unpack({...}))
end
