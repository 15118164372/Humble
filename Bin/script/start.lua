--[[
�������
--]]

require("macros")
require("logicmacro")
local humble = require("humble")
local utile = require("utile")
local cjson = require("cjson")
local tcp3 = require("tcp3")
local table = table
local pairs = pairs
local EnevtType = EnevtType
local SockType = SockType
local pBuffer = g_pBuffer

--����Ϊ����
local httpd = require("httpd")
--��������chan��Ӧ
if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--��ʼ��  ����ע������
function onStart()
	--�����������Ҫ������
	humble.setParser(SockType.CMD, "tcp3")
	humble.setParser(SockType.RPC, "tcp3")
	
	--�������
	humble.addListener(SockType.CMD, "127.0.0.1", 15100)	
	--rpc
	humble.addListener(SockType.RPC, "0.0.0.0", 15200)	
	--���ӹ���
	humble.regTask("task_link", 1024 * 10)
    tChan.task_link = humble.getChan("task_link")
	
	--TODO
	--����Ϊ����
	humble.addTcpLink(SockType.RPC, "127.0.0.1", 15200)
	humble.addListener(11, "0.0.0.0", 80)
	humble.setParser(11, "http")
	humble.regTask("echo1", 1024 * 10)
	humble.regTask("echo2", 1024 * 10)
    tChan.echo1 = humble.getChan("echo1")
	tChan.echo2 = humble.getChan("echo2")
end

--�˳�
function onStop()
	
end

--accept��������
function onTcpAccept(sock, uiSession, usSockType)
	--utile.chanSend(tChan.xxx, utile.Pack(EnevtType.NetAccept, nil, sock, uiSession, usSockType))
end

--���ӳɹ�
function onTcpLinked(sock, uiSession, usSockType)
	utile.chanSend(tChan.task_link, utile.Pack(EnevtType.NetLinked, nil, sock, uiSession, usSockType))
end

--���ӶϿ�
function onTcpClose(sock, uiSession, usSockType)
	utile.chanSend(tChan.task_link, utile.Pack(EnevtType.NetClose, nil, sock, uiSession, usSockType))
end

local function dispCMD(sock, uiSession)
	local strCmd = pBuffer:getString()
	local strTask = pBuffer:getString()
	local strMsg = pBuffer:getString()
	local objChan
	
	if "hotfix" == strCmd and "all" == strTask then
		for _, objChan in pairs(tChan) do
			utile.chanSend(objChan, utile.Pack(EnevtType.CMD, strCmd, -1, 0, strMsg))
		end
		
		humble.sendB(sock, uiSession, 
			tcp3.Response(cjson.encode({"ok", ""})))
		return	
	end
	
	objChan = tChan[strTask]
	if not objChan then
		humble.sendB(sock, uiSession, 
			tcp3.Response(cjson.encode({"fail", string.format("not find task %s.", strTask)})))
		return
	end
		
	utile.chanSend(objChan, utile.Pack(EnevtType.CMD, strCmd, sock, uiSession, strMsg))
end

local function dispRPC(sock, uiSession)
	local tRPC = cjson.decode(pBuffer:getByte(pBuffer:getSurpLens()))
	if tRPC.Enevt == EnevtType.CallRPC then --����
		humble.netToTask(tChan, tRPC.Method, utile.Pack(tRPC.Enevt, tRPC.Method, sock, uiSession, tRPC))
	else --���÷���
		local objChan = tChan[tRPC.RecvTask]
		if objChan then
			utile.chanSend(objChan, utile.Pack(tRPC.Enevt, nil, sock, uiSession, tRPC))
		else
			utile.Log(LogLV.Err, "not find task %s", tRPC.RecvTask)
		end
	end
end

--tcp����һ�����İ�
function onTcpRead(sock, uiSession, usSockType)	
	if SockType.CMD == usSockType then --����
		dispCMD(sock, uiSession)
	elseif SockType.RPC == usSockType then --��������RPC
		dispRPC(sock, uiSession)
	else
		--����Ϊ����
		local strName = humble.getParserNam(usSockType)
		if 0 == #strName then
			return
		end
		if "http" == strName then
			local buffer = httpd.parsePack(pBuffer)
			--������Ϣ����Ӧ����
			humble.netToTask(tChan, buffer.url, utile.Pack(EnevtType.TcpRead, buffer.url, sock, uiSession, buffer))
		end
	end	
end

--udp�ɶ�
function onUdpRead(sock, strHost, usPort)
	--TODO
end
