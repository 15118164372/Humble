--视野区域
local table = table
local pairs = pairs

--视野区域
local AOI = {}
AOI.__index = AOI

function AOI:new(maxX, maxY, maxZ)     
    local self = {}
    setmetatable(self, AOI)
    
	self.CAOI = CLAOI(thisState, maxX, maxY, maxZ)

    return self
end

--获取所在视野区域 xDist, yDist, zDist 区域
function AOI:getArea(id, xDist, yDist, zDist)    
    return self.CAOI:getArea(id, xDist, yDist, zDist)
end

--进入
function AOI:Enter(id, x, y, z)
	self.CAOI:Enter(id, x, y, z)
end

--离开 返回所在视野区域ids
function AOI:Leave(id)
    self.CAOI:Leave(id)
end

--移动 返回 离开的区域  新进的区域
function AOI:Move(id, x, y, z, xDist, yDist, zDist)
	local newArea = {}
    local outArea = self.CAOI:Move(id, x, y, z, xDist, yDist, zDist, newArea)
    
    return outArea, newArea
end

return AOI
