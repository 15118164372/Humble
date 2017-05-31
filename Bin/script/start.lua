--ϵͳ��ʼ��  �����������ִ��һ��

require("proto")
local humble = require("humble")
local SockType = SockType

--�������
--CMD���� humble.addListener(strParser, sockType, strHost, usPort)
humble.addListener("tcp2", SockType.CMD, "127.0.0.1", 16000)
--rpc���� 
humble.addListener("tcp2", SockType.RPC, "0.0.0.0", 16001)

humble.addListener("tcp1", 11, "0.0.0.0", 15000)
humble.addListener("http", SockType.HTTP, "0.0.0.0", 80)

humble.addListener("tcp2", 10, "0.0.0.0", 16002)

--����ע��
humble.regTask("rpclink.lua", "rpclink", 1024 * 5)
humble.regTask("echo2.lua", "echo2", 1024 * 10)
humble.regTask("echo1.lua", "echo1", 1024 * 10)
--humble.unregTask("echo1")
