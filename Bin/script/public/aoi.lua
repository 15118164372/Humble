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

--��ȡ������Ұ���� xDist, yDist, zDist ����
function AOI:getArea(id, xDist, yDist, zDist)    
    return self.CAOI:getArea(id, xDist, yDist, zDist)
end

--����
function AOI:Enter(id, x, y, z)
	self.CAOI:Enter(id, x, y, z)
end

--�뿪 ����������Ұ����ids
function AOI:Leave(id)
    self.CAOI:Leave(id)
end

--�ƶ� ���� �뿪������  �½�������
function AOI:Move(id, x, y, z, xDist, yDist, zDist)
	local newArea = {}
    local outArea = self.CAOI:Move(id, x, y, z, xDist, yDist, zDist, newArea)
    
    return outArea, newArea
end

return AOI
