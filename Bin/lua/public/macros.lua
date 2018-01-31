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

--soekt类型
SockType = {
    "Debug",
    "RPC",
	"Httpd",
	"WS",
	--以下自定义的
}
SockType = table.enum(SockType, 0)
