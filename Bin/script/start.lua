--[[
����������
--]]

require("macros")
require("logicmacro")
local humble = require("humble")
local utile = require("utile")
local httpd = require("httpd")
local websock = require("websock")
local mqtt = require("mqtt")
local table = table
local pairs = pairs
local pBuffer = g_pBuffer

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

if not g_tListener then
    g_tListener = {}
end
local tListener = g_tListener

if not g_tLinker then
    g_tLinker = {}
end
local tLinker = g_tLinker

--��ʼ��  ����ע������
function onStart()
    tListener.test = humble.addListener(1, "0.0.0.0", 15000)
    tListener.test2 = humble.addListener(2, "0.0.0.0", 15001)  
    humble.setParser(1, "http")
	humble.setParser(2, "tcp1")
	
    tListener.udp = humble.addUdp("0.0.0.0", 15001)
    
    humble.regTask("echo1")
	humble.regTask("echo2")
	humble.regTask("echo3")
    humble.regTask("test")      
    
    tChan.echo = humble.getChan("echo1")
end

--�˳�����Ҫ��������ӷ�ֹ������ֹͣʱ������Ϣ����
function onStop()
    humble.closeByType(1)
    humble.delListener(tListener.test)
    humble.delUdp(tListener.udp)
end

--���ӳɹ� �����������ӵ���
function onTcpLinked(sock, uiSession, usSockType)
    --tChan.echo:Send(utile.Pack(EnevtType.NetLinked, nil, sock, uiSession, usSockType))
end

--���ӶϿ�
function onTcpClose(sock, uiSession, usSockType)
    --tChan.echo:Send(utile.Pack(EnevtType.NetClose, nil, sock, uiSession, usSockType))
end

--tcp����һ�����İ�
function onTcpRead(sock, uiSession, usSockType)
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
	
	if "tcp1" == strName then
		local iProto = pBuffer:getUint16()
		local strMsg = pBuffer:getByte(pBuffer:getSurpLens())
		print(iProto)
		print(strMsg)
		local param = utile.Pack(EnevtType.TcpRead, iProto, sock, uiSession, strMsg)
		--������Ϣ����Ӧ����
		if not humble.netToTask(iProto, param) then
			utile.unPack(param)--�ͷŵ�
		end
	end
end

--udp�ɶ�
function onUdpRead(sock, pHost, usPort)
    local strBuf = pBuffer:getByte(pBuffer:getSurpLens())
    humble.sendU(sock, pHost, usPort, strBuf.."lua1")
    humble.broadCastU(sock, usPort, strBuf.."lua2")
end
