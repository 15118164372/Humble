--系统初始化  程序启动完成执行一次

require("proto")
local humble = require("humble")
local SockType = SockType

--任务注册
humble.regTask("rpclink.lua", "rpclink", 1024)

--TODO 你自己的
humble.regTask("echo2.lua", "echo2", 1024 * 10)
humble.regTask("echo1.lua", "echo1", 1024 * 10)
humble.regTask("test.lua", "test", 10)
--humble.unregTask("echo1")

--监听添加
--CMD监听
humble.cmdServer(15000)
--rpc监听 服务期间连接
humble.rpcServer("0.0.0.0", 15001)
--http
humble.httpServer("0.0.0.0", 80)

--TODO 你自己的
humble.addListener("tcp2", 11, "0.0.0.0", 16000)
