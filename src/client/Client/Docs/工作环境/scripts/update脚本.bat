::请先设置环境变量, 等号后面不要有空格
set _SVN_PATH_=D:\Program Files\TortoiseSVN\bin


@path %_SVN_PATH_%;%PATH%
TortoiseProc.exe /command:update /path:"D:\EQ\Client" /closeonend:2
TortoiseProc.exe /command:update /path:"D:\EQ\SDK" /closeonend:2
TortoiseProc.exe /command:update /path:"D:\EQ\EQDocs" /closeonend:2
TortoiseProc.exe /command:update /path:"D:\EQ\Product" /closeonend:2
TortoiseProc.exe /command:update /path:"D:\EQ\Tools" /closeonend:2


