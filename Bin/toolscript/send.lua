
local msgtype = require("msgtype")
local humble = require("humble")
local utile = require("utile")

function onSend(strParser, strComm)
    local pChan = humble.getChan("tool")
	utile.chanSend(pChan, utile.Pack(msgtype.send, nil, strParser, strComm))
end
