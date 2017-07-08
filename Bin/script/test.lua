--²âÊÔ
require("init")
local aoi_crosslist = require("aoi_crosslist")
local humble = require("humble")

function initTask()
	
end

function destroyTask()
	
end

local function TestEncrypt()
	local msg = "²âÊÔEncrypt*&*^^1"
	local enMsg = humble.zlibEncode(msg)
	local deMsg = humble.zlibDecode(enMsg)
	assert(msg == deMsg)
	
	enMsg = humble.b64Encode(msg)
	deMsg = humble.b64Decode(enMsg)
	assert(msg == deMsg)
	
	enMsg = humble.md5Str("admin")
	assert("21232f297a57a5a743894a0e4a801fc3" == enMsg)
	
    for _, val in pairs(AES) do
        local aes = CAES()
        aes:setKey("lqf1101*", val)
        enMsg = aes:Encode(msg, #msg)
        deMsg = aes:Decode(enMsg, #enMsg)
        assert(msg == deMsg)
    end
	
    for _, val in pairs(RSA) do
        local rsaKey = CRSAKey()
        rsaKey:creatKey(val)
        local rsa = CRSA()
        rsa:setKey(rsaKey)
        enMsg = rsa:pubEncrypt(msg, #msg)
        deMsg = rsa:priDecrypt(enMsg, #enMsg)
        assert(msg == deMsg)
        enMsg = rsa:priEncrypt(msg, #msg)
        deMsg = rsa:pubDecrypt(enMsg, #enMsg)
        assert(msg == deMsg)
    end
    
    for _, val in pairs(DES) do
        local desEncode = CDESEncrypt()
        desEncode:setKey("lqf1101*", val, DESMode.Encode)
        enMsg = desEncode:Encrypt(msg, #msg) 
        
        local desDecode = CDESEncrypt()
        desDecode:setKey("lqf1101*", val, DESMode.Decode)
        deMsg = desDecode:Encrypt(enMsg, #enMsg)
        assert(msg == deMsg)
    end
end

local function TestMail()
	--163ÎªÀý
	local mail = humble.newMail()
	mail:setSMTPSV("smtp.163.com")
	mail:setAuthPlain()
	mail:setFromAddr("xxx@163.com")
	mail:setUserName("xxx@163.com")
	mail:setPSW("xxxxxxx")
	mail:addToAddr("xxxxxxx@qq.com")
	mail:setSubject("test subject")
	mail:setMsg("test message")
	mail:Send()
end

local function checkAOI(area, okarea)
	if #area ~= #okarea then
		return false
	end
	
	local bHave = false
	for _,val in pairs(area) do
		bHave = false
		for _, val2 in pairs(okarea) do
			if val == val2 then
				bHave = true
				break
			end
		end
		if not bHave then
			return false
		end
	end
	
	return true
end
local function TestAOI()
	local aoi = aoi_crosslist:new(10, 10)
	local inArea = aoi:Enter(1, 0, 0, 1, 1)
	assert(checkAOI(inArea, {1}))
	inArea = aoi:Enter(2, 0, 1, 1, 1)
	assert(checkAOI(inArea, {1, 2}))
	inArea = aoi:Enter(3, 1, 0, 1, 1)
	assert(checkAOI(inArea, {1, 2, 3}))
	inArea = aoi:Enter(4, 1, 1, 1, 1)
	assert(checkAOI(inArea, {1, 2, 3, 4}))
	
	inArea = aoi:Enter(5, 1, 2, 1, 1)
	assert(checkAOI(inArea, {5, 4, 2}))
	inArea = aoi:Enter(6, 2, 1, 1, 1)
	assert(checkAOI(inArea, {3, 4, 5, 6}))
	inArea = aoi:Enter(7, 0, 2, 1, 1)
	assert(checkAOI(inArea, {2, 4, 5, 7}))
	inArea = aoi:Enter(8, 2, 0, 1, 1)
	assert(checkAOI(inArea, {3, 4, 6, 8}))
	
	inArea = aoi:getAOIArea(8)
	assert(checkAOI(inArea, {3, 4, 6, 8}))
	inArea = aoi:getAOIArea(8, 2, 2)
	assert(checkAOI(inArea, {1, 2, 3, 4, 5, 6, 7, 8}))
	
	inArea = aoi:Leave(8)
	assert(checkAOI(inArea, {3, 4, 6, 8}))
	
	local inArea, outArea, newArea  = aoi:Move(1, 2, 0)
	assert(checkAOI(inArea, {3, 4, 6, 1}))
	assert(checkAOI(outArea, {2}))
	assert(checkAOI(newArea, {6}))	
	inArea, outArea, newArea  = aoi:Move(1, 2, 0)
	assert(nil == inArea)	
	inArea, outArea, newArea  = aoi:Move(1, 5, 2)
	assert(checkAOI(inArea, {1}))
	assert(checkAOI(outArea, {3, 4, 6}))
	assert(checkAOI(newArea, {}))
end

local function Test()
	print("---------------------test bagin-------------------------")
	TestAOI()
	TestMail()
	TestEncrypt()
	print("---------------------test end-------------------------")
end
regDelayEv(1, Test)
