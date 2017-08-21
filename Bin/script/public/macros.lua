--全局定义
local table = table

function table.enum(tMsg, iBegin) 
    assert("table" == type(tMsg))    
    local tEnum = {} 
    local iEnumIndex = iBegin or 0 
    for key, val in pairs(tMsg) do 
        tEnum[val] = iEnumIndex + key - 1
    end 
    
    return tEnum 
end 

--无效ID
Invalid_ID = -1
--无效socket
Invalid_Sock = -1

--错误码
ErrCode = {
    "Fail",
    "Ok",
    "Error",
}
ErrCode = table.enum(ErrCode, -1)

--aes key类型
AES = {
    Key128 = 128,
    Key192 = 192,
    Key256 = 256,
}

--rsa key长度
RSA = {
    Key512 = 512,
    Key1024 = 1024,
}

--des 类型
DES = {
    "Des",
    "D2Des",
    "D3Des",
}
DES = table.enum(DES, 0)
--des标明加密还是解密
DESMode = {
    "Encode",
    "Decode",
}
DESMode = table.enum(DESMode, 0)

--连接类型
SockType = {
	"CMD",
	"RPC",
	"HTTP",
	"WS",
	"MQTT",
	"WSMQTT",
	--以上与C++对应 保留的连接类型
	
	
}
SockType = table.enum(SockType, 0)

--事件 与c++中的对应
Event = {
	"NET_ACCEPT",--接收到新连接
	"NET_LINKED",--连接成功
	"NET_CLOSE",--连接关闭
	
	"NET_READ",--有新消息
	
	"NET_CMD",--命令
	
	"NET_RPCCALL",--网络rpc调用
	"NET_RPCRTN",--网络rpc返回
	
	"TASK_RPCCALL",--任务间rpc调用
	"TASK_RPCRTN",--任务间rpc返回
	
	"TIME_FRAME",--帧事件
	"TIME_SEC",--秒事件
	
	"TASK_INIT",--任务初始化
	"TASK_DEL",--任务删除
	
	"MQTT_CONNECT",     --客户端请求连接服务端
    "MQTT_PUBLISH",     --发布消息
    "MQTT_PUBACK",      --QoS 1 消息发布收到确认
    "MQTT_PUBREC",      --发布收到（保证交付第一步）
    "MQTT_PUBREL",      --两个方向都允许 发布释放（保证交付第二步）
    "MQTT_PUBCOMP",     --QoS 2 消息发布完成（保证交互第三步）
    "MQTT_SUBSCRIBE",   --订阅请求
    "MQTT_UNSUBSCRIBE", --取消订阅
    "MQTT_PINGREQ",     --心跳
    "MQTT_DISCONNECT",  --客户端断开连接
}
Event = table.enum(Event, 0)
