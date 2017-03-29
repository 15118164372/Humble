--[[
模块网络事件
--]]

require("macros")
local utile = require("utile")
local humble = require("humble")
local table = table
local tostring = tostring

local NetDisp = {}
NetDisp.__index = NetDisp

--strTaskName 模块名
function NetDisp:new(strTaskName)
	assert(strTaskName and 0 ~= #strTaskName)
    local self = {}
    setmetatable(self, NetDisp)
    
    self.TaskName = strTaskName
	self.Proto = {}

    return self
end

function NetDisp:onNetEvent(protocol, ...)
    local Func = self.Proto[protocol]
    if Func then
        utile.callFunc(Func, table.unpack{...})
	else
		utile.Log(LogLV.Err, "net protocol %s may not register.", tostring(protocol))
    end
end

function NetDisp:regNetEvent(protocol, Func)
	assert("string" == type(protocol) or "number" == type(protocol))
	assert("function" == type(Func))
	
	humble.regProto(protocol, self.TaskName)
	
	self.Proto[protocol] = Func
    utile.Debug("register net protocol %s", tostring(protocol))
end

return NetDisp
