--A*
local AStar = {}
AStar.__index = AStar

function AStar:new()
    local self = {}
    setmetatable(self, AStar)
	self.CAStar = CLAStar(thisState)
	
    return self
end

--是否平滑处理
function AStar:setSmooth(bSmooth)
	self.CAStar:setSmooth(bSmooth)
end

--起点或终点不可达时，搜索最近点范围
function AStar:setRange(iRange)
	self.CAStar:setRange(iRange)
end

--寻路
function AStar:findPath(fX1, fY1, fX2, fY2, pAMap)
	return self.CAStar:findPath(fX1, fY1, fX2, fY2, pAMap)
end

--打印
function AStar:Print(tPath, pAMap)
	self.CAStar:Print(tPath, pAMap)
end

return AStar
