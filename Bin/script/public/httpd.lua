--[[
http½âÎö
--]]

local type = type
local table = table
local string = string
local assert = assert
local tonumber = tonumber
local cjson = require("cjson")
local httpResponse = httpResponse

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

local ContentLength = "Content-Length"
local TransferEncoding = "Transfer-Encoding"
local LineFlag = "\r\n"

local function parseHead(netMsg)    
    local tHead = {}
    local strLine, strUrl, strMethod, strHttpver    
    local strName, strVal
    
	local pos = 0
	local strLine
	for st,sp in function() return string.find(netMsg, LineFlag, pos, true) end do
        strLine = string.sub(netMsg, pos, st - 1)
		pos = sp + 1
		if 0 == #strLine then
			break
		end
		
		if not strMethod then
            strMethod, strUrl, strHttpver = string.match(strLine, "^(%a+)%s+(.-)%s+HTTP/([%d%.]+)$")
            assert(strMethod and strUrl and strHttpver)
            assert("1.0" == strHttpver or "1.1" == strHttpver)
        else
            strName, strVal = string.match(strLine, "^(.-):%s*(.*)")
            tHead[strName] = strVal
        end
    end
    
    return string.lower(strMethod), strUrl, tHead, pos
end

local function parseChunked(netMsg, pos)    
    local tChunked = {}
    local strLine
    local iLens = 0
    for st,sp in function() return string.find(netMsg, LineFlag, pos, true) end do
        strLine = string.sub(netMsg, pos, st - 1)
		pos = sp + 1
		
        if 0 ~= #strLine then
            iLens = tonumber(strLine, 16)
            if 0 == iLens then
                break 
            end
			
            table.insert(tChunked, string.sub(netMsg, pos, pos + iLens - 1))
			pos = pos + iLens
        end
    end
    
    return table.concat(tChunked, "")
end

function httpd.parsePack(netMsg)	
    local tInfo = {}
    local strMethod, strUrl, tHead, pos = parseHead(netMsg)        
    if tHead[ContentLength] then
        local iLens = tonumber(tHead[ContentLength])
        tHead[ContentLength] = iLens            
        tInfo.method = strMethod
        tInfo.url = strUrl
        tInfo.head = tHead
        tInfo.info = string.sub(netMsg, pos, #netMsg)
    elseif (tHead[TransferEncoding] and "chunked" == tHead[TransferEncoding]) then
        local strChunked = parseChunked(netMsg, pos)            
        tInfo.method = strMethod
        tInfo.url = strUrl
        tInfo.head = tHead
        tInfo.info = strChunked
    else
        tInfo.method = strMethod
        tInfo.url = strUrl
        tInfo.head = tHead 
        tInfo.info = nil            
    end
    
    return tInfo 
end

function httpd.Response(sock, iCode, varBodyFunc, tHeader, ...)
    local tRtnMsg = {}
    
	local strMsg = string.format("HTTP/1.1 %03d %s\r\n", iCode, http_status_msg[iCode] or "")
	table.insert(tRtnMsg, strMsg)
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
	httpResponse(sock, strMsg, #strMsg)
end

return httpd
