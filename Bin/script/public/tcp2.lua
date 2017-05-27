--[[
tcp2
unsigned int + unsigned short + data
--]]

local tcp2Response = tcp2Response

local tcp2 = {}

function tcp2.Response(sock, proto, val)
    if val then
		tcp2Response(sock, proto, val, #val)
	else
		tcp2Response(sock, proto, val, 0)
	end
end

return tcp2
