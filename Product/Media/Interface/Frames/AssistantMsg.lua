-----------------------------------------
-- �ļ�˵��	����Lua�ļ�Ϊ�߻��ṩ��Ҫ����Ϣ
-- ����		��xiaoMing
-- ����		��2010-10-20
-----------------------------------------
mainCamera 	= EQGlobaFunc.getMainCamera()

local PI = 3.1415

g_Loger:logMessage("------------------CameraMessage------------------")
g_Loger:logMessage("pitch  = " .. mainCamera:getPitch())
g_Loger:logMessage("yaw    = " .. mainCamera:getYaw())
g_Loger:logMessage("zoom   = " .. mainCamera:getZoom())
g_Loger:logMessage("lookAt = " ..mainCamera:getLookAtX().." "..mainCamera:getLookAtY().." "..mainCamera:getLookAtZ())
g_Loger:logMessage("dir    = " ..EQGlobaClass.getMainPlayer():getYaw())
g_Loger:logMessage("--------------------------------------------------")
