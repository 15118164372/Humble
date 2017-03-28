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
	"CMD", --命令
	"CallRPC",--服务期间请求执行rpc
	"RPCRtn", --服务期间rpc返回
	"TaskCallRPC",--任务间请求执行rpc
	"TaskRPCRtn", --任务间rpc返回
    "Frame",--每帧
    "Second_1",--延时1秒
	
	--自定义
}
EnevtType = table.enum(EnevtType, 0)

--socket类型
SockType = {
	"CMD", --命令
	"RPC",--rpc
	"RPCCLIENT",--rpc clinet
	"MONGO",--mongodb
	
	--自定义
}
SockType = table.enum(SockType, 0)
