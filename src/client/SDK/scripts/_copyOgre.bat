echo "========copy ogre========"

xcopy %__ogre__%\OgreMain\include\*.h ..\include\Ogre  /i /q /y
xcopy %__ogre__%\OgreMain\include\WIN32\*.h ..\include\ogre\WIN32  /i /q /y
xcopy %__ogre__%\lib\OgreMain.lib ..\lib\  /y
xcopy %__ogre__%\lib\OgreMain_d.lib ..\lib\  /y
xcopy %__ogre__%\Dependencies\include\ois ..\include\OIS /s /e  /i /q /y
xcopy %__ogre__%\Dependencies\lib\Release\OIS.lib ..\lib\  /y
xcopy %__ogre__%\Dependencies\lib\Debug\OIS_d.lib ..\lib\  /y

xcopy %__ogre__%\lib\OgreMain.dll ..\Bin\Release\  /y
xcopy %__ogre__%\lib\OgreMain.pdb ..\Bin\Release\  /y
xcopy %__ogre__%\lib\RenderSystem_Direct3D9.dll ..\Bin\Release\  /y
xcopy %__ogre__%\lib\RenderSystem_Direct3D9.pdb ..\Bin\Release\  /y
xcopy %__ogre__%\lib\Plugin_ParticleFX.dll ..\Bin\Release\  /y
xcopy %__ogre__%\lib\Plugin_ParticleFX.pdb ..\Bin\Release\  /y
xcopy %__ogre__%\lib\Plugin_OctreeSceneManager.dll ..\Bin\Release\  /y
xcopy %__ogre__%\lib\Plugin_OctreeSceneManager.pdb ..\Bin\Release\  /y

xcopy %__ogre__%\lib\OgreMain_d.dll ..\Bin\Debug\  /y
xcopy %__ogre__%\lib\OgreMain_d.pdb ..\Bin\Debug\  /y
xcopy %__ogre__%\lib\RenderSystem_Direct3D9_d.dll ..\Bin\Debug\  /y
xcopy %__ogre__%\lib\RenderSystem_Direct3D9_d.pdb ..\Bin\Debug\  /y
xcopy %__ogre__%\lib\Plugin_ParticleFX_d.dll ..\Bin\Debug\  /y
xcopy %__ogre__%\lib\Plugin_ParticleFX_d.pdb ..\Bin\Debug\  /y
xcopy %__ogre__%\lib\Plugin_OctreeSceneManager_d.dll ..\Bin\Debug\  /y
xcopy %__ogre__%\lib\Plugin_OctreeSceneManager_d.pdb ..\Bin\Debug\  /y


xcopy %__ogre__%\Samples\Common\bin\Release\OIS.dll ..\Bin\Release\  /y

xcopy %__ogre__%\Samples\Common\bin\Debug\OIS_d.dll ..\Bin\Debug\  /y


