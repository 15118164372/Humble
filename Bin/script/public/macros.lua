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
Invalid_Sock = -1

ErrCode = {
    "Fail",
    "Ok",
    "Error",
}
ErrCode = table.enum(ErrCode, -1)

--事件 与c++中的对应
Event = {
	"Accept",
	"Linked",
	"Closed",
	
	"Read",
	
	"Cmd",
	
	"RPCCall",
	"RPCRtn",
	
	"TaskRPCCall",
	"TaskRPCRtn",
	
	"Frame",
	"Sec",
	
	"TaskInit",
	"TaskDel",
	
	"UserChan",
}
Event = table.enum(Event, 0)
