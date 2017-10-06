echo "========copy PDB To Project========"
::作用是把调试信息拷贝到工程, 根据需求自行设置要拷贝的目录吧

::xcopy ..\Bin\Release\*.pdb ..\..\Product\Bin\ /i /q /y
::xcopy ..\Bin\Debug\*.pdb ..\..\Product\BinDebug\ /i /q /y

::xcopy ..\Bin\Release\*.pdb ..\..\Tools\Bin\ /i /q /y
::xcopy ..\Bin\Debug\*.pdb ..\..\Tools\BinDebug\ /i /q /y

pause