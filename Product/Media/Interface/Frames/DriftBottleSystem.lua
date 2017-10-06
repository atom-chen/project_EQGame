----------------------------------------------------
--漂流瓶系统
--
--
--
----------------------------------------------------
----------------------------------------------------
--
g_myDriftBottleUIopened = false
local mStartDBS = false
local mViaQuickView = false
local mrec_bt_type = 0
local mBottleTypeEnum = {normal = 1, communicate = 2}
local DriftBottlesTable = {}
local PickBottlesTable = {}
local DriftBottles_ShowTable = {}
local mDriftBottlePlayerPhoto = {}
local mcurPage = 1
local mtotalPage = 1
local MaxSize = 4

local view_in = 1
local delete_id = 0
local click_win = nil

local mPickFailMsg = ""
local mPickBottle_type = 0
--
local mKeyPointTime = 3600 * 24 * 3  --1小时

----------------------------------------------------
for i = 1, MaxSize do
	g_WinMgr:getWindow("NewFrameWindow1/xinxi/touxiang" ..tostring(i)):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("NewFrameWindow1/xinxi/time" ..tostring(i)):setProperty("MousePassThroughEnabled", "true")

	g_WinMgr:getWindow("NewFrameWindow1/xinxi/" ..tostring(i)):setProperty("MousePassThroughEnabled", "true")
end
g_MyBottleItemMoveInUI:setProperty("MousePassThroughEnabled", "true")

g_WinMgr:getWindow("Root/drifting/guanbi"):subscribeEvent("Clicked","DBS_closeDBSUI")
g_WinMgr:getWindow("DefaultWindow/guanbi"):subscribeEvent("Clicked","DBS_closeDropBottleStartUI")

g_WinMgr:getWindow("ordinary__auto_closebutton__"):subscribeEvent("Clicked","DBS_closeNormalBottleEditUI")
g_WinMgr:getWindow("contacts/n/__auto_closebutton__"):subscribeEvent("Clicked","DBS_closeContactBottleEditUI")
g_WinMgr:getWindow("NewFrameWindow1__auto_closebutton__"):subscribeEvent("Clicked","DBS_closeMyBottleRecordsUI")

g_WinMgr:getWindow("Root/drifting/pingzi"):subscribeEvent("Clicked","DBS_DropBottleStartClick")
g_WinMgr:getWindow("Root/drifting/wodepingzi"):subscribeEvent("Clicked","DBS_openMyBottleRecordsUI")
g_WinMgr:getWindow("DefaultWindow/putong"):subscribeEvent("Clicked","DBS_openNormalBottleEditUI")

g_WinMgr:getWindow("DefaultWindow/jiaowang"):subscribeEvent("Clicked","DBS_openContactBottleEditUI")

g_WinMgr:getWindow("contacts/n/reng"):subscribeEvent("Clicked","DBS_dropContactBottle")

g_WinMgr:getWindow("Root/drifting/laoqu"):subscribeEvent("Clicked","DBS_PickBottle")
g_WinMgr:getWindow("ordinary/reng"):subscribeEvent("Clicked","DBS_DropButtonClick")

g_WinMgr:getWindow("NewFrameWindow1/1"):subscribeEvent("Clicked","DBS_setMyDropBottleRecordsData")
g_WinMgr:getWindow("NewFrameWindow1/"):subscribeEvent("Clicked","DBS_showPickBottlesRecord")
g_WinMgr:getWindow("NewFrameWindow1/zuo"):subscribeEvent("Clicked","DBS_prePage")
g_WinMgr:getWindow("NewFrameWindow1/zuo1"):subscribeEvent("Clicked","DBS_nextPage")

g_WinMgr:getWindow("NewFrameWindow1/xinxi1"):subscribeEvent("MouseButtonDown","DBS_clickBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi2"):subscribeEvent("MouseButtonDown","DBS_clickBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi3"):subscribeEvent("MouseButtonDown","DBS_clickBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi4"):subscribeEvent("MouseButtonDown","DBS_clickBottleRecordsItem")

g_WinMgr:getWindow("NewFrameWindow1/xinxi1"):subscribeEvent("MouseEnter","DBS_moveInBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi2"):subscribeEvent("MouseEnter","DBS_moveInBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi3"):subscribeEvent("MouseEnter","DBS_moveInBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi4"):subscribeEvent("MouseEnter","DBS_moveInBottleRecordsItem")

g_WinMgr:getWindow("NewFrameWindow1/xinxi1"):subscribeEvent("MouseLeave","DBS_moveOutBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi2"):subscribeEvent("MouseLeave","DBS_moveOutBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi3"):subscribeEvent("MouseLeave","DBS_moveOutBottleRecordsItem")
g_WinMgr:getWindow("NewFrameWindow1/xinxi4"):subscribeEvent("MouseLeave","DBS_moveOutBottleRecordsItem")

g_WinMgr:getWindow("DriftBottleItemMoveInUI/deleteBtn"):subscribeEvent("MouseEnter","DBS_mouseEnterDelBtn")
g_WinMgr:getWindow("DriftBottleItemMoveInUI/deleteBtn"):subscribeEvent("MouseLeave","DBS_mouseLeaveDelBtn")
g_WinMgr:getWindow("DriftBottleItemMoveInUI/deleteBtn"):subscribeEvent("Clicked","DBS_DelBtnClicked")

g_WinMgr:getWindow("countkan__auto_closebutton__"):subscribeEvent("Clicked","DBS_closeBottleDetailInfoUI")

g_GameEventMgr:subscribeEvent("DRIFTBOTTLELISTUPDATE","DBS_updateAllTables")
g_GameEventMgr:subscribeEvent("DRIFTBOTTLELIST_PAGECHANGE","DBS_pageChanged")

--g_GameEventMgr:subscribeEvent("PlayEnd","DBS_DropAnimationEnd")


CEGUI.toRadioButton(g_WinMgr:getWindow("contacts/nan")):setGroupID(1)
CEGUI.toRadioButton(g_WinMgr:getWindow("contacts/nv")):setGroupID(1)
CEGUI.toMultiLineEditbox(g_WinMgr:getWindow("contacts/n/wenzi")):setMaxTextLength(140)  --聊天最大字符长度为140
CEGUI.toMultiLineEditbox(g_WinMgr:getWindow("ordinary/wenzi")):setMaxTextLength(140)  --聊天最大字符长度为140

Common_injectEmptyEvt("Root/drifting")
MainUI_ShieldClick(g_DriftStartUI)
Common_injectEmptyEvt("DefaultWindow")
MainUI_ShieldClick(g_selectBottleTypeUI)
Common_injectEmptyEvt("ordinary")
MainUI_ShieldClick(g_NormalDriftEditUI)
Common_injectEmptyEvt("contacts/n/")
MainUI_ShieldClick(g_ContactBottleEditUI)
Common_injectEmptyEvt("NewFrameWindow1")
MainUI_ShieldClick(g_MyBottleRecordsUI)
Common_injectEmptyEvt("reng")
MainUI_ShieldClick(g_RengAnimatUI)
Common_injectEmptyEvt("lao")
MainUI_ShieldClick(g_PickAnimatUI)
Common_injectEmptyEvt("countkan")
MainUI_ShieldClick(g_BottleDetailInfoUI)
----------------------------------------------------
--
function DBS_DelBtnClicked(args)
	click_win = CEGUI.toWindowEventArgs(args).window
	delete_id = click_win:getParent():getID()
	DBS_openDelBottleUI()
end
----------------------------------------------------
--
function DBS_openDelBottleUI()
	MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","确定删除？", "DBS_confirmDelBottle", "DBS_cancelDelBottle")
end
----------------------------------------------------
--
function DBS_confirmDelBottle()
	if delete_id ~= 0 then
		if click_win ~= nil then
			click_win:getParent():setID(0)
		end
		DBS_DelBottle(delete_id, view_in)
		g_DriftBottleSys:deleteBottle(delete_id, view_in)
	end
end
----------------------------------------------------
--
function DBS_cancelDelBottle()
	delete_id = 0
	click_win = nil
end
----------------------------------------------------
--
function DBS_DelBottle(id,own_list)
	local s_id = tostring(id)
	local del_idx = 0
	if own_list == 1 then
		for i,v in ipairs(DriftBottlesTable) do
			if v.bt_id == s_id then
				del_idx = i
			end
		end
		table.remove(DriftBottlesTable,del_idx)
		DBS_setTotalPage(getTotalPage(DriftBottlesTable, MaxSize))
	else
		for i,v in ipairs(PickBottlesTable) do
			if v.bt_id == s_id then
				del_idx = i
			end
		end
		table.remove(PickBottlesTable,del_idx)
		DBS_setTotalPage(getTotalPage(PickBottlesTable, MaxSize))
	end
	DBS_update(own_list)
	DBS_setPageTxt()
	DBS_setUIRecordsData(DriftBottles_ShowTable)
end
----------------------------------------------------
--
function DBS_mouseEnterDelBtn()

end
----------------------------------------------------
--
function DBS_mouseLeaveDelBtn()

end
----------------------------------------------------
--
function DBS_clickBottleRecordsItem(args)
--	g_Loger:logError("MouseClick")
	local click_win = CEGUI.toWindowEventArgs(args).window
	local item_id = click_win:getID()
	if item_id ~= nil and item_id ~= 0 then
		DBS_openBottleDetailInfoUI()
		DBS_setBottleDetailInfo(args)
	end
end
----------------------------------------------------
--
function DBS_openBottleDetailInfoUI()
	UIM_OpenWindow("countkan")
	UIM_CloseMutexUI("countkan")
	DBS_closeDBSUI()
end
----------------------------------------------------
--
function DBS_closeBottleDetailInfoUI()
	UIM_CloseWindow("countkan")
	UIM_OpenRelationUI("countkan")
	if mViaQuickView == true then
		DBS_closeMyBottleRecordsUI()
		mViaQuickView = false
	end
end
----------------------------------------------------
--
function DBS_setBottleDetailInfo(args)
	local click_win = CEGUI.toWindowEventArgs(args).window
	local name_len = string.len(click_win:getName())
	local idx = tonumber(string.sub(click_win:getName(),name_len))
	--	g_Loger:logError(click_win:getName())
	--	g_Loger:logError(tostring(name_len))
--		g_Loger:logError(tostring(idx))
	--item_id = click_win:getID()
--	g_Loger:logError(tostring(table.getn(DriftBottles_ShowTable)))
	local item_table = DriftBottles_ShowTable[idx]
	if item_table ~= nil then
		DBS__setBottleDetailInfo(item_table)
	end
end
----------------------------------------------------
--
function DBS__setBottleDetailInfo(item_table)

	local from_txt = ""
	if item_table.bt_nick == "" then
		from_txt = DBS_setBottleDetailFrom(item_table.bt_acc, item_table.bt_type)
	else
		from_txt = DBS_setBottleDetailFrom(item_table.bt_nick, item_table.bt_type)
	end
	EQGlobaFunc.SetText("countkan/nicheng","")
	EQGlobaFunc.SetText("countkan/2","")
	g_WinMgr:getWindow("countkan/touxiang"):setProperty("Image", "")
	EQGlobaFunc.SetText("countkan/nicheng",from_txt)
	EQGlobaFunc.SetText("countkan/2",item_table.bt_content)
--	g_Loger:logError(tostring(item_table.bt_acc))
--	g_Loger:logError(tostring(item_table.bt_content))
	if view_in == 1 then
		DBS_setFriendPhoto("countkan/touxiang", g_mainplayer:getPlayerAccount())
	else
		DBS_setFriendPhoto("countkan/touxiang", item_table.bt_acc)
	end
end
----------------------------------------------------
--
function DBS_setBottleDetailFrom(from_txt, bt_type)
	if view_in == 2 then
		if from_txt == "" then
			from_txt = "匿名"
		end
	else
		from_txt = "扔出"
	end
	local btType = "的瓶子"
	if bt_type == mBottleTypeEnum.normal then
		btType = "的普通瓶"
	elseif bt_type == mBottleTypeEnum.communicate then
		btType = "的交往瓶"
	else
		btType = "的未分类瓶"
	end
	return from_txt ..btType
end
----------------------------------------------------
--
function DBS_moveInBottleRecordsItem(args)
--	g_Loger:logError("MouseEnter")
	local enter_win = CEGUI.toWindowEventArgs(args).window
	delete_id = enter_win:getID()
--	g_Loger:logError(tostring(delete_id))
	if delete_id ~= 0 then
		enter_win:addChildWindow(g_MyBottleItemMoveInUI)
		enter_win:addChildWindow(g_MyBottleDelBtnUI)
		g_MyBottleItemMoveInUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0,0), CEGUI.UDim(0,0)))
		g_MyBottleItemMoveInUI:setSize(CEGUI.UVector2(CEGUI.UDim(1, 0), CEGUI.UDim(1, 0)))
	end
	--g_MyBottleDelBtnUI:setPosition(CEGUI.UVector2(CEGUI.UDim(1,-20), CEGUI.UDim(1,-50)))
end
----------------------------------------------------
--
function DBS_moveOutBottleRecordsItem(args)
--	g_Loger:logError("MouseLeave")
	local leave_win = CEGUI.toWindowEventArgs(args).window
	local mousein_win = g_GuiSystem:getWindowContainingMouse()
	leave_win:removeChildWindow(g_MyBottleItemMoveInUI)
--	g_Loger:logError(tostring(mousein_win:getName()))
	if mousein_win:getName() == "DriftBottleItemMoveInUI/deleteBtn" then
		return
	end
	leave_win:removeChildWindow(g_MyBottleDelBtnUI)
end
----------------------------------------------------
--
function DBS_openStartUI()
	DBS_openDBSUI()
end
----------------------------------------------------
--
function DBS_openDBSUI()
	UIM_OpenWindow("Root/drifting")
	mStartDBS = true
	PlayerGuide_do_NewerTeachTaskScript(15005) --新手教学任务
end
----------------------------------------------------
--
function DBS_closeDBSUI()
	UIM_CloseWindow("Root/drifting")
	UIM_CloseMutexUI("Root/drifting")
end
----------------------------------------------------
--
function DBS_DropBottleStartClick(args)
	DBS_openDropBottleStartUI()
end
----------------------------------------------------
--
function DBS_openDropBottleStartUI()
	UIM_OpenWindow("DefaultWindow")
	UIM_CloseMutexUI("DefaultWindow")
end
----------------------------------------------------
--
function DBS_closeDropBottleStartUI()
	UIM_CloseWindow("DefaultWindow")
	UIM_OpenRelationUI("DefaultWindow")
end
----------------------------------------------------
--
function DBS_openNormalBottleEditUI()
	DBS__openNormalBottleEditUI()
	--g_MainUIWindows:addChildWindow(g_NormalDriftEditUI)
end
----------------------------------------------------
--
function DBS__openNormalBottleEditUI()
	UIM_OpenWindow("ordinary")
	UIM_CloseMutexUI("ordinary")
	UIM_CloseWindow("Root/drifting")
end
----------------------------------------------------
--
function DBS_closeNormalBottleEditUI()
	g_WinMgr:getWindow("ordinary/wenzi"):setText("")
	UIM_CloseWindow("ordinary")
	UIM_OpenRelationUI("ordinary")
end
----------------------------------------------------
--
function DBS_openContactBottleEditUI()
	local sex = g_mainplayer:getSex()
	if sex == 1 then
		CEGUI.toRadioButton(g_WinMgr:getWindow("contacts/nv")):setSelected(true)
	else
		CEGUI.toRadioButton(g_WinMgr:getWindow("contacts/nan")):setSelected(true)
	end
	DBS__openContactBottleEditUI()
	--g_MainUIWindows:addChildWindow(g_ContactBottleEditUI)
end
----------------------------------------------------
--
function DBS__openContactBottleEditUI()
	UIM_OpenWindow("contacts/n/")
	UIM_CloseMutexUI("contacts/n/")
	UIM_CloseWindow("Root/drifting")
end
----------------------------------------------------
--
function DBS_closeContactBottleEditUI()
	g_WinMgr:getWindow("contacts/n/wenzi"):setText("")
	UIM_CloseWindow("contacts/n/")
	UIM_OpenRelationUI("contacts/n/")
end
----------------------------------------------------
--
function DBS_DropButtonClick(args)
	--如果文字为空或者全都是空格，则不发送请求
	local content = g_WinMgr:getWindow("ordinary/wenzi"):getText()
	local isAnonymous = CEGUI.toCheckbox(g_WinMgr:getWindow("ordinary/niming")):isSelected()
	if content ~= "\n" then
		DBS_closeNormalBottleEditUI()
		if isAnonymous == true then
--			g_Loger:logError("isAnonymous")
			g_DriftBottleSys:dropBottle(content, 1, -1, 0)
		else
--			g_Loger:logError("not Anonymous")
			local drop_player = g_mainplayer:getPlayerAccount()
			local drop_Player_name = g_mainplayer:getName()
			g_DriftBottleSys:dropBottle(content, 1, -1, 1)
			--DBS_playDropAnimation()
		end
	else
		MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","童鞋，塞点文字吧.", "MsgBox_cancelFunc", "")
	end
end
-----------------------------------------------------
--
function DBS_playDropAnimation()
	g_MainUIWindows:addChildWindow(g_RengAnimatUI)
	DBS_playFadeAnimation("reng")
end
-----------------------------------------------------
--
local renging_flag = false
local laoing_flag = false
function DBS_playFadeAnimation(winName)
	UIM_CloseWindow("DefaultWindow")
	PlayFadeAnimation(winName, 5000, 1, 0, 0)
	if winName == "reng" then
		renging_flag = true
	elseif winName == "lao" then
		laoing_flag = true
	end
end
-----------------------------------------------------
--
function DBS_getDropingFlag()
	return renging_flag
end
-----------------------------------------------------
--
function DBS_getPickingFlag()
	return laoing_flag
end
-----------------------------------------------------
function DBS_cantChangeSceneTip()
	MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","正在使用漂流瓶，无法切换，请稍等。", "MsgBox_cancelFunc", "")
end
-----------------------------------------------------
--
function DBS_DropAnimationEnd()
	--g_Loger:logError("***DBS_DropAnimationEnd*")
	--if true == EQGlobaFunc.uiAnimationIsPlaying("reng") then
	--	return
	--end
	g_MainUIWindows:removeChildWindow(g_RengAnimatUI)
	--DBS_openDropBottleStartUI()
	DBS_dropSuccess()
end
-----------------------------------------------------
--
function DBS_closeDropEndMsgBox()
	DBS_openDropBottleStartUI()
	if renging_flag then
		renging_flag = false
	end
end
-----------------------------------------------------
--
function DBS_dropSuccess()
	MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","瓶子已经扔出.", "DBS_closeDropEndMsgBox", "")
end
-----------------------------------------------------
--
function DBS_dropFail()
	MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","丢瓶子的次数已用完.", "MsgBox_cancelFunc", "")
end
-----------------------------------------------------
--
function DBS_pickFail()
	MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","捞瓶子的次数已用完.", "DBS_closePickNothingMsgBox", "")
end
-----------------------------------------------------
function DBS_recPickFailMsg(content)
	mPickFailMsg = content
end
-----------------------------------------------------
--
function DBS_setDropCountUI(dropcount)
	
	g_WinMgr:getWindow("Root/drifting/1"):setText(tostring(dropcount))
end
-----------------------------------------------------
--
function DBS_setPickCountUI(pickcount)
	g_WinMgr:getWindow("Root/drifting/2"):setText(tostring(pickcount))
end
-----------------------------------------------------
--
function DBS_matchSpace(str)

end
-----------------------------------------------------
--
function DBS_dropContactBottle()
	local anony = 0
	local req_sex = 1
	local content = g_WinMgr:getWindow("contacts/n/wenzi"):getText()
	local isAnonymous = CEGUI.toCheckbox(g_WinMgr:getWindow("contacts/niming")):isSelected()
	local male_select = CEGUI.toRadioButton(g_WinMgr:getWindow("contacts/nan")):isSelected()
	local female_select = CEGUI.toRadioButton(g_WinMgr:getWindow("contacts/nv")):isSelected()

	if isAnonymous == true then
		anony = 0
	else
		anony = 1
	end
	if male_select == true then
		req_sex = 1
	end
	if female_select == true then
		req_sex = 2
	end
	if content ~= "\n" then
		DBS_closeContactBottleEditUI()
		g_DriftBottleSys:dropBottle(content, 2, req_sex, anony)
		
	else
		MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","童鞋，塞点文字吧.", "MsgBox_cancelFunc", "")
	end
end
-----------------------------------------------------
--
function DBS_PickBottle(args)
	--播放淡入动画
	--PlayFadeAnimation(g_works.imageWindow, 2000, 0, 1, 0)
	DBS_closeDBSUI()
	g_DriftBottleSys:pickBottle()
end
-----------------------------------------------------
--
function DBS_playPickAnimation()
	g_MainUIWindows:addChildWindow(g_PickAnimatUI)
	DBS_playFadeAnimation("lao")
end
-----------------------------------------------------
--
function DBS_PickAnimationEnd()
	g_MainUIWindows:removeChildWindow(g_PickAnimatUI)
	--DBS_openDBSUI()
	DBS_pickSuccess()
	if mPickFailMsg ~= "" then
		MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示",mPickFailMsg, "DBS_closePickNothingMsgBox", "")
		mPickFailMsg = ""
	end
	if mPickBottle_type ~= 0 then
		local bt_type = ""
		if mPickBottle_type == 1 then
			bt_type = "普通瓶"
		else
			bt_type = "交往瓶"
		end
		MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","您捡到了" ..bt_type, "DBS_quickViewPickBottleInfo", "","打开")
		mPickBottle_type = 0
	end
	
end
-----------------------------------------------------
--
function DBS_closePickNothingMsgBox()
	DBS_openDBSUI()
	if laoing_flag then
		laoing_flag = false
	end
end
-----------------------------------------------------
--
--
function DBS_quickViewPickBottleInfo()
	mViaQuickView = true
	DBS_openBottleDetailInfoUI()
	--DBS_closeMyBottleRecordsUI()
	DBS_closeDBSUI()
	if laoing_flag then
		laoing_flag = false
	end
end
-----------------------------------------------------
--
function DBS_pickWhatBottle(bottle_type)
	mPickBottle_type = bottle_type
end
-----------------------------------------------------
--
function DBS_pickSuccess()

end
-----------------------------------------------------
--
function DBS_updateBottleList(	own_list,
								bottle_id,
								bottle_acc,
								bottle_content,
								bottle_type,
								bottle_time,
								elapsedsecs)
	--g_Loger:logMessage("**" ..tostring(bottle_id) .." " ..tostring(bottle_acc) .." "
	--					..tostring(bottle_content) .." " ..tostring(bottle_type) .." "
	--					..tostring(bottle_time))
	--if DBS_isBottleExist(bottle_id,own_list) then
		--g_Loger:logError("already exist!")
	--else
		DBS_addBottleInfo(own_list,bottle_id, bottle_acc, bottle_content, bottle_type, bottle_time,elapsedsecs)
	--end
end
------------------------------------------------------
--
function DBS_isBottleExist(id,own_list)
	local t= {}
	if own_list == 1 then
		t = DriftBottlesTable
	else
		t = PickBottlesTable
	end
	--g_Loger:logError(tostring(id))
	for i,v in ipairs(t) do
		if tonumber(v.bt_id) == id then
			--g_Loger:logError(tostring(v.bt_id))
			return true
		else
			--g_Loger:logError(tostring(v.bt_id))
			g_Loger:logError("not exist")
		end
	end
	return false
end
------------------------------------------------------
--
function DBS_addBottleInfo(	own_list,
							bottle_id,
							bottle_acc,
							bottle_content,
							bottle_type,
							bottle_time,
							elapsedsecs)
	if own_list == 1 then
--		g_Loger:logError("DriftBottlesTable")
		table.insert(DriftBottlesTable,
					 {bt_id = bottle_id,
					  bt_acc = bottle_acc,
					  bt_nick = "",
					  bt_content = bottle_content,
					  bt_type = bottle_type,
					  bt_time = bottle_time,
					  bt_elapsedsecs_str = elapsedsecs
					  })
		DBS_sortDropBottlesList(DriftBottlesTable)
	elseif own_list == 2 then
--		g_Loger:logError("PickBottlesTable")
		table.insert(PickBottlesTable,
					 {bt_id = bottle_id,
					  bt_acc = bottle_acc,
					  bt_nick = "",
					  bt_content = bottle_content,
					  bt_type = bottle_type,
					  bt_time = bottle_time,
					  bt_elapsedsecs_str = elapsedsecs
					  })
		DBS_sortDropBottlesList(PickBottlesTable)
		local item_table = {
							  bt_id = bottle_id,
							  bt_acc = bottle_acc,
							  bt_nick = "",
							  bt_content = bottle_content,
							  bt_type = bottle_type,
							  bt_time = bottle_time,
							  bt_elapsedsecs_str = elapsedsecs
							}
		view_in = 2
		DBS__setBottleDetailInfo(item_table)
		mrec_bt_type = bottle_type
		if bottle_acc ~= "" then
			DBS_insertPlayerPhoto(bottle_acc, "")
			DBS_reqOnePickAccEqInfo(bottle_acc)
		else
		
		end

	end
end
------------------------------------------------------
--
function DBS_reqOnePickAccEqInfo(bottle_acc)
	g_DriftBottleSys:reqEqUserInfo(bottle_acc)
	mfuckFlag = true
end
------------------------------------------------------
--
function DBS_quickSetEqInfo(account, info)
	DBS_insertPlayerPhoto(account, info.photo_data)
	g_WinMgr:getWindow("countkan/touxiang"):setProperty("Image", "")
	view_in = 2
	local name_param = ""
	if info.nick_name ~= "" then
		name_param = info.nick_name
	elseif info.full_name ~= "" then
		name_param = info.full_name
	else
		name_param = account
	end
	local from_txt = DBS_setBottleDetailFrom(name_param, mrec_bt_type)

	EQGlobaFunc.SetText("countkan/nicheng",from_txt)
	DBS_setFriendPhoto("countkan/touxiang", account)
end
------------------------------------------------------
--
function DBS_sortDropBottlesList(source_table)
	if source_table ~= nil then
		--table_num = table.g
		table.sort(source_table,DBS_sortTable)
	end
end
------------------------------------------------------
--按时间排序
function DBS_sortTable(a,b)
	local a_time_table = {}
	local b_time_table = {}
	a_time_table = Split(a.bt_time,",")
	b_time_table = Split(b.bt_time,",")
	
	if a_time_table[1] == b_time_table[1] then --year
		if a_time_table[2] == b_time_table[2] then --month
			if a_time_table[3] == b_time_table[3] then --day
				if a_time_table[4] == b_time_table[4] then --hour
					if a_time_table[5] == b_time_table[5] then --minite
						if a_time_table[6] == b_time_table[6] then --second
						
						else
							return a_time_table[6] > b_time_table[6]
						end
					else
						return a_time_table[5] > b_time_table[5]
					end
				else
					return a_time_table[4] > b_time_table[4]
				end
			else
				return a_time_table[3] > b_time_table[3]
			end
		else
			return a_time_table[2] > b_time_table[2]
		end
	else
		return a_time_table[1] > b_time_table[1]
	end
end
------------------------------------------------------
--
function DBS_update(own_list)
	if own_list == 1 then
		DBS_updateShowList(DriftBottlesTable,mcurPage)
	else
		DBS_updateShowList(PickBottlesTable,mcurPage)
	end
end
------------------------------------------------------
--
function DBS_updateAllTables()
	DBS_updateShowList(DriftBottlesTable,mcurPage)
	--DBS_updateShowList(PickBottlesTable,mcurPage)
end
------------------------------------------------------
--
function DBS_openMyBottleRecordsUI(args)
	DBS__openMyBottleRecordsUI()
	DBS_setMyDropBottleRecordsData()
end
-------------------------------------------------------
--
function DBS__openMyBottleRecordsUI()
	g_myDriftBottleUIopened = true
	UIM_OpenWindow("NewFrameWindow1")
	UIM_CloseMutexUI("NewFrameWindow1")
end
-------------------------------------------------------
--
function DBS_closeMyBottleRecordsUI()
	UIM_CloseWindow("NewFrameWindow1")
	UIM_OpenRelationUI("NewFrameWindow1")
	g_myDriftBottleUIopened = false
end
-------------------------------------------------------
--
function DBS_setMyDropBottleRecordsData()
	view_in = 1
	DBS_setCurpage(1)
	DBS_setTotalPage(getTotalPage(DriftBottlesTable, MaxSize))
	DBS_setPageTxt()
	DBS_updateShowList(DriftBottlesTable,mcurPage)
	DBS_setUIRecordsData(DriftBottles_ShowTable)
end
-------------------------------------------------------
--
function DBS_showPickBottlesRecord()
	view_in = 2
	DBS_setCurpage(1)
	DBS_setTotalPage(getTotalPage(PickBottlesTable, MaxSize))
	DBS_setPageTxt()
	DBS_updateShowList(PickBottlesTable,mcurPage)
	DBS_setMyPickBottlesRecordsData()
end
-------------------------------------------------------
--
function DBS_setCurpage(page)
	mcurPage = page
end
-------------------------------------------------------
--
function DBS_setMyPickBottlesRecordsData()
	DBS_setUIRecordsData(DriftBottles_ShowTable)
end
-------------------------------------------------------
--
function DBS_setUIRecordsData(show_table)
	local contentWinBaseName = "NewFrameWindow1/xinxi/"
	local baseWinName = "NewFrameWindow1/xinxi"
	local timeTxtBaseName = "NewFrameWindow1/xinxi/time"
	local touxiang_winBase = "NewFrameWindow1/xinxi/touxiang"
	for i=1,MaxSize do
		g_WinMgr:getWindow(baseWinName ..tostring(i)):setID(0)
		g_WinMgr:getWindow(contentWinBaseName ..tostring(i)):setText("")
		g_WinMgr:getWindow(timeTxtBaseName ..tostring(i)):setText("")
		DBS_setBottleTypeIcon(touxiang_winBase ..tostring(i),0)
	end
	if show_table ~= nil then
		local item_num = table.getn(show_table)
		if item_num ~= 0 then
			for i,v in ipairs(show_table) do
				DBS_insertPlayerPhoto(v.bt_acc, "")
				DBS_reqEQInfo(i,v.bt_acc,v.bt_type)
				local item_title = ""
				item_title = DBS_setInfoItemTitle(v.bt_acc,v.bt_type)
				EQGlobaFunc.SetText(contentWinBaseName ..tostring(i),item_title)
				
				DBS_setBottleTypeIcon(touxiang_winBase ..tostring(i),v.bt_type)
				--
				local secs = g_DriftBottleSys:caculateTimeInterval(v.bt_elapsedsecs_str)
				local bottle_time = DBS_formmatShowTime(tonumber(secs),v.bt_time)
				EQGlobaFunc.SetText(timeTxtBaseName ..tostring(i),tostring(bottle_time))
				g_WinMgr:getWindow(baseWinName ..tostring(i)):setID(v.bt_id)
				--g_Loger:logError("***" ..tostring(v.bt_id) .." "
				--				..tostring(v.bt_acc) .. " "
				--				..tostring(v.bt_content).." "
				--				..tostring(v.bt_type) .." "
				--				..tostring(v.bt_time)
				--				)
			end
		end
	else
		
	end
end
-------------------------------------------------------
--
function DBS_setBottleTypeIcon(win_name,bt_type)
	local property = ""
	if bt_type == mBottleTypeEnum.normal then
		property = "set:drifting_face image:NewImage1"
	elseif bt_type == mBottleTypeEnum.communicate then
		property = "set:drifting_face image:NewImage6"
	else
		property = ""
	end
	g_WinMgr:getWindow(win_name):setProperty("Image", property)
end
-------------------------------------------------------
--
function DBS_reqEQInfo(i,acc,bt_type)
	if view_in == 2 then
		if acc ~= "" then
			g_DriftBottleSys:reqEqUserInfo(acc)
		else
			local contentWinBaseName = "NewFrameWindow1/xinxi/"	
			local item_title = DBS_setInfoItemTitle(acc,bt_type)
			EQGlobaFunc.SetText(contentWinBaseName ..tostring(i),item_title)
		end
	else
		g_DriftBottleSys:reqEqUserInfo(acc)
		local contentWinBaseName = "NewFrameWindow1/xinxi/"	
		local item_title = DBS_setInfoItemTitle(acc,bt_type)
		EQGlobaFunc.SetText(contentWinBaseName ..tostring(i),item_title)
	end
end
-------------------------------------------------------
--
function DBS_setEqInfo(acc, info)
--	g_Loger:logError("DBS_setEQInfo")
	local contentWinBaseName = "NewFrameWindow1/xinxi/"	
	if view_in == 1 and DriftBottlesTable ~= nil then
		--for i,v in ipairs(DriftBottlesTable) do
		--	if acc == v.bt_acc then
		--		if info.nick_name ~= "" then
		--			v.bt_nick = info.nick_name
		--		else
		--			v.bt_nick = info.full_name
		--		end
		--	end
		--end
		for i,v in ipairs(DriftBottles_ShowTable) do
			DBS_insertPlayerPhoto(g_mainplayer:getPlayerAccount(),info.photo_data)
		end
	else
		local item_title = ""
		for i,v in ipairs(DriftBottles_ShowTable) do
			if acc == v.bt_acc then
				if info.nick_name ~= "" then
					v.bt_nick = info.nick_name
				elseif info.full_name ~= "" then
					v.bt_nick = info.full_name
				else
					v.bt_nick = v.bt_acc
				end
				DBS_insertPlayerPhoto(v.bt_acc,info.photo_data)
			end
			--if info.nick_name == "" and info.full_name == "" then
			--	item_title = DBS_setInfoItemTitle(v.bt_acc,v.bt_type)
			--else
				item_title = DBS_setInfoItemTitle(v.bt_nick,v.bt_type)
			--end
			EQGlobaFunc.SetText(contentWinBaseName ..tostring(i),item_title)
		end
		
	end
end
-------------------------------------------------------
--
function DBS_setInfoItemTitle(bt_acc,bt_type)
	local title_content = ""
	local bottle_type = ""
	local bottle_from = ""
	if bt_type == mBottleTypeEnum.normal then
		bottle_type = "的普通瓶"
	elseif bt_type == mBottleTypeEnum.communicate then
		bottle_type = "的交往瓶"
	else
		bottle_type = "未分类的瓶子"
	end
	
	if view_in == 2 then
		if bt_acc ~= "" then
			bottle_from = "收到来自" .."[" ..tostring(bt_acc) .."]"
		else
			bottle_from = "收到" .."匿名"
		end
	else
		bottle_from = "扔出"
	end
	title_content = bottle_from ..bottle_type
	return title_content
end
-------------------------------------------------------
--
function DBS_formmatShowTime(secs,bt_time)
	local create_time_table = {}
	local now_time_table = {}
	local now_time = EQGlobaClass.getTimekeeper():getTimeNow()
	create_time_table = Split(bt_time,",")
	now_time_table = Split(now_time,",")
	if secs < mKeyPointTime then
		local d = secs/(3600*24)
		d = math.ceil(d) - 1
		if d < 0 then
			d = 0
		end
		if d == 0 then 
			local h = math.fmod(secs,3600*24)/3600
			h = math.ceil(h) - 1
			if h < 0 then
				h = 0
			end
			if h == 0 then
				local m = math.fmod(math.fmod(secs,3600*24),3600)/60
				m = math.ceil(m) - 1
				if m < 0 then
					m = 0
				end
				if m == 0 then
					local s = math.fmod(math.fmod(math.fmod(secs,3600*24),3600),60)
					return tostring(s) .."秒前"
				else
					return tostring(m) .."分钟前"
				end
			else
				return tostring(h) .."小时前"
			end
		else
			if d == 1 then
				return "昨天" .."\n"
						..tostring(create_time_table[4]) ..":" 
						..tostring(create_time_table[5])
			elseif d == 2 then
				return "前天" .."\n"
						..tostring(create_time_table[4]) ..":" 
						..tostring(create_time_table[5])
			else
				return tostring(d) .."天前"
			end
		end
		return "small"
	else
		if create_time_table[1] == now_time_table[1] then --year
			return 	tostring(create_time_table[2]) .."月" 
					..tostring(create_time_table[3]) .."日" .."\n"
					..tostring(create_time_table[4]) ..":" 
					..tostring(create_time_table[5])
		else
			return 	tostring(create_time_table[1]) .."年" 
					..tostring(create_time_table[2]) .."月"
					..tostring(create_time_table[3]) .."日" .."\n"
					..tostring(create_time_table[4]) ..":" 
					..tostring(create_time_table[5])
		end
		return bt_time
	end
end
-------------------------------------------------------
--
function DBS_fmod(a,b)
	return a - math.floor(a/b)*b
end
-------------------------------------------------------
--
function DBS_updateShowList(sourceTable,page)
	DBS_setShowList(sourceTable,page)
end
-------------------------------------------------------
--
function DBS_setShowList(sourceTable,page)
	DriftBottles_ShowTable = getData(sourceTable, page, MaxSize)
end
-------------------------------------------------------
--
function DBS_setTotalPage(totalpage)
	if totalpage == 0 then
		mtotalPage = 1
	else
		mtotalPage = totalpage
	end
end
-------------------------------------------------------
--
function DBS_pageChanged()
	if view_in == 1 then
		DBS_updateShowList(DriftBottlesTable,mcurPage)
	else
		DBS_updateShowList(PickBottlesTable,mcurPage)
	end
	DBS_setUIRecordsData(DriftBottles_ShowTable)
	DBS_setPageTxt()
end
-------------------------------------------------------
--
function DBS_setPageTxt()
	g_WinMgr:getWindow("NewFrameWindow1/yeshu"):
		setText(tostring(mcurPage) .."/" ..tostring(mtotalPage))
end
-------------------------------------------------------
--
function DBS_nextPage()
	--if view_in == 0 then
		if mcurPage < mtotalPage then
			mcurPage = mcurPage + 1
		else
			mcurPage = mtotalPage
		end
	--else
		
	--end
	
	g_GameEventMgr:fireEvent("DRIFTBOTTLELIST_PAGECHANGE")
end
-------------------------------------------------------
--
function DBS_prePage()
	if mcurPage <= 1 then
		mcurPage = 1
	else
		mcurPage = mcurPage - 1
	end
	g_GameEventMgr:fireEvent("DRIFTBOTTLELIST_PAGECHANGE")
end
-------------------------------------------------------
--
local function _insertFriendPhotoVec(friendPhoto, photoData)
	local photoName = EQGlobaClass.getDynamicCreateImagesetMgr():getImageName(photoData)
--	g_Loger:logError("***5" ..tostring(photoName))
	if "" ~= photoName then	-- 存在相同数据的图片
		friendPhoto.curName = photoName
		return friendPhoto
	end
	photoName = friendPhoto.account --.."_"..tostring(table.getn(friendPhoto.photoVec))
--	g_Loger:logError("***6" ..tostring(photoName))
	local ret = EQGlobaClass.getDynamicCreateImagesetMgr():saveImage(photoData, photoName) -- 保存图片
	if 0 == ret then	-- 保存成功
		table.insert(friendPhoto.photoVec, photoName)
		friendPhoto.curName = photoName
--		g_Loger:logError("***6" ..tostring(0))
	elseif -1 == ret then	-- 存在相同数据的图片
		friendPhoto.curName = EQGlobaClass.getDynamicCreateImagesetMgr():getImageName(photoData)
--		g_Loger:logError("***6" ..tostring(-1))
	elseif -2 == ret then	-- 存在同名图片
		friendPhoto.curName = photoName
--		g_Loger:logError("***6" ..tostring(-2))
	else	-- 保存失败
		friendPhoto.curName = ""
--		g_Loger:logError("***6" .."save fail")
	end
	return friendPhoto
end
-------------------------------------------------------
--
function DBS_insertPlayerPhoto(account, photoData)
	for i, v in pairs(mDriftBottlePlayerPhoto) do
		if account == v.account then	-- 存在该账号
			v = _insertFriendPhotoVec(v, photoData)
			--g_Loger:logError(account)
			return
		end
	end
	if account ~= "" then
--		g_Loger:logError("init friendPhoto")
		local photoVec = {}
		table.insert(photoVec, "")
		local friendPhoto = {}
		friendPhoto.account = account	-- 账号
		friendPhoto.curName = ""		-- 当前头像名
		friendPhoto.photoVec = photoVec	-- 头像名数组
		friendPhoto.needUpdate = true	-- 是否需要更新头像
		table.insert(mDriftBottlePlayerPhoto, friendPhoto)
		--local index = table.getn(mDriftBottlePlayerPhoto)
		--mDriftBottlePlayerPhoto[index] = _insertFriendPhotoVec(mDriftBottlePlayerPhoto[index], photoData)
	end
end
-------------------------------------------------------
--
function DBS_getFriendPhotoName(account)
	--g_Loger:logError(tostring(table.getn(mDriftBottlePlayerPhoto)))
	for i, v in pairs(mDriftBottlePlayerPhoto) do
--		g_Loger:logError(v.account .."  " ..v.curName)

		if account == v.account then	-- 存在该账号
			--g_Loger:logError(v.account)
			--g_Loger:logError(v.curName)
			return v.curName
		end
	end
	return "NULL"
end
-----------------------------------------
--
function DBS_setFriendPhoto(winname, account)
	local photoName = DBS_getFriendPhotoName(account)
	if "NULL"~=photoName and ""~=photoName then
		EQGlobaClass.getDynamicCreateImagesetMgr():setWindowImage(winname, photoName)
	else	-- 设置默认好友头像
		g_WinMgr:getWindow(winname):setProperty("Image", "set:Button1 image:ImageButton41")
	end
end
-----------------------------------------
--
function DBS_exitDBS()
	if mStartDBS == true then
		DBS_closeBottleDetailInfoUI()
		DBS_closeMyBottleRecordsUI()
		DBS_closeContactBottleEditUI()
		DBS_closeNormalBottleEditUI()
		DBS_closeDropBottleStartUI()
		DBS_closeDBSUI()
		mStartDBS = false
	end
end