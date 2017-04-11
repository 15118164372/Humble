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

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--��ʼ��  ����ע������
function onStart()
	humble.setParser(SockType.CMD, "tcp3")
	humble.setParser(SockType.RPC, "tcp3")

	--�������
	humble.addListener(SockType.CMD, "127.0.0.1", 15100)	
	--rpc
	humble.addListener(SockType.RPC, "0.0.0.0", 15200)
	
	--���ӹ���
	humble.regTask("task_link")
    tChan.task_link = humble.getChan("task_link")
	
	--TODO
	--����Ϊ����
	humble.addTcpLink(SockType.RPC, "127.0.0.1", 15200)
	humble.addListener(11, "0.0.0.0", 80)
	humble.setParser(11, "http")
	humble.regTask("echo1")
	humble.regTask("echo2")
    tChan.echo1 = humble.getChan("echo1")
	tChan.echo2 = humble.getChan("echo2")
end

--�˳�
function onStop()
	
end

--accept��������
function onTcpAccept(sock, uiSession, usSockType)
	--tChan.xxx:Send(utile.Pack(EnevtType.NetAccept, nil, sock, uiSession, usSockType))
end

--���ӳɹ�
function onTcpLinked(sock, uiSession, usSockType)
    tChan.task_link:Send(utile.Pack(EnevtType.NetLinked, nil, sock, uiSession, usSockType))
end

--���ӶϿ�
function onTcpClose(sock, uiSession, usSockType)
    tChan.task_link:Send(utile.Pack(EnevtType.NetClose, nil, sock, uiSession, usSockType))
end

--tcp����һ�����İ�
function onTcpRead(sock, uiSession, usSockType)	
	if SockType.CMD == usSockType then --����
		local strCmd = pBuffer:getString()
		local strTask = pBuffer:getString()
		local strMsg = pBuffer:getString()
		local objChan = humble.getChan(strTask)
		if not objChan then
			humble.sendB(sock, uiSession, 
				tcp3.Response(cjson.encode({"fail", "not find task."})))
			return
		end
		
		local packMsg = utile.Pack(EnevtType.CMD, strCmd, sock, uiSession, strMsg)
		objChan:Send(packMsg)
	elseif SockType.RPC == usSockType then --��������RPC
		local tRPC = cjson.decode(pBuffer:getByte(pBuffer:getSurpLens()))
		if tRPC.Enevt == EnevtType.CallRPC then --����
			local param = utile.Pack(tRPC.Enevt, tRPC.Method, sock, uiSession, tRPC)
			if not humble.netToTask(tRPC.Method, param) then
				utile.unPack(param)--�ͷŵ�
			end
		else --���÷���
			local objChan = humble.getChan(tRPC.RecvTask)
			if objChan then
				local param = utile.Pack(tRPC.Enevt, nil, sock, uiSession, tRPC)
				objChan:Send(param)
			else
				utile.Log(LogLV.Err, "not find task %s", tRPC.RecvTask)
			end
		end
	else
		--����Ϊ����
		local strName = humble.getParserNam(usSockType)
		if 0 == #strName then
			return
		end
		if "http" == strName then
			local buffer = httpd.parsePack(pBuffer)
			local param = utile.Pack(EnevtType.TcpRead, buffer.url, sock, uiSession, buffer)
			--������Ϣ����Ӧ����
			if not humble.netToTask(buffer.url, param) then
				utile.unPack(param)--�ͷŵ�
			end
		end
	end	
end

--udp�ɶ�
function onUdpRead(sock, strHost, usPort)
	--TODO
end
