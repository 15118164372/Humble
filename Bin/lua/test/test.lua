require("init")
local humble = require("humble")

function initTask()
	humble.debugServer("0.0.0.0", 16001)
	humble.rpcServer("0.0.0.0", 16002)
	humble.httpServer("0.0.0.0", 8080)
	humble.wsServer("0.0.0.0", 16100)
	
	humble.regTask("test/test1.lua", "test1", 1024 * 10)
	humble.regTask("test/test2.lua", "test2", 1024 * 10)
end

function destroyTask()
	
end
