--A* Â·¾¶µã
local PathPoint = {}
PathPoint.__index = PathPoint

function PathPoint:new(point, key)
    local self = {}
    setmetatable(self, PathPoint)
	
	self.Weight = 0
	self.FillWeight = 0
	self.DistTraveled = 0
	self.Point = point
	self.PointKey = key
    
    return self
end

function PathPoint:getKey()
	return self.PointKey
end

function PathPoint:setWeight(weight)
	self.Weight = weight
end
function PathPoint:getWeight()
	return self.Weight
end

function PathPoint:setFillWeight(fillw)
	self.FillWeight = fillw
end
function PathPoint:getFillWeight()
	return self.FillWeight
end

function PathPoint:setDistTraveled(dis)
	self.DistTraveled = dis
end
function PathPoint:getDistTraveled()
	return self.DistTraveled
end

function PathPoint:getPoint()
	return self.Point
end

function PathPoint:setParent(pathpoint)
	self.Parent = pathpoint
end
function PathPoint:getParent()
	return self.Parent
end

function PathPoint:setNext(pathpoint)
	self.Next = pathpoint
end
function PathPoint:getNext()
	return self.Next
end

return PathPoint
