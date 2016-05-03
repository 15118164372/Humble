--[[
�궨��
--]]

require("utile")
local table = table

--��ЧID
Invalid_ID = -1
Invalid_Sock = -1

ErrCode = {
    "faile",
    "ok",
    "error",
}
ErrCode = table.enum(ErrCode, -1)

--�ı���־����
LogLV = {
    "Err",
    "Warn",
    "Info",
    "Debug",
}
LogLV = table.enum(LogLV, 1)

ChanNam = {
    CloseSock = "closesockchan",
    Timer = "timerchan",
}

SockType = {
    "Tcp",
}
SockType = table.enum(SockType, 1)
