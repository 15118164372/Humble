--��Ұ����
local table = table
local pairs = pairs

--��Ұ����
local AOI = {}
AOI.__index = AOI

function AOI:new(maxX, maxY, maxZ)     
    local self = {}
    setmetatable(self, AOI)
    
	self.CAOI = CLAOI(thisState, maxX, maxY, maxZ)

    return self
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

--��ȡ������Ұ���� xDist, yDist, zDist ����
function AOI:getArea(id, xDist, yDist, zDist)    
    return self.CAOI:getArea(id, xDist, yDist, zDist)
end

--���� ����������Ұ����ids
function AOI:Enter(id, x, y, z, xDist, yDist, zDist)
	self.CAOI:Enter(id, x, y, z)
    return self:getArea(id, xDist, yDist, zDist)
end

--�뿪 ����������Ұ����ids
function AOI:Leave(id, xDist, yDist, zDist)
    local inArea = self:getArea(id, xDist, yDist, zDist)
    self.CAOI:Leave(id)
    return inArea
end

--�ƶ� ����������Ұ����ids �뿪�����  �½������
function AOI:Move(id, x, y, z, xDist, yDist, zDist)
    local oldArea = self:getArea(id, xDist, yDist, zDist)
    self.CAOI:Move(id, x, y, z)
    local inArea = self:getArea(id, xDist, yDist, zDist)
    local outArea = self:getLeaveArea(oldArea, inArea)
    local newArea = self:getEnterArea(oldArea, inArea)
    
    return inArea, outArea, newArea
end

return AOI
