--ϵͳ��ʼ��  �����������ִ��һ��

require("macros")
local humble = require("humble")
local SockType = SockType

--����ע��
humble.regTask("rpclink.lua", "rpclink", 1024)

--TODO ���Լ���
humble.regTask("echo2.lua", "echo2", 1024 * 10)
humble.regTask("echo1.lua", "echo1", 1024 * 10)
humble.regTask("test.lua", "test", 1024 * 10)
humble.regTask("testmqtt.lua", "testmqtt", 1024 * 10)
--humble.unregTask("echo1")

--�������
--CMD����
humble.cmdServer(15000)
--rpc���� �����ڼ�����
humble.rpcServer("0.0.0.0", 15001)
--http
humble.httpServer("0.0.0.0", 80)

--TODO ���Լ���
humble.addListener("tcp2", 11, "0.0.0.0", 16000)
humble.wsServer("0.0.0.0", 16100)
humble.mqttServer("0.0.0.0", 1883)
humble.wsMQTTServer("0.0.0.0", 16200)
