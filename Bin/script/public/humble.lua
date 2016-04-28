--[[
C++ȫ�ֺ���
--]]

require("macros")
local serialize = require("serialize")
local humble = {}
local string = string
local pWorkerMgr = g_pWorkerMgr
local pSender = g_pSender
local pEmail = g_pEmail
local channam = channam

--����
function humble.closesock(sock, uisession)
    local pclosechan = humble.getchan(channam.closesock)
    if not pclosechan then
        return
    end
    
    pclosechan:Send(serialize.pack({sock, uisession}))
end
function humble.send(sock, uisession, strbuf)
    pSender:Send(sock, uisession, strbuf, string.len(strbuf))
end
--tsock: {{sock,session},...}
function humble.broadcast(tsock, strbuf)
    pSender:broadCast(tsock, strbuf, string.len(strbuf))
end

--�ʼ�
function humble.sendemail(stremail)
    pEmail:sendMail(stremail)
end

--chanע��
function humble.regsendchan(strchannam, strtasknam, uicount)
    return pWorkerMgr:regSendChan(strchannam, strtasknam, uicount)
end
function humble.regrecvchan(strchannam, strtasknam, uicount)
    return pWorkerMgr:regRecvChan(strchannam, strtasknam, uicount)
end
function humble.getchan(strchannam)
    return pWorkerMgr:getChan(strchannam)
end

return humble
