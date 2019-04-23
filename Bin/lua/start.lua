--系统初始化  程序启动完成执行一次
local humble = require("humble")

humble.regTask("test/test.lua", "test", 1024)
