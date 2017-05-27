--[[
tcp1  
消息长度小于126  char(消息长度) + unsigned short + 消息
小于等于65535    char(值126) + unsigned short(消息长度) + unsigned short + 消息
大于65535        char(值127) + unsigned int(消息长度) + unsigned short + 消息
--]]

local tcp1Response = tcp1Response

local tcp1 = {}

function tcp1.Response(sock, proto, val)
	if val then
		tcp1Response(sock, proto, val, #val)
	else
		tcp1Response(sock, proto, val, 0)
	end
end

return tcp1
