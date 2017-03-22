
local msgtype = require("msgtype")
local humble = require("humble")
local utile = require("utile")

function onSend(strParser, strComm)
    local pChan = humble.getChan("tool")
    pChan:Send(utile.Pack(msgtype.send, nil, strParser, strComm))
end
