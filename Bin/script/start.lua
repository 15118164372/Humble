--ϵͳ��ʼ��  �����������ִ��һ��

require("proto")
local humble = require("humble")
local SockType = SockType

--�������
--CMD����
humble.cmdServer(15000)
--rpc���� �����ڼ�����
humble.rpcServer("0.0.0.0", 15001)
--http
humble.httpServer("0.0.0.0", 80)

--TODO ���Լ���
humble.addListener("tcp1", 11, "0.0.0.0", 16000)


--����ע��
humble.regTask("rpclink.lua", "rpclink", 1024)

--TODO ���Լ���
humble.regTask("echo2.lua", "echo2", 1024 * 10)
humble.regTask("echo1.lua", "echo1", 1024 * 10)
--humble.unregTask("echo1")
