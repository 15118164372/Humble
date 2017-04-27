--[[
utile
--]]

require("macros")
local http = require("http")
local ltn12 = require "ltn12"
local cjson = require("cjson")
local serialize = require("serialize")
local type = type
local table = table
local string = string
local xpcall = xpcall
local assert = assert
local Sleep = Sleep
local H_LOG = H_LOG
local CRC16 = CRC16
local CRC32 = CRC32
local urlEncode = urlEncode
local urlDecode = urlDecode
local zlibEncode = zlibEncode
local zlibDecode = zlibDecode
local b64Encode = b64Encode
local b64Decode = b64Decode
local md5Str = md5Str
local md5File = md5File
local LogLV = LogLV
local bDebug = true

local utile = {}

function utile.Sleep(uims)
    Sleep(uims)
end

local days = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
function utile.dayInMonth(year, mon)
	assert(mon >= 1 and mon <= 12)
	local day = 0
	if 2 == mon then
		if (0 == year %4 and 0 ~= year % 100) or 0 == year % 400 then
			day = 29
		else
			day = 28
		end
	else
		day = days[mon]
	end
	
	return day
end

function utile.Debug(fmt, ...)
    if bDebug then
        local strMsg = string.format(fmt, table.unpack({...}))
        print(string.format("[%s][LDEBUG] %s", os.date("%Y-%m-%d %H:%M:%S"), strMsg))
    end
end

function utile.Log(loglv, fmt, ...)
    local strmsg = string.format(fmt, table.unpack({...}))
    H_LOG(loglv, strmsg)
end

function utile.CRC16(strval)
    return CRC16(strval, string.len(strval))
end

function utile.CRC32(strval)
    return CRC32(strval, string.len(strval))
end

function utile.urlEncode(strval)
    return urlEncode(strval, string.len(strval))
end

function utile.urlDecode(strval)
    return urlDecode(strval, string.len(strval))
end

function utile.zlibEncode(strval)
    return zlibEncode(strval, string.len(strval))
end

function utile.zlibDecode(strval)
    return zlibDecode(strval, string.len(strval))
end

function utile.b64Encode(strval)
    return b64Encode(strval, string.len(strval))
end

function utile.b64Decode(strval)
    return b64Decode(strval, string.len(strval))
end

function utile.md5Str(strval)
    return md5Str(strval, string.len(strval))
end

function utile.md5File(strFile)
    return md5File(strFile)
end

--与 unPack 成对出现，否则内存泄露
function utile.Pack(evType, proto, ...)
    return serialize.pack({evType, proto, {...}})
end

function utile.freePack(pVal)
    return serialize.freepack(pVal)
end

function utile.unPack(pVal)
	if not pVal then
		return nil
	end
	
    return table.unpack(serialize.unpack(pVal))
end

function utile.chanSend(objChan, pVal)
	if not objChan:Send(pVal) then
		utile.freePack(pVal)
		return false
	end
	
	return true
end

function utile.callFunc(Func, ...)
    assert("function" == type(Func))

    local function onExcept(strMsg)
        local strStack = debug.trace(3, true, 2)
        utile.Log(LogLV.Err, "%s", strMsg)
        utile.Log(LogLV.Err, "%s", strStack)
    end
    
    return xpcall(Func, onExcept, table.unpack({...}))
end

function utile.http_get(url)
	local response_body = {}  
    local res, code = http.request{  
        url = url,  
        method = "GET",
        sink = ltn12.sink.table(response_body)  
    }
	
    res = table.concat(response_body) 
	response_body = nil
  
    return res,code 
end

--param table
function utile.http_post(url, param)
	assert("table" == type(param))
	local postParam = cjson.encode(param)
	
	local response_body = {}  
    local res, code = http.request{  
        url = url,  
        method = "POST",  
        headers =  
        {
            ["Content-Type"] = "application/json",  
            ["Content-Length"] = #postParam,  
        },  
        source = ltn12.source.string(postParam),  
        sink = ltn12.sink.table(response_body)  
    }
	
    res = table.concat(response_body) 
	response_body = nil
  
    return res,code 
end

function string.split(str, delimiter)
    assert("string" == type(str))
    assert("string" == type(delimiter))
    
    if ('' == delimiter) then 
        return {str} 
    end
    
    local pos,arr = 0, {}
    for st,sp in function() return string.find(str, delimiter, pos, true) end do
        table.insert(arr, string.sub(str, pos, st - 1))
        pos = sp + 1
    end
    
    table.insert(arr, string.sub(str, pos))
    
    return arr
end

function string.trim(str) 
    return (string.gsub(str, "^%s*(.-)%s*$", "%1"))
end 

function table.print(lua_table, indent)    
    assert("table" == type(lua_table))    
    indent = indent or 0
    for k, v in pairs(lua_table) do
        if type(k) == "string" then
            k = string.format("%q", k)
        end
        
        local szSuffix = ""
        if type(v) == "table" then
            szSuffix = "{"
        end
        
        local szPrefix = string.rep("    ", indent)
        formatting = szPrefix.."["..k.."]".." = "..szSuffix
        
        if type(v) == "table" then
            print(formatting)
            table.print(v, indent + 1)
            print(szPrefix.."},")
        else
            local szValue = ""
            if type(v) == "string" then
                szValue = string.format("%q", v)
            else
                szValue = tostring(v)
            end
            
            print(formatting..szValue..",")
        end
    end
end

function table.empty(lua_table)
    assert("table" == type(lua_table))    
    for _, _ in pairs(lua_table) do
        return false
    end
    
    return true
end

function table.len(lua_table)
    assert("table" == type(lua_table))    
    local iCount = 0
    for _, _ in pairs(lua_table) do
        iCount = iCount + 1
    end
    
    return iCount
end

function table.copy(tTable)
    assert("table" == type(lua_table))    
    local tNewTab = {}  
    for i, v in pairs(tTable) do  
        local vtyp = type(v)
        
        if ("table" == vtyp) then  
            tNewTab[i] = table.copy(v)  
        elseif ("thread" == vtyp) then  
            tNewTab[i] = v  
        elseif ("userdata" == vtyp) then  
            tNewTab[i] = v  
        else  
            tNewTab[i] = v  
        end
    end  
    
    return tNewTab
end

--debug
local function getIndent(level)
    return string.rep("    ", level)
end

local function dump(obj, offset)
    local dumpObj
    offset = offset or 0

    local function quoteStr(str)
        str = string.gsub(str, "[%c\\\"]", {
            ["\t"] = "\\t",
            ["\r"] = "\\r",
            ["\n"] = "\\n",
            ["\""] = "\\\"",
            ["\\"] = "\\\\",
        })
        return '"' .. str .. '"'
    end
    local function wrapKey(val)
        if type(val) == "number" then
            return "[" .. val .. "]"
        elseif type(val) == "string" then
            return "[" .. quoteStr(val) .. "]"
        else
            return "[" .. tostring(val) .. "]"
        end
    end
    local function wrapVal(val, level)
        if type(val) == "table" then
            return dumpObj(val, level)
        elseif type(val) == "number" then
            return val
        elseif type(val) == "string" then
            return quoteStr(val)
        else
            return tostring(val)
        end
    end
    local isArray = function(arr)
        local count = 0 
        for k, v in pairs(arr) do
            count = count + 1 
        end 
        for i = 1, count do
            if arr[i] == nil then
                return false
            end 
        end 
        return true, count
    end

    dumpObj = function(obj, level)
        if type(obj) ~= "table" then
            return wrapVal(obj)
        end
        --level = level + 1
        local tokens = {}
        tokens[#tokens + 1] = "\n"..getIndent(level).."{"
        --tokens[#tokens + 1] = "{"
        local ret, count = isArray(obj)
        if ret then
            for i = 1, count do
                tokens[#tokens + 1] = getIndent(level + 1) .. wrapVal(obj[i], level + 1) .. ","
            end
        else
            for k, v in pairs(obj) do
                tokens[#tokens + 1] = getIndent(level + 1) .. wrapKey(k) .. " = " .. wrapVal(v, level + 1) .. ","
            end
        end
        tokens[#tokens + 1] = getIndent(level) .. "}"
        return table.concat(tokens, "\n")
    end
    return dumpObj(obj, offset)
end

function debug.var_export(obj, prt)
    if prt then
        print(dump(obj))
    else
        return dump(obj)
    end
end

function debug.var_dump(obj)
    print(dump(obj))
end

function debug.trace(depth, asStr, baseDepth)
    local sInfo = "stack traceback:\n"

    -- escape trace function stacks
    if not baseDepth then
        baseDepth = 3
    else
        baseDepth = baseDepth + 3
    end

    local iIndent = 0
    
    local function doTrace(curLevel, baseLevel)
        local stack = debug.getinfo(curLevel)
        if not stack then 
            return false 
        end

        local label = curLevel - baseLevel + 1

        if stack.what == "C" then
            sInfo = sInfo..string.format("%d.[C FUNCTION]\n",label)
        else
            sInfo = sInfo..string.format("[%s]:%s:%s:%s:\n",
                label, stack.short_src, stack.currentline, 
                stack.name or "")
        end

        local i = 1
        while true do
            local paramName, paramVal = debug.getlocal(curLevel, i)
            if not paramName then 
                break 
            end
            sInfo = sInfo..getIndent(iIndent + 1)..string.format("%s = %s\n", 
                paramName, dump(paramVal, iIndent + 2))
            i = i + 1
        end

        return true
    end

    local i = baseDepth
    repeat
        local ret = doTrace(i, baseDepth)
        i = i + 1
        if depth and i >= depth + baseDepth then
            break
        end
    until ret ~= true

    sInfo = sInfo.."\n\n"

    if asStr then
        return sInfo
    else
        print(sInfo)
    end 
end

return utile
