require("init")
local humble = require("humble")

function initTask()
	humble.addListener("default", 1, "0.0.0.0", 16000)
	
	humble.regTask("bench/bench1.lua", "bench1", 1024 * 10)
	humble.regTask("bench/bench2.lua", "bench2", 1024 * 10)
	humble.regTask("bench/bench3.lua", "bench3", 1024 * 10)
	humble.regTask("bench/bench4.lua", "bench4", 1024 * 10)
	humble.regTask("bench/bench5.lua", "bench5", 1024 * 10)
	humble.regTask("bench/bench6.lua", "bench6", 1024 * 10)
	humble.regTask("bench/bench7.lua", "bench7", 1024 * 10)
	humble.regTask("bench/bench8.lua", "bench8", 1024 * 10)
	humble.regTask("bench/bench9.lua", "bench9", 1024 * 10)
	humble.regTask("bench/bench10.lua", "bench10", 1024 * 10)
	humble.regTask("bench/bench11.lua", "bench11", 1024 * 10)
	humble.regTask("bench/bench12.lua", "bench12", 1024 * 10)
	humble.regTask("bench/bench13.lua", "bench13", 1024 * 10)
	humble.regTask("bench/bench14.lua", "bench14", 1024 * 10)
	humble.regTask("bench/bench15.lua", "bench15", 1024 * 10)
	humble.regTask("bench/bench16.lua", "bench16", 1024 * 10)
	humble.regTask("bench/bench17.lua", "bench17", 1024 * 10)
	humble.regTask("bench/bench18.lua", "bench18", 1024 * 10)
	humble.regTask("bench/bench19.lua", "bench19", 1024 * 10)
	humble.regTask("bench/bench20.lua", "bench20", 1024 * 10)
end

function destroyTask()
	
end
