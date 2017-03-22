--[[
tcp1 
--]]

local tcp1 = {}

function tcp1.Response(val)
    local pWBinary = CBinary()    
    local iLens = #val
    if iLens < 126 then  
        pWBinary:setUint8(iLens)
    elseif iLens <= 0xFFFF then  
        pWBinary:setUint8(126)
        pWBinary:setUint16(iLens)
    else
        pWBinary:setUint8(127)
        pWBinary:setUint32(iLens)
    end
    
    pWBinary:setByte(val, #val)
    
    return pWBinary
end

return tcp1
