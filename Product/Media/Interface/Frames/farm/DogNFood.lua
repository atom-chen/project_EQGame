-----------------------------------------
-- 文件说明	:狗和狗粮
-- 作者		：dj
-- 日期		：2010-12-29
-----------------------------------------
--
g_openDFRFlag = false
------------------------------------------------------------
--
function DogNFood_onLoad()
	g_WinMgr:getWindow("dog_food_remain/background/cancel"):subscribeEvent("Clicked","DogNFood_closeRemainUI")
	g_WinMgr:getWindow("dog_food_remain__auto_closebutton__"):subscribeEvent("Clicked","DogNFood_closeRemainUI")
	g_WinMgr:getWindow("dog_food_remain/background/to_storage"):subscribeEvent("Clicked","DogNFood_ToFarmStorage")
	g_WinMgr:getWindow("dog_food_remain/background/to_shop"):subscribeEvent("Clicked","DogNFood_ToFarmShop")

end
------------------------------------------------------------
--
function DogNFood_popRemainDogFoodUI()
	g_FarmUI:addChildWindow(g_RemainDogFoodUI)
	DogNFood_setRemainWinPos()
	Common_injectEmptyEvt("dog_food_remain")
	g_openDFRFlag = true
end
-----------------------------------------------------------
--
function DogNFood_setDogFoodRemainData(day,hour,min,sec,hasFood)
	if hasFood == 0 then
		g_WinMgr:getWindow("dog_food_remain/background/message"):setVisible(true)
	else
		g_WinMgr:getWindow("dog_food_remain/background/message"):setVisible(false)
	end
	remainTime = tostring(day) .."天" ..tostring(hour) .."小时" ..tostring(min) .."分钟" ..tostring(sec) .."秒"
	EQGlobaFunc.SetText("dog_food_remain/background/remain_time",remainTime)
	g_WinMgr:getWindow("dog_food_remain/background/icon"):setProperty("Image","set:32900001Imageset image:01")
	DogNFood_popRemainDogFoodUI()
end
------------------------------------------------------------
--
function DogNFood_setRemainWinPos()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local winWide = g_RemainDogFoodUI:getWidth():asAbsolute(screenW)
	local winH = g_RemainDogFoodUI:getHeight():asAbsolute(screenH)
	g_RemainDogFoodUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-(winWide/2)),CEGUI.UDim(0.5,-(winH/2))))
end
------------------------------------------------------------
--
function DogNFood_closeRemainUI()
	g_FarmUI:removeChildWindow(g_RemainDogFoodUI)
	g_openDFRFlag = false
end
------------------------------------------------------------
--
function DogNFood_ToFarmStorage()
	FPopStoragePanel()
	CEGUI.toTabControl(g_WinMgr:getWindow(g_farmStorage.tabName)):setSelectedTabAtIndex(1)
	DogNFood_closeRemainUI()
end
------------------------------------------------------------
--
function DogNFood_ToFarmShop()
	PopFarmShopUI()
	CEGUI.toTabControl(g_WinMgr:getWindow(g_farmShop.tabName)):setSelectedTabAtIndex(1)
	DogNFood_closeRemainUI()
end
