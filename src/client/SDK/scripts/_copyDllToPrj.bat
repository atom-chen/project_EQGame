echo "========copy Dll To Project========"

xcopy ..\Bin\Release\*.dll ..\..\Product\Bin\  /i /q /y
xcopy ..\Bin\Debug\*.dll ..\..\Product\BinDebug\  /i /q /y

xcopy ..\Bin\Release\*.dll ..\..\Tools\Bin\  /i /q /y
xcopy ..\Bin\Debug\*.dll ..\..\Tools\BinDebug\  /i /q /y
