--视野区域十字链表算法
local table = table
local pairs = pairs

--实体
local Object = {}
Object.__index = Object

function Object:new()     
    local self = {}
    setmetatable(self, Object)
    
    return self
end

function Object:setId(id)
    self.Id = id
end
function Object:getId()
    return self.Id
end

function Object:setX(x)
    self.X = x
end
function Object:getX()
    return self.X
end

function Object:setY(y)
    self.Y = y
end
function Object:getY()
    return self.Y
end

function Object:setXDist(xDist)
    self.XDist = xDist
end
function Object:getXDist()
    return self.XDist
end

function Object:setYDist(yDist)
    self.YDist = yDist
end
function Object:getYDist()
    return self.YDist
end

--视野区域
local AOI_CRSLST = {}
AOI_CRSLST.__index = AOI_CRSLST

function AOI_CRSLST:new(maxX, maxY)     
    local self = {}
    setmetatable(self, AOI_CRSLST)
    
    self.MaxX = maxX
    self.MaxY = maxY
	self.XList = {}
    self.YList = {}
    self.Object = {}
    
    for i = 1, self.MaxX do
        table.insert(self.XList, {})
    end
    for i = 1, self.MaxY do
        table.insert(self.YList, {})
    end

    return self
end

function AOI_CRSLST:checkParam(x, y)
    if x < 0 or x >= self.MaxX
        or y <0 or y >= self.MaxY then
       return false 
    end
    
    return true
end

function AOI_CRSLST:getRange(obj)
    local xStart = obj:getX() - obj:getXDist()
	if xStart < 0 then
		xStart = 0
	end	
	local xEnd = obj:getX() + obj:getXDist()	
	if xEnd >= self.MaxX then	
		xEnd = self.MaxX - 1
	end	
	

	local yStart = obj:getY() - obj:getYDist()
	if yStart < 0 then
		yStart = 0
	end
	local yEnd = obj:getY() + obj:getYDist()
	if yEnd >= self.MaxY then
		yEnd = self.MaxY - 1
	end	
	
	return xStart, xEnd, yStart, yEnd
end

function AOI_CRSLST:getArea(obj)
    local inArea = {}
    table.insert(inArea, obj:getId())
    local xStart, xEnd, yStart, yEnd = self:getRange(obj)
    for x = xStart + 1, xEnd + 1 do
        for keyX, _ in pairs(self.XList[x]) do
            if keyX ~= obj:getId() then
                 for y = yStart + 1, yEnd + 1 do
					if self.YList[y][keyX] then
						table.insert(inArea, keyX)
						break
					end
                end
            end
        end    
    end

    return inArea
end

function AOI_CRSLST:delXList(obj)
    self.XList[obj:getX() + 1][obj:getId()] = nil
end
function AOI_CRSLST:delYList(obj)
    self.YList[obj:getY() + 1][obj:getId()] = nil
end

function AOI_CRSLST:getEntity(id)
    return self.Object[id]
end
function AOI_CRSLST:delEntity(id)
    self.Object[id] = nil
end

function AOI_CRSLST:moveData(obj, x, y)
    local bAddX = false
	local bAddY = false
    
    if obj:getX() ~= x then
        bAddX = true
        self:delXList(obj)
        obj:setX(x)
    end
    
    if obj:getY() ~= y then
        bAddY = true
        self:delYList(obj)
        obj:setY(y)
    end
    
    if bAddX then
        self.XList[obj:getX() + 1][obj:getId()] = obj
    end
    
    if bAddY then
        self.YList[obj:getY() + 1][obj:getId()] = obj
    end
    
    return obj
end

function AOI_CRSLST:getLeaveArea(oldArea, inArea)
    local outArea = {}
    local bInArea = false
    for _, oldVal in pairs(oldArea) do
        bInArea = false
        for _, inVal in pairs(inArea) do
            if oldVal == inVal then
                bInArea = true
                break
            end
        end
        
        if not bInArea then
            table.insert(outArea, oldVal)
        end
    end
    
    return outArea
end
function AOI_CRSLST:getEnterArea(oldArea, inArea)
    local newArea = {}
    local bInArea = false
    for _, inVal in pairs(inArea) do
        bInArea = false
        for _, oldVal in pairs(oldArea) do
            if inVal == oldVal then
                bInArea = true
                break
            end
        end
        
        if not bInArea then
            table.insert(newArea, inVal)
        end
    end
    
    return newArea
end

function AOI_CRSLST:Print()    
    table.print(self.XList)
    table.print(self.YList)
    table.print(self.Object)
end

function AOI_CRSLST:Enter(id, x, y, xDist, yDist)
    if not self:checkParam(x, y) then
        return
    end
    
    local obj = self:getEntity(id)
    if obj then
        return
    end
    
    obj = Object:new()
    obj:setId(id)
    obj:setX(x)
    obj:setY(y)
    obj:setXDist(xDist)
    obj:setYDist(yDist)
	
    self.XList[x + 1][id] = obj
    self.YList[y + 1][id] = obj
    self.Object[id] = obj
    
    return self:getArea(obj)
end

function AOI_CRSLST:Leave(id)
    local obj = self:getEntity(id)
    if not obj then
        return
    end
    
    local inArea = self:getArea(obj)
    
    self:delXList(obj)
    self:delYList(obj)
    self:delEntity(id)
    
    return inArea
end

function AOI_CRSLST:Move(id, x, y)
    if not self:checkParam(x, y) then
        return
    end
    
    local obj = self:getEntity(id)
    if not obj then
        return
    end
    
    if obj:getX() == x and obj:getY() == y then
        return
    end
    
    local oldArea = self:getArea(obj)
    obj = self:moveData(obj, x, y)
    local inArea = self:getArea(obj)	
    local outArea = self:getLeaveArea(oldArea, inArea)
    local newArea = self:getEnterArea(oldArea, inArea)
    
    return inArea, outArea, newArea
end

function AOI_CRSLST:getAOIArea(id)
    local obj = self:getEntity(id)
    if not obj then
        return
    end
    
    return self:getArea(obj)
end

return AOI_CRSLST
