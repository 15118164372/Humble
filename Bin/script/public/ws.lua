--[[
websocket version:13 ��֧�ַ�֡   
�������ݳ��˿���֡ȫΪ WSOCK_BINARYFRAME = 0x02
--]]

local wsResponse = wsResponse
local wsBroadCast = wsBroadCast
local wsResWithOutProto = wsResWithOutProto

local ws = {}

function ws.Response(sock, proto, val)
    if val then
		wsResponse(sock, proto, val, #val)
	else
		wsResponse(sock, proto, val, 0)
	end
end

function ws.resWithOutProto(sock, val)
	if val then
		wsResWithOutProto(sock, val, #val)
	else
		wsResWithOutProto(sock, val, 0)
	end
end

function ws.broadCast(socks, proto, val)
    if val then
		wsBroadCast(socks, proto, val, #val)
	else
		wsBroadCast(socks, proto, val, 0)
	end
end

return ws
