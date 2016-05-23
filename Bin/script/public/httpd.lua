--[[
http����
--]]

local type = type
local table = table
local string = string
local assert = assert
local tonumber = tonumber

local httpd = {}
local pWBinary = CBinary()

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

local ContentLength = "content-length"
local TransferEncoding = "transfer-encoding"

local function parseHead(pBinary)    
    local tHead = {}
    local strLine, strUrl, strMethod, strHttpver    
    local strName, strVal
    
    while true do
        strLine = pBinary:readLine()        
        if 0 == string.len(strLine) then
            break
        end
        
        if not strMethod then
            strMethod, strUrl, strHttpver = string.match(strLine, "^(%a+)%s+(.-)%s+HTTP/([%d%.]+)$")
            assert(strMethod and strUrl and strHttpver)
            assert("1.0" == strHttpver or "1.1" == strHttpver)
        else
            strName, strVal = string.match(strLine, "^(.-):%s*(.*)")
            tHead[string.lower(strName)] = string.lower(strVal)
        end
    end
    
    return strMethod, strUrl, tHead
end

local function parseChunked(pBinary)    
    local tChunked = {}
    local strLine
    local iLens = 0
    while true do
        strLine = pBinary:readLine()
        if 0 ~= string.len(strLine) then
            iLens = tonumber(strLine, 16)
            if 0 == iLens then
                break 
            end
            
            table.insert(tChunked, pBinary:getByte(iLens))
        end
    end
    
    return table.concat(tChunked, "")
end

function httpd.parsePack(pBinary)
    local tInfo = {}
    local strMethod, strUrl, tHead = parseHead(pBinary)        
    if tHead[ContentLength] then
        local iLens = tonumber(tHead[ContentLength])
        tHead[ContentLength] = iLens            
        tInfo.method = strMethod
        tInfo.url = strUrl
        tInfo.head = tHead
        tInfo.info = pBinary:getByte(iLens)            
    elseif (tHead[TransferEncoding] and "chunked" == tHead[TransferEncoding]) then
        local strChunked = parseChunked(pBinary)            
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

function httpd.Response(iCode, varBodyFunc, tHeader)
    pWBinary:reSetWrite()
    
	local strMsg = string.format("HTTP/1.1 %03d %s\r\n", iCode, http_status_msg[iCode] or "")
    pWBinary:setByte(strMsg, #strMsg)
    if tHeader then
		for key, val in pairs(tHeader) do
            strMsg = string.format("%s: %s\r\n", key, val)
            pWBinary:setByte(strMsg, #strMsg)
		end
	end

	local iType = type(varBodyFunc)
	if iType == "string" then
		strMsg = string.format("content-length: %d\r\n\r\n", string.len(varBodyFunc))
        pWBinary:setByte(strMsg, #strMsg)
        pWBinary:setByte(varBodyFunc, #varBodyFunc)
	elseif iType == "function" then
		strMsg = "transfer-encoding: chunked\r\n"
        pWBinary:setByte(strMsg, #strMsg)
        local str
		while true do
			local str = varBodyFunc()
			if str then
				if str ~= "" then
					strMsg = string.format("\r\n%x\r\n", string.len(str))
                    pWBinary:setByte(strMsg, #strMsg)
                    pWBinary:setByte(str, #str)
				end
			else
				strMsg = "\r\n0\r\n\r\n"
                pWBinary:setByte(strMsg, #strMsg)
				break
			end
		end
	else
        strMsg = "\r\n"
        pWBinary:setByte(strMsg, #strMsg)
	end

    return pWBinary
end

return httpd
