::ʹ�÷���: ������ű���EQStart.cfg��ProductĿ¼����ͬһ��Ŀ¼�����оͿ���
::������������EQStart.cfg�ű�
::��������Ҫ��Ҫ����Pdb, ���������Ľű��޸�



echo "ִ��EQ��԰���ýű�.."

::ɾ�����ڵ��ļ�
rd /s /q Product\Error
rd /s /q Product\Log
rd /s /q Product\BinDebug
rd /s /q Product\Screenshots
::�����Ҫɾ��pdb, ע�͵�
del Product\Bin\*.pdb
del Product\Bin\CEGUI.log
del Product\Bin\FuckingAutoRun

::���������ļ�
copy EQStart.cfg Product\Config\

echo "ִ�н���"
pause