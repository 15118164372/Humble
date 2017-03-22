--[[
逻辑类型定义
--]]

require("macros")

--事件类型
EnevtType = {
	"NetLinked",--网络链接成功，或者accept到新链接
	"NetClose",--网络断开
	"TcpRead",--tcp可读
	"UdpRead",--udp可读
    "Frame",--每帧
	--自定义
    "Delay",--延时
}
EnevtType = table.enum(EnevtType, 0)
