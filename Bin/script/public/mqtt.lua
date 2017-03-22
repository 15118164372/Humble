--[[
mqtt
--]]

require("macros")
local assert = assert
local table = table
local math = math
local type = type

local mqtt = {}
local strProName = "MQIsdp"
local iProVersion = 13

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

mqtt.Type = MsgType

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
        if 0 ~= pBinary:getSurpLens() then
            iLens = pBinary:getUint16()
            tPayload.user = pBinary:getByte(iLens)
        end
    end    
    if 1 == tVHead.pswflag then
        if 0 ~= pBinary:getSurpLens() then
            iLens = pBinary:getUint16()
            tPayload.psw = pBinary:getByte(iLens)
        end
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

--������Ϣ
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

--����ȷ�� ��QoS����1�� PUBLISH ��Ϣ�Ļ�Ӧ
local function parsePUBACK(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--�������գ��б�֤�Ľ�����1���֣� ��QoS����2�� PUBLISH ��Ϣ�Ļ�Ӧ
local function parsePUBREC(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--�����ͷţ��б�֤�Ľ�����2���֣������߿ͻ��˶Է��������͸����� PUBREC ��Ϣ�Ļ�Ӧ
local function parsePUBREL(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--������ɣ��б�֤�Ľ�����3���֣��������Է����߿ͻ��˷��͸����� PUBREL ��Ϣ�Ļ�Ӧ
local function parsePUBCOMP(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--�ͻ��˶�������
local function parseSUBSCRIBE(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    tVHead.msgid = pBinary:getUint16()
    tInfo.vhead = tVHead
    
    --��Ч�غ�
    local iLens, topic, qos
    local tPayload = {}
    while true
    do
        if 2 > pBinary:getSurpLens() then
            break
        end
        
        iLens = pBinary:getUint16()
        topic = pBinary:getByte(iLens)
        qos = pBinary:getUint8()
        tPayload[topic] = qos
    end
    
    tInfo.payload = tPayload
end

--����ȷ��
local function parseSUBACK(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
    
    --��Ч�غ�
    local tPayload = {}
    
    while true
    do
        if 0 == pBinary:getSurpLens() then
            break
        end
        
        table.insert(tPayload, pBinary:getUint8())
    end
    
    tInfo.payload = tPayload
end

--�ͻ���ȡ����������
local function parseUNSUBSCRIBE(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
    
    --��Ч�غ�
    local tPayload = {}
    local iLens
    while true
    do
        if 2 > pBinary:getSurpLens() then
            break
        end
        
        iLens = pBinary:getUint16()
        table.insert(tPayload, pBinary:getByte(iLens))
    end
    
    tInfo.payload = tPayload
end

--ȡ������ȷ��
local function parseUNSUBACK(pBinary, tInfo)
    --�ɱ�ͷ
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--PING����
local function parsePINGREQ(pBinary, tInfo)
    
end

--PING�ظ�
local function parsePINGRESP(pBinary, tInfo)
    
end

--�ͻ��˶Ͽ�����
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
    until(lens <= 0)
    
    return table.concat(tHead, "")
end

--�ͻ����������ӷ�����
function mqtt.CONNECT(clinetID, strUser, strPsw, 
    willretain, willqos, will, 
    topic, payload, 
    cleansession, keepalive)
    
    assert(clinetID and 0 ~= #clinetID)
    
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = {}
    table.insert(vHead, string.pack(">H", #strProName))
    table.insert(vHead, strProName)
    table.insert(vHead, string.pack("B", iProVersion))
    local cConnFlag = 0
    if strUser and 0 ~= #strUser then
        cConnFlag = 1 << 7
    end
    if strPsw and 0 ~= #strPsw then
        cConnFlag = cConnFlag | (1 << 6)
    end
    if willretain and 0 ~= willretain then
        cConnFlag = cConnFlag | (1 << 5)
    end
    if willqos and 0 ~= willqos then
        cConnFlag = cConnFlag | (willqos << 3)
    end
    if will and 0 ~= will then
        assert(topic and 0 ~= #topic)
        cConnFlag = cConnFlag | (1 << 2)
    end
    if cleansession and 0 ~= cleansession then
        cConnFlag = cConnFlag | (1 << 1)
    end    
    table.insert(vHead, string.pack("B", cConnFlag))    
    table.insert(vHead, string.pack(">H", keepalive))
    
    --��Ч�غ�
    table.insert(vHead, string.pack(">H", #clinetID))
    table.insert(vHead, clinetID)
    if will and 0 ~= will then
        table.insert(vHead, string.pack(">H", #topic))
        table.insert(vHead, topic)
        
        if not payload then
            table.insert(vHead, string.pack(">H", 0))
            table.insert(vHead, "")
        else
            table.insert(vHead, string.pack(">H", #payload))
            table.insert(vHead, payload)
        end
    end
    
    if strUser and 0 ~= #strUser then
        table.insert(vHead, string.pack(">H", #strUser))
        table.insert(vHead, strUser)
    end
    if strPsw and 0 ~= #strPsw then
        table.insert(vHead, string.pack(">H", #strPsw))
        table.insert(vHead, strPsw)
    end
    
    local msg = table.concat(vHead, "")
    local fHead = createHead(MsgType.CONNECT, 0, 0, 0, #msg)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(msg, #msg)
    
    return pWBinary
end

--����ȷ��
function mqtt.CONNACK(rtnCode)
    local pWBinary = CBinary()
   
    --�ɱ�ͷ
    local vHead = string.pack("B", 0)
    vHead = vHead .. string.pack("B", rtnCode)
    
    --�̶�ͷ
    local fHead = createHead(MsgType.CONNACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--������Ϣ
function mqtt.PUBLISH(topic, qos, msg, msgid)
    assert(topic and 0 ~= #topic)
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = {}
    table.insert(vHead, string.pack(">H", #topic))
    table.insert(vHead, topic)
    if 1 == qos or 2 == qos then
        assert(msgid)
        table.insert(vHead, string.pack(">H", msgid))
    end
    
    --��Ч�غ�
    table.insert(vHead, msg)
    
    --�̶�ͷ
    local msg = table.concat(vHead, "")
    local fHead = createHead(MsgType.PUBLISH, 0, qos, 0, #msg)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(msg, #msg)
    
    return pWBinary
end

--����ȷ�� PUBLISH QoS����1 �Ļ�Ӧ
function mqtt.PUBACK(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = string.pack(">H", msgid)    
    local fHead = createHead(MsgType.PUBACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--�������գ��б�֤�Ľ�����1���֣�PUBLISH QoS����2 �Ļ�Ӧ
function mqtt.PUBREC(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = string.pack(">H", msgid)    
    local fHead = createHead(MsgType.PUBREC, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--�����ͷţ��б�֤�Ľ�����2���֣� ��PUBREC�Ļ�Ӧ
function mqtt.PUBREL(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = string.pack(">H", msgid)    
    local fHead = createHead(MsgType.PUBREL, 0, 1, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--������ɣ��б�֤�Ľ�����3���֣���PUBREL�Ļ�Ӧ
function mqtt.PUBCOMP(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = string.pack(">H", msgid)
    local fHead = createHead(MsgType.PUBCOMP, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--�ͻ��˶�������
function mqtt.SUBSCRIBE(msgid, tTopic)
    assert(msgid)
    assert("table" == type(tTopic))
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = {}
    table.insert(vHead, string.pack(">H", msgid))
    
    --��Ч�غ�
    for topic, qos in pairs(tTopic) do
        table.insert(vHead, string.pack(">H", #topic))
        table.insert(vHead, topic)
        table.insert(vHead, string.pack("B", qos))
    end
    
    local msg = table.concat(vHead, "")
    local fHead = createHead(MsgType.SUBSCRIBE, 0, 1, 0, #msg)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(msg, #msg)
    
    return pWBinary
end

--����ȷ��
function mqtt.SUBACK(msgid, tQos)
    assert(msgid)
    assert("table" == type(tQos))
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = {}
    table.insert(vHead, string.pack(">H", msgid))
    
    --��Ч�غ�
    for _, qos in pairs(tQos) do
        table.insert(vHead, string.pack("B", qos))
    end
    
    local msg = table.concat(vHead, "")
    local fHead = createHead(MsgType.SUBACK, 0, 0, 0, #msg)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(msg, #msg)
    
    return pWBinary
end

--�ͻ���ȡ����������
function mqtt.UNSUBSCRIBE(msgid, tTopic)
    assert(msgid)
    assert("table" == type(tTopic))
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = {}
    table.insert(vHead, string.pack(">H", msgid))
    
    --��Ч�غ�
    for _, topic in pairs(tTopic) do
        table.insert(vHead, string.pack(">H", #topic))
        table.insert(vHead, topic)
    end
    
    local msg = table.concat(vHead, "")
    local fHead = createHead(MsgType.UNSUBSCRIBE, 0, 1, 0, #msg)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(msg, #msg)
    
    return pWBinary
end

--ȡ������ȷ��
function mqtt.UNSUBACK(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --�ɱ�ͷ
    local vHead = string.pack(">H", msgid)
    
    local fHead = createHead(MsgType.UNSUBACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--PING����
function mqtt.PINGREQ()
    local pWBinary = CBinary()
    
    local fHead = createHead(MsgType.PINGREQ, 0, 0, 0, 0)
    
    pWBinary:setByte(fHead, #fHead)
    
    return pWBinary
end

--PING�ظ�
function mqtt.PINGRESP()
    local pWBinary = CBinary()
    
    local fHead = createHead(MsgType.PINGRESP, 0, 0, 0, 0)
    
    pWBinary:setByte(fHead, #fHead)
    
    return pWBinary
end

--�ͻ��˶Ͽ�����
function mqtt.DISCONNECT()
    local pWBinary = CBinary()
    
    local fHead = createHead(MsgType.DISCONNECT, 0, 0, 0, 0)
    
    pWBinary:setByte(fHead, #fHead)
    
    return pWBinary
end

return mqtt
