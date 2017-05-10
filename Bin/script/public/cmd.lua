--[[
√¸¡Ó 
--]]

require("macros")
local EnevtDisp = require("enevtdisp")
local utile = require("utile")
local humble = require("humble")
local hotfix = require("hotfix")
local cjson = require("cjson")
local tcp3 = require("tcp3")
local table = table
local load = load

local objEnevtDisp = EnevtDisp:new()

local function hotFix(uiSock, uiSession, strMsg)
	local bRtn, rtnMsg = utile.callFunc(hotfix.hotfix_module, strMsg)
	if bRtn then
		humble.sendB(uiSock, uiSession, 
			tcp3.Response(cjson.encode({"ok", rtnMsg})))
	else
		humble.sendB(uiSock, uiSession, 
			tcp3.Response(cjson.encode({"fail", rtnMsg})))
	end	
end
objEnevtDisp:regEvent("hotfix", hotFix)

local function doString(uiSock, uiSession, strMsg)
	local Func, strMsg = load(strMsg)
    if Func then
        local bRtn, rtnMsg = utile.callFunc(Func)
        if bRtn then
			humble.sendB(uiSock, uiSession, 
				tcp3.Response(cjson.encode({"ok", rtnMsg})))
        else
			humble.sendB(uiSock, uiSession, 
				tcp3.Response(cjson.encode({"fail", rtnMsg})))
        end
		
		return
    end
    
	humble.sendB(uiSock, uiSession, 
		tcp3.Response(cjson.encode({"fail", "load string error."})))
end
objEnevtDisp:regEvent("do", doString)

function doCmd(strCmd, uiSock, uiSession, strMsg)
	objEnevtDisp:onEvent(strCmd, uiSock, uiSession, strMsg)
end
