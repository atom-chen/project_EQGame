-----------------------------------------
-- 文件说明	：任务相关逻辑处理
-- 作者		：lf
-- 修改		：
-- 日期		：10/06/25
-----------------------------------------
local step_img_maptable = {}
local m_taskList = CEGUI.toListbox(g_WinMgr:getWindow("Taskboardroot/Taskboard/list"))
local m_taskContent = g_WinMgr:getWindow("Taskboardroot/Taskboard/content")
g_taskVec:initialise(m_taskContent:getName())	-- 任务面板容器初始化
g_taskVec:setScrollbarVisible("SST_AUTO")	-- 设置任务面板容器滚动条自动显示
local m_taskCtrlWidth = 185 - g_taskVec:getScrollbarWidth()	-- 任务面板容器中控件的宽度
local m_task = g_mainplayer:getTask()
local m_openTaskUI = false	--	是否打开任务界面标识
local m_isDoOpenUI = false
local m_taskSelect = 0	-- 选择任务的id
local m_animateWin = nil	-- 任务提示动画窗口
local m_animate = EQGlobaClass.createAnimate()	-- 任务提示动画
local m_firstReqTask = true	-- 第一次请求任务列表
g_WinMgr:getWindow("Taskboardroot/Taskboard/giveup"):setVisible(false)	-- 隐藏(hezhr)
g_WinMgr:getWindow("Taskboardroot/Taskboard/trace"):setVisible(false)	-- 隐藏(hezhr)
g_WinMgr:getWindow("DFwnd_MainRoot/GuideTreeBtn"):subscribeEvent("Clicked", "Task_hideGuideTreeUI")
-----------------------------------------
-- 脚本加载后执行
-----------------------------------------
function Task_Onload()
	MainUI_ShieldClick(g_Task)
	MainUI_ShieldClick(g_WinMgr:getWindow("DFwnd_MainRoot/Task_guide_win/Task_guide_Tree"))
	-- 事件注册
	g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):subscribeEvent("Clicked", "Task_ClickOpenBtn")
	g_WinMgr:getWindow("Taskboardroot/Taskboard__auto_closebutton__"):subscribeEvent("Clicked", "Task_Close")
	g_WinMgr:getWindow("Taskboardroot/Taskboard/list"):subscribeEvent("ItemSelectionChanged", "Task_Select")
	g_WinMgr:getWindow("Taskboardroot/Taskboard/giveup"):subscribeEvent("Clicked", "Task_GiveUp")
	g_WinMgr:getWindow("DFwnd_MainRoot/Task_guide_win"):setVisible(false)
	g_WinMgr:getWindow("DFwnd_MainRoot/GuideTreeBtn"):setVisible(false)
	-- 更新列表
	g_GameEventMgr:subscribeEvent("UPDATE_TASK_UI", "Task_Update")
	m_taskList:setMultiselectEnabled(false)
end
-----------------------------------------
-- 有任务时，提示动画(hezhr)
-----------------------------------------
function Task_TipAnimate(animate)
	animate:initAnimate(700, 120, 64)
	animate:addAnimateFrame("Button5", "NewImage429")
	animate:addAnimateFrame("Button5", "NewImage429") --NewImage188

	m_animateWin = g_WinMgr:getWindow("Task/TipAnimate")
	if nil == m_animateWin then
		m_animateWin = g_WinMgr:createWindow("EQHomeLook/AnimateBox", "Task/TipAnimate")
		m_animateWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, 120), CEGUI.UDim(0, 64)))
		EQGlobaFunc.toAnimateBox(m_animateWin:getName()):addAnimate(animate)
		--m_animateWin:setMousePassThroughEnabled(true)
		m_animateWin:subscribeEvent("MouseClick", "Task_clickTaskTipPaoPao")
	end
	local task_btn_posx = g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):getXPosition()
	local task_btn_posy = g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):getYPosition()
	local task_btn_width = g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):getWidth()
	m_animateWin:setXPosition(task_btn_posx + task_btn_width - CEGUI.UDim(0,30))
	m_animateWin:setYPosition(task_btn_posy - CEGUI.UDim(0,56))
	g_MainUIWindows:addChildWindow(m_animateWin)
	EQGlobaFunc.toAnimateBox(m_animateWin:getName()):play(true)
end
-----------------------------------------
--
function Task_clickTaskTipPaoPao()
	Task_ClickOpenBtn()
end
-- 该调用放在函数ChatSet_MsgAnimate()定义后面
Task_TipAnimate(m_animate)
-----------------------------------------
-- 点击任务按钮
-----------------------------------------
function Task_ClickOpenBtn(args)
	if m_openTaskUI then
		--Task_Close(nil)
	else
		ReqOpenTaskUI()
		UIM_DoOpenAndCloseUI(g_TaskUINames.root)
	end
end
-----------------------------------------
-- 打开任务面板界面
-----------------------------------------
function Task_OpenUI()
	Task_Update()
	if m_isDoOpenUI then
		if false == m_openTaskUI then
			g_WinMgr:getWindow("Taskboardroot/Taskboard/giveup"):setVisible(false)
			OpenTaskUI()
		end
		m_isDoOpenUI = false
		do_task_script("script2")	-- (任务面板)执行新手任务脚本2
	end
end

function ReqOpenTaskUI()
	m_isDoOpenUI = true
	m_task:requestTask()	-- 向服务器请求任务
end

function OpenTaskUI()
	UIM_OpenWindow(g_TaskUINames.root)
	SetWindowCenter(g_WinMgr:getWindow(g_TaskUINames.root))
	m_animateWin:setVisible(false)
	PlayerGuide_setNewTaskFlag(false)
	EQGlobaFunc.toAnimateBox(m_animateWin:getName()):stop()
	m_openTaskUI = true
end

function CloseTaskUI()
	UIM_CloseWindow(g_TaskUINames.root)
	m_openTaskUI = false
end
-----------------------------------------
-- 关闭任务面板界面
-----------------------------------------
function Task_Close(args)
	CloseTaskUI()
	Task_ClearTaskContent()
	m_taskSelect = 0
	EQGlobaFunc.toAnimateBox(m_animateWin:getName()):play(true)		
end
-----------------------------------------
-- 刷新任务列表
local initTaskGuideTree=false
-----------------------------------------
function Task_Update(args)
	m_task = g_mainplayer:getTask()
	if m_firstReqTask then
		m_firstReqTask = false
		do_task_script("script1")	-- (进入游戏)执行新手任务脚本1(add by hezhr)
	end
	m_taskList:resetList()
	local show_task_count = 0	-- 可显示任务数目(add by hezhr)
	local taskGuideTree_id = 1
	for i=0, m_task:getTaskCount()-1 do
		
		local id = m_task:getTaskIDByIndex(i)
--		g_Loger:logError("****************taskid***********----" ..tostring(id))
		local taskrow = g_tasktableMgr:getTaskRow(id)
		if nil ~= taskrow then
			if 1 == taskrow.is_show then	-- 任务可显示
				g_Loger:logError("------iiiiii0-----"..tostring(i))
				show_task_count = show_task_count + 1
				if not initTaskGuideTree then
					m_task:initCeguiTree("DFwnd_MainRoot/Task_guide_win/Task_guide_Tree","任务指引","simsun-12")
					initTaskGuideTree = true
				end
				g_WinMgr:getWindow("DFwnd_MainRoot/Task_guide_win"):setVisible(true)
				g_WinMgr:getWindow("DFwnd_MainRoot/GuideTreeBtn"):setVisible(true)
				local r_val = 255.0
				local g_val = 255.0
				local b_val = 255.0
				local col = CEGUI.colour:new_local(r_val, g_val, b_val, 1)
				local crect = CEGUI.ColourRect(col)
				local crect_str = CEGUI.PropertyHelper:colourRectToString(crect)
				m_task:Task_addTaskGuideTitleTreeItem(taskGuideTree_id,"",taskrow.title,"simsun-12",crect_str,"","")
				--Task_getTaskGuideSubTable()
				Task_addTaskGuidesubStepItem(taskGuideTree_id,taskrow.title,taskrow.sub_id)
				taskGuideTree_id = taskGuideTree_id + 1
--				g_Loger:logError("taskGuideTree_id-------------" ..tostring(taskGuideTree_id))
				EQGlobaFunc.createListboxTextItem(m_taskList:getName(), taskrow.title, taskrow.id)
				m_task:setSelTask(taskrow.id)
				--g_Loger:logError("------iiiiii0-----"..tostring(i))
				--if i == 0 then
					g_Loger:logError("------iiiiii-----")
					EQGlobaFunc.setListboxItemSelect(m_taskList:getName(),taskrow.title)
					Task_ShowTaskContent(taskrow)
				--end
			end
		end
	end
	TaskGuide_caculateTreeSize(true)
	-- 没有任务，设置任务按钮隐藏，反之设置为显示
	if 0 == show_task_count then
		g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):setVisible(false)
		m_animateWin:setVisible(false)
		PlayerGuide_setNewTaskFlag(false)
	else
		g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):setVisible(true)
		m_animateWin:setVisible(true)
	end
end
-----------------------------------------
--
g_GuideTreeItmClicked = false
--
function Task_handleEventSelectionChanged(selectitm_id,txt)
	g_Loger:logError(tostring(selectitm_id) .."***" ..txt)
	g_GuideTreeItmClicked = true
	if step_img_maptable[selectitm_id]~= nil then
		for i,v in ipairs(step_img_maptable[selectitm_id]) do
--			g_Loger:logError("i" .."*"..tostring(v))
			Task_showTaskGuidePic(tonumber(v))
		end
	end
end
-----------------------------------------
--
g_GuideTreeOpened = true
function Task_hideGuideTreeUI(args)
	if g_GuideTreeOpened == false then
		g_WinMgr:getWindow("DFwnd_MainRoot/Task_guide_win"):setVisible(true)
		g_GuideTreeOpened = true
	else
		g_WinMgr:getWindow("DFwnd_MainRoot/Task_guide_win"):setVisible(false)
		g_GuideTreeOpened = false
	end
	
end
-----------------------------------------
--
function Task_handleEventBranchOpened()
	TaskGuide_caculateTreeSize(true)
	--local pos = g_WinMgr:getWindow("DFwnd_MainRoot/Task_guide_win/Task_guide_Tree"):getPosition()
	--local pso_str = CEGUI.PropertyHelper:uvector2ToString(pos)
	--g_Loger:logMessage("-------" ..tostring(pso_str))
end
-----------------------------------------
--
function Task_handleEventBranchClosed()
	TaskGuide_caculateTreeSize(false)
end
-----------------------------------------
--
function Task_openMatchTaskContent(itm_id)
	OpenTaskUI()
	g_Loger:logError("---------------->"..tostring(itm_id))
	local task_row = g_tasktableMgr:getTaskRow(itm_id)
	--g_Loger:logError("---------------->"..tostring(task_row.title))
	if task_row ~= nil then
		m_task:setSelTask(itm_id)
		--EQGlobaFunc.setListboxItemSelect(m_taskList:getName(),task_row.title)
		Task_ShowTaskContent(task_row)
	end
	do_task_script("script2")
end
-----------------------------------------
--
function Task_showTaskGuidePic(idx)
	show_picture(idx)
end
-----------------------------------------
--
function Task_addTaskGuidesubStepItem(tree_id,parentItemtxt,sub_id)
	if sub_id ~= 0 then
		local guiderow = g_tasktableMgr:getTaskGuideStepRow(sub_id)
		local taskGuideTree_subitm_id = 1
		--g_Loger:logError(guiderow.id .."*" ..guiderow.stepNum .."*" ..guiderow.content)
		local tabn = guiderow.stepNum
		local TotalstepTable = Split(guiderow.content," ")
		for i,v in ipairs(TotalstepTable) do
			local stepTable = Split(v,";")
			local stepn = table.getn(stepTable)
			local stepTitle =stepTable[1]
			local stepimgset = Split(stepTable[2],",")
			local tree_subitm_id = tonumber(tostring(tree_id) ..tostring(taskGuideTree_subitm_id))
			--local tmptable = {tree_subitm_id=stepimgset}
			step_img_maptable[tree_subitm_id] = stepimgset
			--table.insert(step_img_maptable,tree_subitm_id=stepimgset)
--			g_Loger:logError("step_img_maptable" .."**" ..tostring(tree_subitm_id) .."---" ..step_img_maptable[tree_subitm_id][1])
			local r_val = 0.0
			local g_val = 1.0
			local b_val = 0.95
			local col = CEGUI.colour:new_local(r_val, g_val, b_val, 1)
			local crect = CEGUI.ColourRect(col)
			local crect_str = CEGUI.PropertyHelper:colourRectToString(crect)
			Task_addGuideItem(tree_subitm_id,parentItemtxt,stepTitle,"simsun-10",crect_str,"","")
			taskGuideTree_subitm_id = taskGuideTree_subitm_id + 1
		end
		
	end
end
-----------------------------------------
--
function Task_addGuideItem(subitm_id,parentItemtxt,txt,font,colrect,imgset,img)
	m_task:Task_addTaskGuideTitleTreeItem(subitm_id,parentItemtxt,txt,font,colrect,imgset,img)
end
----------------------------------------
--
function TaskGuide_caculateTreeSize(root_isopened)
	m_task:caculateTreeSize(root_isopened)
end
-----------------------------------------
-- 选择任务
-----------------------------------------
function Task_Select(args)
	local item = m_taskList:getFirstSelectedItem()
	if nil ~= item then
		m_taskSelect = item:getID()
		local task_row = g_tasktableMgr:getTaskRow(m_taskSelect)
		if nil == task_row then
			m_task:setSelTask(0)
		else
			if 0 == task_row.is_give_up then	-- 任务不可放弃
				g_WinMgr:getWindow("Taskboardroot/Taskboard/giveup"):setVisible(false)
			elseif 1 == task_row.is_give_up then	-- 任务可放弃
				g_WinMgr:getWindow("Taskboardroot/Taskboard/giveup"):setVisible(true)
			end
			m_task:setSelTask(m_taskSelect)
			Task_ShowTaskContent(task_row)
		end
	end
end
-----------------------------------------
-- 清除任务内容
-----------------------------------------
function Task_ClearTaskContent()
	g_taskVec:removeAllControl()
	g_taskVec:setScrollbarPosition(0)
end
-----------------------------------------
-- 显示所选择的任务内容
-----------------------------------------
function Task_ShowTaskContent(taskrow)
	Task_ClearTaskContent()
	--任务名
	local title = g_WinMgr:getWindow("Taskboard/task_title")
	if nil == title then
		title = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Taskboard/task_title")
		EQGlobaFunc.toMultiLineBox(title:getName()):setWidth(m_taskCtrlWidth)
		EQGlobaFunc.toMultiLineBox(title:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(title:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(title:getName()):setCols("#000000")
	end
	local win = EQGlobaFunc.toMultiLineBox(title:getName())
	win:clearUp()
	win:addElementText(taskrow.title, true)
	win:addElementText("", true)
	win:setHeight(win:getFormatHeight())
	g_taskVec:addControl(win:getName())	
	--任务内容
	local describe = g_WinMgr:getWindow("Taskboard/task_describe")
	if nil == describe then
		describe = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Taskboard/task_describe")
		EQGlobaFunc.toMultiLineBox(describe:getName()):setWidth(m_taskCtrlWidth)
		EQGlobaFunc.toMultiLineBox(describe:getName()):setAlign("AT_LEFT")
		EQGlobaFunc.toMultiLineBox(describe:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(describe:getName()):setCols("#000000")
	end
	win = EQGlobaFunc.toMultiLineBox(describe:getName())
	win:clearUp()
	win:addElementText("任务目标：", true)
	win:addElementText(taskrow.intention, true)
	win:addElementText("", true)
	
	local actionDesc = m_task:getTaskActionDesc(taskrow.id)
	if string.len(actionDesc) > 0 then
		win:addElementText(actionDesc, true);
		win:addElementText("", true)
	end
	
	win:addElementText("任务描述：", true)
	win:addElementText("", true)
	g_mlbParse:getData(taskrow.describe, win:getName())
	win:addElementText("", true)
	win:setHeight(win:getFormatHeight())
	g_taskVec:addControl(win:getName())
	--固定奖励
	if m_task:getRewardCount() > 0 then
		--奖励标题
		local rewardTitle = g_WinMgr:getWindow("Taskboard/task_reward_title")
		if nil == rewardTitle then
			rewardTitle = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Taskboard/task_reward_title")
			EQGlobaFunc.toMultiLineBox(rewardTitle:getName()):setWidth(m_taskCtrlWidth)
			EQGlobaFunc.toMultiLineBox(rewardTitle:getName()):setAlign("AT_LEFT")
			EQGlobaFunc.toMultiLineBox(rewardTitle:getName()):setFont("simsun-10")
			EQGlobaFunc.toMultiLineBox(rewardTitle:getName()):setCols("#000000")
		end
		win = EQGlobaFunc.toMultiLineBox(rewardTitle:getName())
		win:clearUp()
		win:addElementText("任务奖励：", true)
		win:setHeight(win:getFormatHeight())
		g_taskVec:addControl(win:getName())
		local count = 0
		--奖励物品
		for i=0, m_task:getRewardCount()-1 do
			local itemrow = g_itemtableMgr:getItemRow(m_task:getRewardItem(i))
			if nil ~= itemrow then
				for j=1, m_task:getRewardAmount(i) do
					--物品图片
					win = g_WinMgr:createWindow("EQHomeLook/StaticImage6")
					win:setSize(CEGUI.UVector2(CEGUI.UDim(0,34), CEGUI.UDim(0,34)))
					StorageUI_DynamicLoadItem(win:getName(), "item", i, itemrow.imageset, itemrow.icon, "False")
					if count%2 > 0 then
						g_taskVec:setAtRight()
						g_taskVec:setHorInterval(10)
					end
					g_taskVec:addControl(win:getName())
					--物品名
					win = EQGlobaFunc.toMultiLineBox(g_WinMgr:createWindow("EQHomeLook/MultiLineBox2"):getName())
					win:setWidth(45)
					win:setFont("simsun-10")
					win:setCols("#000000")
					win:setAlign("AT_LEFT")
					win:addElementText(itemrow.name, true)
					win:setHeight(win:getFormatHeight())
					g_taskVec:setAtRight()
					g_taskVec:addControl(win:getName())
					count = count + 1
				end
			end
		end
	end
	--可选奖励
	if m_task:getChoiceCount() > 0 then
		--奖励标题
		local choiceTitle = getWindow("Taskboard/task_choice_title")
		if nil == choiceTitle then
			choiceTitle = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Taskboard/task_choice_title")
			EQGlobaFunc.toMultiLineBox(choiceTitle:getName()):setWidth(m_taskCtrlWidth)
			EQGlobaFunc.toMultiLineBox(choiceTitle:getName()):setAlign("AT_LEFT")
			EQGlobaFunc.toMultiLineBox(choiceTitle:getName()):setFont("simsun-10")
			EQGlobaFunc.toMultiLineBox(choiceTitle:getName()):setCols("#000000")
		end	
		win = EQGlobaFunc.toMultiLineBox(choiceTitle:getName())
		win:clearUp()
		win:addElementText("可选奖励：", true)
		win:setHeight(win:getFormatHeight())
		g_taskVec:addControl(win:getName())
		local count = 0
		-- 奖励物品
		for i=0, m_task:getChoiceCount()-1 do
			local itemrow = g_itemtableMgr:getItemRow(m_task:getChoiceItem(i))
			if nil ~= itemrow then
				for j=1, m_task:getChoiceAmount(i) do
					--物品图片
					win = g_WinMgr:createWindow("EQHomeLook/StaticImage6")
					win:setSize(CEGUI.UVector2(CEGUI.UDim(0,34), CEGUI.UDim(0,34)))
					StorageUI_DynamicLoadItem(win:getName(), "item", i, itemrow.imageset, itemrow.icon, "False")
					if count%2 > 0 then
						g_taskVec:setAtRight()
						g_taskVec:setHorInterval(10)
					end
					g_taskVec:addControl(win:getName())
					--物品名
					win = EQGlobaFunc.toMultiLineBox(g_WinMgr:createWindow("EQHomeLook/MultiLineBox2"):getName())
					win:setWidth(45)
					win:setFont("simsun-10")
					win:setCols("#000000")
					win:setAlign("AT_LEFT")
					win:addElementText(itemrow.name, true)
					win:setHeight(win:getFormatHeight())
					g_taskVec:setAtRight()
					g_taskVec:addControl(win:getName())					
					count = count + 1
				end
			end
		end
	end
	-- 金钱
	local gold = g_WinMgr:getWindow("Taskboard/task_gold")
	if nil == gold then
		gold = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Taskboard/task_gold")
		EQGlobaFunc.toMultiLineBox(gold:getName()):setWidth(m_taskCtrlWidth)
		EQGlobaFunc.toMultiLineBox(gold:getName()):setAlign("AT_LEFT")
		EQGlobaFunc.toMultiLineBox(gold:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(gold:getName()):setCols("#000000")
	end
	win = EQGlobaFunc.toMultiLineBox(gold:getName())
	win:clearUp()
	win:addElementText("获得金钱："..taskrow.game_reward, true)
	win:setHeight(win:getFormatHeight())
	g_taskVec:addControl(win:getName())
	-- eq币
	local eq = g_WinMgr:getWindow("Taskboard/task_eq")
	if nil == eq then
		eq = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Taskboard/task_eq")
		EQGlobaFunc.toMultiLineBox(eq:getName()):setWidth(m_taskCtrlWidth)
		EQGlobaFunc.toMultiLineBox(eq:getName()):setAlign("AT_LEFT")
		EQGlobaFunc.toMultiLineBox(eq:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(eq:getName()):setCols("#000000")
	end
	win = EQGlobaFunc.toMultiLineBox(eq:getName())
	win:clearUp()
	win:addElementText("获得EQ币："..taskrow.eq_reward, true)
	win:setHeight(win:getFormatHeight())
	g_taskVec:addControl(win:getName())
	-- 任务面板排版
	g_taskVec:controlTypeset()
end
-----------------------------------------
--点击放弃任务按钮
-----------------------------------------
function Task_GiveUp(args)
	if 0 ~= m_taskSelect then
		g_MainUIWindows:addChildWindow(g_ExitWindow)
		Common_setModalState(g_ExitWindow:getName(), true)
		EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit", "是否放弃任务？")
		g_ExitWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,230), CEGUI.UDim(0,117)))
		g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-100), CEGUI.UDim(0.5,-30)))
		g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setSize(CEGUI.UVector2(CEGUI.UDim(0,195), CEGUI.UDim(0,30)))
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):removeEvent("Clicked")
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):removeEvent("Clicked")
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):subscribeEvent("Clicked", "Task_ConfirmGiveup")
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):subscribeEvent("Clicked", "Task_CancelGiveup")
	end
end
-----------------------------------------
--确认放弃
-----------------------------------------
function Task_ConfirmGiveup()
	if 0 ~= m_taskSelect then
		m_task:reqGiveup()
		Common_setModalState(g_ExitWindow:getName(), false)
		g_MainUIWindows:removeChildWindow(g_ExitWindow)
		CloseTaskUI()
		Task_ClearTaskContent()
		m_taskSelect = 0			
	end
end
-----------------------------------------
--取消放弃
-----------------------------------------
function Task_CancelGiveup()
	Common_setModalState(g_ExitWindow:getName(), false)
	g_MainUIWindows:removeChildWindow(g_ExitWindow)	
end
-----------------------------------------
local m_reqCompleteId = 0	-- 请求完成的任务id
local m_curCompleteId = 0	-- 当前完成的任务id
-----------------------------------------
--是否为当前完成的任务
-----------------------------------------
function Task_isCurComplete(task_id)
	if task_id == m_curCompleteId then
		return true
	end
	return false
end
-----------------------------------------
--请求结束任务
-----------------------------------------
function Task_reqCompleteTask(task_id)
	m_task:reqCompleteTask(task_id)
	m_reqCompleteId = task_id
end
-----------------------------------------
--返回请求结束任务结果
-----------------------------------------
function Task_completeTask(task_id)
	-- g_Loger:logMessage("--- Task_completeTask(): "..task_id.." ---")
	Task_removeCompleteTaskGuideTreeItm(task_id)
	m_curCompleteId = task_id
	PlayerGuide_do_completeTaskScript(task_id)
	do_task_script("script5")	-- (完成任务)执行新手任务脚本5
	if m_curCompleteId == m_reqCompleteId then	-- 当前完成任务是所请求完成的任务
		PlayerGuide_setExcute(true)
	end
end
-----------------------------------------
function Task_giveupTask(task_id)
	Task_removeCompleteTaskGuideTreeItm(task_id)
end
-----------------------------------------
--
function Task_removeCompleteTaskGuideTreeItm(task_id)
	if task_id ~= nil and task_id ~= 0 then
		local taskrow = g_tasktableMgr:getTaskRow(task_id)
		if taskrow ~= nil then
			local ret_removed = m_task:removeTaskGuideTitleItm(taskrow.title)
			local TreeItmCount = m_task:getTaskGuideTreeItemCount()
			TaskGuide_caculateTreeSize(true)
			if ret_removed and TreeItmCount == 0 then
				g_WinMgr:getWindow("DFwnd_MainRoot/Task_guide_win"):setVisible(false)
				g_WinMgr:getWindow("DFwnd_MainRoot/GuideTreeBtn"):setVisible(false)
			end
		end
	end
end
-----------------------------------------
--检查任务是否存在
-----------------------------------------
function Task_existTask(task_id)
	-- g_Loger:logMessage("--- Task_existTask(): "..task_id.." ---")
	return m_task:existTask(task_id)
end
-----------------------------------------
--定位任务位置
-----------------------------------------
function Task_locateTask(task_id)
	-- g_Loger:logMessage("--- Task_locateTask(): "..task_id.." ---")
	local res = EQGlobaFunc.locateListboxTextItem(m_taskList:getName(), task_id)
	if true == res then
		if 1000~=task_id and 1001~=task_id then	-- 对任务1000和1001进行特殊处理，不能放弃
			g_WinMgr:getWindow("Taskboardroot/Taskboard/giveup"):setVisible(true)
		end
	end
	return res
end
