--[[
tcp2
unsigned int + unsigned short + data
--]]

local tcp2Response = tcp2Response
local tcp2BroadCast = tcp2BroadCast

local tcp2 = {}

function tcp2.Response(sock, proto, val)
    if val then
		tcp2Response(sock, proto, val, #val)
	else
		tcp2Response(sock, proto, val, 0)
	end
end

function tcp2.broadCast(socks, proto, val)
    if val then
		tcp2BroadCast(socks, proto, val, #val)
	else
		tcp2BroadCast(socks, proto, val, 0)
	end
end

return tcp2
