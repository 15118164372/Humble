--常用方法
local humble = require("humble")
local assert = assert
local type = type
local table = table
local string = string
local tostring = tostring
local debug = debug
local xpcall = xpcall

local utile = {}

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

function utile.callFunc(Func, ...)
    assert("function" == type(Func))

    local function onExcept(strMsg)
        local strStack = debug.trace(3, true, 2)
        humble.Errorf("%s", strMsg)
        humble.Errorf("%s", strStack)
    end
    
    return xpcall(Func, onExcept, table.unpack({...}))
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

function table.tostring(obj)  
    return dump(obj)
end
function table.print(obj)
    print(table.tostring(obj))
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

return utile
