::�������û�������, �Ⱥź��治Ҫ�пո�
set _SVN_PATH_=D:\Program Files\TortoiseSVN\bin


@path %_SVN_PATH_%;%PATH%
TortoiseProc.exe /command:checkout /path:"D:\EQ\Client"		/url:"http://10.35.12.156/svn/EQCode/Client" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\SDK"		/url:"http://10.35.12.156/svn/EQCode/SDK" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\EQDocs"		/url:"http://10.35.12.156/svn/EQCode/EQDocs" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\Product"	/url:"http://10.35.12.156/svn/EQProduct/Product" /closeonend:0
TortoiseProc.exe /command:checkout /path:"D:\EQ\Tools"		/url:"http://10.35.12.156/svn/EQProduct/Tools" /closeonend:0


::�����Ҫ���Ե�������(����Ogre,Boost��)�Ĳ���Ҫ���µĲ���.
::������Ķ�������, ����Ҫȫ������
::TortoiseProc.exe /command:checkout /path:"D:\EQSDK"		/url:"http://10.35.12.156/svn/EQSDK" /closeonend:0

::�������Ĵ���
::TortoiseProc.exe /command:checkout /path:"D:\EQ\server"		/url:"http://10.35.12.156/svn/EQCode/server" /closeonend:0


pause