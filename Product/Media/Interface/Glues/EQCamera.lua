-----------------------------------------
-- 文件说明	：camera初始化相关操作
-- 作者		：
-- 修改		：麦
-- 日期		：10/03/25
-----------------------------------------

--local pitchMin,pitchMax,yawMin,yawMax,zoomMin,zoomMax,lookatMin,lookMax
--local mainCamera
--local exitFlag = true
--
--local mainCamera= EQGlobaClass.getMainCamera()
--
--local pitchMin 	= mainCamera:getPitchMin()
--local pitchMax 	= mainCamera:getPitchMax()
--local yawMin 	= mainCamera:getYawMin()
--local yawMax	= mainCamera:getYawMax()
--local zoomMin	= mainCamera:getZoomMin()
--local zoomMax	= mainCamera:getZoomMax()
--local lookatMin	= mainCamera:getLookAtMin()
--local lookatMax = mainCamera:getLookAtMax()

function EQCamera_Onload()
	g_GameEventMgr:subscribeEvent("UPDATE_UI_FARM_DATA","EnterFarm")
	g_GameEventMgr:subscribeEvent("EXIT_FUNNY_FARM","ExitFarm")
end

function EnterFarm()
	--mainCamera 	= EQGlobaClass.getMainCamera()
	--
	--pitchMin 	= mainCamera:getPitchMin()
	--pitchMax 	= mainCamera:getPitchMax()
	--yawMin 	 	= mainCamera:getYawMin()
	--yawMax		= mainCamera:getYawMax()
	--zoomMin		= mainCamera:getZoomMin()
	--zoomMax		= mainCamera:getZoomMax()
	--lookatMin	= mainCamera:getLookAtMin()
	--lookatMax 	= mainCamera:getLookAtMax()
	--
	--mainCamera:setPitchMin(30)
	--mainCamera:setPitchMax(30)
	--if exitFlag == true then
	--	mainCamera:setYawMin(225)
	--	mainCamera:setYawMax(225)
	--	exitFlag = false
	--end
	--mainCamera:setYawMin(210)
	--mainCamera:setYawMax(240)
	--mainCamera:setZoomMin(600)
	--mainCamera:setZoomMax(600)
	--mainCamera:setLookAtMin(160)
	--mainCamera:setLookAtMax(160)
	--
	--mainCamera:setPitchMin(pitchMin)
	--mainCamera:setPitchMax(pitchMax)
	--mainCamera:setYawMin(yawMin)
	--mainCamera:setYawMax(yawMax)
	--mainCamera:setZoomMin(zoomMin)
	--mainCamera:setZoomMax(zoomMax)
	--mainCamera:setLookAtMin(lookatMin)
	--mainCamera:setLookAtMax(lookatMax)
end

function ExitFarm()
	--mainCamera:setYawMin(180)
	--mainCamera:setYawMax(180)
	
	--mainCamera:setZoomMin(1200)
	--mainCamera:setZoomMax(1200)
	
	--mainCamera:setLookAtMin(0)
	--mainCamera:setLookAtMax(0)
	
	--mainCamera:setPitchMin(pitchMin)
	--mainCamera:setPitchMax(pitchMax)
	--mainCamera:setYawMin(yawMin)
	--mainCamera:setYawMax(yawMax)
	--mainCamera:setZoomMin(zoomMin)
	--mainCamera:setZoomMax(zoomMax)
	--mainCamera:setLookAtMin(lookatMin)
	--mainCamera:setLookAtMax(lookatMax)
	--
	--exitFlag = true
end