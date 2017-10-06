-----------------------------------------
-- 文件说明	：农场任务
-- 作者		：何展然
-- 修改		：
-- 日期		：2011/4/14
-----------------------------------------

local m_newBieCurPage = 1	-- 新手引导界面当前页
local m_seeNextTask = false	-- 查看下一条任务
g_newBieTaskUI:removeChildWindow("newbie_task_background__auto_button__")
local m_newBieImage = g_WinMgr:getWindow("newbie_task_background/img")
local m_newBieCloseBtn = g_WinMgr:getWindow("newbie_task_background__auto_closebutton__")
local m_newBiePgUp = g_WinMgr:getWindow("newbie_task_background/pg_up")
local m_newBiePgUpText = g_WinMgr:getWindow("newbie_task_background/pgup_text")
local m_newBiePgDown = g_WinMgr:getWindow("newbie_task_background/pg_down")
local m_newBiePgDownText = g_WinMgr:getWindow("newbie_task_background/pgdown_text")
local m_newBieKnow = g_WinMgr:getWindow("newbie_task_background/got_it")
local m_newBieOk = g_WinMgr:getWindow("newbie_task_background/done")
local m_taskButton = g_WinMgr:getWindow("FarmmainUI/task_icon")
-- 农场鼠标动作类型
local m_actionType = {till=2, water=3, weed=4, bug=5, harvest=6, seed=8}
-- 预设置新手引导界面
m_newBieCloseBtn:setVisible(false)
m_newBiePgUp:setVisible(false)
m_newBiePgUpText:setVisible(false)
m_newBieKnow:setVisible(false)
m_newBieOk:setVisible(false)
g_WinMgr:getWindow("farm_complete_task/last_task"):setVisible(false)
g_WinMgr:getWindow("farm_get_task/sure"):setVisible(false)
m_taskButton:setVisible(false)
-- 注册新手引导界面按钮事件
m_newBieCloseBtn:subscribeEvent("Clicked", "FarmTask_newBieUIClose")
m_newBiePgUp:subscribeEvent("Clicked", "FarmTask_newBieUIPageUp")
m_newBiePgDown:subscribeEvent("Clicked", "FarmTask_newBieUIPageDown")
m_newBieKnow:subscribeEvent("Clicked", "FarmTask_newBieUIKnow")
m_newBieOk:subscribeEvent("Clicked", "FarmTask_newBieUIOk")
g_WinMgr:getWindow("farm_get_task__auto_closebutton__"):subscribeEvent("Clicked", "FarmTask_getTaskCancle")
g_WinMgr:getWindow("farm_get_task/done"):subscribeEvent("Clicked", "FarmTask_getTaskDone")
g_WinMgr:getWindow("farm_get_task/cancle"):subscribeEvent("Clicked", "FarmTask_getTaskCancle")
g_WinMgr:getWindow("farm_get_task/sure"):subscribeEvent("Clicked", "FarmTask_getTaskCancle")
g_WinMgr:getWindow("farm_complete_task__auto_closebutton__"):subscribeEvent("Clicked", "FarmTask_completeTaskCancle")
g_WinMgr:getWindow("farm_complete_task/next_task"):subscribeEvent("Clicked", "FarmTask_completeTaskNextTask")
g_WinMgr:getWindow("farm_complete_task/cancle"):subscribeEvent("Clicked", "FarmTask_completeTaskCancle")
g_WinMgr:getWindow("farm_complete_task/last_task"):subscribeEvent("Clicked", "FarmTask_completeTaskCancle")
m_taskButton:subscribeEvent("MouseClick", "FarmTask_clickTaskButton")
-- 屏蔽鼠标穿透
MainUI_ShieldClick(g_newBieTaskUI)
MainUI_ShieldClick(g_getFarmTaskUI)
MainUI_ShieldClick(g_completeFarmTaskUI)
Common_injectEmptyEvt(g_newBieTaskUI:getName())
Common_injectEmptyEvt(g_getFarmTaskUI:getName())
Common_injectEmptyEvt(g_completeFarmTaskUI:getName())
g_newBieTaskUI:setAlwaysOnTop(true)
g_getFarmTaskUI:setAlwaysOnTop(true)
g_completeFarmTaskUI:setAlwaysOnTop(true)
-----------------------------------------
-- 开始新手引导界面
function FarmTask_startNewBieUI()
	g_FarmUI:addChildWindow(g_newBieTaskUI)
	FarmTask_setNewBieUI(1)
	Common_setModalState(g_newBieTaskUI:getName(), true)
	FarmTask_setTaskButton(true)
	m_newBiePgDown:setVisible(true)
	m_newBiePgDownText:setVisible(true)
	m_newBiePgUp:setVisible(false)
	m_newBiePgUpText:setVisible(false)
	m_newBieKnow:setVisible(false)
	m_newBieOk:setVisible(false)
	m_newBieCloseBtn:setVisible(false)
	m_newBieCurPage = 1
end
-----------------------------------------
-- 点击新手引导界面向上翻页按钮
function FarmTask_newBieUIPageUp(args)
	m_newBieCurPage = m_newBieCurPage -1
	FarmTask_setNewBieUI(m_newBieCurPage)
	if 1 == m_newBieCurPage then
		m_newBiePgUp:setVisible(false)
		m_newBiePgUpText:setVisible(false)
	end
	m_newBiePgDown:setVisible(true)
	m_newBiePgDownText:setVisible(true)
	m_newBieKnow:setVisible(false)
end
-----------------------------------------
-- 点击新手引导界面向下翻页按钮
function FarmTask_newBieUIPageDown(args)
	m_newBieCurPage = m_newBieCurPage + 1
	FarmTask_setNewBieUI(m_newBieCurPage)
	if 5 == m_newBieCurPage then
		m_newBiePgDown:setVisible(false)
		m_newBiePgDownText:setVisible(false)
		m_newBieKnow:setVisible(true)
	end
	m_newBiePgUp:setVisible(true)
	m_newBiePgUpText:setVisible(true)
end
-----------------------------------------
-- 点击新手引导界面我明白了按钮
function FarmTask_newBieUIKnow(args)
	m_newBiePgUp:setVisible(false)
	m_newBiePgUpText:setVisible(false)
	m_newBiePgDown:setVisible(false)
	m_newBiePgDownText:setVisible(false)
	m_newBieKnow:setVisible(false)
	EQGlobaFunc.SetText(g_newBieTaskUI:getName(), "新手礼包")	-- 标题改变
	m_newBieCloseBtn:setVisible(true)	-- 关闭按钮显示
	m_newBieOk:setVisible(true)
end
-----------------------------------------
-- 点击新手引导界面确定按钮
function FarmTask_newBieUIOk(args)
	m_seeNextTask = true
	g_FarmUI:removeChildWindow(g_newBieTaskUI)	-- 关闭新手引导界面
	Common_setModalState(g_newBieTaskUI:getName(), false)
	FarmTask_reqTaskComplete(FarmTask_getCurTaskId())	-- 完成当前任务
end
-----------------------------------------
-- 点击新手引导界面关闭按钮
function FarmTask_newBieUIClose(args)
	g_FarmUI:removeChildWindow(g_newBieTaskUI)	-- 关闭新手引导界面
	Common_setModalState(g_newBieTaskUI:getName(), false)
	FarmTask_reqTaskComplete(FarmTask_getCurTaskId())	-- 完成当前任务
end
-----------------------------------------
-- 清除新手引导界面
function FarmTask_clearNewBieUI()
	if 0 == m_newBieImage:getChildCount() then
		return
	end
	local child = m_newBieImage:getChildAtIdx(0)
	while true do
		m_newBieImage:removeChildWindow(child)
		if 0 == m_newBieImage:getChildCount() then
			return
		end
		child = m_newBieImage:getChildAtIdx(0)
	end
end
-----------------------------------------
-- 设置新手引导界面
function FarmTask_setNewBieUI(page)
	FarmTask_clearNewBieUI()
	if 1 == page then
		FarmTask_newBieUI_1()
	elseif 2 == page then
		FarmTask_newBieUI_2()
	elseif 3 == page then
		FarmTask_newBieUI_3()
	elseif 4 == page then
		FarmTask_newBieUI_4()
	elseif 5 == page then
		FarmTask_newBieUI_5()
	end
end
-----------------------------------------
-- 第1个新手引导界面
function FarmTask_newBieUI_1()
	-- 创建第一行文字控件
	local page1_text1 = g_WinMgr:getWindow("newbie_task_background/page1/text1")
	if nil == page1_text1 then
		page1_text1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page1/text1")
		EQGlobaFunc.toMultiLineBox(page1_text1:getName()):setSize(310, 60)
		EQGlobaFunc.toMultiLineBox(page1_text1:getName()):setPosition(20, 5)
		EQGlobaFunc.toMultiLineBox(page1_text1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page1_text1:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(page1_text1:getName()):setAlign("AT_CENTER_VER")
		local str1 = "恭喜你成为一个农场主，快开始种植各种蔬菜和水果吧！"
		local str2 = "先到<clr v=\"#FF0000\">商店</clr>买种子，买好之后，打开<clr v=\"#FF0000\">背包</clr>将种子种到<clr v=\"#FF0000\">农田</clr>里就可以了。"
		g_mlbParse:getData(str1..str2, page1_text1:getName())
	end
	m_newBieImage:addChildWindow(page1_text1)
	-- 创建第二行icon控件
	local spec = (340 - 3*50)/(3+1)
	local page1_icon1 = g_WinMgr:getWindow("newbie_task_background/page1/icon1")
	if nil == page1_icon1 then
		page1_icon1 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page1/icon1")
		page1_icon1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 50)))
		page1_icon1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, spec), CEGUI.UDim(0, 96)))
		page1_icon1:setProperty("Image", "set:Button3 image:ImageButton16_Normal")
	end
	m_newBieImage:addChildWindow(page1_icon1)
	--
	local page1_icon2 = g_WinMgr:getWindow("newbie_task_background/page1/icon2")
	if nil == page1_icon2 then
		page1_icon2 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page1/icon2")
		page1_icon2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 36), CEGUI.UDim(0, 36)))
		page1_icon2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 155), CEGUI.UDim(0, 110)))
		page1_icon2:setProperty("Image", "set:Button3 image:ImageButton20_Normal")
	end
	m_newBieImage:addChildWindow(page1_icon2)
	--
	local page1_icon3 = g_WinMgr:getWindow("newbie_task_background/page1/icon3")
	if nil == page1_icon3 then
		page1_icon3 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page1/icon3")
		page1_icon3:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 50)))
		page1_icon3:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 3*spec+2*50), CEGUI.UDim(0, 96)))
		page1_icon3:setProperty("Image", "set:farm_task image:NewImage4")
	end
	m_newBieImage:addChildWindow(page1_icon3)
	-- 创建第三行icon描述控件
	local page1_tip1 = g_WinMgr:getWindow("newbie_task_background/page1/tip1")
	if nil == page1_tip1 then
		page1_tip1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page1/tip1")
		page1_tip1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page1_tip1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, spec), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page1_tip1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page1_tip1:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page1_tip1:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page1_tip1:getName()):addElementText("商店", false)
	end
	m_newBieImage:addChildWindow(page1_tip1)
	--
	local page1_tip2 = g_WinMgr:getWindow("newbie_task_background/page1/tip2")
	if nil == page1_tip2 then
		page1_tip2 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page1/tip2")
		page1_tip2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page1_tip2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 155), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page1_tip2:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page1_tip2:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page1_tip2:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page1_tip2:getName()):addElementText("背包", false)
	end
	m_newBieImage:addChildWindow(page1_tip2)
	--
	local page1_tip3 = g_WinMgr:getWindow("newbie_task_background/page1/tip3")
	if nil == page1_tip3 then
		page1_tip3 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page1/tip3")
		page1_tip3:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page1_tip3:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 3*spec+2*50), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page1_tip3:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page1_tip3:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page1_tip3:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page1_tip3:getName()):addElementText("土地", false)
	end
	m_newBieImage:addChildWindow(page1_tip3)
end
-----------------------------------------
-- 第2个新手引导界面
function FarmTask_newBieUI_2()
	-- 创建第一行文字控件
	local page2_text1 = g_WinMgr:getWindow("newbie_task_background/page2/text1")
	if nil == page2_text1 then
		page2_text1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page2/text1")
		EQGlobaFunc.toMultiLineBox(page2_text1:getName()):setSize(310, 80)
		EQGlobaFunc.toMultiLineBox(page2_text1:getName()):setPosition(20, 5)
		EQGlobaFunc.toMultiLineBox(page2_text1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page2_text1:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(page2_text1:getName()):setAlign("AT_CENTER_VER")
		local str1 = "作物有<clr v=\"#FF0000\">发芽</clr>、<clr v=\"#FF0000\">小叶子</clr>、<clr v=\"#FF0000\">大叶子</clr>、<clr v=\"#FF0000\">开花</clr>、<clr v=\"#FF0000\">成熟</clr>"
		local str2 = "这五个生长阶段（少数作物不会开花）。<br/><br/><br/>例：南瓜的五个阶段"
		g_mlbParse:getData(str1..str2, page2_text1:getName())
	end
	m_newBieImage:addChildWindow(page2_text1)
	-- 创建第二行icon控件
	local spec = (340 - 3*50)/(3+1)
	local page2_icon1 = g_WinMgr:getWindow("newbie_task_background/page2/icon1")
	if nil == page2_icon1 then
		page2_icon1 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page2/icon1")
		page2_icon1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 310), CEGUI.UDim(0, 56)))
		page2_icon1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 20), CEGUI.UDim(0, 96)))
		page2_icon1:setProperty("Image", "set:farm_task image:NewImage1")
	end
	m_newBieImage:addChildWindow(page2_icon1)
	-- 创建第三行icon描述控件
	local page2_tip1 = g_WinMgr:getWindow("newbie_task_background/page2/tip1")
	if nil == page2_tip1 then
		page2_tip1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page2/tip1")
		page2_tip1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page2_tip1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 15), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page2_tip1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page2_tip1:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page2_tip1:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page2_tip1:getName()):addElementText("发芽", false)
	end
	m_newBieImage:addChildWindow(page2_tip1)
	--
	local page2_tip2 = g_WinMgr:getWindow("newbie_task_background/page2/tip2")
	if nil == page2_tip2 then
		page2_tip2 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page2/tip2")
		page2_tip2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page2_tip2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 55), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page2_tip2:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page2_tip2:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page2_tip2:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page2_tip2:getName()):addElementText("小叶子", false)
	end
	m_newBieImage:addChildWindow(page2_tip2)
	--
	local page2_tip3 = g_WinMgr:getWindow("newbie_task_background/page2/tip3")
	if nil == page2_tip3 then
		page2_tip3 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page2/tip3")
		page2_tip3:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page2_tip3:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 120), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page2_tip3:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page2_tip3:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page2_tip3:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page2_tip3:getName()):addElementText("大叶子", false)
	end
	m_newBieImage:addChildWindow(page2_tip3)
	--
	local page2_tip4 = g_WinMgr:getWindow("newbie_task_background/page2/tip4")
	if nil == page2_tip4 then
		page2_tip4 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page2/tip4")
		page2_tip4:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page2_tip4:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 185), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page2_tip4:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page2_tip4:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page2_tip4:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page2_tip4:getName()):addElementText("开花", false)
	end
	m_newBieImage:addChildWindow(page2_tip4)
	--
	local page2_tip5 = g_WinMgr:getWindow("newbie_task_background/page2/tip5")
	if nil == page2_tip5 then
		page2_tip5 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page2/tip5")
		page2_tip5:setSize(CEGUI.UVector2(CEGUI.UDim(0, 50), CEGUI.UDim(0, 20)))
		page2_tip5:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 255), CEGUI.UDim(0, 155)))
		EQGlobaFunc.toMultiLineBox(page2_tip5:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page2_tip5:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page2_tip5:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page2_tip5:getName()):addElementText("成熟", false)
	end
	m_newBieImage:addChildWindow(page2_tip5)
end
-----------------------------------------
-- 第3个新手引导界面
function FarmTask_newBieUI_3()
	-- 创建第一行文字控件
	local page3_text1 = g_WinMgr:getWindow("newbie_task_background/page3/text1")
	if nil == page3_text1 then
		page3_text1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/text1")
		EQGlobaFunc.toMultiLineBox(page3_text1:getName()):setSize(305, 30)
		EQGlobaFunc.toMultiLineBox(page3_text1:getName()):setPosition(20, 5)
		EQGlobaFunc.toMultiLineBox(page3_text1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_text1:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(page3_text1:getName()):setAlign("AT_CENTER_VER")
		local str = "用<clr v=\"#FF0000\">水壶</clr>、<clr v=\"#FF0000\">镰刀</clr>、<clr v=\"#FF0000\">除草剂</clr>好好照顾作物，不然健康值下降，对产量会有影响。"
		g_mlbParse:getData(str, page3_text1:getName())
	end
	m_newBieImage:addChildWindow(page3_text1)
	-- 创建第二行icon控件
	local page3_icon1 = g_WinMgr:getWindow("newbie_task_background/page3/icon1")
	if nil == page3_icon1 then
		page3_icon1 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon1")
		page3_icon1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 36), CEGUI.UDim(0, 36)))
		page3_icon1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 1), CEGUI.UDim(0, 52)))
		page3_icon1:setProperty("Image", "set:Button3 image:ImageButton2_Normal")
	end
	m_newBieImage:addChildWindow(page3_icon1)
	--
	local page3_icon2 = g_WinMgr:getWindow("newbie_task_background/page3/icon2")
	if nil == page3_icon2 then
		page3_icon2 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon2")
		page3_icon2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 36), CEGUI.UDim(0, 36)))
		page3_icon2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 44), CEGUI.UDim(0, 52)))
		page3_icon2:setProperty("Image", "set:Button3 image:ImageButton3_Normal")
	end
	m_newBieImage:addChildWindow(page3_icon2)
	--
	local page3_icon3 = g_WinMgr:getWindow("newbie_task_background/page3/icon3")
	if nil == page3_icon3 then
		page3_icon3 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon3")
		page3_icon3:setSize(CEGUI.UVector2(CEGUI.UDim(0, 36), CEGUI.UDim(0, 36)))
		page3_icon3:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 86), CEGUI.UDim(0, 52)))
		page3_icon3:setProperty("Image", "set:Button3 image:ImageButton4_Normal")
	end
	m_newBieImage:addChildWindow(page3_icon3)
	--
	local page3_icon4 = g_WinMgr:getWindow("newbie_task_background/page3/icon4")
	if nil == page3_icon4 then
		page3_icon4 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon4")
		page3_icon4:setSize(CEGUI.UVector2(CEGUI.UDim(0, 70), CEGUI.UDim(0, 50)))
		page3_icon4:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 127), CEGUI.UDim(0, 38)))
		page3_icon4:setProperty("Image", "set:farm_task image:NewImage7")
	end
	m_newBieImage:addChildWindow(page3_icon4)
	--
	local page3_icon5 = g_WinMgr:getWindow("newbie_task_background/page3/icon5")
	if nil == page3_icon5 then
		page3_icon5 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon5")
		page3_icon5:setSize(CEGUI.UVector2(CEGUI.UDim(0, 70), CEGUI.UDim(0, 50)))
		page3_icon5:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 198), CEGUI.UDim(0, 38)))
		page3_icon5:setProperty("Image", "set:farm_task image:NewImage2")
	end
	m_newBieImage:addChildWindow(page3_icon5)
	--
	local page3_icon6 = g_WinMgr:getWindow("newbie_task_background/page3/icon6")
	if nil == page3_icon6 then
		page3_icon6 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon6")
		page3_icon6:setSize(CEGUI.UVector2(CEGUI.UDim(0, 70), CEGUI.UDim(0, 50)))
		page3_icon6:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 269), CEGUI.UDim(0, 38)))
		page3_icon6:setProperty("Image", "set:farm_task image:NewImage6")
	end
	m_newBieImage:addChildWindow(page3_icon6)
	-- 创建第三行icon描述控件
	local page3_tip1 = g_WinMgr:getWindow("newbie_task_background/page3/tip1")
	if nil == page3_tip1 then
		page3_tip1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip1")
		page3_tip1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page3_tip1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 2), CEGUI.UDim(0, 88)))
		EQGlobaFunc.toMultiLineBox(page3_tip1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip1:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip1:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip1:getName()):addElementText("水壶", false)
	end
	m_newBieImage:addChildWindow(page3_tip1)
	--
	local page3_tip2 = g_WinMgr:getWindow("newbie_task_background/page3/tip2")
	if nil == page3_tip2 then
		page3_tip2 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip2")
		page3_tip2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 42), CEGUI.UDim(0, 20)))
		page3_tip2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 88)))
		EQGlobaFunc.toMultiLineBox(page3_tip2:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip2:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip2:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip2:getName()):addElementText("镰刀", false)
	end
	m_newBieImage:addChildWindow(page3_tip2)
	--
	local page3_tip3 = g_WinMgr:getWindow("newbie_task_background/page3/tip3")
	if nil == page3_tip3 then
		page3_tip3 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip3")
		page3_tip3:setSize(CEGUI.UVector2(CEGUI.UDim(0, 42), CEGUI.UDim(0, 20)))
		page3_tip3:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 82), CEGUI.UDim(0, 88)))
		EQGlobaFunc.toMultiLineBox(page3_tip3:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip3:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip3:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip3:getName()):addElementText("杀虫剂", false)
	end
	m_newBieImage:addChildWindow(page3_tip3)
	--
	local page3_tip4 = g_WinMgr:getWindow("newbie_task_background/page3/tip4")
	if nil == page3_tip4 then
		page3_tip4 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip4")
		page3_tip4:setSize(CEGUI.UVector2(CEGUI.UDim(0, 56), CEGUI.UDim(0, 20)))
		page3_tip4:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 135), CEGUI.UDim(0, 88)))
		EQGlobaFunc.toMultiLineBox(page3_tip4:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip4:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip4:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip4:getName()):addElementText("干燥土地", false)
	end
	m_newBieImage:addChildWindow(page3_tip4)
	--
	local page3_tip5 = g_WinMgr:getWindow("newbie_task_background/page3/tip5")
	if nil == page3_tip5 then
		page3_tip5 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip5")
		page3_tip5:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page3_tip5:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 218), CEGUI.UDim(0, 88)))
		EQGlobaFunc.toMultiLineBox(page3_tip5:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip5:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip5:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip5:getName()):addElementText("虫害", false)
	end
	m_newBieImage:addChildWindow(page3_tip5)
	--
	local page3_tip6 = g_WinMgr:getWindow("newbie_task_background/page3/tip6")
	if nil == page3_tip6 then
		page3_tip6 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip6")
		page3_tip6:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page3_tip6:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 288), CEGUI.UDim(0, 88)))
		EQGlobaFunc.toMultiLineBox(page3_tip6:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip6:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip6:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip6:getName()):addElementText("杂草", false)
	end
	m_newBieImage:addChildWindow(page3_tip6)
	-- 创建第四行文字控件
	local page3_text2 = g_WinMgr:getWindow("newbie_task_background/page3/text2")
	if nil == page3_text2 then
		page3_text2 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/text2")
		EQGlobaFunc.toMultiLineBox(page3_text2:getName()):setSize(305, 30)
		EQGlobaFunc.toMultiLineBox(page3_text2:getName()):setPosition(20, 110)
		EQGlobaFunc.toMultiLineBox(page3_text2:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_text2:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(page3_text2:getName()):setAlign("AT_CENTER_VER")
		local str = "作物成熟以后，记得<clr v=\"#FF0000\">收</clr>进<clr v=\"#FF0000\">仓库</clr>，卖出仓库里的果实就可以获得金币。"
		g_mlbParse:getData(str, page3_text2:getName())
	end
	m_newBieImage:addChildWindow(page3_text2)
	-- 创建第五行icon控件
	local page3_icon7 = g_WinMgr:getWindow("newbie_task_background/page3/icon7")
	if nil == page3_icon7 then
		page3_icon7 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon7")
		page3_icon7:setSize(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 40)))
		page3_icon7:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 80), CEGUI.UDim(0, 142)))
		page3_icon7:setProperty("Image", "set:Button3 image:ImageButton21_Normal")
	end
	m_newBieImage:addChildWindow(page3_icon7)
	--
	local page3_icon8 = g_WinMgr:getWindow("newbie_task_background/page3/icon8")
	if nil == page3_icon8 then
		page3_icon8 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page3/icon8")
		page3_icon8:setSize(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 40)))
		page3_icon8:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 206), CEGUI.UDim(0, 142)))
		page3_icon8:setProperty("Image", "set:Button3 image:ImageButton16_Normal")
	end
	m_newBieImage:addChildWindow(page3_icon8)
	-- 创建第六行icon描述控件
	local page3_tip7 = g_WinMgr:getWindow("newbie_task_background/page3/tip7")
	if nil == page3_tip7 then
		page3_tip7 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip7")
		page3_tip7:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page3_tip7:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 88), CEGUI.UDim(0, 184)))
		EQGlobaFunc.toMultiLineBox(page3_tip7:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip7:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip7:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip7:getName()):addElementText("收获", false)
	end
	m_newBieImage:addChildWindow(page3_tip7)
	--
	local page3_tip8 = g_WinMgr:getWindow("newbie_task_background/page3/tip8")
	if nil == page3_tip8 then
		page3_tip8 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page3/tip8")
		page3_tip8:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page3_tip8:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 210), CEGUI.UDim(0, 184)))
		EQGlobaFunc.toMultiLineBox(page3_tip8:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page3_tip8:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page3_tip8:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page3_tip8:getName()):addElementText("仓库", false)
	end
	m_newBieImage:addChildWindow(page3_tip8)
end
-----------------------------------------
-- 第4个新手引导界面
function FarmTask_newBieUI_4()
	-- 创建第一行文字控件
	local page4_text1 = g_WinMgr:getWindow("newbie_task_background/page4/text1")
	if nil == page4_text1 then
		page4_text1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page4/text1")
		EQGlobaFunc.toMultiLineBox(page4_text1:getName()):setSize(305, 30)
		EQGlobaFunc.toMultiLineBox(page4_text1:getName()):setPosition(20, 5)
		EQGlobaFunc.toMultiLineBox(page4_text1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page4_text1:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(page4_text1:getName()):setAlign("AT_CENTER_VER")
		local str = "经常到好友农场里逛逛，帮好友浇浇水、杀杀虫、除除草，也可以“<clr v=\"#FF0000\">摘取</clr>”点果实哦！"
		g_mlbParse:getData(str, page4_text1:getName())
	end
	m_newBieImage:addChildWindow(page4_text1)
	-- 创建第二行icon控件
	local page4_icon1 = g_WinMgr:getWindow("newbie_task_background/page4/icon1")
	if nil == page4_icon1 then
		page4_icon1 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page4/icon1")
		page4_icon1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 36), CEGUI.UDim(0, 36)))
		page4_icon1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 155), CEGUI.UDim(0, 64)))
		page4_icon1:setProperty("Image", "set:Button3 image:ImageButton5_Normal")
	end
	m_newBieImage:addChildWindow(page4_icon1)
	-- 创建第三行icon描述控件
	local page4_tip1 = g_WinMgr:getWindow("newbie_task_background/page4/tip1")
	if nil == page4_tip1 then
		page4_tip1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page4/tip1")
		page4_tip1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page4_tip1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 158), CEGUI.UDim(0, 102)))
		EQGlobaFunc.toMultiLineBox(page4_tip1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page4_tip1:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page4_tip1:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page4_tip1:getName()):addElementText("摘取", false)
	end
	m_newBieImage:addChildWindow(page4_tip1)
end
-----------------------------------------
-- 第5个新手引导界面
function FarmTask_newBieUI_5()
	-- 创建第一行文字控件
	local page5_text1 = g_WinMgr:getWindow("newbie_task_background/page5/text1")
	if nil == page5_text1 then
		page5_text1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page5/text1")
		EQGlobaFunc.toMultiLineBox(page5_text1:getName()):setSize(305, 30)
		EQGlobaFunc.toMultiLineBox(page5_text1:getName()):setPosition(20, 5)
		EQGlobaFunc.toMultiLineBox(page5_text1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page5_text1:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(page5_text1:getName()):setAlign("AT_CENTER_VER")
		local str = "可以接受小任务，会让你更加熟悉这个游戏。"
		g_mlbParse:getData(str, page5_text1:getName())
	end
	m_newBieImage:addChildWindow(page5_text1)
	-- 创建第二行icon控件
	local page5_icon1 = g_WinMgr:getWindow("newbie_task_background/page5/icon1")
	if nil == page5_icon1 then
		page5_icon1 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page5/icon1")
		page5_icon1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 46), CEGUI.UDim(0, 46)))
		page5_icon1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 148), CEGUI.UDim(0, 40)))
		page5_icon1:setProperty("Image", "set:farm_task image:NewImage11")
	end
	m_newBieImage:addChildWindow(page5_icon1)
	-- 创建第三行icon描述控件
	local page5_tip1 = g_WinMgr:getWindow("newbie_task_background/page5/tip1")
	if nil == page5_tip1 then
		page5_tip1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page5/tip1")
		page5_tip1:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page5_tip1:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 158), CEGUI.UDim(0, 88)))
		EQGlobaFunc.toMultiLineBox(page5_tip1:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page5_tip1:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page5_tip1:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page5_tip1:getName()):addElementText("任务", false)
	end
	m_newBieImage:addChildWindow(page5_tip1)
	-- 创建第四行文字控件
	local page5_text2 = g_WinMgr:getWindow("newbie_task_background/page5/text2")
	if nil == page5_text2 then
		page5_text2 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page5/text2")
		EQGlobaFunc.toMultiLineBox(page5_text2:getName()):setSize(305, 30)
		EQGlobaFunc.toMultiLineBox(page5_text2:getName()):setPosition(20, 110)
		EQGlobaFunc.toMultiLineBox(page5_text2:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page5_text2:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(page5_text2:getName()):setAlign("AT_CENTER_VER")
		local str = "此外，我们还给你准备了大礼包，现在就去查收吧。"
		g_mlbParse:getData(str, page5_text2:getName())
	end
	m_newBieImage:addChildWindow(page5_text2)
	-- 创建第五行icon控件
	local page5_icon2 = g_WinMgr:getWindow("newbie_task_background/page5/icon2")
	if nil == page5_icon2 then
		page5_icon2 = g_WinMgr:createWindow("EQHomeLook/StaticImage", "newbie_task_background/page5/icon2")
		page5_icon2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 40)))
		page5_icon2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 150), CEGUI.UDim(0, 142)))
		page5_icon2:setProperty("Image", "set:liwu001 image:liwu_002")
	end
	m_newBieImage:addChildWindow(page5_icon2)
	-- 创建第六行icon描述控件
	local page5_tip2 = g_WinMgr:getWindow("newbie_task_background/page5/tip2")
	if nil == page5_tip2 then
		page5_tip2 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "newbie_task_background/page5/tip2")
		page5_tip2:setSize(CEGUI.UVector2(CEGUI.UDim(0, 30), CEGUI.UDim(0, 20)))
		page5_tip2:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 158), CEGUI.UDim(0, 184)))
		EQGlobaFunc.toMultiLineBox(page5_tip2:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(page5_tip2:getName()):setCols("#000E90")
		EQGlobaFunc.toMultiLineBox(page5_tip2:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(page5_tip2:getName()):addElementText("礼包", false)
	end
	m_newBieImage:addChildWindow(page5_tip2)
end
-----------------------------------------
-- 根据任务id设置领取界面内容
function FarmTask_setGetFarmTaskUI(task_id)
	-- 根据任务id获取任务
	local row = g_FarmTableMgr:getFarmTaskRow(task_id)
	if nil == row then
		return
	end
	-- 创建标题控件
	local title = g_WinMgr:getWindow("farm_get_task/taskTitle")
	if nil == title then
		title = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "farm_get_task/taskTitle")
		EQGlobaFunc.toMultiLineBox(title:getName()):setSize(280, 20)
		EQGlobaFunc.toMultiLineBox(title:getName()):setPosition(20, 30)
		EQGlobaFunc.toMultiLineBox(title:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(title:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(title:getName()):setAlign("AT_CENTER_VER")
		g_getFarmTaskUI:addChildWindow(title)
	end
	-- 设置标题内容
	EQGlobaFunc.toMultiLineBox(title:getName()):clearUp()	-- 标题
	g_mlbParse:getData(row.title, title:getName())
	-- 清除icon控件
	for i=1, 5 do	-- (这里的图片控件应该不会超过5个)
		g_WinMgr:destroyWindow("farm_get_task/taskIcon"..i)
	end
	-- 创建icon控件
	local num = g_FarmTableMgr:getFarmTaskImagesetCount(task_id)
	local spec = (280 - num*40)/(num+1)
	for i=1, num do
		local icon = g_WinMgr:createWindow("EQHomeLook/StaticImage", "farm_get_task/taskIcon"..i)
		icon:setSize(CEGUI.UVector2(CEGUI.UDim(0, 40), CEGUI.UDim(0, 40)))
		local xPos = i*spec + (i-1)*40
		icon:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos+20), CEGUI.UDim(0, 75)))
		local imageset = g_FarmTableMgr:getFarmTaskImageset(task_id, i-1)
		local image = g_FarmTableMgr:getFarmTaskImage(task_id, i-1)
		icon:setProperty("Image", "set:"..imageset.." image:"..image)
		g_getFarmTaskUI:addChildWindow(icon)
	end
	-- 创建描述控件
	local describe = g_WinMgr:getWindow("farm_get_task/taskDescribe")
	if nil == describe then
		describe = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "farm_get_task/taskDescribe")
		EQGlobaFunc.toMultiLineBox(describe:getName()):setSize(280, 50)
		EQGlobaFunc.toMultiLineBox(describe:getName()):setPosition(20, 140)
		EQGlobaFunc.toMultiLineBox(describe:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(describe:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(describe:getName()):setAlign("AT_CENTER_VER")
		g_getFarmTaskUI:addChildWindow(describe)
	end
	-- 设置描述内容
	EQGlobaFunc.toMultiLineBox(describe:getName()):clearUp()	-- 内容
	g_mlbParse:getData(row.describe, describe:getName())
	-- 设置界面
	g_FarmUI:addChildWindow(g_getFarmTaskUI)
	Common_setModalState(g_getFarmTaskUI:getName(), true)
	g_getFarmTaskUI:setAlwaysOnTop(true)
end
-----------------------------------------
-- 点击领取任务界面确定按钮
function FarmTask_getTaskDone(args)
	-- 设置界面
	g_FarmUI:removeChildWindow(g_getFarmTaskUI)
	Common_setModalState(g_getFarmTaskUI:getName(), false)
	-- 其他设置
	FarmTask_reqTaskComplete(FarmTask_getCurTaskId())	-- 完成当前任务
	FarmTask_setTaskButton(true)
end
-----------------------------------------
-- 点击领取任务取消按钮
function FarmTask_getTaskCancle(args)
	-- 设置界面
	g_FarmUI:removeChildWindow(g_getFarmTaskUI)
	FarmTask_changeGetFarmTaskUI(true)
	Common_setModalState(g_getFarmTaskUI:getName(), false)
end
-----------------------------------------
-- 改变领取任务界面显示
function FarmTask_changeGetFarmTaskUI(flag)
	if flag then	-- 领取任务
		g_WinMgr:getWindow("farm_get_task/done"):setVisible(true)
		g_WinMgr:getWindow("farm_get_task/cancle"):setVisible(true)
		g_WinMgr:getWindow("farm_get_task/sure"):setVisible(false)
		EQGlobaFunc.SetText(g_getFarmTaskUI:getName(), "领取任务")
	else	-- 查看任务
		g_WinMgr:getWindow("farm_get_task/done"):setVisible(false)
		g_WinMgr:getWindow("farm_get_task/cancle"):setVisible(false)
		g_WinMgr:getWindow("farm_get_task/sure"):setVisible(true)
		EQGlobaFunc.SetText(g_getFarmTaskUI:getName(), "查看任务")
		if g_WinMgr:isWindowPresent("farm_get_task/taskTitle") then
			EQGlobaFunc.toMultiLineBox("farm_get_task/taskTitle"):addElementText("（任务等待完成）","#FF0000",false)
		end
	end
end
-----------------------------------------
-- 根据任务id设置完成界面内容
function FarmTask_setCompleteFarmTaskUI(task_id)
	-- 根据任务id获取任务
	local row = g_FarmTableMgr:getFarmTaskRow(task_id)
	if nil == row then
		return
	end
	-- 创建标题控件
	local title = g_WinMgr:getWindow("farm_complete_task/taskTitle")
	if nil == title then
		title = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "farm_complete_task/taskTitle")
		EQGlobaFunc.toMultiLineBox(title:getName()):setSize(280, 35)
		EQGlobaFunc.toMultiLineBox(title:getName()):setPosition(20, 30)
		EQGlobaFunc.toMultiLineBox(title:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(title:getName()):setCols("#000000")
		EQGlobaFunc.toMultiLineBox(title:getName()):setAlign("AT_CENTER_VER")
		g_completeFarmTaskUI:addChildWindow(title)
	end
	-- 设置标题内容
	EQGlobaFunc.toMultiLineBox(title:getName()):clearUp()
	local titleText, spec = row.title, "~s"
	local s, e = string.find(row.title, spec)
	if nil ~= s then
		titleText = string.gsub(string.sub(row.title, 1, e), spec, tostring(row.exp))
		local temp = titleText..string.sub(row.title, e+1, -1)
		titleText = string.gsub(temp, spec, tostring(row.game_reward))
	end
	g_mlbParse:getData(titleText, title:getName())
	-- 设置经验icon
	local imageset = g_FarmTableMgr:getFarmTaskImageset(task_id, 0)
	local image = g_FarmTableMgr:getFarmTaskImage(task_id, 0)
	local icon = g_WinMgr:getWindow("farm_complete_task/image_exp")
	icon:setProperty("Image", "set:"..imageset.." image:"..image)
	-- 设置金币icon
	imageset = g_FarmTableMgr:getFarmTaskImageset(task_id, 1)
	image = g_FarmTableMgr:getFarmTaskImage(task_id, 1)
	icon = g_WinMgr:getWindow("farm_complete_task/image_coin")
	icon:setProperty("Image", "set:"..imageset.." image:"..image)
	-- 设置经验值
	EQGlobaFunc.SetText("farm_complete_task/quantity_exp", tostring(row.exp))
	-- 设置金币值
	EQGlobaFunc.SetText("farm_complete_task/quantity_coin", tostring(row.game_reward))
	-- 设置界面
	g_FarmUI:addChildWindow(g_completeFarmTaskUI)
	Common_setModalState(g_completeFarmTaskUI:getName(), true)
	g_completeFarmTaskUI:setAlwaysOnTop(true)
end
-----------------------------------------
-- 点击完成任务界面下一个任务按钮
function FarmTask_completeTaskNextTask(args)
	-- 设置界面
	g_FarmUI:removeChildWindow(g_completeFarmTaskUI)
	Common_setModalState(g_completeFarmTaskUI:getName(), false)
	-- 其他设置
	FarmTask_doScript("script4")	-- 执行脚本4
	FarmTask_doScript("script6")	-- 执行脚本6
	FarmTask_doScript("script8")	-- 执行脚本8
	FarmTask_doScript("script10")	-- 执行脚本10
	FarmTask_doScript("script12")	-- 执行脚本12
	FarmTask_doScript("script14")	-- 执行脚本14
	FarmTask_doScript("script16")	-- 执行脚本16
	FarmTask_doScript("script18")	-- 执行脚本18
	FarmTask_doScript("script20")	-- 执行脚本20
end
-----------------------------------------
-- 点击完成任务界面取消按钮
function FarmTask_completeTaskCancle(args)
	-- 设置界面
	g_FarmUI:removeChildWindow(g_completeFarmTaskUI)
	Common_setModalState(g_completeFarmTaskUI:getName(), false)
	-- 其他设置
	if 0 == FarmTask_getCurTaskId() then
		m_taskButton:setVisible(false)
	end
end
-----------------------------------------
-- 点击任务按钮
function FarmTask_clickTaskButton(args)
	-- 屏蔽鼠标右键
	if 1 == CEGUI.toMouseEventArgs(args).button then
		return
	end
	-- 如果当前任务为"领取任务"类型,打开领取任务界面
	local curTaskId = FarmTask_getCurTaskId()
	local row = g_FarmTableMgr:getFarmTaskRow(curTaskId)
	if nil == row then
		return
	end
	if 1001 == curTaskId then
		FarmTask_startNewBieUI()
		return
	end
	if "0" ~= row.describe then	-- 当前任务为"领取任务"类型
		FarmTask_setGetFarmTaskUI(curTaskId)
		FarmTask_changeGetFarmTaskUI(true)
	else	-- 当前任务为"完成任务"类型
		FarmTask_setGetFarmTaskUI(curTaskId-1)
		FarmTask_changeGetFarmTaskUI(false)
	end
end
-----------------------------------------
-- 设置点击按钮后按钮的显示状态
function FarmTask_setTaskButton(accept)
	if accept then	-- 已接受任务
		m_taskButton:setProperty("Image", "set:farm_task image:NewImage10")
	else	-- 未接受任务
		m_taskButton:setProperty("Image", "set:farm_task image:NewImage11")
	end
end
-----------------------------------------
--			农场任务事件模块
-----------------------------------------
local m_eventMap = {}	-- 事件映射表
-----------------------------------------
-- 签订事件
function FarmTask_subscribeEvent(event, func)
	for i, v in pairs(m_eventMap) do
		if event == v.event then	-- 事件已注册
			return
		end
	end
	-- 事件未注册
	local e = {}
	e.event, e.func = event, func	-- 事件名,处理函数
	table.insert(m_eventMap, e)
end
-----------------------------------------
-- 取消签订事件
function FarmTask_unSubscribeEvent(event)
	for i, v in pairs(m_eventMap) do
		if event == v.event then	-- 事件已注册
			table.remove(m_eventMap, i)
		end
	end
end
-----------------------------------------
-- 执行事件(局部)
local function _farmTask_fireEvent(event)
	for i, v in pairs(m_eventMap) do
		if event == v.event then	-- 事件已注册
			if nil ~= v.func then
				v.func()	-- 执行事件对应处理函数
			end
		end
	end
end
-----------------------------------------
-- 执行事件:flag(true表执行完事件后删除事件)
function FarmTask_fireEvent(event, flag)
	_farmTask_fireEvent(event)
	if flag then
		FarmTask_unSubscribeEvent(event)
	end
end
-----------------------------------------
--			农场任务操作模块
-----------------------------------------
-----------------------------------------
-- 请求农场任务列表
function FarmTask_reqTaskList()
	g_FarmMgr:reqFarmTaskList()
end
-----------------------------------------
-- 任务有更新
function FarmTask_taskUpdate()
	-- g_Loger:logMessage("--- FarmTask_taskUpdate() ---")
	local row = g_FarmTableMgr:getFarmTaskRow(FarmTask_getCurTaskId())
	if nil == row then	-- 无任务
		m_taskButton:setVisible(false)	-- 隐藏任务按钮
		return
	end
	-- 有任务
	if "0" == row.describe then	-- 结束的任务为"完成任务"类型
		FarmTask_setTaskButton(true)
	end
	m_taskButton:setVisible(true)	-- 显示任务按钮
	FarmTask_doScript("script1")	-- 执行脚本1
	if true == m_seeNextTask then	-- 判断是否查看下一条任务
		m_seeNextTask = false
		FarmTask_doScript("script2")	-- 执行脚本2
	end
end
-----------------------------------------
-- 请求任务完成
function FarmTask_reqTaskComplete(task_id)
	g_FarmMgr:reqFarmTaskComplete(task_id)
end
-----------------------------------------
-- 返回请求任务完成
function FarmTask_taskComplete(task_id)
	--g_Loger:logMessage("--- FarmTask_taskComplete(): "..task_id.." ---")
	-- 如果完成的任务为"完成任务"类型,显示完成任务界面
	if 1001 == task_id then	-- 初始任务,特殊处理
		FarmTask_setTaskButton(false)
		return
	end
	local row = g_FarmTableMgr:getFarmTaskRow(task_id)
	if "0" == row.describe then	-- 结束的任务为"完成任务"类型
		FarmTask_setTaskButton(false)
		FarmTask_setCompleteFarmTaskUI(task_id)	-- 显示完成任务界面
	end
end
-----------------------------------------
-- 农场任务数量
function FarmTask_taskCount()
	return g_FarmMgr:getFarmTaskCount()
end
-----------------------------------------
-- 获取当前任务id
function FarmTask_getCurTaskId()
	return g_FarmMgr:getFarmTaskIdByIndex(0)	-- 取0值是因为农场任务只能存在一个
end
-----------------------------------------
-- 是否存在任务
function FarmTask_existTask(task_id)
	--g_Loger:logMessage("--- FarmTask_existTask(): "..task_id.." ---")
	return g_FarmMgr:existFarmTask(task_id)
end
-----------------------------------------
-- 鼠标操作(每次都会触发)
function FarmTask_clickUnit(account, action)
	if m_actionType.till == action then	-- 铲除
		FarmTask_doScript("script5")	-- 执行脚本5
	elseif m_actionType.water == action then	-- 浇水
		if g_objmgr:isMainPlayer(account) then
			FarmTask_doScript("script11")	-- 执行脚本11
		else	-- 好友
			FarmTask_doScript("script17")	-- 执行脚本17
		end
	elseif m_actionType.weed == action then	-- 除草
		if g_objmgr:isMainPlayer(account) then
			FarmTask_doScript("script13")	-- 执行脚本13
		else	-- 好友
			FarmTask_doScript("script19")	-- 执行脚本19
		end
	elseif m_actionType.bug == action then		-- 除虫
		if g_objmgr:isMainPlayer(account) then
			FarmTask_doScript("script15")	-- 执行脚本15
		else	-- 好友
			FarmTask_doScript("script21")	-- 执行脚本21
		end
	elseif m_actionType.harvest == action then	-- 采摘
		FarmTask_doScript("script3")	-- 执行脚本3
	elseif m_actionType.seed == action then	-- 种植
		FarmTask_doScript("script9")	-- 执行脚本9
	end
end
-----------------------------------------
--			农场新手任务脚本模块
-----------------------------------------
-----------------------------------------
-- 执行某个脚本
function FarmTask_doScript(event)
	FarmTask_fireEvent(event, false)
end
-----------------------------------------
-- 注册所有脚本
function FarmTask_subscribeScripts()
	FarmTask_subscribeEvent("script1", FarmTask_script1)
	FarmTask_subscribeEvent("script2", FarmTask_script2)
	FarmTask_subscribeEvent("script3", FarmTask_script3)
	FarmTask_subscribeEvent("script4", FarmTask_script4)
	FarmTask_subscribeEvent("script5", FarmTask_script5)
	FarmTask_subscribeEvent("script6", FarmTask_script6)
	FarmTask_subscribeEvent("script7", FarmTask_script7)
	FarmTask_subscribeEvent("script8", FarmTask_script8)
	FarmTask_subscribeEvent("script9", FarmTask_script9)
	FarmTask_subscribeEvent("script10", FarmTask_script10)
	FarmTask_subscribeEvent("script11", FarmTask_script11)
	FarmTask_subscribeEvent("script12", FarmTask_script12)
	FarmTask_subscribeEvent("script13", FarmTask_script13)
	FarmTask_subscribeEvent("script14", FarmTask_script14)
	FarmTask_subscribeEvent("script15", FarmTask_script15)
	FarmTask_subscribeEvent("script16", FarmTask_script16)
	FarmTask_subscribeEvent("script17", FarmTask_script17)
	FarmTask_subscribeEvent("script18", FarmTask_script18)
	FarmTask_subscribeEvent("script19", FarmTask_script19)
	FarmTask_subscribeEvent("script20", FarmTask_script20)
	FarmTask_subscribeEvent("script21", FarmTask_script21)	
end
-----------------------------------------
-- 脚本1(新手引导)
function FarmTask_script1()
	if true == FarmTask_existTask(1001) then
		FarmTask_startNewBieUI()
	end
end
-----------------------------------------
-- 脚本2(接收任务：收获成熟的作物)
function FarmTask_script2()
	if true == FarmTask_existTask(1002) then
		FarmTask_setGetFarmTaskUI(1002)
	end
end
-----------------------------------------
-- 脚本3(完成任务：收获成熟的作物)
function FarmTask_script3()
	if true == FarmTask_existTask(1003) then
		FarmTask_reqTaskComplete(1003)
	end
end
-----------------------------------------
-- 脚本4(接收任务：铲除枯萎的作物)
function FarmTask_script4()
	if true == FarmTask_existTask(1004) then
		FarmTask_setGetFarmTaskUI(1004)
	end
end
-----------------------------------------
-- 脚本5(完成任务：铲除枯萎的作物)
function FarmTask_script5()
	if true == FarmTask_existTask(1005) then
		FarmTask_reqTaskComplete(1005)
	end
end
-------------------------------------
-- 脚本6(接收任务：卖出仓库里的果实)
function FarmTask_script6()
	if true == FarmTask_existTask(1006) then
		FarmTask_setGetFarmTaskUI(1006)
	end
end
g_farmTaskFinishSell = false
local m_flag = true
-------------------------------------
-- 脚本7(完成任务：卖出仓库里的果实)
function FarmTask_script7()
	if false == g_farmTaskFinishSell then
		g_farmTaskFinishSell = true
		return
	end
	if true == m_flag then
		m_flag = false
		if true == FarmTask_existTask(1007) then
			FarmTask_reqTaskComplete(1007)
		end
	end
end
-----------------------------------------
-- 脚本8(接收任务：种植新的作物)
function FarmTask_script8()
	if true == FarmTask_existTask(1008) then
		FarmTask_setGetFarmTaskUI(1008)
	end
end
-----------------------------------------
-- 脚本9(完成任务：种植新的作物)
function FarmTask_script9()
	if true == FarmTask_existTask(1009) then
		FarmTask_reqTaskComplete(1009)
	end
end
-----------------------------------------
-- 脚本10(接收任务：给自己干旱的作物浇水)
function FarmTask_script10()
	if true == FarmTask_existTask(1010) then
		FarmTask_setGetFarmTaskUI(1010)
	end
end
-----------------------------------------
-- 脚本11(完成任务：给自己干旱的作物浇水)
function FarmTask_script11()
	if true == FarmTask_existTask(1011) then
		FarmTask_reqTaskComplete(1011)
	end
end
-----------------------------------------
-- 脚本12(接收任务：给自己的作物除草)
function FarmTask_script12()
	if true == FarmTask_existTask(1012) then
		FarmTask_setGetFarmTaskUI(1012)
	end
end
-----------------------------------------
-- 脚本13(完成任务：给自己的作物除草)
function FarmTask_script13()
	if true == FarmTask_existTask(1013) then
		FarmTask_reqTaskComplete(1013)
	end
end
-----------------------------------------
-- 脚本14(接收任务：给自己的作物除虫)
function FarmTask_script14()
	if true == FarmTask_existTask(1014) then
		FarmTask_setGetFarmTaskUI(1014)
	end
end
-----------------------------------------
-- 脚本15(完成任务：给自己的作物除虫)
function FarmTask_script15()
	if true == FarmTask_existTask(1015) then
		FarmTask_reqTaskComplete(1015)
	end
end
-----------------------------------------
-- 脚本16(接收任务：给好友干旱的作物浇水)
function FarmTask_script16()
	if true == FarmTask_existTask(1016) then
		FarmTask_setGetFarmTaskUI(1016)
	end
end
-----------------------------------------
-- 脚本17(完成任务：给好友干旱的作物浇水)
function FarmTask_script17()
	if true == FarmTask_existTask(1017) then
		FarmTask_reqTaskComplete(1017)
	end
end
-----------------------------------------
-- 脚本18(接收任务：给好友的作物除草)
function FarmTask_script18()
	if true == FarmTask_existTask(1018) then
		FarmTask_setGetFarmTaskUI(1018)
	end
end
-----------------------------------------
-- 脚本19(完成任务：给好友的作物除草)
function FarmTask_script19()
	if true == FarmTask_existTask(1019) then
		FarmTask_reqTaskComplete(1019)
	end
end
-----------------------------------------
-- 脚本20(接收任务：给好友的作物除虫)
function FarmTask_script20()
	if true == FarmTask_existTask(1020) then
		FarmTask_setGetFarmTaskUI(1020)
	end
end
-----------------------------------------
-- 脚本21(完成任务：给好友的作物除虫)
function FarmTask_script21()
	if true == FarmTask_existTask(1021) then
		FarmTask_reqTaskComplete(1021)
		g_WinMgr:getWindow("farm_complete_task/next_task"):setVisible(false)
		g_WinMgr:getWindow("farm_complete_task/cancle"):setVisible(false)
		g_WinMgr:getWindow("farm_complete_task/last_task"):setVisible(true)
	end
end
-- g_GameEventMgr:subscribeEvent("DRAG_ITEM_UNDOED","Event_undo")
