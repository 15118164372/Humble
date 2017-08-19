
#include "MQTT.h"
#include "Sender.h"
#include "Log.h"

H_BNAMSP

SINGLETON_INIT(CMQTT)
CMQTT objMQTT;

#define MQTT_PRORONAME "MQTT"
#define MQTT_PROROLEVEL 0x04
#define MQTT_HEADLENS 2
#define CHECK(parsed, lens) if(parsed > lens) return false
#define CHECKQOS(Qos) if(Qos > QOS2) return false

enum
{
    QOS0 = 0,
    QOS1,
    QOS2,
};

CMQTT::CMQTT(void)
{
    setName(H_H_PARSER_MQTT);
}

CMQTT::~CMQTT(void)
{
}

H_Binary CMQTT::parsePack(H_Session *pSession, char *pAllBuf, const size_t &iLens, size_t &iParsed, bool &)
{
    H_Binary stBinary;
    if (MQTT_HEADLENS > iLens)
    {
        return stBinary;
    }

    unsigned char ucByte;    
    size_t iHeadLens(1);
    size_t iMultiplier(1);
    size_t iBufLens(H_INIT_NUMBER);

    do
    {
        ucByte = (unsigned char)pAllBuf[iHeadLens];
        iBufLens += (ucByte & 127) * iMultiplier;
        iMultiplier *= 128;

        ++iHeadLens;
        if (iHeadLens + 1 > iLens)
        {
            return stBinary;
        }

    } while (H_INIT_NUMBER != (ucByte & 128));

    if (iHeadLens + iBufLens > iLens)
    {
        return stBinary;
    }

    iParsed = iHeadLens + iBufLens;
    stBinary.pBufer = pAllBuf;
    stBinary.iLens = iParsed;

    return stBinary;
}

size_t CMQTT::parseHeadLens(H_Binary *pBinary)
{
    unsigned char ucByte;
    size_t iHeadLens(1);
    do
    {
        ucByte = (unsigned char)pBinary->pBufer[iHeadLens];
        ++iHeadLens;
    } while (H_INIT_NUMBER != (ucByte & 128));

    return iHeadLens;
}

void CMQTT::parseHead(H_Binary *pBinary, MQTT_FixedHead &stFixedHead)
{
    unsigned char ucByte((unsigned char)pBinary->pBufer[0]);

    stFixedHead.MsgType = (ucByte & 0xF0) >> 4;
    stFixedHead.DUP = (ucByte & 0x8) >> 3;
    stFixedHead.QoS = (ucByte & 0x6) >> 1;
    stFixedHead.RETAIN = ucByte & 0x1;
}

bool CMQTT::parseCONNECT(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_CONNECT_Info &stCONNECTInfo)
{
    parseHead(pBinary, stFixedHead);
    CHECKQOS(stFixedHead.QoS);
    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //协议名
    unsigned short usLens(ntohs(*(unsigned short *)(pBinary->pBufer + iParsed)));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);
    stCONNECTInfo.ProtoName.append(pBinary->pBufer + iParsed, usLens);
    if (MQTT_PRORONAME != stCONNECTInfo.ProtoName)
    {
        //协议名必须为MQTT
        H_LOG(LOGLV_ERROR, "%s", "protocol name error.");
        return false;
    }
    iParsed += usLens;
    CHECK(iParsed, pBinary->iLens);

    //协议级别
    stCONNECTInfo.ProtoLevel = pBinary->pBufer[iParsed];
    if (MQTT_PROROLEVEL != stCONNECTInfo.ProtoLevel)
    {
        //协议级别0x04
        H_LOG(LOGLV_ERROR, "%s", "protocol level error.");
        return false;
    }
    ++iParsed;
    CHECK(iParsed, pBinary->iLens);

    //连接标志
    unsigned char ucByte((unsigned char)pBinary->pBufer[iParsed]);
    stCONNECTInfo.UserNameFlag = (ucByte & 0x80) >> 7;
    stCONNECTInfo.PswFlag = (ucByte & 0x40) >> 6;
    stCONNECTInfo.WillRetain = (ucByte & 0x20) >> 5;
    stCONNECTInfo.WillQoS = (ucByte & 0x18) >> 3;
    stCONNECTInfo.WillFlag = (ucByte & 0x04) >> 2;
    stCONNECTInfo.CleanSession = (ucByte & 0x02) >> 1;
    stCONNECTInfo.Reserved = ucByte & 0x01;
    if (0 != stCONNECTInfo.Reserved)
    {
        //保留标志位必须为0
        H_LOG(LOGLV_ERROR, "%s", "reserved flag error.");
        return false;
    }
    if (0 == stCONNECTInfo.WillFlag && 0 != stCONNECTInfo.WillQoS)
    {
        //如果遗嘱标志被设置为 0，遗嘱 QoS 也必须设置为 0
        H_LOG(LOGLV_ERROR, "%s", "will flag eq zero, so will qos must eq zero.");
        return false;
    }
    ++iParsed;
    CHECK(iParsed, pBinary->iLens);

    //保持连接
    stCONNECTInfo.KeepAlive = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);

    //有效载荷
    //客户端ID
    usLens = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);
    if (0 == usLens && 1 != stCONNECTInfo.CleanSession)
    {
        //如果客户端提供了一个零字节的客户端标识符，它必须同时将清理会话标志设置为 1
        H_LOG(LOGLV_ERROR, "%s", "empty client id and clean session not eq one.");
        return false;
    }
    if (usLens > 0)
    {
        stCONNECTInfo.ClientId.append(pBinary->pBufer + iParsed, usLens);
        iParsed += usLens;
        CHECK(iParsed, pBinary->iLens);
    }
    else
    {
#ifdef H_UUID
        //分配一ID
        stCONNECTInfo.ClientId = m_objUUID.getUUID();
#endif
    }

    //遗嘱
    if (1 == stCONNECTInfo.WillFlag)
    {
        usLens = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
        iParsed += sizeof(unsigned short);
        CHECK(iParsed, pBinary->iLens);
        stCONNECTInfo.WillTopic.append(pBinary->pBufer + iParsed, usLens);
        iParsed += usLens;
        CHECK(iParsed, pBinary->iLens);

        usLens = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
        iParsed += sizeof(unsigned short);
        CHECK(iParsed, pBinary->iLens);
        stCONNECTInfo.WillMessage.append(pBinary->pBufer + iParsed, usLens);
        iParsed += usLens;
        CHECK(iParsed, pBinary->iLens);
    }
    //用户名
    if (1 == stCONNECTInfo.UserNameFlag)
    {
        usLens = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
        iParsed += sizeof(unsigned short);
        CHECK(iParsed, pBinary->iLens);
        stCONNECTInfo.UserName.append(pBinary->pBufer + iParsed, usLens);
        iParsed += usLens;
        CHECK(iParsed, pBinary->iLens);
    }
    //密码
    if (1 == stCONNECTInfo.PswFlag)
    {
        usLens = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
        iParsed += sizeof(unsigned short);
        CHECK(iParsed, pBinary->iLens);
        stCONNECTInfo.Psw.append(pBinary->pBufer + iParsed, usLens);
        iParsed += usLens;
        CHECK(iParsed, pBinary->iLens);
    }

    return true;
}

bool CMQTT::parsePUBLISH(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBLISH_Info &stPUBLISHInfo)
{
    parseHead(pBinary, stFixedHead);
    CHECKQOS(stFixedHead.QoS);
    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //主题
    unsigned short usLens(ntohs(*(unsigned short *)(pBinary->pBufer + iParsed)));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);
    stPUBLISHInfo.Topic.append(pBinary->pBufer + iParsed, usLens);
    //不能包含通配符
    if (std::string::npos != stPUBLISHInfo.Topic.find("+") 
        || std::string::npos != stPUBLISHInfo.Topic.find("#"))
    {
        H_LOG(LOGLV_ERROR, "%s", "cannot include wildcards");
        return false;
    }
    iParsed += usLens;
    CHECK(iParsed, pBinary->iLens);

    //报文标识符 QOS1 QOS2才有
    if (QOS1 == stFixedHead.QoS || QOS2 == stFixedHead.QoS)
    {
        stPUBLISHInfo.MsgId = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
        iParsed += sizeof(unsigned short);
        CHECK(iParsed, pBinary->iLens);
    }

    stPUBLISHInfo.Payload.append(pBinary->pBufer + iParsed, pBinary->iLens - iParsed);

    return true;
}

bool CMQTT::parsePUBACK(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBACK_Info &stPUBACKInfo)
{
    parseHead(pBinary, stFixedHead);
    CHECKQOS(stFixedHead.QoS);
    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //报文标识符
    stPUBACKInfo.MsgId = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);

    return true;
}

bool CMQTT::parsePUBREC(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBREC_Info &stPUBRECInfo)
{
    parseHead(pBinary, stFixedHead);
    CHECKQOS(stFixedHead.QoS);
    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //报文标识符
    stPUBRECInfo.MsgId = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);

    return true;
}

bool CMQTT::parsePUBREL(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBREL_Info &stPUBRELInfo)
{
    parseHead(pBinary, stFixedHead);
    if (0 != stFixedHead.DUP
        || QOS1 != stFixedHead.QoS
        || 0 != stFixedHead.RETAIN)
    {
        //固定报头3 2 1 0位 必须为 0 0 1 0
        H_LOG(LOGLV_ERROR, "%s", "invalid flags.");
        return false;
    }

    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //报文标识符
    stPUBRELInfo.MsgId = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);

    return true;
}

bool CMQTT::parsePUBCOMP(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBCOMP_Info &stPUBCOMPInfo)
{
    parseHead(pBinary, stFixedHead);
    CHECKQOS(stFixedHead.QoS);
    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //报文标识符
    stPUBCOMPInfo.MsgId = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);

    return true;
}

bool CMQTT::parseSUBSCRIBE(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_SUBSCRIBE_Info &stSUBSCRIBEInfo)
{
    parseHead(pBinary, stFixedHead);
    if (0 != stFixedHead.DUP
        || QOS1 != stFixedHead.QoS
        || 0 != stFixedHead.RETAIN)
    {
        //固定报头3 2 1 0位 必须为 0 0 1 0
        H_LOG(LOGLV_ERROR, "%s", "invalid flags.");
        return false;
    }

    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //报文标识符
    stSUBSCRIBEInfo.MsgId = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);

    //主题
    unsigned short usLens(H_INIT_NUMBER);
    while (pBinary->iLens - iParsed > 2)
    {
        usLens = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
        iParsed += sizeof(unsigned short);
        CHECK(iParsed, pBinary->iLens);

        SUBSCRIBETopic stTopic;
        stTopic.Topic.append(pBinary->pBufer + iParsed, usLens);
        iParsed += usLens;
        CHECK(iParsed, pBinary->iLens);

        stTopic.QoS = pBinary->pBufer[iParsed];
        CHECKQOS(stTopic.QoS);

        ++iParsed;
        CHECK(iParsed, pBinary->iLens);

        stSUBSCRIBEInfo.vcTopic.push_back(stTopic);
    }

    return true;
}

bool CMQTT::parseUNSUBSCRIBE(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_UNSUBSCRIBE_Info &stUNSUBSCRIBEInfo)
{
    parseHead(pBinary, stFixedHead);
    if (0 != stFixedHead.DUP
        || QOS1 != stFixedHead.QoS
        || 0 != stFixedHead.RETAIN)
    {
        //固定报头3 2 1 0位 必须为 0 0 1 0
        H_LOG(LOGLV_ERROR, "%s", "invalid flags.");
        return false;
    }

    size_t iHeadLens = parseHeadLens(pBinary);

    size_t iParsed(iHeadLens);
    CHECK(iParsed, pBinary->iLens);

    //报文标识符
    stUNSUBSCRIBEInfo.MsgId = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
    iParsed += sizeof(unsigned short);
    CHECK(iParsed, pBinary->iLens);

    //主题
    unsigned short usLens(H_INIT_NUMBER);
    while (pBinary->iLens - iParsed > 2)
    {
        usLens = ntohs(*(unsigned short *)(pBinary->pBufer + iParsed));
        iParsed += sizeof(unsigned short);
        CHECK(iParsed, pBinary->iLens);

        std::string strTopic(pBinary->pBufer + iParsed, usLens);
        iParsed += usLens;
        CHECK(iParsed, pBinary->iLens);

        stUNSUBSCRIBEInfo.vcTopic.push_back(strTopic);
    }

    return true;
}

bool CMQTT::parsePINGREQ(H_Binary *pBinary, MQTT_FixedHead &stFixedHead)
{
    parseHead(pBinary, stFixedHead);
    CHECKQOS(stFixedHead.QoS);

    return true;
}

bool CMQTT::parseDISCONNECT(H_Binary *pBinary, MQTT_FixedHead &stFixedHead)
{
    parseHead(pBinary, stFixedHead);
    CHECKQOS(stFixedHead.QoS);

    return true;
}

H_ENAMSP
