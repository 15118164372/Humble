--[[
tcp2 
--]]

local tcp2 = {}

function tcp2.Response(val)
    local pWBinary = CBinary()   
   
    pWBinary:setUint16(#val)
    pWBinary:setByte(val, #val)
    
    return pWBinary
end

return tcp2
