--��ϢЭ�� ��������
require("macros")
local table = table

--��������
SockType = {
	"CMD",
	"RPC",
	"HTTP"
	--������C++��Ӧ ��������������
	
	
}
SockType = table.enum(SockType, 0)

--������
Proto = {
	"CMD",
	"RPCCAL",
	"RPCRTN",
	"HTTP",
	--������C++��Ӧ �����Ĳ�����
	
	
}
Proto = table.enum(Proto, 0)
