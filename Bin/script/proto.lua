--消息协议 连接类型
require("macros")
local table = table

--连接类型
SockType = {
	"CMD",
	"RPC",
	"HTTP"
	--以上与C++对应 保留的连接类型
	
	
}
SockType = table.enum(SockType, 0)
