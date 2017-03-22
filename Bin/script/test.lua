
require("macros")
local humble = require("humble")
local utile = require("utile")
local table = table
local string = string
local pChan = g_pChan

--任务初始化
function initTask()

end

--有新任务执行
function runTask()
    local varRecv = pChan:Recv()
    local evType, _, param = utile.unPack(varRecv)
	table.print(param)
end

--任务销毁
function destroyTask()
    
end