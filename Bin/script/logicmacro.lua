--[[
�߼����Ͷ���
--]]

require("macros")

--�¼�����
EnevtType = {
	"NetAccept",--accept��������
	"NetLinked",--�������ӳɹ�
	"NetClose",--����Ͽ�
	"TcpRead",--tcp�ɶ�
	"UdpRead",--udp�ɶ�
	"CMD", --����
	"CallRPC",--�����ڼ�����ִ��rpc
	"RPCRtn", --�����ڼ�rpc����
	"TaskCallRPC",--���������ִ��rpc
	"TaskRPCRtn", --�����rpc����
    "Frame",--ÿ֡
    "Second_1",--��ʱ1��
	
	--�Զ���
}
EnevtType = table.enum(EnevtType, 0)

--socket����
SockType = {
	"CMD", --����
	"RPC",--rpc
	"HTTP",--http
	
	--�Զ���
}
SockType = table.enum(SockType, 0)
