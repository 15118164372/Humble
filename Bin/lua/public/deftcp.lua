--[[
deftcp  string.pack参数 lstrlib.c 1221行
--]]
local humble = require("humble")
local string = string
local headFmt = ">IH"
local protoLens = 2

local deftcp = {}

function deftcp.Response(sock, proto, val)
	local netPack
	local iLens = protoLens
	if val then
		iLens = iLens + #val
		netPack = string.pack(headFmt, iLens, proto)..val
	else
		netPack = string.pack(headFmt, iLens, proto)
	end
	
	humble.sendMsg(sock, netPack)
end

return deftcp
