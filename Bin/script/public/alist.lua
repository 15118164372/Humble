--A* 路径链表
local ListPath = {}
ListPath.__index = ListPath

function ListPath:new()
    local self = {}
    setmetatable(self, ListPath)
    
    return self
end

function ListPath:Push(pathpoint)
	if not self.Head then
		self.Head = pathpoint
		return
	end
	
	local curNode = self.Head
	local preNode
	while(curNode)
	do
		if curNode:getWeight() >= pathpoint:getWeight() then
			if not preNode then
				--表头
				self.Head = pathpoint
				pathpoint:setNext(curNode)
			else
				--表中
				preNode:setNext(pathpoint)
				pathpoint:setNext(curNode)
			end
			
			return
		end
		
		preNode = curNode
		curNode = curNode:getNext()
	end
	
	--表尾
	preNode:setNext(pathpoint)
end
function ListPath:Pop()
	if not self.Head then
		return nil
	end
	
	local head = self.Head
	self.Head = self.Head:getNext()
	
	return head
end

return ListPath
