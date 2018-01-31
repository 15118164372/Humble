--ȫ�ֶ���
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

--��ЧID
Invalid_ID = -1
--��Чsocket
Invalid_Sock = -1

--������
ErrCode = {
    "Fail",
    "Ok",
    "Error",
}
ErrCode = table.enum(ErrCode, -1)

--aes key����
AES = {
    Key128 = 128,
    Key192 = 192,
    Key256 = 256,
}

--rsa key����
RSA = {
    Key512 = 512,
    Key1024 = 1024,
}

--des ����
DES = {
    "Des",
    "D2Des",
    "D3Des",
}
DES = table.enum(DES, 0)
--des�������ܻ��ǽ���
DESMode = {
    "Encode",
    "Decode",
}
DESMode = table.enum(DESMode, 0)

--soekt����
SockType = {
    "Debug",
    "RPC",
	"Httpd",
	"WS",
	--�����Զ����
}
SockType = table.enum(SockType, 0)
