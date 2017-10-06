::请先设置环境变量, 等号后面不要有空格
set _SVN_PATH_=D:\Program Files\TortoiseSVN\bin


@path %_SVN_PATH_%;%PATH%
TortoiseProc.exe /command:checkout /path:"D:\EQ\Client"		/url:"http://10.35.12.156/svn/EQCode/Client" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\SDK"		/url:"http://10.35.12.156/svn/EQCode/SDK" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\EQDocs"		/url:"http://10.35.12.156/svn/EQCode/EQDocs" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\Product"	/url:"http://10.35.12.156/svn/EQProduct/Product" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\Tools"		/url:"http://10.35.12.156/svn/EQProduct/Tools" /closeonend:0


::如果需要调试第三方库(包括Ogre,Boost等)的才需要如下的操作.
::这里面的东西不好, 不需要全部更新
::TortoiseProc.exe /command:checkout /path:"D:\EQSDK"		/url:"http://10.35.12.156/svn/EQSDK" /closeonend:0

::服务器的代码
::TortoiseProc.exe /command:checkout /path:"D:\EQ\server"		/url:"http://10.35.12.156/svn/EQCode/server" /closeonend:0


pause