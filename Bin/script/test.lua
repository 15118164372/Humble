local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local table = table
local string = string
local ChanNam = ChanNam

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function initTask()
    tChan.timer = humble.regRecvChan("testtimer", "test")
end

function runTask()    
    local varRecv = tChan.timer:Recv()
    if varRecv then
        local itick, icount = table.unpack(serialize.unpack(varRecv))
        print(string.format("timer task tick %d count %d", itick, icount))
    end
end

function destroyTask()
    
end