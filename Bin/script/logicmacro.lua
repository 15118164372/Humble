--[[
�߼����Ͷ���
--]]

require("macros")

--�¼�����
EnevtType = {
	"NetLinked",--�������ӳɹ�������accept��������
	"NetClose",--����Ͽ�
	"TcpRead",--tcp�ɶ�
	"UdpRead",--udp�ɶ�
    "Frame",--ÿ֡
	--�Զ���
    "Delay",--��ʱ
}
EnevtType = table.enum(EnevtType, 0)
