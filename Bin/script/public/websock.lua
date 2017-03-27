--[[
websock
--]]

local websock = {}

local MsgType = {}
MsgType.CONTINUATION = 0x00
MsgType.TEXT = 0x01
MsgType.BINARY = 0x02
MsgType.CLOSE = 0x08
MsgType.PING = 0x09
MsgType.PONG = 0x0A

local function Response(code, fin, val)
    local pWBinary = CBinary()
    
    local cType = code | 0x80
    if 0 == fin then
        cType = cType & 0x7F
    end    
    pWBinary:setUint8(cType)
    
    local iLens = #val
    if iLens <= 125 then  
        pWBinary:setUint8(iLens)
    elseif iLens <= 0xFFFF then  
        pWBinary:setUint8(126)
        pWBinary:setUint16(iLens)
    else
        pWBinary:setUint8(127)
        pWBinary:setUint64(iLens)
    end
    
    if val then
        pWBinary:setByte(val, iLens)
    end
  
    return pWBinary
end

function websock.parsePack(pBinary)
    local tInfo = {}
    
	--消息体
    tInfo.info = pBinary:getByte(pBinary:getSurpLens())
    pBinary:setW2R()
	--MsgType
    tInfo.code = pBinary:getUint8()
	--FIN 帧
    tInfo.fin = pBinary:getUint8()
    
    return tInfo
end

--[[分片帧
起始帧（FIN为0，opcode非0），
中间帧（FIN为0，opcode为0），
结束帧（FIN为1，opcode为0）--]]
function websock.ContBegin(code, val)
    assert(0 ~= code)
    return Response(code, 0, val)
end

function websock.Cont(val)    
    return Response(MsgType.CONTINUATION, 0, val)
end

function websock.ContEnd(val)
    return Response(MsgType.CONTINUATION, 1, val)
end
--文本
function websock.Text(val)
    return Response(MsgType.TEXT, 1, val)
end
--二进制
function websock.Binary(val)
    return Response(MsgType.BINARY, 1, val)
end
--关闭
function websock.Close()
    return Response(MsgType.CLOSE, 1)
end
--ping
function websock.Ping()
    return Response(MsgType.PING, 1)
end
--pong
function websock.Pong()
    return Response(MsgType.PONG, 1)
end

return websock
