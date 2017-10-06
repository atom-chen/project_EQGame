::把EQSDK所需要的库拷贝到工程里面

::请先设置环境变量, 等号后面不要有空格
@set __boost__=D:\EQSDK\boost_1_40_0
@set __ogre__=D:\EQSDK\ogre_1_6_4
@set __cegui__=D:\EQSDK\CEGUI-0.6.2


::====================================
@echo off
if not exist ..\include md ..\include
if not exist ..\lib md ..\lib



echo "copy SDK Now!"

::要执行的目录
call _copyBoost.bat
call _copyOgre.bat
call _copyCEGUI.bat

::拷贝动态库
call _copyDllToPrj.bat

echo "================"
echo "copy SDK OK!"
pause
