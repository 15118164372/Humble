require("init")
require("macros")
local humble = require("humble")

function initTask()
	
end

function destroyTask()
	
end

local function test_cfuncs()
	print("--------------test c++º¯Êý--------------")
	print("--------------CConHash--------------")
    local pConHash = CConHash()
    for i=1, 10 do
        pConHash:addNode(tostring(i), 3)
    end    
    print(pConHash:findNode("1asdas45"))
    print(pConHash:findNode("testnode"))
    
    print("--------------CSnowFlake--------------")
    local pSnowFlake = CSnowFlake()
    pSnowFlake:setWorkId(1)
    pSnowFlake:setCenterId(2)
    for i=1, 3 do
        print(pSnowFlake:getId())
    end
    
    print("--------------CClock--------------")
    local pClock = CClock()
    H_Sleep(1000)
    print(pClock:Elapsed())
    
    print("--------------CUtils--------------")
    print("nowStrMilSecond:" .. CUtils.nowStrMilSecond())
    print("nowMilSecond:" .. CUtils.nowMilSecond())
    
    print("--------------CAES--------------")
    local pAES = CAES()
    pAES:setKey("123456", AES.Key256)
    local strMsg = "AASssssDSÈö·¢Éú@"
    print("old string:"..strMsg)
    local strEncode = pAES:Encode(strMsg, #strMsg)
    print("Encode:"..strEncode)
    print("Decode:"..pAES:Decode(strEncode, #strEncode))
    
    print("--------------CEnUtils--------------")
    strEncode = CEnUtils.zEncode(strMsg, #strMsg)
    print("zEncode:"..strEncode)
    print("zDecode:"..CEnUtils.zDecode(strEncode, #strEncode))
    
    strEncode = CEnUtils.b64Encode(strMsg, #strMsg)
    print("b64Encode:"..strEncode)
    print("b64Decode:"..CEnUtils.b64Decode(strEncode, #strEncode))
    
    print("md5Str:"..CEnUtils.md5Str(strMsg, #strMsg))    
end
regDelayEv(7000, test_cfuncs)