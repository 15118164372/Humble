--A* ��
local Point = {}
Point.__index = Point

function Point:new(x, y)
    local self = {}
    setmetatable(self, Point)
	
	self.X = x
	self.Y = y
    
    return self
end

function Point:setX(x)
	self.X = x
end
function Point:getX()
	return self.X
end

function Point:setY(y)
	self.Y = y
end
function Point:getY()
	return self.Y
end

return Point
