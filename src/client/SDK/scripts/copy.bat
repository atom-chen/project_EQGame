::��EQSDK����Ҫ�Ŀ⿽������������

::�������û�������, �Ⱥź��治Ҫ�пո�
@set __boost__=D:\EQSDK\boost_1_40_0
@set __ogre__=D:\EQSDK\ogre_1_6_4
@set __cegui__=D:\EQSDK\CEGUI-0.6.2


::====================================
@echo off
if not exist ..\include md ..\include
if not exist ..\lib md ..\lib



echo "copy SDK Now!"

::Ҫִ�е�Ŀ¼
call _copyBoost.bat
call _copyOgre.bat
call _copyCEGUI.bat

::������̬��
call _copyDllToPrj.bat

echo "================"
echo "copy SDK OK!"
pause
