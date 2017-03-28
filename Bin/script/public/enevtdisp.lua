--[[
ÊÂ¼þ
--]]

require("macros")
local utile = require("utile")
local table = table
local tostring = tostring

local EnevtDisp = {}
EnevtDisp.__index = EnevtDisp

function EnevtDisp:new()     
    local self = {}
    setmetatable(self, EnevtDisp)
    
	self.Proto = {}

    return self
end

function EnevtDisp:onEvent(protocol, ...)
    local Func = self.Proto[protocol]
    if Func then
        utile.callFunc(Func, table.unpack{...})
	else
		utile.Log(LogLV.Err, "enevt protocol %s may not register.", tostring(protocol))
    end
end

function EnevtDisp:regEvent(protocol, Func)
	if "function" ~= type(Func) then
        return
    end
	
	self.Proto[protocol] = Func
    utile.Debug("register enevt protocol %s", tostring(protocol))
end

return EnevtDisp
