--A* 地图
local AMap = {}
AMap.__index = AMap

function AMap:new(maxX, maxY)
    local self = {}
    setmetatable(self, AMap)
    
	self.MaxX = maxX
	self.MaxY = maxY
	self.Filled = {}
	
    return self
end

function AMap:getKey(x, y)
	return x.."-"..y
end
function AMap:getPKey(point)
	return self:getKey(point:getX(), point:getY())
end

function AMap:getMaxX()
	return self.MaxX
end
function AMap:getMaxY()
	return self.MaxY
end

--weight -1 不可移动的点
function AMap:setFilled(x, y, weight)
	self.Filled[self:getKey(x, y)] = weight
end
function AMap:getFilled(x, y)
	local weight = self.Filled[self:getKey(x, y)]
	if not weight then
		return 0
	end
	
	return weight
end
function AMap:getPFilled(point)
	return self:getFilled(point:getX(), point:getY())
end

function AMap:canMove(x, y)
	local weight = self.Filled[self:getKey(x, y)]
	if not weight then
		return true
	end
	
	return weight >= 0
end

function AMap:canMoveK(key)
	local weight = self.Filled[key]
	if not weight then
		return true
	end
	
	return weight >= 0
end

function AMap:Print(points)
	local bHave = false
	print("")
	for y = self.MaxY - 1, 0, -1 do
		local printStr = {}
		for x = 0, self.MaxX - 1 do
			bHave = false
			for _, p in pairs(points) do
				if x == p:getX() and y == p:getY() then
					if self:canMove(x, y) then
						table.insert(printStr, "*")
					else
						table.insert(printStr, "&")
					end
					
					bHave = true
					break
				end				
			end
			if not bHave then
				if self:canMove(x, y) then
					table.insert(printStr, " ")
				else
					table.insert(printStr, "#")
				end
			end
		end
		print(table.concat(printStr))
	end
	print("")
end

return AMap
