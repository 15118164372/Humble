--[[
tcp1  
消息长度小于126  char(消息长度) + 消息   
小于等于65535    char(值126) + unsigned short(消息长度) + 消息
大于65535        char(值127) + unsigned int(消息长度) + 消息
--]]

local tcp1 = {}

function tcp1.Response(val)
    local pWBinary = CBinary()    
    local iLens = #val
    if iLens <= 125 then  
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
