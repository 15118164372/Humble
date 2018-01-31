--[[
websocket 返回全为BINARYFRAME
--]]
local humble = require("humble")
local string = string

local ws = {}

local function creatHead(bCode, cFin, iLens)
	bCode = bCode | 0x80
	if 0 == cFin then
		bCode = bCode & 0x7f
	end
	
	if iLens <= 125 then
		return string.pack(">BB", bCode, iLens)
	elseif iLens > 125 and iLens <= 0xFFFF then
		return string.pack(">BBH", bCode, 126, iLens)
	else
		return string.pack(">BBJ", bCode, 127, iLens)
	end
end

function ws.binaryFrame(sock, iProto, pszBuf)
	local netPack = string.pack(">H", iProto) .. pszBuf
	netPack = creatHead(0x02, 1, #netPack) .. netPack
	
	humble.sendMsg(sock, netPack)
end

function ws.bgContinuation(sock, iProto, pszBuf)
	local netPack = string.pack(">H", iProto) .. pszBuf
	netPack = creatHead(0x02, 0, #netPack) .. netPack
	
	humble.sendMsg(sock, netPack)
end
function ws.Continuation(sock, pszBuf)
	local netPack = creatHead(0, 0, #pszBuf) .. pszBuf
	humble.sendMsg(sock, netPack)
end
function ws.endContinuation(sock, pszBuf)
	local netPack = creatHead(0, 1, #pszBuf) .. pszBuf
	humble.sendMsg(sock, netPack)
end

return ws
