echo "========copy cegui========"

xcopy %__cegui__\include\*.h ..\include\CEGUI  /i /q /y

xcopy %__cegui__%\lib\CEGUIBase.lib ..\lib\  /y
xcopy %__cegui__%\lib\CEGUIBase_d.lib ..\lib\  /y

xcopy %__cegui__%\bin\CEGUIBase.dll ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUIBase_d.dll ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\CEGUIExpatParser.dll ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUIExpatParser_d.dll ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\CEGUIFalagardWRBase.dll ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUIFalagardWRBase_d.dll ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\CEGUILua.dll ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUILua_d.dll ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\tolua++.dll ..\Bin\Release\  /y
xcopy %__cegui__%\bin\tolua++_d.dll ..\Bin\Debug\  /y

xcopy %__cegui__%\bin\CEGUIBase.pdb ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUIBase_d.pdb ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\CEGUIExpatParser.pdb ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUIExpatParser_d.pdb ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\CEGUIFalagardWRBase.pdb ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUIFalagardWRBase_d.pdb ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\CEGUILua.pdb ..\Bin\Release\  /y
xcopy %__cegui__%\bin\CEGUILua_d.pdb ..\Bin\Debug\  /y
xcopy %__cegui__%\bin\tolua++.pdb ..\Bin\Release\  /y
xcopy %__cegui__%\bin\tolua++_d.pdb ..\Bin\Debug\  /y