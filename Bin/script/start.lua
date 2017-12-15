--系统初始化  程序启动完成执行一次

require("macros")
local humble = require("humble")

--任务注册
humble.regTask("humble/rpclink.lua", "rpclink", 1024)

--TODO 你自己的
humble.regTask("test/echo2.lua", "echo2", 1024 * 10)
humble.regTask("test/echo1.lua", "echo1", 1024 * 10)
humble.regTask("test/test.lua", "test", 1024 * 10)
humble.regTask("test/testmqtt.lua", "testmqtt", 1024 * 10)
--humble.unregTask("echo1")

--监听添加
--CMD监听
humble.cmdServer(15000)
--rpc监听 服务期间连接
humble.rpcServer("0.0.0.0", 15001)
--http
humble.httpServer("0.0.0.0", 80)

--TODO 你自己的
humble.wsServer("0.0.0.0", 16100)
humble.mqttServer("0.0.0.0", 1883)
humble.wsMQTTServer("0.0.0.0", 16200)
humble.addListener("tcp2", 12, "0.0.0.0", 16000)
