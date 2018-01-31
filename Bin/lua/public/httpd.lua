--[[
http½âÎö
--]]

local type = type
local table = table
local string = string
local assert = assert
local tonumber = tonumber
local cjson = require("cjson")
local humble = require("humble")

local httpd = {}

local http_status_msg = {
	[100] = "Continue",
	[101] = "Switching Protocols",
	[200] = "OK",
	[201] = "Created",
	[202] = "Accepted",
	[203] = "Non-Authoritative Information",
	[204] = "No Content",
	[205] = "Reset Content",
	[206] = "Partial Content",
	[300] = "Multiple Choices",
	[301] = "Moved Permanently",
	[302] = "Found",
	[303] = "See Other",
	[304] = "Not Modified",
	[305] = "Use Proxy",
	[307] = "Temporary Redirect",
	[400] = "Bad Request",
	[401] = "Unauthorized",
	[402] = "Payment Required",
	[403] = "Forbidden",
	[404] = "Not Found",
	[405] = "Method Not Allowed",
	[406] = "Not Acceptable",
	[407] = "Proxy Authentication Required",
	[408] = "Request Time-out",
	[409] = "Conflict",
	[410] = "Gone",
	[411] = "Length Required",
	[412] = "Precondition Failed",
	[413] = "Request Entity Too Large",
	[414] = "Request-URI Too Large",
	[415] = "Unsupported Media Type",
	[416] = "Requested range not satisfiable",
	[417] = "Expectation Failed",
	[500] = "Internal Server Error",
	[501] = "Not Implemented",
	[502] = "Bad Gateway",
	[503] = "Service Unavailable",
	[504] = "Gateway Time-out",
	[505] = "HTTP Version not supported",
}

local function httpMsg(sock, strHead, varBodyFunc, tHeader, ...)
    local tRtnMsg = {}    
	table.insert(tRtnMsg, strHead)
	
	local strMsg = ""
    if tHeader then
		for key, val in pairs(tHeader) do
            strMsg = string.format("%s: %s\r\n", key, val)
			table.insert(tRtnMsg, strMsg)
		end
	end

	local strType = type(varBodyFunc)
	if strType == "string" then
		strMsg = string.format("Content-Length: %d\r\n\r\n", #varBodyFunc)
		table.insert(tRtnMsg, strMsg)
		table.insert(tRtnMsg, varBodyFunc)
	elseif strType == "table" then
		local strJsonMsg = cjson.encode(varBodyFunc)
		strMsg = string.format("Content-Type: application/json\r\nContent-Length: %d\r\n\r\n", 
			#strJsonMsg)
		table.insert(tRtnMsg, strMsg)
		table.insert(tRtnMsg, strJsonMsg)
	elseif strType == "function" then
		strMsg = "Transfer-Encoding: chunked\r\n"
		table.insert(tRtnMsg, strMsg)
        local str
		while true do
			str = varBodyFunc(table.unpack({...}))
			if str then
				strMsg = string.format("\r\n%x\r\n", #str)
				table.insert(tRtnMsg, strMsg)
				table.insert(tRtnMsg, str)
			else
				strMsg = "\r\n0\r\n\r\n"
				table.insert(tRtnMsg, strMsg)
				break
			end
		end
	else
        strMsg = "\r\n"
		table.insert(tRtnMsg, strMsg)
	end

    strMsg = table.concat(tRtnMsg)
	humble.sendMsg(sock, strMsg)
end
function httpd.Get(sock, strUrl, varBodyFunc, tHeader, ...)
	local strHead = string.format("%s %s HTTP/1.1\r\n", "GET", strUrl)
	httpMsg(sock, strHead, varBodyFunc, tHeader, table.unpack({...}))
end
function httpd.Post(sock, strUrl, varBodyFunc, tHeader, ...)
	local strHead = string.format("%s %s HTTP/1.1\r\n", "POST", strUrl)
	httpMsg(sock, strHead, varBodyFunc, tHeader, table.unpack({...}))
end
function httpd.Response(sock, iCode, varBodyFunc, tHeader, ...)
	local strHead = string.format("HTTP/1.1 %03d %s\r\n", iCode, http_status_msg[iCode] or "")
	httpMsg(sock, strHead, varBodyFunc, tHeader, table.unpack({...}))
end

return httpd
