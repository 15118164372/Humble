--[[
debugtcp
--]]

local humble = require("humble")
local cjson = require("cjson")
local string = string
local headFmt = ">H"

local debugtcp = {}

function debugtcp.Response(sock, tVal)
	local strMsg = cjson.encode(tVal)
	local netPack = string.pack(headFmt, #strMsg)..strMsg	
	humble.sendMsg(sock, netPack)
end

return debugtcp
