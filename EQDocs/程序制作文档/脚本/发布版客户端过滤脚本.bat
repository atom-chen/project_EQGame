::使用方法: 把这个脚本和EQStart.cfg和Product目录放在同一个目录下运行就可以
::根据需求配置EQStart.cfg脚本
::根据需求要不要发布Pdb, 这个在下面的脚本修改



echo "执行EQ家园配置脚本.."

::删除多于的文件
rd /s /q Product\Error
rd /s /q Product\Log
rd /s /q Product\BinDebug
rd /s /q Product\Screenshots
::如果不要删除pdb, 注释掉
del Product\Bin\*.pdb
del Product\Bin\CEGUI.log
del Product\Bin\FuckingAutoRun

::拷贝配置文件
copy EQStart.cfg Product\Config\

echo "执行结束"
pause