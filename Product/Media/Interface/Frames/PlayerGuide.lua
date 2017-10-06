-----------------------------------------
-- 文件说明	：玩家向导
-- 作者		：何展然
-- 修改		：
-- 日期		：10/12/25
-----------------------------------------
local newTaskFlag = false
local m_nextstep_click = false
local m_prestep_click = false
local m_dopre = false
--事件注册
g_WinMgr:getWindow("map/3"):subscribeEvent("Clicked", "PlayerGuide_Open")
g_WinMgr:getWindow("newbiehelp__auto_closebutton__"):subscribeEvent("Clicked", "PlayerGuide_Close")
g_WinMgr:getWindow("newbiehelp/zhuchuangkou/help1"):subscribeEvent("Clicked", "PlayerGuide_MouseOperator")
g_WinMgr:getWindow("newbiehelp/zhuchuangkou/help2"):subscribeEvent("Clicked", "PlayerGuide_ButtonUsage")
g_WinMgr:getWindow("newbiehelp/zhuchuangkou/help3"):subscribeEvent("Clicked", "PlayerGuide_ViewTask")
g_WinMgr:getWindow("newbiehelp/zhuchuangkou/help4"):subscribeEvent("Clicked", "PlayerGuide_ChangeClothes")
g_WinMgr:getWindow("newbiehelp/zhuchuangkou/help5"):subscribeEvent("Clicked", "PlayerGuide_PlaceFurniture")
g_WinMgr:getWindow("newbiehelp/zhuchuangkou/help6"):subscribeEvent("Clicked", "PlayerGuide_ViewCityMap")
g_WinMgr:getWindow("newbiehelp/zhuchuangkou/likai"):subscribeEvent("Clicked", "PlayerGuide_Close")
local m_guideWin = g_WinMgr:getWindow("GuideWindow")
m_guideWin:subscribeEvent("MouseClick", "PlayerGuide_clickImage")
--
MainUI_ShieldClick(g_NewBieHelpUI)
MainUI_ShieldClick(g_PlayerguideUI)
MainUI_ShieldClick(m_guideWin)

local m_showType = 0	-- 图像显示类型(0.自动显示, 1.固定显示)
local m_showPicture = false	-- 是否在show图
local m_imageData = {}	-- 图像数据表
local m_ctrl = EQGlobaClass.createImageCombineCtrl()	-- 图像组合控件

-----------------------------------------
-- 根据id获取图片
local function _PlayerGuide_getPictureData(id)
	--g_Loger:logError("******^^^1"..tostring(id))
	local row = g_logictableMgr:getPlayerGuideRow(id)
	if nil == row then
		return nil
	end
	local data = {}
	data["row"] = row.row
	data["col"] = row.col
	data["set"] = {}
	data["img"] = {}
	local num = row.row * row.col
	for i=0, num-1 do
		table.insert(data["set"], g_logictableMgr:getPlayerGuideImageset(id, i))
		table.insert(data["img"], g_logictableMgr:getPlayerGuideImage(id, i))
	end
	return data
end
-----------------------------------------
-- 根据id显示图片
function PlayerGuide_ShowPicture(picId)
	g_Loger:logMessage("--- picId: "..tostring(picId).." ---")
	local data = _PlayerGuide_getPictureData(picId)
	if nil ~= data then
		PlayerGuide_ShowPictureByData(data)
	end
end
-----------------------------------------
-- 根据data显示图片,data:{row=0,col=0,set={"",...}, img={"",...}}
function PlayerGuide_ShowPictureByData(data)
	table.insert(m_imageData, data)
	if false == m_showPicture then
		m_showPicture = true
		if false == PlayerGuide_setCtrlImage(m_ctrl, m_imageData[1]) then
			g_Loger:logError("at function PlayerGuide_setCtrlImage()...")
			return
		end
		g_MainUIWindows:addChildWindow(g_PlayerguideUI)	
	end
end
-----------------------------------------
-- 点击图片，查看下一张
function PlayerGuide_clickImage(args)
	-- 屏蔽鼠标非左键
	if 0 ~= CEGUI.toMouseEventArgs(args).button then
		return
	end
	table.remove(m_imageData, 1)
	-- 没有图片,说明图片播放完毕
	if false == PlayerGuide_setCtrlImage(m_ctrl, m_imageData[1]) then
		g_MainUIWindows:removeChildWindow(g_PlayerguideUI)
		m_showPicture = false
		if 0 == m_showType then
			if g_GuideTreeItmClicked == false then
				PlayerGuide_fireEvent("PICTURE_SHOW_OVER", false)	-- 执行图片显示结束事件
			else
				PlayerGuide_fireEvent("Guide_PICTURE_SHOW_OVER", false)
				g_GuideTreeItmClicked = false
			end
		elseif 1 == m_showType then
			Common_setModalState(g_NewBieHelpUI:getName(), true)
		end
	end
end
-----------------------------------------
-- 设置组合控件图片,data:{row=0,col=0,set={"",...}, img={"",...}}
function PlayerGuide_setCtrlImage(ctrl, data)
	ctrl:clear()
	m_guideWin:setProperty("Image", "set: image:")
	m_guideWin:setAlwaysOnTop(true)
	if nil == data then
		return false
	end
	local r, c, set, img = data.row, data.col, data.set, data.img
	if 0==r or 0==c then
		return false
	end
	if r*c~=table.getn(set) or r*c~=table.getn(img) then
		return false
	end
	ctrl:init(r, c, m_guideWin:getName())
	for i=1, r*c do
		if 0~= ctrl:addImage(set[i], img[i]) then
			return false
		end
	end
	return true
end
-----------------------------------------
-- 打开新手帮助界面
function PlayerGuide_Open(args)
	g_MainUIWindows:addChildWindow(g_NewBieHelpUI)
	Common_setModalState(g_NewBieHelpUI:getName(), true)
	m_showType = 1
end
-----------------------------------------
-- 关闭新手帮助界面
function PlayerGuide_Close(args)
	g_MainUIWindows:removeChildWindow(g_NewBieHelpUI)
	Common_setModalState(g_NewBieHelpUI:getName(), false)
	m_showType = 0
end
-----------------------------------------
-- 鼠标操作帮助
function PlayerGuide_MouseOperator(args)
	PlayerGuide_ShowPicture(1)
	Common_setModalState(g_NewBieHelpUI:getName(), false)
end
-----------------------------------------
-- 按钮操作帮助
function PlayerGuide_ButtonUsage(args)
	PlayerGuide_ShowPicture(2)
	PlayerGuide_ShowPicture(3)
	Common_setModalState(g_NewBieHelpUI:getName(), false)
end
-----------------------------------------
-- 任务查看帮助
function PlayerGuide_ViewTask(args)
	PlayerGuide_ShowPicture(4)
	PlayerGuide_ShowPicture(5)	
	Common_setModalState(g_NewBieHelpUI:getName(), false)
end
-----------------------------------------
-- 换装帮助
function PlayerGuide_ChangeClothes(args)
	PlayerGuide_ShowPicture(6)
	Common_setModalState(g_NewBieHelpUI:getName(), false)
end
-----------------------------------------
-- 家具摆放帮助
function PlayerGuide_PlaceFurniture(args)
	PlayerGuide_ShowPicture(7)
	PlayerGuide_ShowPicture(8)	
	Common_setModalState(g_NewBieHelpUI:getName(), false)
end
-----------------------------------------
-- 地图查看帮助
function PlayerGuide_ViewCityMap(args)
	PlayerGuide_ShowPicture(9)
	Common_setModalState(g_NewBieHelpUI:getName(), false)
end
-----------------------------------------
--			新手教学事件模块
-----------------------------------------
local m_eventMap = {}	-- 事件映射表
-----------------------------------------
-- 签订事件
function PlayerGuide_subscribeEvent(event, func)
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
function PlayerGuide_unSubscribeEvent(event)
	for i, v in pairs(m_eventMap) do
		if event == v.event then	-- 事件已注册
			table.remove(m_eventMap, i)
		end
	end
end
-----------------------------------------
-- 执行事件(局部)
local function _playerGuide_fireEvent(event)
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
function PlayerGuide_fireEvent(event, flag)
	_playerGuide_fireEvent(event)
	if flag then
		PlayerGuide_unSubscribeEvent(event)
	end
end
-----------------------------------------
--			新手教学逻辑模块
-----------------------------------------
local m_openCiWeiUI = false	-- 是否显示刺猬界面
local m_ciWeiUITable = {}	-- 刺猬界面里的文字内容
local m_event = nil	-- 当前事件
local m_pre_event = nil
local m_excute = false	-- 是否执行当前事件
local m_showOver = false
local mCiWeiUIWidth = 0
local mCiWeiUIHeight = 0
-----------------------------------------
-- 初始刺猬教学界面(局部)
local function _playerGuide_initCiWeiUI()
	local win = g_WinMgr:getWindow("ciweijiaoxue/content")
	if nil == win then
		win = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "ciweijiaoxue/content")
		EQGlobaFunc.toMultiLineBox(win:getName()):setSize(233, 95)
		EQGlobaFunc.toMultiLineBox(win:getName()):setPosition(20, 30)
		EQGlobaFunc.toMultiLineBox(win:getName()):setFont("simsun-12")
		EQGlobaFunc.toMultiLineBox(win:getName()):setCols("#000000")
		g_CiWeiTeachUI:addChildWindow(win)
		local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
		local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
		mCiWeiUIWidth = g_CiWeiTeachUI:getWidth():asAbsolute(screenW)
		mCiWeiUIHeight = g_CiWeiTeachUI:getHeight():asAbsolute(screenH)
		--g_CiWeiTeachUI:setPosition(CEGUI.UVector2(CEGUI.UDim(1, -470), CEGUI.UDim(1, -(140+mCiWeiUIHeight))))
		g_CiWeiTeachUI:setPosition(CEGUI.UVector2(CEGUI.UDim(1, -390), CEGUI.UDim(1, -(40+mCiWeiUIHeight))))
		--SetWindowCenter(g_CiWeiTeachUI)
		--g_CiWeiTeachUI:setAlwaysOnTop(true)
		g_WinMgr:getWindow("ciweijiaoxue/nextBtn"):subscribeEvent("Clicked", "PlayerGuide_viewNextStep")
		g_WinMgr:getWindow("ciweijiaoxue/preBtn"):subscribeEvent("Clicked", "PlayerGuide_viewPreStep")
		MainUI_ShieldClick(g_CiWeiTeachUI)
	end
	EQGlobaFunc.toMultiLineBox(win:getName()):clearUp()
	return win:getName()
end
-----------------------------------------
-- 打开刺猬教学界面
function PlayerGuide_openCiWeiUI(id)
	g_Loger:logMessage("--- id: "..id.." ---")
	local row = g_logictableMgr:getNoviceTeachStepRow(id)
	if nil ~= row then
		table.insert(m_ciWeiUITable, row.content)
		if false == m_openCiWeiUI then
			m_openCiWeiUI = true
			g_CiWeiTeachUI:setAlwaysOnTop(true)
			--Common_setModalState(g_CiWeiTeachUI:getName(), true)
			g_mlbParse:getData(m_ciWeiUITable[1], _playerGuide_initCiWeiUI())
			if row.preBtnStr ~= "" then
				EQGlobaFunc.SetText("ciweijiaoxue/preBtn",row.preBtnStr)
				g_WinMgr:getWindow("ciweijiaoxue/preBtn"):setVisible(true)
			else
				EQGlobaFunc.SetText("ciweijiaoxue/preBtn","")
				g_WinMgr:getWindow("ciweijiaoxue/preBtn"):setVisible(false)
			end
			EQGlobaFunc.SetText("ciweijiaoxue/nextBtn",row.nextBtnstr)
			g_MainUIWindows:addChildWindow(g_CiWeiTeachUI)
			if Fitmode_getPutFunitureFlag() then
				--SetWindowCenter(g_CiWeiTeachUI)
				--g_CiWeiTeachUI:setPosition(CEGUI.UVector2(CEGUI.UDim(1, -390), CEGUI.UDim(1, -(40+mCiWeiUIHeight))))
			else
				--SetWindowCenter(g_CiWeiTeachUI)
				--g_CiWeiTeachUI:setPosition(CEGUI.UVector2(CEGUI.UDim(1, -390), CEGUI.UDim(1, -(40+mCiWeiUIHeight))))
			end
			--PlayerGuide_SetOtherBtnEnbled(false)
			PlayerGuide_enabledUI(false)
		else
			PlayerGuide_clear()
			PlayerGuide_updateShowContent(id)
		end
	end
end
-----------------------------------------
-- 点击刺猬教学界面下一步
function PlayerGuide_viewNextStep(args)
	-- 屏蔽鼠标右键
	if 1 == CEGUI.toMouseEventArgs(args).button then
		return
	end
	PlayerGuide_NextStepPass()
end
-----------------------------------------
--
function PlayerGuide_NextStepPass()
	m_nextstep_click = true
	table.remove(m_ciWeiUITable, 1)
	if nil == m_ciWeiUITable[1] then	-- 没有可显示文字
		m_openCiWeiUI = false
		--Common_setModalState(g_CiWeiTeachUI:getName(), false)
		--g_MainUIWindows:removeChildWindow(g_CiWeiTeachUI)
		PlayerGuide_fireEvent("CLOSE_CIWEIUI", false)
	else
		g_mlbParse:getData(m_ciWeiUITable[1], _playerGuide_initCiWeiUI())
	end
end

-----------------------------------------
--点击查看上一步
function PlayerGuide_viewPreStep(args)
	m_prestep_click = true
	-- 屏蔽鼠标右键
	if 1 == CEGUI.toMouseEventArgs(args).button then
		return
	end
	table.remove(m_ciWeiUITable, 1)
	if nil == m_ciWeiUITable[1] then	-- 没有可显示文字
		m_openCiWeiUI = false
		--Common_setModalState(g_CiWeiTeachUI:getName(), false)
		PlayerGuide_fireEvent("CLOSE_CIWEIUI", false)
	else
		g_mlbParse:getData(m_ciWeiUITable[1], _playerGuide_initCiWeiUI())
	end
end
-----------------------------------------
-- 关闭刺猬教学界面
function PlayerGuide_closeCiWeiUI()
	m_showOver = true
	m_openCiWeiUI = false
	g_MainUIWindows:removeChildWindow(g_CiWeiTeachUI)
	--PlayerGuide_SetOtherBtnEnbled(true)
	PlayerGuide_enabledUI(true)
	if m_prestep_click then
		PlayerGuide_setDopre(true)
		reset_preBtnclick()
	elseif m_nextstep_click then
		PlayerGuide_setExcute(true)
		reset_nextBtnclick()
	end
end
----
function PlayerGuide_isCiWeiUIOpened()
	return m_openCiWeiUI
end
-----------------------------------------
-- 图片显示结束
function PlayerGuide_pictureShowOver()
	m_showOver = true
	--if m_prestep_click then
	--	PlayerGuide_setDopre(true)
	--elseif m_nextstep_click then
		PlayerGuide_setExcute(true)
	--end
end
-----------------------------------------
--
function PlayerGuide_setDopre(dopre)
	if false == m_showOver then
		return
	end
	m_dopre = dopre
end
-----------------------------------------
-- 设置事件
function PlayerGuide_setEvent(event)
	m_event = event
end
-----------------------------------------
-- 设置是否执行事件
function PlayerGuide_setExcute(excute)
	if false == m_showOver then
		return
	end
	m_excute = excute
end
-----------------------------------------
-- 监听事件
function PlayerGuide_listen()
	if m_dopre then
		--g_Loger:logError("**dopre*")
		--g_Loger:logError("PlayerGuide_listen" ..tostring(m_pre_event))
		if m_pre_event ~= nil then
			PlayerGuide_fireEvent(m_pre_event, true)
			--PlayerGuide_setPreEvent(nil)
		end
		m_dopre = false
		return
	end
	if m_excute then
		g_Loger:logError("**doNext*")
		m_excute = false
		PlayerGuide_fireEvent(m_event, true)
	end
end
-----------------------------------------
--		提供给策划的任务脚本指令
-----------------------------------------
-- 注册所有脚本
function task_subscribe_scripts()
	PlayerGuide_subscribeEvent("PICTURE_SHOW_OVER", PlayerGuide_pictureShowOver)
	PlayerGuide_subscribeEvent("Guide_PICTURE_SHOW_OVER", PlayerGuide_guidePictureShowOver)
	PlayerGuide_subscribeEvent("CLOSE_CIWEIUI", PlayerGuide_closeCiWeiUI)
	PlayerGuide_subscribeEvent("script1", script1)
	PlayerGuide_subscribeEvent("script2", script2)
	PlayerGuide_subscribeEvent("script3", script3)
	PlayerGuide_subscribeEvent("script4", script4)
	PlayerGuide_subscribeEvent("script5", script5)
	PlayerGuide_subscribeEvent("script6", script6)
	PlayerGuide_subscribeEvent("script7", script7)
	PlayerGuide_subscribeEvent("script8", script8)
	-- 新手任务二期
	PlayerGuide_subscribeEvent("script9", script9)
	PlayerGuide_subscribeEvent("script10", script10)
	PlayerGuide_subscribeEvent("script11", script11)
	PlayerGuide_subscribeEvent("script12", script12)
	PlayerGuide_subscribeEvent("script13", script13)
	PlayerGuide_subscribeEvent("script14", script14)
	PlayerGuide_subscribeEvent("script15", script15)
	PlayerGuide_subscribeEvent("script16", script16)
	PlayerGuide_subscribeEvent("script17", script17)
	PlayerGuide_subscribeEvent("script18", script18)
	PlayerGuide_subscribeEvent("script19", script19)
	PlayerGuide_subscribeEvent("script20", script20)
	PlayerGuide_subscribeEvent("script21", script21)
	PlayerGuide_subscribeEvent("script22", script22)
	PlayerGuide_subscribeEvent("script23", script23)
	PlayerGuide_subscribeEvent("script24", script24)
	PlayerGuide_subscribeEvent("script25", script25)
	PlayerGuide_subscribeEvent("script26", script26)
	PlayerGuide_subscribeEvent("script27", script27)
	PlayerGuide_subscribeEvent("script28", script28)
	PlayerGuide_subscribeEvent("script29", script29)
	PlayerGuide_subscribeEvent("script30", script30)
	PlayerGuide_subscribeEvent("script31", script31)
	PlayerGuide_subscribeEvent("script32", script32)
	PlayerGuide_subscribeEvent("script33", script33)
	
	-- 新手教学任务
	PlayerGuide_subscribeEvent("script15000", script15000)
	PlayerGuide_subscribeEvent("script15001", script15001)
	PlayerGuide_subscribeEvent("script15002", script15002)
	PlayerGuide_subscribeEvent("script15003", script15003)
	PlayerGuide_subscribeEvent("script15004", script15004)
	PlayerGuide_subscribeEvent("script15005", script15005)
	PlayerGuide_subscribeEvent("script15006", script15006)
	PlayerGuide_subscribeEvent("script15007", script15007)
	PlayerGuide_subscribeEvent("script15008", script15008)
	PlayerGuide_subscribeEvent("script15009", script15009)
	PlayerGuide_subscribeEvent("script15010", script15010)
	PlayerGuide_subscribeEvent("script15011", script15011)
	PlayerGuide_subscribeEvent("script15012", script15012)
	PlayerGuide_subscribeEvent("script15013", script15013)
	PlayerGuide_subscribeEvent("script15014", script15014)
	
	PlayerGuide_subscribeEvent("script17500", script17500)
	PlayerGuide_subscribeEvent("script17501", script17501)
	PlayerGuide_subscribeEvent("script17502", script17502)
	PlayerGuide_subscribeEvent("script17503", script17503)
	PlayerGuide_subscribeEvent("script17504", script17504)
	PlayerGuide_subscribeEvent("script17505", script17505)
	PlayerGuide_subscribeEvent("script17506", script17506)
	PlayerGuide_subscribeEvent("script17507", script17507)
	PlayerGuide_subscribeEvent("script17508", script17508)
	PlayerGuide_subscribeEvent("script17509", script17509)
	PlayerGuide_subscribeEvent("script17510", script17510)
end
-----------------------------------------
--
function PlayerGuide_guidePictureShowOver()
	PlayerGuide_pictureShowOver()
end
-----------------------------------------
-- 执行某个脚本
function do_task_script(event)
	PlayerGuide_fireEvent(event, false)
end
-----------------------------------------
-- 任务是否存在
function is_exist_task(id)
	return Task_existTask(id)
end
-----------------------------------------
-- 完成任务
function complete_task(id)
	Task_reqCompleteTask(id)
end
-----------------------------------------
-- 是否为当前完成的任务
function is_cur_complete(id)
	return Task_isCurComplete(id)
end
-----------------------------------------
-- 定位任务
function locate_task(id)
	return Task_locateTask(id)
end
-----------------------------------------
-- 秀图
function show_picture(id)
	--g_Loger:logError("******^^^"..tostring(id))
	PlayerGuide_ShowPicture(id)
	m_showOver = false
end
-----------------------------------------
-- 打开刺猬界面
function open_ciweiui(id)
	PlayerGuide_openCiWeiUI(id)
	m_showOver = false
end
-----------------------------------------
--指定上一步
function pre_step(event,func)
	PlayerGuide_subscribeEvent(event, func)
	--g_Loger:logError("pre_step" ..event)
	PlayerGuide_setPreEvent(event)
	if m_showOver then
		m_dopre = true
	else
		m_dopre = false
	end
end
-----------------------------------------
--
function PlayerGuide_setPreEvent(event)
	--g_Loger:logError("PlayerGuide_setPreEvent**" ..tostring(event))
	m_pre_event = event
end
-----------------------------------------
-- 指定下一步
function next_step(event, func)
	--g_Loger:logError("*****" ..tostring(event))
	PlayerGuide_subscribeEvent(event, func)
	PlayerGuide_setEvent(event)
	if m_showOver then
		m_excute = true
	else
		m_excute = false
	end
end
-----------------------------------------
-- 指定下一步(扩展)
function next_step_ex(event, func)
	PlayerGuide_subscribeEvent(event, func)
	PlayerGuide_setEvent(event)
end
-----------------------------------------
-- 玩家性别
function is_male_player()
	return g_mainplayer:isMale()
end
-----------------------------------------
-- 玩家是否在家
function is_at_home()
	return g_SceneMgr:inMyHouse()
end
-----------------------------------------
--
function is_nextstep_click()
	return m_nextstep_click
end
------------------------------------------
--
function is_prestep_click()
	return m_prestep_click
end
------------------------------------------
--
function reset_nextBtnclick()
	m_nextstep_click = false
end
------------------------------------------
--
function reset_preBtnclick()
	m_prestep_click = false
end
-------------------------------------------
--
--function PlayerGuide_SetOtherBtnEnbled(enable)
--	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Gohome"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Navigate"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_PlayerProperty"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_House"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Storage"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Mall"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_PrePaid"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Action"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Friendlist"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_System"):setEnabled(enable)
--	if g_sceneMgr:getSceneType() ~= EQGlobaClass.EQGameSceneMgr.SCENE_COMMON and g_isInFriendHouse == false then
--		g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):setEnabled(enable)
--		g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Storage"):setEnabled(enable)
--	end
--	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PlayGame"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Email"):setEnabled(enable)
--	g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang2"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/telephone"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof"):setEnabled(enable)
--	g_WinMgr:getWindow("MainRoot/liwu"):setEnabled(enable)
--	g_WinMgr:getWindow("DFwnd_MainRoot/xiala1"):setEnabled(enable)
--	g_WinMgr:getWindow("map"):setEnabled(enable)
--	if g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):isVisible() then
--		g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):setEnabled(enable)
--	end
--	FitModeShopCenter_closeIntroUI()	-- 关闭家具介绍界面(2011/4/15 add by hezhr)
--end
-------------------------------------------
--
function PlayerGuide_do_useLightScript()
	do_task_script("script29")
end
-------------------------------------------
--
function PlayerGuide_do_EnterGMHouseScript()
	do_task_script("script33")
end
--------------------------------------------
--
function PlayerGuide_do_ReceveTaskScript(task_id)
	newTaskFlag = true
	--g_Loger:logMessage("-------------rec_task_id**" ..tostring(task_id))
	--if task_id == 1005 or task_id == 1105 then
		--do_task_script("")
	--end
	--if task_id == 1103 then
	--	g_Loger:logMessage("---***")
	--	m_ciWeiUITable = {}
	--	PlayerGuide_closeCiWeiUI()
	--	open_ciweiui(5)
	--	next_step("script2_step4", script2_step4)
	--end

end

--------------------------------------------
--新手教学任务
function PlayerGuide_do_NewerTeachTaskScript(task_id)
	g_Loger:logMessage("PlayerGuide_do_NewerTeachTaskScript:" .. tostring(task_id))
	if is_exist_task(task_id) then
		if m_openCiWeiUI == false then
	  	--运行策划脚本
	  	do_task_script("script" .. tostring(task_id))
	  end
	end
end
------------------------------------------------
--
function PlayerGuide_getNewTaskFlag()
	return newTaskFlag
end
------------------------------------------------
--
function PlayerGuide_setNewTaskFlag(flag)
	newTaskFlag = flag
end
--
function PlayerGuide_do_completeTaskScript(task_id)
	g_Loger:logMessage("-------------com_task_id**" ..tostring(task_id))
	if task_id == 1005 or task_id == 1105 then
		--do_task_script("")
	end
	if task_id == 1102 then
		--g_Loger:logMessage("---***")
		--m_ciWeiUITable = {}
		--PlayerGuide_closeCiWeiUI()
		
		--do_task_script("script2")
		--open_ciweiui(5)
		--next_step("script2_step4", script2_step4)
	end
	if task_id == 1109 then
		g_Loger:logMessage("----------complete(1109)----_________--")
		if CreatePlayer_getFirstInflag() then
			CreatePlayer_setFirstInflag(false)
		end
		FitModeShopCenter_RestoreDayNight()
	end
end
------
function PlayerGuide_clear()
	m_ciWeiUITable = {}
end
-----
function PlayerGuide_updateShowContent(id)
	local row = g_logictableMgr:getNoviceTeachStepRow(id)
	if nil ~= row then
		--table.insert(m_ciWeiUITable, row.content)
		g_mlbParse:getData(row.content, _playerGuide_initCiWeiUI())
	end
end