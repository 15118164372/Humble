--[[
mqtt
--]]

require("macros")
local table = table
local math = math

local mqtt = {}
local pWBinary = CBinary()

local MsgType = {
    "CONNECT",    --1 �ͻ����������ӷ�����
    "CONNACK",    --2 ����ȷ��
    "PUBLISH",    --3 ������Ϣ
    "PUBACK",     --4 ����ȷ��
    "PUBREC",     --5 �������գ��б�֤�Ľ�����1���֣�
    "PUBREL",     --6 �����ͷţ��б�֤�Ľ�����2���֣�
    "PUBCOMP",    --7 ������ɣ��б�֤�Ľ�����3���֣�
    "SUBSCRIBE",  --8 �ͻ��˶�������
    "SUBACK",     --9 ����ȷ��
    "UNSUBSCRIBE",--10 �ͻ���ȡ����������
    "UNSUBACK",   --11 ȡ������ȷ��
    "PINGREQ",    --12 PING����
    "PINGRESP",   --13 PING�ظ�
    "DISCONNECT"  --14 �ͻ��˶Ͽ�����
}
MsgType = table.enum(MsgType, 1)

--�ͻ����������ӷ�����
local function parseCONNECT(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
        
    local iLens = pBinary:getUint16()
    tVHead.proname = pBinary:getByte(iLens)
    tVHead.version = pBinary:getUint8()

    local cConnectFlags = pBinary:getUint8()        
    tVHead.userflage = (cConnectFlags & 0x80) >> 7
    tVHead.pswflag = (cConnectFlags & 0x40) >> 6
    tVHead.willretain = (cConnectFlags & 0x20) >> 5
    tVHead.willqos = (cConnectFlags & 0x18) >> 3
    tVHead.will = (cConnectFlags & 0x04) >> 2
    tVHead.cleansession = (cConnectFlags & 0x02) >> 1
    
    tVHead.keepalive = pBinary:getUint16()
        
    tInfo.vhead = tVHead
    
    --��Ч�غ�
    local tPayload = {}
    iLens = pBinary:getUint16()
    tPayload.clientid = pBinary:getByte(iLens)
    if 1 == tVHead.will then
        iLens = pBinary:getUint16()
        tPayload.topic = pBinary:getByte(iLens)
        iLens = pBinary:getUint16()
        tPayload.payload = pBinary:getByte(iLens)
    end    
    if 1 == tVHead.userflage then
        iLens = pBinary:getUint16()
        tPayload.user = pBinary:getByte(iLens)
    end    
    if 1 == tVHead.pswflag then
        iLens = pBinary:getUint16()
        tPayload.psw = pBinary:getByte(iLens)
    end
        
    tInfo.payload = tPayload
end

--����ȷ��
local function parseCONNACK(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    pBinary:skipRead(1)
    tVHead.rtn = pBinary:getUint8()
    
    tInfo.vhead = tVHead
end

local function parsePUBLISH(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    local iLens = pBinary:getUint16()
    tVHead.topic = pBinary:getByte(iLens)
    if 1 == tInfo.fhead.qos or 2 == tInfo.fhead.qos then
        tVHead.msgid = pBinary:getUint16()
    end
    
    tInfo.vhead = tVHead
    
    --��Ч�غ�
    local tPayload = {}
    tPayload.info = pBinary:getByte(pBinary:getSurpLens())
    
    tInfo.payload = tPayload
end
local function parsePUBACK(pBinary, tInfo)
end
local function parsePUBREC(pBinary, tInfo)
end
local function parsePUBREL(pBinary, tInfo)
end
local function parsePUBCOMP(pBinary, tInfo)
end
local function parseSUBSCRIBE(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    tVHead.msgid = pBinary:getUint16()
    tInfo.vhead = tVHead
    
    --��Ч�غ�
    local tPayload = {}
    while true
    do
        if 2 > pBinary:getSurpLens() then
            break
        end
        
        local iLens = pBinary:getUint16()
        if 0 == iLens then
            break
        end
        
        local tTmp = {}
        tTmp.topic = pBinary:getByte(iLens)
        tTmp.qos = pBinary:getUint8()
        table.insert(tPayload, tTmp)
    end
    
    tInfo.payload = tPayload
end
local function parseSUBACK(pBinary, tInfo)
end
local function parseUNSUBSCRIBE(pBinary, tInfo)
end
local function parseUNSUBACK(pBinary, tInfo)
end
local function parsePINGREQ(pBinary, tInfo)
end
local function parsePINGRESP(pBinary, tInfo)
end
local function parseDISCONNECT(pBinary, tInfo)
end

function mqtt.parsePack(pBinary)
    local tInfo = {}    
    
    --ͷ��Ϣ
    local tFixedHead = {}
    pBinary:setW2R()
    local cType = pBinary:getUint8()
    tFixedHead.type = cType
    tFixedHead.dup = pBinary:getUint8()
    tFixedHead.qos = pBinary:getUint8()
    tFixedHead.retain = pBinary:getUint8()
    tFixedHead.lens = pBinary:getUint32()    
    tInfo.fhead = tFixedHead
    pBinary:resetW2R()
    
    if MsgType.CONNECT == cType then    --�ͻ����������ӷ�����
        parseCONNECT(pBinary, tInfo)
    elseif MsgType.CONNACK == cType then  --����ȷ��
        parseCONNACK(pBinary, tInfo)
    elseif MsgType.PUBLISH == cType then  --������Ϣ
        parsePUBLISH(pBinary, tInfo)
    elseif MsgType.PUBACK == cType then   --����ȷ��
        parsePUBACK(pBinary, tInfo)
    elseif MsgType.PUBREC == cType then   --�������գ��б�֤�Ľ�����1���֣�
        parsePUBREC(pBinary, tInfo)
    elseif MsgType.PUBREL == cType then   --�����ͷţ��б�֤�Ľ�����2���֣�
        parsePUBREL(pBinary, tInfo)
    elseif MsgType.PUBCOMP == cType then  --������ɣ��б�֤�Ľ�����3���֣�
        parsePUBCOMP(pBinary, tInfo)
    elseif MsgType.SUBSCRIBE == cType then    --�ͻ��˶�������
        parseSUBSCRIBE(pBinary, tInfo)
    elseif MsgType.SUBACK == cType then   --����ȷ��
        parseSUBACK(pBinary, tInfo)
    elseif MsgType.UNSUBSCRIBE == cType then  --�ͻ���ȡ����������
        parseUNSUBSCRIBE(pBinary, tInfo)
    elseif MsgType.UNSUBACK == cType then --ȡ������ȷ��
        parseUNSUBACK(pBinary, tInfo)
    elseif MsgType.PINGREQ == cType then  --PING����
        parsePINGREQ(pBinary, tInfo)
    elseif MsgType.PINGRESP == cType then --PING�ظ�
        parsePINGRESP(pBinary, tInfo)
    elseif MsgType.DISCONNECT == cType then   --�ͻ��˶Ͽ�����
        parseDISCONNECT(pBinary, tInfo)
    else 
        assert(false)
    end   
    
    return tInfo
end

local function createHead(msgtype, dup, qos, retain, lens)
    local cType = (msgtype << 4) | (dup << 3) | (qos << 1) | retain
    
    local tHead = {}
    table.insert(tHead, string.pack("B", cType))
    local cDigit = 0
    repeat
       cDigit = lens % 128
       lens =  math.floor(lens / 128)
       if lens > 0 then
            cDigit = cDigit | 0x80
       end
       
       table.insert(tHead, string.pack("B", cDigit))
       print(lens)
    until(lens <= 0)
    
    return table.concat(tHead, "")
end

--����ȷ��
function mqtt.CONNACK(rtnCode)
    pWBinary:reSetWrite()
   
    local vHead = string.pack("B", 0)
    vHead = vHead .. string.pack("B", rtnCode)
    local fHead = createHead(MsgType.CONNACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

return mqtt
