--ϵͳ��ʼ��  �����������ִ��һ��

require("macros")
local humble = require("humble")

--����ע��
humble.regTask("humble/rpclink.lua", "rpclink", 1024)

--TODO ���Լ���
humble.regTask("test/echo2.lua", "echo2", 1024 * 10)
humble.regTask("test/echo1.lua", "echo1", 1024 * 10)
humble.regTask("test/test.lua", "test", 1024 * 10)
humble.regTask("test/testmqtt.lua", "testmqtt", 1024 * 10)
--humble.unregTask("echo1")

--�������
--CMD����
humble.cmdServer(15000)
--rpc���� �����ڼ�����
humble.rpcServer("0.0.0.0", 15001)
--http
humble.httpServer("0.0.0.0", 80)

--TODO ���Լ���
humble.wsServer("0.0.0.0", 16100)
humble.mqttServer("0.0.0.0", 1883)
humble.wsMQTTServer("0.0.0.0", 16200)
humble.addListener("tcp2", 12, "0.0.0.0", 16000)
