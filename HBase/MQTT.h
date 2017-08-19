#ifndef H_MQTT_H_
#define H_MQTT_H_

#include "NetParser.h"
#include "Singleton.h"
#include "HStruct.h"
#include "UUID.h"

H_BNAMSP

//固定头
struct MQTT_FixedHead
{
    unsigned char MsgType;
    unsigned char DUP;
    unsigned char QoS;
    unsigned char RETAIN;
};
//连接
struct MQTT_CONNECT_Info
{
    unsigned char UserNameFlag;
    unsigned char PswFlag;
    unsigned char WillRetain;
    unsigned char WillQoS;
    unsigned char WillFlag;
    unsigned char CleanSession;
    unsigned char Reserved;//验证是否为 0，如果不为 0 必须断开客户端连接
    unsigned char ProtoLevel;

    unsigned short KeepAlive;
    std::string ProtoName;
    std::string UserName;
    std::string Psw;
    std::string WillTopic;
    std::string WillMessage;
    std::string ClientId;
};

struct MQTT_PUBLISH_Info
{
    unsigned short MsgId;
    std::string Topic;
    std::string Payload;
};

struct MQTT_PUBACK_Info
{
    unsigned short MsgId;
};
struct MQTT_PUBREC_Info
{
    unsigned short MsgId;
};
struct MQTT_PUBREL_Info
{
    unsigned short MsgId;
};
struct MQTT_PUBCOMP_Info
{
    unsigned short MsgId;
};

struct SUBSCRIBETopic
{
    unsigned char QoS;
    std::string Topic;
};
struct MQTT_SUBSCRIBE_Info
{
    unsigned short MsgId;
    std::vector<SUBSCRIBETopic> vcTopic;
};

struct MQTT_UNSUBSCRIBE_Info
{
    unsigned short MsgId;
    std::vector<std::string> vcTopic;
};

//mqtt v3.1.1
class CMQTT : public CParser, public CSingleton<CMQTT>
{
public:
    CMQTT(void);
    ~CMQTT(void);

    H_Binary parsePack(H_Session *pSession, char *pAllBuf, const size_t &iLens, size_t &iParsed, bool &);

    bool parseCONNECT(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_CONNECT_Info &stCONNECTInfo);

    bool parsePUBLISH(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBLISH_Info &stPUBLISHInfo);
    bool parsePUBACK(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBACK_Info &stPUBACKInfo);
    bool parsePUBREC(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBREC_Info &stPUBRECInfo);
    bool parsePUBREL(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBREL_Info &stPUBRELInfo);
    bool parsePUBCOMP(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_PUBCOMP_Info &stPUBCOMPInfo);

    bool parseSUBSCRIBE(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_SUBSCRIBE_Info &stSUBSCRIBEInfo);
    bool parseUNSUBSCRIBE(H_Binary *pBinary, MQTT_FixedHead &stFixedHead, MQTT_UNSUBSCRIBE_Info &stUNSUBSCRIBEInfo);

    bool parsePINGREQ(H_Binary *pBinary, MQTT_FixedHead &stFixedHead);
    bool parseDISCONNECT(H_Binary *pBinary, MQTT_FixedHead &stFixedHead);

private:
    size_t parseHeadLens(H_Binary *pBinary);
    void parseHead(H_Binary *pBinary, MQTT_FixedHead &stFixedHead);

private:
    CUUID m_objUUID;
};

H_ENAMSP

#endif //H_MQTT_H_