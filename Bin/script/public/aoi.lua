--视野区域十字链表算法
local table = table
local pairs = pairs

--实体
local Entity = {}
Entity.__index = Entity

function Entity:new()     
    local self = {}
    setmetatable(self, Entity)
    
    return self
end

function Entity:setId(id)
    self.Id = id
end
function Entity:getId()
    return self.Id
end

function Entity:setX(x)
    self.X = x
end
function Entity:getX()
    return self.X
end

function Entity:setY(y)
    self.Y = y
end
function Entity:getY()
    return self.Y
end

function Entity:setXDist(xDist)
    self.XDist = xDist
end
function Entity:getXDist()
    return self.XDist
end

function Entity:setYDist(yDist)
    self.YDist = yDist
end
function Entity:getYDist()
    return self.YDist
end

--视野区域
local AOI = {}
AOI.__index = AOI

function AOI:new(maxX, maxY)     
    local self = {}
    setmetatable(self, AOI)
    
    self.MaxX = maxX
    self.MaxY = maxY
	self.XList = {}
    self.YList = {}
    self.Entity = {}
    
    for i = 1, self.MaxX do
        table.insert(self.XList, {})
    end
    for i = 1, self.MaxY do
        table.insert(self.YList, {})
    end

    return self
end

function AOI:checkParam(x, y)
    if x < 0 or x >= self.MaxX
        or y <0 or y >= self.MaxY then
       return false 
    end
    
    return true
end

function AOI:getRange(objEntity)
    local xStart = objEntity:getX() - objEntity:getXDist()
	if xStart < 0 {
		xStart = 0
	}
	local xEnd = objEntity:getX() + objEntity:getXDist()
	if xEnd >= self.maxX {
		xEnd = self.maxX - 1
	}

	local yStart = objEntity:getY() - objEntity:getYDist()
	if yStart < 0 {
		yStart = 0
	}
	local yEnd = objEntity:getY() + objEntity:getYDist()
	if yEnd >= self.maxY {
		yEnd = self.maxY - 1
	}

	return xStart, xEnd, yStart, yEnd
end

function AOI:getArea(objEntity)
    local bFind = false
    local inArea = {}
    table.insert(inArea, objEntity:getId())
    local xStart, xEnd, yStart, yEnd = self:getRange(objEntity)
    
    for x = xStart + 1, xEnd + 1 do
        for _, valx in pairs(self.XList[x]) do
            if valx:getId() ~= objEntity:getId() then
                bFind = false
                for y = yStart + 1, yEnd + 1 do
                    for _, valy in pairs(self.YList[y]) do
                        if valx:getId() == valy:getId() then
                            bFind = true
                            table.insert(inArea, valx:getId())
                            break
                        end
                    end
                    
                    if bFind then
                        break
                    end
                end
            end
        end    
    end
    
    return inArea
end

function AOI:delXList(objEntity)
    local xEntity = self.XList[objEntity:getX() + 1]
    for key, val in pairs(xEntity) do
        if val:getId() == objEntity:getId() then
            table.remove(xEntity, key)
            return
        end
    end
end
function AOI:delYList(objEntity)
    local yEntity = self.YList[objEntity:getY() + 1]
    for key, val in pairs(yEntity) do
        if val:getId() == objEntity:getId() then
            table.remove(yEntity, key)
            return
        end
    end
end

function AOI:getEntity(id)
    return self.Entity[id]
end
function AOI:delEntity(id)
    if self.Entity[id] then
        self.Entity[id] = nil
    end
end

function AOI:moveData(objEntity, x, y)
    local bAddX = false
	local bAddY = false
    
    if objEntity:getX() ~= x then
        bAddX = true
        self:delXList(objEntity)
        objEntity:setX(x)
    end
    
    if objEntity:getY() ~= y then
        bAddY = true
        self:delYList(objEntity)
        objEntity:setY(y)
    end
    
    if bAddX then
        table.insert(self.XList[objEntity:getX() + 1], objEntity)
    end
    
    if bAddY then
        table.insert(self.YList[objEntity:getY() + 1], objEntity)
    end
    
    return objEntity
end

function AOI:getLeaveArea(oldArea, inArea)
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
function AOI:getEnterArea(oldArea, inArea)
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

function AOI:Print()    
    table.print(self.XList)
    print("----------------------------------")
    table.print(self.YList)
    print("----------------------------------")
    table.print(self.Entity)
    print("..................................")
end

function AOI:Enter(id, x, y, xDist, yDist)
    if not self:checkParam(x, y) then
        return
    end
    
    local objEntity = self:getEntity(id)
    if objEntity then
        return
    end
    
    objEntity = Entity:new()
    objEntity:setId(id)
    objEntity:setX(x)
    objEntity:setY(y)
    objEntity:setXDist(xDist)
    objEntity:setYDist(yDist)
    
    table.insert(self.XList[x + 1], objEntity)
    table.insert(self.YList[y + 1], objEntity)
    self.Enter[id] = objEntity
    
    return self:getArea(objEntity)
end

function AOI:Leave(id)
    local objEntity = self:getEntity(id)
    if not objEntity then
        return
    end
    
    local inArea = self:getArea(objEntity)
    
    self:delXList(objEntity)
    self:delYList(objEntity)
    self:delEntity(id)
    
    return inArea
end

function AOI:Move(id, x, y)
    if not self:checkParam(x, y) then
        return
    end
    
    local objEntity = self:getEntity(id)
    if not objEntity then
        return
    end
    
    if objEntity:getX() == x and objEntity:getY() == y then
        return self:getArea(objEntity), {}, {}
    end
    
    local oldArea = self:getArea(objEntity)
    objEntity = self:moveData(objEntity, x, y)
    local inArea = self:getArea(objEntity)
    local outArea = self:getLeaveArea(oldArea, inArea)
    local newArea = self:getEnterArea(oldArea, inArea)
    
    return inArea, outArea, newArea
end

function AOI:getAOIArea(id)
    local objEntity = self:getEntity(id)
    if not objEntity then
        return
    end
    
    return self:getArea(objEntity)
end

return AOI
