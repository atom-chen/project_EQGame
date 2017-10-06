-----------------------------------------
-- 文件说明	：处理路标逻辑
-- 作者		：
-- 修改		：xj
-- 日期		：2010/12/23
-----------------------------------------
local mSelcopy_id = 0
local mSelitm_idx = -1
local mLastClickSlotName = ""
local m_selectSceneFlag = 0	-- 当前选择的场景标识(add by hezhr)
g_gotoDlgVisible = false
g_gotoReqChangeScene = false
g_gotoSceneTotalPages = 1
g_gotoSceneCurPage = 0
g_gotoSceneSelItem = "" --选择的场景GID
g_gotoSceneSaveItem = ""
g_gotoRecSelItem = "" --选择的最近GID
g_gotoFavSelItem = "" --选择的收藏GID
g_gotoFavSelItemName = "" --选择的收藏框名
g_gotoFavSelItemIdx = 0 --选择的收藏索引
g_gotoEachPageMaxPanelCnt = 8 --每一页的最大框显示个数
g_gotoEachPageMaxPanelLogicCnt = 10 --每一页的最大框个数
g_gotoScenePanelNamePrefix = "Goto/Windows/Tab/Tab1/Blank"
g_gotoSceneTextNamePrefix = "Goto/Windows/Tab/Tab1/Blanktxt"
g_gotoRecPanelNamePrefix = "Goto/Windows/Tab/Tab2/Blank"
g_gotoRecTextNamePrefix = "Goto/Windows/Tab/Tab2/Blanktxt"
g_gotoFavPanelNamePrefix = "Goto/Windows/Tab/Tab3/Blank"
g_gotoFavTextNamePrefix = "Goto/Windows/Tab/Tab3/Blanktxt"
g_gotoSelPanelName = "Goto/Windows/SelPanel"
g_gotoLocalFile = "../Config/"
g_gotoNes = {} --最近
g_gotoFas = {} --收藏

g_WinMgr:getWindow("Goto/Windows__auto_closebutton__"):subscribeEvent("Clicked", "Goto_SetDialogVisible")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Btn_Go"):subscribeEvent("Clicked", "Goto_SceneBtnGo")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab2/Btn_Go"):subscribeEvent("Clicked", "Goto_RecBtnGo")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab3/Btn_Go"):subscribeEvent("Clicked", "Goto_FavBtnGo")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Btn_Add"):subscribeEvent("Clicked", "Goto_SceneBtnAddToFavorite")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab2/Btn_Add"):subscribeEvent("Clicked", "Goto_RecBtnAddToFavorite")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab3/Btn_Del"):subscribeEvent("Clicked", "Goto_FavBtnRemoveFavorite")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Gohometxt"):subscribeEvent("MouseClick", "Goto_SceneBtnGoHome")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Gohome"):subscribeEvent("MouseClick", "Goto_SceneBtnGoHome")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab2/Gohometxt"):subscribeEvent("MouseClick", "Goto_SceneBtnGoHome")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab2/Gohome"):subscribeEvent("MouseClick", "Goto_SceneBtnGoHome")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab3/Gohometxt"):subscribeEvent("MouseClick", "Goto_SceneBtnGoHome")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab3/Gohome"):subscribeEvent("MouseClick", "Goto_SceneBtnGoHome")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Pageup"):subscribeEvent("Clicked", "Goto_SceneBtnLastPage")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Pagedown"):subscribeEvent("Clicked", "Goto_SceneBtnNextPage")

g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Btn_Go/Txt"):setProperty("MousePassThroughEnabled", "True")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab2/Btn_Go/Txt"):setProperty("MousePassThroughEnabled", "True")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab3/Btn_Go/Txt"):setProperty("MousePassThroughEnabled", "True")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Btn_Add/Txt"):setProperty("MousePassThroughEnabled", "True")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab2/Btn_Add/Txt"):setProperty("MousePassThroughEnabled", "True")
g_WinMgr:getWindow("Goto/Windows/Tab/Tab3/Btn_Del/Txt"):setProperty("MousePassThroughEnabled", "True")

g_GameEventMgr:subscribeEvent("START_CHANGE_SCENE","Goto_OnStartChangeScene")
g_GameEventMgr:subscribeEvent("END_CHANGE_SCENE","Goto_OnEndChangeScene")

--------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------

--跳到某一页
function Goto_ToPage(p)
	local i,j
	local suffix = 1
	local startp = p * g_gotoEachPageMaxPanelLogicCnt + 1
	local endp = startp + g_gotoEachPageMaxPanelCnt - 1
	local rows = g_logictableMgr:getGotoRows()
	
	for i = 1, g_gotoEachPageMaxPanelCnt, 1 do
		g_WinMgr:getWindow(g_gotoScenePanelNamePrefix..i):setProperty("Image", "")
		g_WinMgr:getWindow(g_gotoScenePanelNamePrefix..i):setText("")
		g_WinMgr:getWindow(g_gotoSceneTextNamePrefix..i):setText("")
	end
	
	for i = startp, endp, 1 do
		for j = 1, rows, 1 do
			local row = g_logictableMgr:getGotoRow(j)
			if row.pos == i then
				g_WinMgr:getWindow(g_gotoScenePanelNamePrefix..suffix):setProperty("Image", row.image)
				g_WinMgr:getWindow(g_gotoScenePanelNamePrefix..suffix):setText(j)--存储rowId
				EQGlobaFunc.SetText(g_gotoSceneTextNamePrefix..suffix, row.name)
				break;
			end
		end
		
		suffix = suffix + 1
	end
	
	g_gotoSceneCurPage = p
	
	g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Number"):setText(tostring(g_gotoSceneCurPage + 1))
end

function Goto_RefreshPanelText(pp, tp, arr)
	local i, n
	
	for i = 1, g_gotoEachPageMaxPanelCnt, 1 do
		g_WinMgr:getWindow(pp..i):setProperty("Image", "")
		g_WinMgr:getWindow(pp..i):setText("")
		g_WinMgr:getWindow(tp..i):setText("")
	end
	
	n = table.getn(arr)
	
	for i = 1, n, 1 do
		local row = g_logictableMgr:getGotoRow(arr[i])
		
		g_WinMgr:getWindow(pp..i):setProperty("Image", row.image)
		g_WinMgr:getWindow(pp..i):setText(arr[i])--存储rowId
		EQGlobaFunc.SetText(tp..i, row.name)
	end
end

function Goto_RefreshFavList()
	--local cmb1 = CEGUI.toCombobox(g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/"))
	--local cmb2 = CEGUI.toCombobox(g_WinMgr:getWindow("Goto/Windows/Tab/Tab2/"))
	--local cmb3 = CEGUI.toCombobox(g_WinMgr:getWindow("Goto/Windows/Tab/Tab3/"))
	--
	--cmb1:resetList()
	--cmb2:resetList()
	--cmb3:resetList()
	--
	--local n = table.getn(g_gotoFas)
	--local i
	--
	--for i = 1, n, 1 do
		--local row = g_logictableMgr:getGotoRow(g_gotoFas[i])
		--
		--EQGlobaFunc.createComboBoxTextItem(cmb1:getName(), row.name, i)
		--EQGlobaFunc.createComboBoxTextItem(cmb2:getName(), row.name, i)
		--EQGlobaFunc.createComboBoxTextItem(cmb3:getName(), row.name, i)
	--end
end

function Goto_AddItemToArray(arr, item, head)
	local n = table.getn(arr)
	
	-- 限制最大值
	if n >= g_gotoEachPageMaxPanelCnt then
		if head then
			table.remove(arr, n)
		else
			table.remove(arr, 1)
		end
	end
	
	if head then
		table.insert(arr, 1, item)
	else
		table.insert(arr, item)
	end
end

function Goto_FillLocalValues(arr, line)
	if line == nil then
		return
	end
	
	local n = 0
	local i, j = string.find(line, "%d+")
	
	while i do
		local idx = tonumber(string.sub(line, i, j))
		local row = g_logictableMgr:getGotoRow(idx) --检查'最近'和'收藏'里的值是否存在
		if row then
			Goto_AddItemToArray(arr, idx, false)
		end
		i, j = string.find(line, "%d+", j + 1)
	end
end

function Goto_ClearArray(arr)
	local n = table.getn(arr)
	local i
	
	for i = 1, n, 1 do
		table.remove(arr)
	end
end

function Goto_LoadLocalData()
	local mPlayer = EQGlobaClass.getMainPlayer()
	
	Goto_ClearArray(g_gotoNes)
	Goto_ClearArray(g_gotoFas)
	Goto_FillLocalValues(g_gotoNes, g_logictableMgr:getGotoUserData(mPlayer:getPlayerAccount(), 0))
	Goto_FillLocalValues(g_gotoFas, g_logictableMgr:getGotoUserData(mPlayer:getPlayerAccount(), 1))
end

function Goto_SaveLocalValues(arr)
	local n = table.getn(arr)
	local str = ""
	
	if n > 0 then
		local tn = n - 1
		for i = 1, tn, 1 do
			str = str..tostring(arr[i]).." "
		end
		str = str..tostring(arr[n])
	end
	
	return str
end

function Goto_SaveLocalData()
	local mPlayer = EQGlobaClass.getMainPlayer()
	
	g_logictableMgr:setGotoUserData(mPlayer:getPlayerAccount(), 0, Goto_SaveLocalValues(g_gotoNes))
	g_logictableMgr:setGotoUserData(mPlayer:getPlayerAccount(), 1, Goto_SaveLocalValues(g_gotoFas))
end

function Goto_LoadUserData()
	--加载本地数据
	Goto_LoadLocalData()
	--刷新最近
	Goto_RefreshPanelText(g_gotoRecPanelNamePrefix, g_gotoRecTextNamePrefix, g_gotoNes)
	--刷新收藏
	Goto_RefreshPanelText(g_gotoFavPanelNamePrefix, g_gotoFavTextNamePrefix, g_gotoFas)
	Goto_RefreshFavList()
end

function Goto_OnLoad()
	local i
	
	--动态创建每个Tab页里的选择高亮框
	for i = 1, 3 do
		local sp = g_WinMgr:createWindow("NewEQHomeLook/NewStaticImage3", g_gotoSelPanelName..i)
		sp:setProperty("Image", "set:Button5 image:NewImage303")
		sp:setProperty("FrameEnabled", "False")
		sp:setProperty("BackgroundEnabled", "False")
		sp:setProperty("MousePassThroughEnabled", "True")
		sp:setWidth(CEGUI.UDim(0, 65))
		sp:setHeight(CEGUI.UDim(0, 65))
		sp:setXPosition(CEGUI.UDim(0, 0))
		sp:setYPosition(CEGUI.UDim(0, 0))
		sp:setVisible(false)
		g_WinMgr:getWindow("Goto/Windows/Tab/Tab"..i):addChildWindow(sp)
	end

	for i = 1, g_gotoEachPageMaxPanelCnt do
		g_WinMgr:getWindow(g_gotoScenePanelNamePrefix..i):subscribeEvent("MouseClick", "Goto_ScenePanelClicked")
		g_WinMgr:getWindow(g_gotoScenePanelNamePrefix..i):subscribeEvent("MouseDoubleClick", "Goto_ScenePanelDbClicked")
		
		g_WinMgr:getWindow(g_gotoRecPanelNamePrefix..i):subscribeEvent("MouseClick", "Goto_RecPanelClicked")
		g_WinMgr:getWindow(g_gotoRecPanelNamePrefix..i):subscribeEvent("MouseDoubleClick", "Goto_RecPanelDbClicked")
		
		g_WinMgr:getWindow(g_gotoFavPanelNamePrefix..i):subscribeEvent("MouseClick", "Goto_FavPanelClicked")
		g_WinMgr:getWindow(g_gotoFavPanelNamePrefix..i):subscribeEvent("MouseDoubleClick", "Goto_FavPanelDbClicked")
	end

	local maxPos = 0;
	local gotoRows = g_logictableMgr:getGotoRows()

	for i = 1, gotoRows, 1 do
		local row = g_logictableMgr:getGotoRow(i)
		if row.pos > maxPos then
			maxPos = row.pos
		end
	end

	g_gotoSceneTotalPages = math.floor(maxPos / g_gotoEachPageMaxPanelLogicCnt + 1)
	
	g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/Total"):setText(tostring(g_gotoSceneTotalPages))
	
	Goto_ToPage(0)
	MainUI_ShieldClick(g_gotoUI)
end

Goto_OnLoad()

function Goto_SetOtherBtnEnbled(enable)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_PlayerProperty"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_House"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Storage"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Mall"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_PrePaid"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Action"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Friendlist"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_System"):setEnabled(enable)
	if g_sceneMgr:getSceneType() ~= EQGlobaClass.EQGameSceneMgr.SCENE_COMMON and g_isInFriendHouse == false then
		g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):setEnabled(enable)
		g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Storage"):setEnabled(enable)
	end
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PlayGame"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Email"):setEnabled(enable)
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang2"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/telephone"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof"):setEnabled(enable)
	g_WinMgr:getWindow("MainRoot/liwu"):setEnabled(enable)
	g_WinMgr:getWindow("DFwnd_MainRoot/xiala1"):setEnabled(enable)
	g_WinMgr:getWindow("map"):setEnabled(enable)
	FitModeShopCenter_closeIntroUI()	-- 关闭家具介绍界面(2011/4/15 add by hezhr)
end

function Goto_CloseOtherWin()
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	CloseHouseSalesUI() --关闭房屋售卖界面
	PlayerPropertyUI_OnClosePlayerProperty() --关闭玩家属性
	closeOtherPlayerInfoUI() --关闭其它玩家属性
	CloseHouseInfoUI() -- 关闭房屋信息
	StorageUI_OnCloseStorage() -- 关闭我的背包
	Action_closeActionUI() -- 关闭表情
	RemoveStorageUI() --关闭仓库
	ConfigOption_Close() -- 关闭系统配置
	RemoveMailBoxUI() --邮件
	MainUI_ClosePlayerAttr() -- 关闭玩家小属性窗口
	MainUI_closeFriendlist() -- 关闭好友
	FList_closePopWin()
	BroadCast_SpeakerClose(nil) --关闭公告
	CloseAddressBook() --关闭家政
end

function Goto_CloseDialog()
	if g_gotoDlgVisible then
		EQGlobaFunc.reqCloseGuide()
	end
end

function Goto_OpenDialog()
	EQGlobaFunc.reqOpenGuide()
end
--
local firstOpenGuideUI = true
function Goto_SetDialogVisible()
	if g_gotoDlgVisible then
		EQGlobaFunc.reqCloseGuide()
	else
		EQGlobaFunc.reqOpenGuide()
		UIM_DoOpenAndCloseUI(g_GuideUINames.root)
		if firstOpenGuideUI then
			do_task_script("script26")
			firstOpenGuideUI = false
		end
	end
end

--成功请求打开场景切换面板
function Goto_OnStartChangeScene(args)
	--Goto_CloseOtherWin()
	Goto_SetOtherBtnEnbled(false)
	UIM_OpenWindow(g_GuideUINames.root)
	g_gotoDlgVisible = true
end

--成功请求关闭场景切换面板
function Goto_OnEndChangeScene(args)
	if g_gotoDlgVisible then
		UIM_CloseWindow(g_GuideUINames.root)
		Goto_setCopyID(0)
		local sceneStateList = CEGUI.toCombobox(g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/"))
		sceneStateList:resetList()
		sceneStateList:getEditbox():setText("")
		Goto_SetOtherBtnEnbled(true)
		g_gotoDlgVisible = false
	end
end
------------------------------------
--
function Goto_getUIOpenedFlag()
	return g_gotoDlgVisible
end
--开始切换场景
function Goto_OnBeginChangeScene()
	if g_gotoReqChangeScene then
		PlaySpecialVoice(g_special_voice_map.change_scene)
	end
end

--成功切换场景回调
function Goto_OnReqChangeScene(args)
	if false == g_gotoReqChangeScene then
		return
	end
	
	--关闭其它
	--Goto_CloseOtherWin()
	--关闭自己
	Goto_SetDialogVisible()
	--保存到最近
	if g_gotoSceneSaveItem ~= "" then
		Goto_AddItemToArray(g_gotoNes, tonumber(g_gotoSceneSaveItem), true)
		--刷新最近
		Goto_RefreshPanelText(g_gotoRecPanelNamePrefix, g_gotoRecTextNamePrefix, g_gotoNes)
		--保存
		Goto_SaveLocalData()
		
		g_gotoSceneSaveItem = ""
	end
	
	g_gotoReqChangeScene = false
end

--根据场景id切换场景
function Goto_ReqChangeScene(sid)
	g_gotoReqChangeScene = true
	g_sceneMgr:reqChangeScene(sid, g_mainplayer:getPlayerAccount())
end

--根据选择的场景索引切换场景
function Goto_ChangeScene(si)
	if si == "" then
		return false
	end
	
	g_gotoReqChangeScene = true
	g_sceneMgr:reqChangeSceneByGuide(tonumber(si), g_mainplayer:getPlayerAccount(),Goto_getCopyID())
	Goto_setCopyID(0)
	local row = g_logictableMgr:getGotoRow(tonumber(si))
	if nil ~= row then
		m_selectSceneFlag = row.target	-- (add by hezhr)
	end
	return true
end

--单击前往按钮
function Goto_SceneBtnGo(args)
	g_gotoSceneSaveItem = ""
	if DBS_getDropingFlag() or DBS_getPickingFlag() then
		DBS_cantChangeSceneTip()
	else
		if Goto_ChangeScene(g_gotoSceneSelItem) then	
			g_gotoSceneSaveItem = g_gotoSceneSelItem
		end
	end
end

function Goto_RecBtnGo(args)
	g_gotoSceneSaveItem = ""
	Goto_ChangeScene(g_gotoRecSelItem)
end

function Goto_FavBtnGo(args)
	g_gotoSceneSaveItem = ""
	Goto_ChangeScene(g_gotoFavSelItem)
end

function Goto_ClickedToSel(si, sn, wp)
	local sp = g_WinMgr:getWindow(sn)
	
	--高亮或清除选择状态
	if si == "" then
		sp:setVisible(false)
	else
		sp:setXPosition(wp:getXPosition())
		sp:setYPosition(wp:getYPosition())
		sp:setVisible(true)
	end
end

--单击选择一个场景
function Goto_ScenePanelClicked(args)
	local win = CEGUI.toWindowEventArgs(args).window
	if win:getName() == mLastClickSlotName then
		
	else
		mSelitm_idx = -1
		Goto_setCopyID(0)
	end
	local sceneStateList = CEGUI.toCombobox(g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/"))
	--sceneStateList:resetList()
	--sceneStateList:getEditbox():setText("")
	g_gotoSceneSelItem = win:getText()
	if g_gotoSceneSelItem ~= "" then
		g_sceneMgr:reqCommSeceneState(tonumber(g_gotoSceneSelItem))
	end
	Goto_ClickedToSel(g_gotoSceneSelItem, g_gotoSelPanelName.."1", win)
	mLastClickSlotName = win:getName()
end

function Goto_RecPanelClicked(args)
	local win = CEGUI.toWindowEventArgs(args).window
	
	g_gotoRecSelItem = win:getText()
	
	Goto_ClickedToSel(g_gotoRecSelItem, g_gotoSelPanelName.."2", win)
end

function Goto_FavPanelClicked(args)
	local win = CEGUI.toWindowEventArgs(args).window
	
	g_gotoFavSelItem = win:getText()
	g_gotoFavSelItemName = win:getName()
	
	Goto_ClickedToSel(g_gotoFavSelItem, g_gotoSelPanelName.."3", win)
end

--双击进入一个场景
function Goto_ScenePanelDbClicked(args)
	Goto_ScenePanelClicked(args)
	Goto_SceneBtnGo(args)
	g_eqoisMgr:setSkip(true)
	--CEGUI.toMouseEventArgs(args).handled = true
end

function Goto_RecPanelDbClicked(args)
	Goto_RecPanelClicked(args)
	Goto_RecBtnGo(args)
	g_eqoisMgr:setSkip(true)
	--CEGUI.toMouseEventArgs(args).handled = true
end

function Goto_FavPanelDbClicked(args)
	Goto_FavPanelClicked(args)
	Goto_FavBtnGo(args)
	g_eqoisMgr:setSkip(true)
	--CEGUI.toMouseEventArgs(args).handled = true
end

function Goto_AddFavorite(si)
	if si == "" then
		return
	end
	
	local idx = tonumber(si)
	local n = table.getn(g_gotoFas)
	local i
	for i = 1, n do
		if idx == g_gotoFas[i] then
			MsgBox_createMsgBox(g_MainUIWindows,0,"","该场景已经收藏！", "MsgBox_cancelFunc", "")
			return
		end
	end
	
	Goto_AddItemToArray(g_gotoFas, idx, true)
	--刷新收藏
	Goto_RefreshPanelText(g_gotoFavPanelNamePrefix, g_gotoFavTextNamePrefix, g_gotoFas)
	Goto_RefreshFavList()
	--保存
	Goto_SaveLocalData()
end

--单击加入收藏
function Goto_SceneBtnAddToFavorite(args)
	Goto_AddFavorite(g_gotoSceneSelItem)
end

function Goto_RecBtnAddToFavorite(args)
	Goto_AddFavorite(g_gotoRecSelItem)
end

--单击删除收藏
function Goto_FavBtnRemoveFavorite(args)
	if g_gotoFavSelItem == "" then
		return
	end
	
	local i
	
	for i = 1, g_gotoEachPageMaxPanelCnt do
		local wn = g_gotoFavPanelNamePrefix..i
		
		if wn == g_gotoFavSelItemName then
			g_gotoFavSelItemIdx = i
			MsgBox_createMsgBox(g_MainUIWindows,0,"","确定要删除该收藏吗？", "Goto_FavDoRemoveFavorite", "MsgBox_cancelFunc")
			break;
		end
	end
end

function Goto_FavDoRemoveFavorite()
	table.remove(g_gotoFas, g_gotoFavSelItemIdx)
	--刷新收藏
	Goto_RefreshPanelText(g_gotoFavPanelNamePrefix, g_gotoFavTextNamePrefix, g_gotoFas)
	Goto_RefreshFavList()
	--保存
	Goto_SaveLocalData()
	
	if g_gotoFavSelItemIdx > table.getn(g_gotoFas) then
		Goto_ClickedToSel("", g_gotoSelPanelName.."3", nil)
	end
end

--回家
function Goto_SceneBtnGoHome(args)
	if DBS_getDropingFlag() or DBS_getPickingFlag() then
		DBS_cantChangeSceneTip()
	else
		Goto_ReqChangeScene(EQGlobaClass.EQGameSceneMgr.SCENE_HOUSE)
	end
end

--上一页
function Goto_SceneBtnLastPage(args)
	if g_gotoSceneCurPage > 0 then
		Goto_ToPage(g_gotoSceneCurPage - 1)
	end
end

--下一页
function Goto_SceneBtnNextPage(args)
	if g_gotoSceneCurPage < g_gotoSceneTotalPages - 1 then
		Goto_ToPage(g_gotoSceneCurPage + 1)
	end
end
-----------------------------------------
--处理场景是否在加载中(add by hezhr)
-----------------------------------------
function GameScene_endLoading()
	--g_Loger:logError("--------m_selectSceneFlag------------" ..tostring(m_selectSceneFlag))
	if EQGlobaClass.EQGameSceneMgr.SCENE_COMMON==g_sceneMgr:getSceneType() then
		if 1==m_selectSceneFlag then	-- 乐园第一街(add by hezhr)
			m_selectSceneFlag = 0
			do_task_script("script3")	--执行新手任务脚本3
		elseif 2 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			--do_task_script("script18")	--幽静山谷
		elseif 3 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			--do_task_script("script19")	--乐园大道
		elseif 4 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			do_task_script("script20")	--服装店
		elseif 5 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			do_task_script("script21")	--家具店
		elseif 6 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			--do_task_script("script22")	--美容院
		elseif 7 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			--do_task_script("script23")	--道具店
		elseif 8 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			--do_task_script("script24")	--医院
		elseif 9 == m_selectSceneFlag then
			m_selectSceneFlag = 0
			--do_task_script("script25")	--售楼部
		end
	end	
	if g_SceneMgr:inMyHouse() then	-- 进入自己房屋
		do_task_script("script9")	-- (玩家进入自己房屋)执行新手任务脚本9(2011/7/11 add by hezhr)
	end
end
---
--
function Goto_setSceneState(id,scene_id,copy_id,copy_name,status)
	--g_Loger:logMessage("------scene_id------"..tostring(scene_id))
	--g_Loger:logMessage("-------copy_id-----"..tostring(copy_id))
	--g_Loger:logMessage("-------copy_name-----"..tostring(copy_name))
	--g_Loger:logMessage("------status------"..tostring(status))
	local sceneStateList = CEGUI.toCombobox(g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/"))
	sceneStateList:removeEvent("ListSelectionAccepted")
	sceneStateList:subscribeEvent("ListSelectionAccepted", "OnLineListSelectionAccepted")

	local status_col
	if status ==0 then --闲 绿色
		local col = CEGUI.colour:new_local(0, 1, 0, 1)
		local crect = CEGUI.ColourRect(col)
		status_col = CEGUI.PropertyHelper:colourRectToString(crect)
	elseif status == 1 then --普通 黄绿
		local col = CEGUI.colour:new_local(0.68, 1, 0.2, 1)
		local crect = CEGUI.ColourRect(col)
		status_col = CEGUI.PropertyHelper:colourRectToString(crect)
	elseif status == 2 then --忙 黄
		local col = CEGUI.colour:new_local(1, 1, 0, 1)
		local crect = CEGUI.ColourRect(col)
		status_col = CEGUI.PropertyHelper:colourRectToString(crect)
	else	--满 红
		local col = CEGUI.colour:new_local(1, 0, 0, 1)
		local crect = CEGUI.ColourRect(col)
		status_col = CEGUI.PropertyHelper:colourRectToString(crect)
	end
	EQGlobaFunc.createComboBoxTextItem("Goto/Windows/Tab/Tab1/",copy_name,copy_id,status_col)
end
--
function Goto_setDefSelLine()
	local sceneStateList = CEGUI.toCombobox(g_WinMgr:getWindow("Goto/Windows/Tab/Tab1/"))
	if mSelitm_idx == -1 then
		local line_item = sceneStateList:getListboxItemFromIndex(0)
		if line_item ~= nil then
			sceneStateList:setItemSelectState(0,true)
			Goto_setCopyID(sceneStateList:getSelectedItem():getID())
			EQGlobaFunc.setComoboxTextItemTxtColors("Goto/Windows/Tab/Tab1/")
			--g_Loger:logMessage("-------mSelcopy_id------"..tostring(mSelcopy_id))
		end
	else
		local line_item = sceneStateList:getListboxItemFromIndex(mSelitm_idx)
		if line_item ~= nil then
			sceneStateList:setItemSelectState(mSelitm_idx,true)
			Goto_setCopyID(sceneStateList:getSelectedItem():getID())
			EQGlobaFunc.setComoboxTextItemTxtColors("Goto/Windows/Tab/Tab1/")
			--g_Loger:logMessage("-------mSelcopy_id------"..tostring(mSelcopy_id))
		end
	end
end
--
function OnLineListSelectionAccepted(args)
	local cbox = CEGUI.toCombobox(CEGUI.toWindowEventArgs(args).window)
	Goto_setCopyID(cbox:getSelectedItem():getID())
	mSelitm_idx = cbox:getItemIndex(cbox:getSelectedItem())
	--g_Loger:logMessage("-------itm_idx------"..tostring(itm_idx))
	--g_Loger:logMessage("-------mSelcopy_id------"..tostring(mSelcopy_id))
	EQGlobaFunc.setComoboxTextItemTxtColors("Goto/Windows/Tab/Tab1/")

end
--
function Goto_getCopyID()
	return mSelcopy_id
end
--
function Goto_setCopyID(copyID)
	--mSelcopy_id = copyID
	mSelcopy_id = 1
end