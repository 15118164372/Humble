--[[
tcp3   unsigned int(消息长度)  + 消息
--]]

local tcp3 = {}

function tcp3.Response(val)
    local pWBinary = CBinary()   
   
    pWBinary:setUint32(#val)
    pWBinary:setByte(val, #val)
    
    return pWBinary
end

return tcp3
