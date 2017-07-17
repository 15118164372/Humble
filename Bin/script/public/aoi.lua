--��Ұ����ʮ�������㷨
local table = table
local pairs = pairs

--ʵ��
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
function Object:setZ(z)
	self.Z = z
end
function Object:getZ()
	return self.Z
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
function Object:setZDist(zDist)
    self.ZDist = zDist
end
function Object:getZDist()
    return self.ZDist
end

--��Ұ����
local AOI = {}
AOI.__index = AOI

function AOI:new(maxX, maxY, maxZ)     
    local self = {}
    setmetatable(self, AOI)
    
	assert(maxX > 0)
    self.MaxX = maxX
    self.MaxY = maxY
	if 0 == maxY then
		self.MaxY = 1
	end
	self.MaxZ = maxZ
	if 0 == maxZ then
		self.MaxZ = 1
	end
	
	self.XList = {}
    self.Object = {}
    
    for i = 1, self.MaxX do
        table.insert(self.XList, {})
    end

    return self
end

function AOI:checkParam(x, y, z)
    if x < 0 or x >= self.MaxX
        or y <0 or y >= self.MaxY
		or z <0 or z >= self.MaxZ then
       return false 
    end
    
    return true
end

function AOI:getRange(obj, xDist, yDist, zDist)
    local xStart = obj:getX() - xDist
	if xStart < 0 then
		xStart = 0
	end	
	local xEnd = obj:getX() + xDist
	if xEnd >= self.MaxX then	
		xEnd = self.MaxX - 1
	end

	local yStart = obj:getY() - yDist
	if yStart < 0 then
		yStart = 0
	end
	local yEnd = obj:getY() + yDist
	if yEnd >= self.MaxY then
		yEnd = self.MaxY - 1
	end	
	
	local zStart = obj:getZ() - zDist
	if zStart < 0 then
		zStart = 0
	end
	local zEnd = obj:getZ() + zDist
	if zEnd >= self.MaxZ then
		zEnd = self.MaxZ - 1
	end
	
	return xStart, xEnd, yStart, yEnd, zStart, zEnd
end

function AOI:getArea(obj, xDist, yDist, zDist)
	if not xDist then
		xDist = obj:getXDist()
	end
	if not yDist then
		yDist = obj:getYDist()
	end
	if not zDist then
		zDist = obj:getZDist()
	end
	
    local inArea = {}
    table.insert(inArea, obj:getId())
	local y,z
    local xStart, xEnd, yStart, yEnd, zStart, zEnd = self:getRange(obj, xDist, yDist, zDist)
    for x = xStart + 1, xEnd + 1 do
		--x����
        for key, val in pairs(self.XList[x]) do
			--�ų��Լ�
            if key ~= obj:getId() then
				--������귶Χ
				y = val:getY()
				if y >= yStart and y <= yEnd then
					z = val:getZ()
					if z >= zStart and z <= zEnd then
						table.insert(inArea, key)
					end
				end
            end
        end
    end

    return inArea
end

function AOI:delXList(obj)
    self.XList[obj:getX() + 1][obj:getId()] = nil
end

function AOI:getEntity(id)
    return self.Object[id]
end
function AOI:delEntity(id)
    self.Object[id] = nil
end

function AOI:moveData(obj, x, y, z)
	obj:setY(y)
	obj:setZ(z)
	
    if obj:getX() ~= x then
        self:delXList(obj)
        obj:setX(x)
		self.XList[obj:getX() + 1][obj:getId()] = obj
    end    
    
    return obj
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

--���� ����������Ұ����ids
function AOI:Enter(id, x, y, z, xDist, yDist, zDist)
    if not self:checkParam(x, y, z) then
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
	obj:setZ(z)
    obj:setXDist(xDist)
    obj:setYDist(yDist)
	obj:setZDist(zDist)
	
    self.XList[x + 1][id] = obj
    self.Object[id] = obj
    
    return self:getArea(obj)
end

--�뿪 ����������Ұ����ids
function AOI:Leave(id)
    local obj = self:getEntity(id)
    if not obj then
        return
    end
    
    local inArea = self:getArea(obj)
    
    self:delXList(obj)
    self:delEntity(id)
    
    return inArea
end

--�ƶ� ����������Ұ����ids �뿪�����  �½������
function AOI:Move(id, x, y, z)
    if not self:checkParam(x, y, z) then
        return
    end
    
    local obj = self:getEntity(id)
    if not obj then
        return
    end
    
    if obj:getX() == x and obj:getY() == y and obj:getZ() == z then
        return
    end
    
    local oldArea = self:getArea(obj)
    obj = self:moveData(obj, x, y, z)
    local inArea = self:getArea(obj)	
    local outArea = self:getLeaveArea(oldArea, inArea)
    local newArea = self:getEnterArea(oldArea, inArea)
    
    return inArea, outArea, newArea
end

--��ȡ������Ұ�����ids xDist��yDist��zDistΪnilȡEnterʱ�����ֵ
function AOI:getAOIArea(id, xDist, yDist, zDist)
    local obj = self:getEntity(id)
    if not obj then
        return
    end
    
    return self:getArea(obj, xDist, yDist, zDist)
end

return AOI
