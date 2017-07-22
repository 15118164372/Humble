--A* 如无说明x,y都为int
local APoint = require("apoint")
local PathPoint = require("apath")
local ListPath = require("alist")
local amath = require("amath")
local math = math
local table = table
local pairs = pairs
local string = string

local AStar = {}
AStar.__index = AStar
function AStar:new()
    local self = {}
    setmetatable(self, AStar)
	self.Range = 0
	self.Smooth = true
	
    return self
end

function AStar:setWeight(pathpoint, fill_weight, end_point)
	if -1 == fill_weight then
		return false
	end
	
	local curpoint = pathpoint:getPoint()
	local weight = pathpoint:getFillWeight() + pathpoint:getDistTraveled() + 
		math.abs(curpoint:getX() - end_point:getX()) + math.abs(curpoint:getY() - end_point:getY())
	pathpoint:setWeight(weight)
	
	return true
end

function AStar:getSurrounding(point, aMap)
	local surrounding = {}
	local x = point:getX()
	local y = point:getY()
	
	if x > 0 then
		table.insert(surrounding, APoint:new(x - 1, y))
	end
	if x < aMap:getMaxX() - 1 then
		table.insert(surrounding, APoint:new(x + 1, y))
	end
	
	if y > 0 then
		table.insert(surrounding, APoint:new(x, y - 1))
	end
	if y < aMap:getMaxY() - 1 then
		table.insert(surrounding, APoint:new(x, y + 1))
	end
	
	return surrounding
end

--a*寻路
function AStar:Find(source, target, aMap)
	local openList = {}
	local closeList = {}
	local listOpen = ListPath:new()
	
	local target_weight = aMap:getPFilled(target)
	local target_point = PathPoint:new(target)
	target_point:setFillWeight(target_weight)
	
	if self:setWeight(target_point, target_weight, source) then
		openList[aMap:getPKey(target)] = target_point
		listOpen:Push(target_point)
	end
	
	local current
	local curPoint
	local curKey
	local surrounding
	
	while(true)
	do
		current = listOpen:Pop()
		if not current then
			break
		end
		
		curPoint = current:getPoint()
		if curPoint:getX() == source:getX() and curPoint:getY() == source:getY() then
			break
		end
		
		curKey = aMap:getPKey(curPoint)
		openList[curKey] = nil
		closeList[curKey] = current
		
		surrounding = self:getSurrounding(curPoint, aMap)
		for _, p in pairs(surrounding) do
			curKey = aMap:getPKey(p)
			if not closeList[curKey] then
				if aMap:canMove(p:getX(), p:getY()) then
					local fill_weight = aMap:getPFilled(p)
					local path_point = PathPoint:new(p)
					path_point:setParent(current)
					path_point:setFillWeight(current:getFillWeight() + fill_weight)
					path_point:setDistTraveled(current:getDistTraveled() + 1)
					
					self:setWeight(path_point, fill_weight, source)
					
					local existing_point = openList[curKey]
					if not existing_point then
						openList[curKey] = path_point
						listOpen:Push(path_point)
					else
						if path_point:getWeight() < existing_point:getWeight() then
							existing_point:setParent(path_point:getParent())
						end
					end
				end
			end
		end
	end
	
	local path = {}
	while current 
	do
		table.insert(path, current:getPoint())
		current = current:getParent()
	end

	return path
end

function AStar:getRange(x1, y1, maxX, maxY, iRange)
	local xStart = x1 - iRange
	if xStart < 0 then
		xStart = 0
	end	
	local xEnd = x1 + iRange
	if xEnd >= maxX then
		xEnd = maxX - 1
	end
	
	local yStart = y1 - iRange
	if yStart < 0 then
		yStart = 0
	end
	local yEnd = y1 + iRange
	if yEnd >= maxY then
		yEnd = maxY - 1
	end
	
	return xStart, xEnd, yStart, yEnd
end

--寻找最近可移动的点
function AStar:findLatelyPoint(x1, y1, aMap)
	local bFirst = true
	local bMove = true
	local minDis = 0
	local latelyPoint
	local xMin, xMax, yMin, yMax, dis
	local xStart, xEnd, yStart, yEnd = self:getRange(x1, y1, aMap:getMaxX(), aMap:getMaxY(), self.searchRange)
	
	for x = xStart, xEnd do
		for y = yStart, yEnd do
			if x ~= x1 and y ~= y1 then
				if aMap:canMove(x, y) then
					--检查周围点是否可以移动
					xMin, xMax, yMin, yMax = self:getRange(x, y, aMap:getMaxX(), aMap:getMaxY(), 1)
					bMove = true
					for sx = xMin, xMax do
						for sy = yMin, yMax do
							if x ~= sx and y ~= sy then
								if not aMap:canMove(sx, sy) then
									bMove = false
									break
								end
							end
						end
						if not bMove then
							break
						end
					end
					
					if bMove then
						--这里不开方,减少运算
						dis = (x1-x)*(x1-x) + (y1-y)*(y1-y)
						if bFirst then
							minDis = dis
							latelyPoint = APoint:new(x, y)
							bFirst = false
						else
							if dis < minDis then
								minDis = dis
								latelyPoint = APoint:new(x, y)
							end
						end
					end
				end
			end
		end
	end
	
	if latelyPoint then
		return latelyPoint:getX(), latelyPoint:getY(), true
	end
	
	return 0,0,false
end

--修正起点，终点
function AStar:correctPoint(x1, y1, x2, y2, aMap)
	--四舍入五找最近的格子
	local ix1 = amath.clampInt(math.floor(amath.Round(x1)), 0, aMap:getMaxX()-1)
	local iy1 = amath.clampInt(math.floor(amath.Round(y1)), 0, aMap:getMaxY()-1)
	local ix2 = amath.clampInt(math.floor(amath.Round(x2)), 0, aMap:getMaxX()-1)
	local iy2 = amath.clampInt(math.floor(amath.Round(y2)), 0, aMap:getMaxY()-1)
	
	local source,target,bAddStart,bAddEnd
	
	if not aMap:canMove(ix1, ix2) then
		local newX, newY, ok = self:findLatelyPoint(ix1, iy1, aMap)
		if not ok then
			return
		end		
		
		bAddStart = true
		source = APoint:new(newX, newY)
	else
		source = APoint:new(ix1, iy1)
	end
	
	if not aMap:canMove(ix2, iy2) then
		local newX, newY, ok = self:findLatelyPoint(ix2, iy2, aMap)
		if not ok then
			return
		end
		
		bAddEnd = true
		target = APoint:new(newX, newY)
	else
		target = APoint:new(ix2, iy2)
	end
	
	return source, target, bAddStart, bAddEnd
end

--检查是否在一直线
function AStar:checkInLine(point1, point2, point3)
	if (point1:getY()-point2:getY())*(point3:getX()-point2:getX()) == 
		(point1:getX()-point2:getY())*(point3:getY()-point2:getY()) then
		return true
	end

	return false
end

--计算起始点到终止点的路径上是否有移动阻挡，无宽度
function AStar:detectMoveCollisionBetween(startX, startY, endX, endY, aMap)
	local x0 = startX
	local y0 = startY
	local x1 = endX
	local y1 = endY
	
	local steep = math.abs(y1-y0) > math.abs(x1-x0)
	
	if steep then
		x0 = startY
		y0 = startX
		x1 = endY
		y1 = endX
	end
	
	if x0 > x1 then
		local x = x0
		local y = y0
		x0 = x1
		x1 = x
		y0 = y1
		y1 = y
	end
	
	local ratio = math.abs((y1 - y0) / (x1 - x0))
	local mirror
	if y1 > y0 then
		mirror = 1
	else
		mirror = -1
	end
	
	local skip = false
	
	for col = math.floor(x0), math.ceil(x1) - 1 do
		local curY = y0 + mirror*ratio*(col-x0)
		--第一格不进行延边计算
		skip = false
		
		if col == math.floor(x0) then
			skip = (math.floor(curY) ~= math.floor(y0))
		end
		
		if not skip then
			if not steep then
				if not aMap:canMove(col, math.floor(math.max(0, math.floor(curY)))) then
					return true
				end
			else
				if not aMap:canMove(math.floor(math.max(0, math.floor(curY))), col) then
					return true
				end
			end
		end
		
		local tmp
		if mirror > 0 then
			tmp = math.ceil(curY) - curY
		else
			tmp = curY - math.floor(curY)
		end
		
		--根据斜率计算是否有跨格
		if tmp < ratio then
			local crossY = math.floor(curY) + mirror
			--判断是否超出范围
			if crossY > math.floor(math.max(y0, y1)) or
				crossY < math.floor(math.min(y0, y1)) then
				return false
			end

			--跨线格子
			if not steep then
				if not aMap:canMove(col, crossY) then
					return true
				end
			else
				if not aMap:canMove(crossY, col) then
					return true
				end
			end
		end
	end
	
	return false
end

--去掉多余的点
function AStar:removePoint(path, aMap)
	if 3 > #path then
		return path
	end
	
	local tmpLine = {}
	table.insert(tmpLine, path[1])
	--移除直线上的点
	for i = 1, #path - 3 do
		if not self:checkInLine(path[i], path[i+1], path[i+2]) then
			table.insert(tmpLine, path[i+1])
		end
	end
	
	table.insert(tmpLine, path[#path])
	if 3 > #tmpLine then
		return tmpLine
	end
	
	--拐点移除
	local tmpPath = {}
	table.insert(tmpPath, tmpLine[1])
	for i = 3, #tmpLine do
		if self:detectMoveCollisionBetween(tmpPath[#tmpPath]:getX(), tmpPath[#tmpPath]:getY(),
			tmpLine[i]:getX(), tmpLine[i]:getY(), aMap) then
			table.insert(tmpPath, tmpLine[i-1])
		end
	end
	
	table.insert(tmpPath, tmpLine[#tmpLine])
	
	return tmpPath
end

--A*寻路 坐标可以为浮点
function AStar:findPath(x1, y1, x2, y2, aMap)
	--检查起点，目标点是否可以寻，不能则找最近的点
	local path = {}
	local source, target, bAddStart, bAddEnd = self:correctPoint(x1, y1, x2, y2, aMap)
	if not source then
		return path
	end
	
	--起点终点重合
	if target:getX() == source:getX() and target:getY() == source:getY() then
		table.insert(path, APoint:new(x1, y1))
		table.insert(path, APoint:new(x2, y2))
		return path
	end
	
	path = self:Find(source, target, aMap)
	if 2 > #path then
		return {}
	end
	
	path = self:removePoint(path, aMap)

	return path
end
--起点或终点在障碍物中，搜索可用点范围
function AStar:searchRange(iRange)
	self.Range = iRange
end
--是否平滑处理
function AStar:Smooth(bSmooth)
	self.Smooth = bSmooth
end

return AStar
