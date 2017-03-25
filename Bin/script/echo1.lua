--以下为测试
require("macros")
require("logicmacro")
require("cmd")
local utile = require("utile")
local humble = require("humble")
local httpd = require("httpd")
local websock = require("websock")
local mqtt = require("mqtt")
local def = require("def")
local tcp1 = require("tcp1")
local tcp2 = require("tcp2")
local netdisp = require("netdisp")
local table = table
local string = string
local pChan = g_pChan

if not g_pBinary then
    g_pBinary = CBinary()
end
local pBinary = g_pBinary

if not g_NetDisp then
	g_NetDisp = netdisp:new("echo1")
end

--任务初始化
function initTask()
    
end

local function readFile(file)
	return file:read(200) 
end

local function echo(sock, session, pack)
	local tmsg = {}
	tmsg.rtn = 0
	tmsg.msg = "echo1 json return"
	--file = io.open("D:/soft/nginx-1.10.1.zip", "rb")
	local pWBinary = httpd.Response(200, tmsg)
    humble.sendB(sock, session, pWBinary)
	--file:close()
end
g_NetDisp:regNetEvent("/echo1", echo)

--有新任务执行
function runTask()
    local varRecv = pChan:Recv()
	if varRecv then
		local evType, proto, msg = utile.unPack(varRecv)
		if evType == EnevtType.CMD then
			local sock,session,pack = table.unpack(msg)
			doCmd(proto, sock, session, pack) 
		end		
		
		if evType == EnevtType.TcpRead then
			local sock,session,pack = table.unpack(msg)
			g_NetDisp:onNetEvent(proto, sock, session, pack)
		end			
	end
end

--任务销毁
function destroyTask()
	
end
