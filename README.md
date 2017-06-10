# Humble

Humble是c++开发的多线程服务器框架,网络底层使用libevent，业务层绑定到lua。       
服务器间通过RPC通信，任务间也是通过RPC通信。      

## 编译Humble     
* windows使用vs2015；  
* linux使用mklib.sh编译依赖库，然后再mk.sh。  

## 配置文件  
* config.ini 文件配置服务器启动参数。   

## 数据解析器    
* 数据解析器继承CParser，将parsePack、Response函数完成，    
然后在Humble.cpp中的setParam函数中添加该解析器。     

## 简单使用(http回显为例)   
* 1、新建echo.lua文件，复制粘贴template.lua中的内容，并增加echo函数：  
......       
function initTask()--服务初始化      
end       
function destroyTask()--服务释放   
end     
--注册echo事件(http://localhost/echo)      
local function echo(sock, sockType, httpInfo)      
	httpd.Response(sock, 200, "echo return.")      
end      
regProto("/echo", echo)      

* 2、建立监听，注册服务(start.lua):     
humble.httpServer("0.0.0.0", 80)--建立监听                     
humble.regTask("echo.lua", "echo", 1024 * 10)--注册echo服务      

* 3、浏览器中输入访问地址，查看结果         

## 命令使用    
* 1、进入命令模式 Humble -d 15000    
   
* 2、执行lua代码   
格式：       
do + 任务名  --回车后进入lua代码输入模式        
...          --可以输入多行，以回车结束一行     
...    
done   
如：  	
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

## 联系我    
279133271@qq.com    
