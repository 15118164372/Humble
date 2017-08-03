# Humble

Humble是c++开发的多线程服务器框架,网络底层使用libevent. 业务层默认绑定到lua(也可用C++).      

## 编译Humble     
* windows使用vs2015;      
* linux执行mklib.sh编译依赖库然后再执行mk.sh.  

## 配置文件  
* config.ini 文件配置服务器启动参数.   

## 数据解析器    
* 数据解析器继承于CParser,主要负责socket数据的解包、组包. 目前实现了一些常用协议如websocket等.     

## 简单使用(http回显为例)   
* 1、新建echo.lua文件并增加如下函数:      
......       
function initTask()--服务初始化      
end       
function destroyTask()--服务释放   
end     
--注册echo事件      
local function echo(sock, sockType, httpInfo)      
	httpd.Response(sock, 200, "echo return.")      
end      
regProto("/echo", echo)      

* 2、建立监听，注册服务(start.lua):     
humble.httpServer("0.0.0.0", 80)--建立监听                     
humble.regTask("echo.lua", "echo", 1024 * 10)--注册echo服务      

* 3、浏览器中输入访问地址 (http://localhost/echo) 查看结果         

## 命令使用    
* 1、进入命令模式 Humble -d 15000    
   
* 2、执行lua代码   
格式:       
do + 任务名  --回车后进入lua代码输入模式        
...          --可以输入多行,以回车结束一行     
...    
done   
如:  	
do echo1    
return collectgarbage("collect")    
done     

* 3、退出lua代码输入模式      
exit      

* 4、热更新  
格式：      
hotfix + 任务名(all 所有任务都执行) + lua文件名(无扩展名)   
如：  
hotfix echo1 global     

* 5、退出命令模式       
quit

更多功能请参考代码...    

## QQ群    
486602190    
