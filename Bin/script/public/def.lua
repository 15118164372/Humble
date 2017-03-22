--[[
default 
--]]

local default = {}

function default.Response(val)
    local pWBinary = CBinary()
    
    if val then
        pWBinary:setByte(val, #val)
    end
    
    return pWBinary
end

return default
