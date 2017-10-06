-----------------------------------------
-- 文件说明	：鲜花鸡蛋记录
-- 作者		：lf
-- 修改		：
-- 日期		：11/09/21
-----------------------------------------
local text=""

-----------------------------------------
--脚本加载后执行
-----------------------------------------
function FlowerEggLog_Onload()
	g_WinMgr:getWindow("flower_history/Btn_exit"):subscribeEvent("Clicked", "FlowerEggLog_CloseFlowerLog")
	g_WinMgr:getWindow("egg_history/Btn_exit"):subscribeEvent("Clicked", "FlowerEggLog_CloseEggLog")
end

-----------------------------------------
--清空鲜花记录
-----------------------------------------
function FlowerEggLog_ClearFlowerLog()
	text = ""
	EQGlobaFunc.SetText("flower_history/txt2", text)
end

-----------------------------------------
--打开鲜花记录
-----------------------------------------
function FlowerEggLog_OpenFlowerLog()
	g_MainUIWindows:addChildWindow(g_FlowerLog)
	Common_setModalState(g_FlowerLog:getName(), true)
end

-----------------------------------------
--插入鲜花记录
-----------------------------------------
function FlowerEggLog_InsertFlowerLog(hour, minute, player)
	text = text..hour..":"..minute.."     "..player.."送了我一朵鲜花！\n"
	EQGlobaFunc.SetText("flower_history/txt2", text)
end

-----------------------------------------
--关闭鲜花记录
-----------------------------------------
function FlowerEggLog_CloseFlowerLog()
	g_MainUIWindows:removeChildWindow(g_FlowerLog)
	Common_setModalState(g_FlowerLog:getName(), false)
end

-----------------------------------------
--清空鸡蛋记录
-----------------------------------------
function FlowerEggLog_ClearEggLog()
	text = ""
	EQGlobaFunc.SetText("egg_history/txt2", text)
end

-----------------------------------------
--打开鸡蛋记录
-----------------------------------------
function FlowerEggLog_OpenEggLog()
	g_MainUIWindows:addChildWindow(g_EggLog)
	Common_setModalState(g_EggLog:getName(), true)
end


-----------------------------------------
--插入鸡蛋记录
-----------------------------------------
function FlowerEggLog_InsertEggLog(hour, minute, player)
	text = text..hour..":"..minute.."     "..player.."朝我的房间扔了一个臭鸡蛋！\n"
	EQGlobaFunc.SetText("egg_history/txt2", text)
end

-----------------------------------------
--关闭鸡蛋记录
-----------------------------------------
function FlowerEggLog_CloseEggLog()
	g_MainUIWindows:removeChildWindow(g_EggLog)
	Common_setModalState(g_EggLog:getName(), false)
end