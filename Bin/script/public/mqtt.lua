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
    "CONNECT",    --1 客户端请求连接服务器
    "CONNACK",    --2 连接确认
    "PUBLISH",    --3 发布消息
    "PUBACK",     --4 发布确认
    "PUBREC",     --5 发布接收（有保证的交付第1部分）
    "PUBREL",     --6 发布释放（有保证的交付第2部分）
    "PUBCOMP",    --7 发布完成（有保证的交付第3部分）
    "SUBSCRIBE",  --8 客户端订阅请求
    "SUBACK",     --9 订阅确认
    "UNSUBSCRIBE",--10 客户端取消订阅请求
    "UNSUBACK",   --11 取消订阅确认
    "PINGREQ",    --12 PING请求
    "PINGRESP",   --13 PING回复
    "DISCONNECT"  --14 客户端断开连接
}
MsgType = table.enum(MsgType, 1)

mqtt.Type = MsgType

--客户端请求连接服务器
local function parseCONNECT(pBinary, tInfo)
    --可变头
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
    
    --有效载荷
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

--连接确认
local function parseCONNACK(pBinary, tInfo)
    --可变头
    local tVHead = {}
    pBinary:skipRead(1)
    tVHead.rtn = pBinary:getUint8()
    
    tInfo.vhead = tVHead
end

--发布消息
local function parsePUBLISH(pBinary, tInfo)
    --可变头
    local tVHead = {}
    local iLens = pBinary:getUint16()
    tVHead.topic = pBinary:getByte(iLens)
    if 1 == tInfo.fhead.qos or 2 == tInfo.fhead.qos then
        tVHead.msgid = pBinary:getUint16()
    end
    
    tInfo.vhead = tVHead
    
    --有效载荷
    local tPayload = {}
    tPayload.info = pBinary:getByte(pBinary:getSurpLens())
    
    tInfo.payload = tPayload
end

--发布确认 对QoS级别1的 PUBLISH 消息的回应
local function parsePUBACK(pBinary, tInfo)
    --可变头
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--发布接收（有保证的交付第1部分） 对QoS级别2的 PUBLISH 消息的回应
local function parsePUBREC(pBinary, tInfo)
    --可变头
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--发布释放（有保证的交付第2部分）发布者客户端对服务器发送给它的 PUBREC 消息的回应
local function parsePUBREL(pBinary, tInfo)
    --可变头
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--发布完成（有保证的交付第3部分）服务器对发布者客户端发送给它的 PUBREL 消息的回应
local function parsePUBCOMP(pBinary, tInfo)
    --可变头
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--客户端订阅请求
local function parseSUBSCRIBE(pBinary, tInfo)
    --可变头
    local tVHead = {}
    tVHead.msgid = pBinary:getUint16()
    tInfo.vhead = tVHead
    
    --有效载荷
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

--订阅确认
local function parseSUBACK(pBinary, tInfo)
    --可变头
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
    
    --有效载荷
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

--客户端取消订阅请求
local function parseUNSUBSCRIBE(pBinary, tInfo)
    --可变头
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
    
    --有效载荷
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

--取消订阅确认
local function parseUNSUBACK(pBinary, tInfo)
    --可变头
    local tVHead = {}
    
    tVHead.msgid = pBinary:getUint16()
    
    tInfo.vhead = tVHead
end

--PING请求
local function parsePINGREQ(pBinary, tInfo)
    
end

--PING回复
local function parsePINGRESP(pBinary, tInfo)
    
end

--客户端断开连接
local function parseDISCONNECT(pBinary, tInfo)
    
end

function mqtt.parsePack(pBinary)
    local tInfo = {}    
    
    --头信息
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
    
    if MsgType.CONNECT == cType then    --客户端请求连接服务器
        parseCONNECT(pBinary, tInfo)
    elseif MsgType.CONNACK == cType then  --连接确认
        parseCONNACK(pBinary, tInfo)
    elseif MsgType.PUBLISH == cType then  --发布消息
        parsePUBLISH(pBinary, tInfo)
    elseif MsgType.PUBACK == cType then   --发布确认
        parsePUBACK(pBinary, tInfo)
    elseif MsgType.PUBREC == cType then   --发布接收（有保证的交付第1部分）
        parsePUBREC(pBinary, tInfo)
    elseif MsgType.PUBREL == cType then   --发布释放（有保证的交付第2部分）
        parsePUBREL(pBinary, tInfo)
    elseif MsgType.PUBCOMP == cType then  --发布完成（有保证的交付第3部分）
        parsePUBCOMP(pBinary, tInfo)
    elseif MsgType.SUBSCRIBE == cType then    --客户端订阅请求
        parseSUBSCRIBE(pBinary, tInfo)
    elseif MsgType.SUBACK == cType then   --订阅确认
        parseSUBACK(pBinary, tInfo)
    elseif MsgType.UNSUBSCRIBE == cType then  --客户端取消订阅请求
        parseUNSUBSCRIBE(pBinary, tInfo)
    elseif MsgType.UNSUBACK == cType then --取消订阅确认
        parseUNSUBACK(pBinary, tInfo)
    elseif MsgType.PINGREQ == cType then  --PING请求
        parsePINGREQ(pBinary, tInfo)
    elseif MsgType.PINGRESP == cType then --PING回复
        parsePINGRESP(pBinary, tInfo)
    elseif MsgType.DISCONNECT == cType then   --客户端断开连接
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

--客户端请求连接服务器
function mqtt.CONNECT(clinetID, strUser, strPsw, 
    willretain, willqos, will, 
    topic, payload, 
    cleansession, keepalive)
    
    assert(clinetID and 0 ~= #clinetID)
    
    local pWBinary = CBinary()
    
    --可变头
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
    
    --有效载荷
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

--连接确认
function mqtt.CONNACK(rtnCode)
    local pWBinary = CBinary()
   
    --可变头
    local vHead = string.pack("B", 0)
    vHead = vHead .. string.pack("B", rtnCode)
    
    --固定头
    local fHead = createHead(MsgType.CONNACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--发布消息
function mqtt.PUBLISH(topic, qos, msg, msgid)
    assert(topic and 0 ~= #topic)
    local pWBinary = CBinary()
    
    --可变头
    local vHead = {}
    table.insert(vHead, string.pack(">H", #topic))
    table.insert(vHead, topic)
    if 1 == qos or 2 == qos then
        assert(msgid)
        table.insert(vHead, string.pack(">H", msgid))
    end
    
    --有效载荷
    table.insert(vHead, msg)
    
    --固定头
    local msg = table.concat(vHead, "")
    local fHead = createHead(MsgType.PUBLISH, 0, qos, 0, #msg)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(msg, #msg)
    
    return pWBinary
end

--发布确认 PUBLISH QoS级别1 的回应
function mqtt.PUBACK(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --可变头
    local vHead = string.pack(">H", msgid)    
    local fHead = createHead(MsgType.PUBACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--发布接收（有保证的交付第1部分）PUBLISH QoS级别2 的回应
function mqtt.PUBREC(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --可变头
    local vHead = string.pack(">H", msgid)    
    local fHead = createHead(MsgType.PUBREC, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--发布释放（有保证的交付第2部分） 对PUBREC的回应
function mqtt.PUBREL(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --可变头
    local vHead = string.pack(">H", msgid)    
    local fHead = createHead(MsgType.PUBREL, 0, 1, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--发布完成（有保证的交付第3部分）对PUBREL的回应
function mqtt.PUBCOMP(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --可变头
    local vHead = string.pack(">H", msgid)
    local fHead = createHead(MsgType.PUBCOMP, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--客户端订阅请求
function mqtt.SUBSCRIBE(msgid, tTopic)
    assert(msgid)
    assert("table" == type(tTopic))
    local pWBinary = CBinary()
    
    --可变头
    local vHead = {}
    table.insert(vHead, string.pack(">H", msgid))
    
    --有效载荷
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

--订阅确认
function mqtt.SUBACK(msgid, tQos)
    assert(msgid)
    assert("table" == type(tQos))
    local pWBinary = CBinary()
    
    --可变头
    local vHead = {}
    table.insert(vHead, string.pack(">H", msgid))
    
    --有效载荷
    for _, qos in pairs(tQos) do
        table.insert(vHead, string.pack("B", qos))
    end
    
    local msg = table.concat(vHead, "")
    local fHead = createHead(MsgType.SUBACK, 0, 0, 0, #msg)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(msg, #msg)
    
    return pWBinary
end

--客户端取消订阅请求
function mqtt.UNSUBSCRIBE(msgid, tTopic)
    assert(msgid)
    assert("table" == type(tTopic))
    local pWBinary = CBinary()
    
    --可变头
    local vHead = {}
    table.insert(vHead, string.pack(">H", msgid))
    
    --有效载荷
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

--取消订阅确认
function mqtt.UNSUBACK(msgid)
    assert(msgid)
    local pWBinary = CBinary()
    
    --可变头
    local vHead = string.pack(">H", msgid)
    
    local fHead = createHead(MsgType.UNSUBACK, 0, 0, 0, #vHead)
    
    pWBinary:setByte(fHead, #fHead)
    pWBinary:setByte(vHead, #vHead)
    
    return pWBinary
end

--PING请求
function mqtt.PINGREQ()
    local pWBinary = CBinary()
    
    local fHead = createHead(MsgType.PINGREQ, 0, 0, 0, 0)
    
    pWBinary:setByte(fHead, #fHead)
    
    return pWBinary
end

--PING回复
function mqtt.PINGRESP()
    local pWBinary = CBinary()
    
    local fHead = createHead(MsgType.PINGRESP, 0, 0, 0, 0)
    
    pWBinary:setByte(fHead, #fHead)
    
    return pWBinary
end

--客户端断开连接
function mqtt.DISCONNECT()
    local pWBinary = CBinary()
    
    local fHead = createHead(MsgType.DISCONNECT, 0, 0, 0, 0)
    
    pWBinary:setByte(fHead, #fHead)
    
    return pWBinary
end

return mqtt
