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
    
	--��Ϣ��
    tInfo.info = pBinary:getByte(pBinary:getSurpLens())
    pBinary:setW2R()
	--MsgType
    tInfo.code = pBinary:getUint8()
	--FIN ֡
    tInfo.fin = pBinary:getUint8()
    
    return tInfo
end

--[[��Ƭ֡
��ʼ֡��FINΪ0��opcode��0����
�м�֡��FINΪ0��opcodeΪ0����
����֡��FINΪ1��opcodeΪ0��--]]
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
--�ı�
function websock.Text(val)
    return Response(MsgType.TEXT, 1, val)
end
--������
function websock.Binary(val)
    return Response(MsgType.BINARY, 1, val)
end
--�ر�
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
