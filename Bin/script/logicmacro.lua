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
    "Delay",--��ʱ
}
EnevtType = table.enum(EnevtType, 0)
