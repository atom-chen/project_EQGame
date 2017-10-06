--------------------------------------
--  文件说明：主界面相关逻辑处理
-----------------------------------------
--
--
--Common_injectCommonEvt("MainRoot/SImg_HMenubar")
--Common_injectCommonEvt("MainRoot/SImg_VMenubar")

g_playerAttrWinVisible = false
g_smallMapVisible = true;
g_reqPlayDataByPlayerAttr = false
g_isInFriendHouse = false

-----------------------------------------
--  房屋观察处理
-----------------------------------------
g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair"):subscribeEvent("Clicked", "MainUI_ViewStair")
g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof"):subscribeEvent("Clicked", "MainUI_ViewWall")
g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Navigate"):subscribeEvent("Clicked", "Goto_SetDialogVisible")
g_GameEventMgr:subscribeEvent("CHANGE_SCENE", "MainUI_ChangeScnen")
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Friendlist"):subscribeEvent("Clicked", "MainUI_ShowFriendList")
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Mall"):subscribeEvent("Clicked", "ShopCenter_Open")
g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PlayGame"):removeEvent("Clicked")
g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PlayGame"):subscribeEvent("Clicked", "PopFunnyUI")
--暂时以此按键作为房屋买卖按钮
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_House"):removeEvent("Clicked")
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_House"):subscribeEvent("Clicked", "PopHouseInfoUI")
--g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Email"):subscribeEvent("Clicked", "NpcShop_Open")

--暂时以此按键作为家政系统按钮
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Email"):subscribeEvent("Clicked", "SwitchMailBox")

--暂时作为家政界面
g_WinMgr:getWindow("MainRoot/telephone"):subscribeEvent("Clicked", "PopAddressBook")

--仓库
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Storage"):subscribeEvent("Clicked", "ReqPopStorageUI")

--制作礼物按钮
g_WinMgr:getWindow("MainRoot/liwu"):removeEvent("Clicked")
g_WinMgr:getWindow("MainRoot/liwu"):subscribeEvent("Clicked", "SwitchGiveGiftUI")

--自动播放公告按钮
g_WinMgr:getWindow("DFwnd_MainRoot/broadcast"):subscribeEvent("Clicked", "BroadCast_OpenAutoUI")

--
g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang2"):subscribeEvent("Clicked", "MainUI_SHPlayerAttr")
g_WinMgr:getWindow("DFwnd_MainRoot/xiala1"):subscribeEvent("Clicked", "MainUI_SHSmallMap")

--放置礼物按钮
g_WinMgr:getWindow("DFwnd_MainRoot/liwuanjian"):removeEvent("MouseClick")
g_WinMgr:getWindow("DFwnd_MainRoot/liwuanjian"):subscribeEvent("MouseClick", "SwitchPutGiftUI")

--分辨率改变
g_GameEventMgr:subscribeEvent("CHANGE_RESOLUTION", "Resolution_Change")
g_GameEventMgr:subscribeEvent("CHANGE_DAYNIGHT", "DayNight_Change")

--角色隐藏按钮
g_WinMgr:getWindow("DFwnd_MainRoot/hide"):subscribeEvent("Clicked", "clickHidePlayerBtn")
g_WinMgr:getWindow("DFwnd_MainRoot/hide"):setVisible(false)

--鲜花鸡蛋
g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/img_flower"):subscribeEvent("MouseClick", "reqFlowerLog")
g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/txt1"):subscribeEvent("MouseClick", "reqFlowerLog")
g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/img_egg"):subscribeEvent("MouseClick", "reqEggLog")
g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/txt2"):subscribeEvent("MouseClick", "reqEggLog")

--回家按钮

g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Gohome"):subscribeEvent("MouseClick", "MainUI_quickGoHome")
--切换跑步
g_WinMgr:getWindow("DFwnd_MainRoot/RunBtn"):subscribeEvent("MouseClick", "MainUI_switchRunWalk")

----------------------------------------------------------------------------------------------------------------------------
--
local isRun = false
function MainUI_switchRunWalk(args)
	local Btn = g_WinMgr:getWindow("DFwnd_MainRoot/RunBtn")

	if isRun then
		isRun = false
		g_mainplayer:setMoveType(0)
		if g_mainplayer:getMoveForWhat() == 1 then
			g_mainplayer:reqContinueWalk()
		elseif g_mainplayer:getMoveForWhat() == 2 then
			EQGlobaFunc.continueWalkForPickMagicbox()
		elseif g_mainplayer:getMoveForWhat() == 3 then
			EQGlobaFunc.continueWalkForUseFurni()
		end
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage445")
		--Btn:setProperty("HoverImage", "set:Button5 image:NewImage446")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage445")
	else
		isRun = true
		g_mainplayer:setMoveType(1)
		if g_mainplayer:getMoveForWhat() == 1 then
			g_mainplayer:reqContinueWalk()
		elseif g_mainplayer:getMoveForWhat() == 2 then
			EQGlobaFunc.continueWalkForPickMagicbox()
		elseif g_mainplayer:getMoveForWhat() == 3 then
			EQGlobaFunc.continueWalkForUseFurni()
		end
		
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage447")
		--Btn:setProperty("HoverImage", "set:Button5 image:NewImage446")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage447")
	end
end
---------------------------------
--
function MainUI_getIsRunFlag()
	return isRun
end
-------
local StairFlag = 1  --1为一楼

function MainUI_ViewStair(args)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	local Btn = CEGUI.toWindowEventArgs(args).window
	if StairFlag == 1 then
		EQGlobaFunc.HouseUp()
		StairFlag = 2
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage239")
		Btn:setProperty("HoverImage", "set:Button5 image:NewImage274")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage239")
		return
	end
	if StairFlag == 2 then
		EQGlobaFunc.HouseDown()
		StairFlag = 1
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage28")
		Btn:setProperty("HoverImage", "set:Button5 image:NewImage29")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage30")
	end
end

-- 改变切换观察楼层按钮的状态(2011/6/14 add by hezhr)
function MainUI_setViewBtn(upView)
	local button = g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair")
	if nil == button then
		return
	end
	if upView then
		StairFlag = 2
		button:setProperty("NormalImage", "set:Button5 image:NewImage239")
		button:setProperty("HoverImage", "set:Button5 image:NewImage274")
		button:setProperty("PushedImage", "set:Button5 image:NewImage239")
	else
		StairFlag = 1
		button:setProperty("NormalImage", "set:Button5 image:NewImage28")
		button:setProperty("HoverImage", "set:Button5 image:NewImage29")
		button:setProperty("PushedImage", "set:Button5 image:NewImage30")
	end
end

function MainUI_RefreshWall()
	local Btn = g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof")
	local WallFlag = EQGlobaFunc.getHouseMode()
	if WallFlag == 0 then
		Btn:setVisible(false)
	elseif WallFlag == 1 then
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage31")
		Btn:setProperty("HoverImage", "set:Button5 image:NewImage32")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage33")
	elseif WallFlag == 2 then
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage272")
		Btn:setProperty("HoverImage", "set:Button5 image:NewImage276")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage272")
	elseif WallFlag == 3 then
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage273")
		Btn:setProperty("HoverImage", "set:Button5 image:NewImage277")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage273")
	elseif WallFlag == 4 then
		Btn:setProperty("NormalImage", "set:Button5 image:NewImage240")
		Btn:setProperty("HoverImage", "set:Button5 image:NewImage275")
		Btn:setProperty("PushedImage", "set:Button5 image:NewImage240")
	end
end

function MainUI_ViewWall()
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	
	local WallFlag = EQGlobaFunc.getHouseMode()
	if WallFlag == 1 then
		EQGlobaFunc.ChangeHouseMode(2)
	elseif WallFlag == 2 then
		EQGlobaFunc.ChangeHouseMode(3)
	elseif WallFlag == 3 then
		EQGlobaFunc.ChangeHouseMode(4)
	elseif WallFlag == 4 then
		EQGlobaFunc.ChangeHouseMode(1)
	end

	MainUI_RefreshWall()
end
-----------------------------------------
--  XiaoMing Add
--  场景切换 1代表室内场景 2代表室外场景
-----------------------------------------

function MainUI_ReqChangeScene()
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	local sceneType = g_sceneMgr:getSceneType()

	if sceneType == EQGlobaClass.EQGameSceneMgr.SCENE_HOUSE then
		g_sceneMgr:reqChangeScene(EQGlobaClass.EQGameSceneMgr.SCENE_COMMON, g_mainplayer:getPlayerAccount())
	else
		g_sceneMgr:reqChangeScene(EQGlobaClass.EQGameSceneMgr.SCENE_HOUSE, g_mainplayer:getPlayerAccount())
	end
end

function MainUI_ChangeScnen()
	local sceneType = g_sceneMgr:getSceneType()
	
	if sceneType ~= EQGlobaClass.EQGameSceneMgr.SCENE_FARM then
		if g_isOpenIITUI == true then
			IIT_closeIITUI()
		end
		DBS_exitDBS()
		Goto_CloseOtherWin()
		Fne_Close()
	end
	
	g_isInFriendHouse = false
	if sceneType == EQGlobaClass.EQGameSceneMgr.SCENE_HOUSE then
		if not g_objmgr:isMainPlayer(MainUI_getHouseOwner()) then
			g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair"):setVisible(true)
			g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof"):setVisible(true)
			g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):setEnabled(false)
			g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Storage"):setEnabled(false)
			g_WinMgr:getWindow("MainRoot/telephone"):setVisible(false)
			g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Gohome"):setVisible(true)
			g_isInFriendHouse = true
			if CreatePlayer_getFirstInflag() then
				FitModeShopCenter_RestoreDayNight()
			end
		else
			g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Gohome"):setVisible(false)
			g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair"):setVisible(true)
			g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof"):setVisible(true)
			g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):setEnabled(true)
			g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Storage"):setEnabled(true)
			g_WinMgr:getWindow("MainRoot/telephone"):setVisible(true)
			if CreatePlayer_getFirstInflag() then
				EQGlobaFunc.changeDayNight(true, true)
			else
				FitModeShopCenter_RestoreDayNight()
			end
		end
		MainUI_setMapTxtPos(false,0,0)
		g_WinMgr:getWindow("DFwnd_MainRoot/hide"):setVisible(false)
		
	elseif sceneType == EQGlobaClass.EQGameSceneMgr.SCENE_COMMON then
		g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Gohome"):setVisible(true)
		g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair"):setVisible(false)
		g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof"):setVisible(false)
		g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):setEnabled(false)
		g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Storage"):setEnabled(false)
		g_WinMgr:getWindow("MainRoot/telephone"):setVisible(false)
		g_MainUIWindows:removeChildWindow("furniMenu")
		g_WinMgr:getWindow("DFwnd_MainRoot/hide"):setVisible(true)
		g_UIRoot:activate()
		if CreatePlayer_getFirstInflag() then
			FitModeShopCenter_RestoreDayNight()
		end
		refreshHouseSaleUI()
		local map_height = g_WinMgr:getWindow("map/1"):getHeight():asAbsolute(600)
		--local mapTxt_height = g_WinMgr:getWindow("map/map5"):getHeight():asAbsolute(600)
		if g_smallMapVisible then
			MainUI_setMapTxtPos(true,0,map_height-5)
		else
			MainUI_setMapTxtPos(false,0,map_height-5)
		end
	end
	
	MainUI_RefreshWall()
	Common_eventFunc()
	MainUI_saveHouseOwner("")
end
--------------------------------------------------------------------
--
local house_owner = ""
function MainUI_saveHouseOwner(who)
	house_owner = who
end
--------------------------------------------------------------------
--
function MainUI_getHouseOwner()
	local owner = house_owner
	
	if owner == "" then
		owner = g_mainplayer:getPlayerAccount()
	end
	
	return owner
end
-----------------------------------------
--  快速定位玩家位置处理
-----------------------------------------
--g_WinMgr:getWindow("MainRoot/PointToPlayer"):subscribeEvent("Clicked", "MainUI_OnLocatePlayer")
--
function MainUI_OnLocatePlayer()
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	EQGlobaFunc.LookAtMainPlayer()
end

--
g_openFriendlist = false
function MainUI_ShowFriendList()
	if g_openFriendlist == false then
		OpenFriendListUI()
		
		UIM_DoOpenAndCloseUI(g_FriendListUINames.root)
	else
		MainUI_closeFriendlist()
	end
end

function DoOpenFriendListUI()
	if g_openFriendlist then
		return
	end
	
	OpenFriendListUI()
end

function OpenFriendListUI()
	--g_FriendSys:reqFriendList()
	local frdListSize = {x,y}
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()

	frdListSize = g_FriendListUI:getSize()
	if 500+g_FriendListUI:getWidth():asAbsolute(screenW) > screenW then
		local posX = screenW - g_FriendListUI:getWidth():asAbsolute(screenW) - 35
		g_FriendListUI:setPosition(CEGUI.UVector2(CEGUI.UDim(1,-40)-frdListSize.x,CEGUI.UDim(1,-35)-frdListSize.y))
	else
		g_FriendListUI:setPosition(CEGUI.UVector2(CEGUI.UDim(1,-40)-frdListSize.x,CEGUI.UDim(1,-35)-frdListSize.y))
	end
	
	g_WinMgr:getWindow("freindlist__auto_closebutton__"):removeEvent("Clicked")
	g_WinMgr:getWindow("freindlist__auto_closebutton__"):subscribeEvent("Clicked", "MainUI_closeFriendlist")
	g_WinMgr:getWindow("freindlist"):subscribeEvent("MouseClick", "FList_closePopWin")
	
	UIM_OpenWindow(g_FriendListUINames.root)
	g_openFriendlist = true
end

function CloseFriendListUI()
	UIM_CloseWindow(g_FriendListUINames.root)
	UIM_DoOpenAndCloseUI(g_FriendListUINames.root)
	g_openFriendlist = false
	g_DoSelectMailRecver = false
	g_DoSelectGiftRecver = false
end

function MainUI_closeFriendlist()
	g_WinMgr:getWindow("freindlist/searchbox"):setText("")
	FList_closePopWin()
	CloseFriendListUI()
end

function DoCloseFriendListUI()
	if false == g_openFriendlist then
		return
	end
	
	MainUI_closeFriendlist()
end
--
function FList_closePopWin()
	if g_WinMgr:isWindowPresent("friendlist_manage") then
		g_MainUIWindows:removeChildWindow("friendlist_manage")
		g_MainUIWindows:removeChildWindow("gmfriend_manage")
	end
end

--在好友列表中左键单击选择
function OnSelectFriendChanged(args)
	if "" == args then
		return
	end
	
	Mail_OnSelectFriendChanged(args)
	Mail_OnSelectFriendChanged1(args)
end

----------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------

function OnLoginRepeat(args)
	local parent = g_MainUIWindows;
	
	if args == 1 then
		parent = g_FarmUI
	end
	
	MsgBox_createMsgBox(parent,0,"","您的帐号在另一地点登录，您被迫下线！", "ConfirmExit", "")
end

function MainUI_updateUserInfo()
	local lbUserName = "DFwnd_MainRoot/tip/touxiang4/1"
	local lbUserMoney = "DFwnd_MainRoot/tip/touxiang7/"
	local lbUserLevel = "DFwnd_MainRoot/tip/touxiang5/1"
	local imgUserHealth = "DFwnd_MainRoot/tip/tip1"
	
	local mPlayer = EQGlobaClass.getMainPlayer()
	local player_name = EQGlobaFunc.returnValidString(mPlayer:getName())
	-- 玩家名字,有描边效果(hezhr)
	local userName = g_WinMgr:getWindow("DFwnd_MainRoot/tip/userName")
	if nil == userName then
		userName = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "DFwnd_MainRoot/tip/userName")
		EQGlobaFunc.toMultiLineBox(userName:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(userName:getName()):setCols("#FFFFFF")
		EQGlobaFunc.toMultiLineBox(userName:getName()):setSide("#000000", 1)
		EQGlobaFunc.toMultiLineBox(userName:getName()):setAlign("AT_CENTER_VER")
		EQGlobaFunc.toMultiLineBox(userName:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(userName:getName()):setSize(78, 20)
		g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang4"):addChildWindow(userName)
	end
	EQGlobaFunc.toMultiLineBox(userName:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(userName:getName()):addElementText(player_name, false)
	-- 玩家等级,有描边效果(hezhr)
	local userLevel = g_WinMgr:getWindow("DFwnd_MainRoot/tip/userLevel")
	if nil == userLevel then
		userLevel = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "DFwnd_MainRoot/tip/userLevel")
		EQGlobaFunc.toMultiLineBox(userLevel:getName()):setFont("simsun-11")
		EQGlobaFunc.toMultiLineBox(userLevel:getName()):setCols("#FFFFFF")
		EQGlobaFunc.toMultiLineBox(userLevel:getName()):setSide("#1443A2", 2)
		EQGlobaFunc.toMultiLineBox(userLevel:getName()):setAlign("AT_CENTER_VER")
		EQGlobaFunc.toMultiLineBox(userLevel:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(userLevel:getName()):setSize(25, 25)
		g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang5"):addChildWindow(userLevel)
	end
	EQGlobaFunc.toMultiLineBox(userLevel:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(userLevel:getName()):addElementText("0", false)
	--g_Loger:logMessage("level: "..tostring(mPlayer:getLevel()))
	-- FSetText(lbUserName, mPlayer:getName())
	--FSetText(lbUserMoney, "$:9999999")
	--FSetText(lbUserLevel, "1")
	
	--g_WinMgr:getWindow(imgUserHealth):setProperty("Image", "set:Button6 image:NewImage81")
end

function MainUI_setUserHead(account, info)
	if "" ~= info.photo_type and "image/gif" ~= info.photo_type then
		EQGlobaClass.getDynamicCreateImagesetMgr():setWindowImage("DFwnd_MainRoot/tip/touxiang6", info.photo_data, account)
	else
		if true == EQGlobaClass.getMainPlayer():isMale() then
			g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang6"):setProperty("Image", "set:Button6 image:NewImage85")
		else
			g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang6"):setProperty("Image", "set:Button6 image:NewImage84")
		end
	end	
end

function MainUI_OpenPlayerAttr()
	--g_MainUIWindows:addChildWindow(g_PlayerAttrUI)
	UIM_OpenWindow(g_PlayerAttrUINames.root)
	g_playerAttrWinVisible = true
end

function MainUI_ClosePlayerAttr()
	--g_MainUIWindows:removeChildWindow(g_PlayerAttrUI)
	UIM_CloseWindow(g_PlayerAttrUINames.root)
	g_playerAttrWinVisible = false
end

function MainUI_SHPlayerAttr()
	if g_playerAttrWinVisible then
		MainUI_ClosePlayerAttr()
	else
		g_reqPlayDataByPlayerAttr = true;
		EQGlobaFunc.reqPlayerBasicData()
		
		MainUI_OpenPlayerAttr()
		UIM_DoOpenAndCloseUI(g_PlayerAttrUINames.root)
	end

end

function MainUI_updateAddress(strAddr)
	local lbAddr = "DFwnd_MainRoot/1/1/1"
	
	FSetText(lbAddr, strAddr)
end

function MainUI_updateXY(nX, nY, nZ)
	local lbXYZ = "DFwnd_MainRoot/1/2"
	
	FSetText(lbXYZ, nX..","..nY..","..nZ)
end

--穿透。。。。
--MainUI_ShieldClick(g_smallMapUI)
MainUI_ShieldClick(g_PlayerAttrUI)

function MainUI_SetSmallMapVisible(visible)
	local sceneType = g_sceneMgr:getSceneType()
	if sceneType == EQGlobaClass.EQGameSceneMgr.SCENE_HOUSE then
		g_WinMgr:getWindow("map/1"):setVisible(false)
		g_WinMgr:getWindow("map/1/"):setVisible(false)
		g_WinMgr:getWindow("map/1/+"):setVisible(false)
		g_WinMgr:getWindow("map/1/-"):setVisible(false)
		g_WinMgr:getWindow("map/2"):setVisible(false)
		if g_smallMapVisible then
			g_WinMgr:getWindow("map/map5"):setVisible(true)
		else
			g_WinMgr:getWindow("map/map5"):setVisible(visible)
		end
	else
		g_WinMgr:getWindow("map/1"):setVisible(visible)
		g_WinMgr:getWindow("map/1/"):setVisible(visible)
		g_WinMgr:getWindow("map/1/+"):setVisible(visible)
		g_WinMgr:getWindow("map/1/-"):setVisible(visible)
		g_WinMgr:getWindow("map/2"):setVisible(visible)
		g_WinMgr:getWindow("map/map5"):setVisible(visible)
	end
end
--
function MainUI_setMapTxtPos(visible,x,y)
	MainUI_SetSmallMapVisible(visible)
	g_WinMgr:getWindow("map/map5"):setPosition(CEGUI.UVector2(CEGUI.UDim(0,x),CEGUI.UDim(0,y)))
end
--
function OpenSmallMapUI()
	--UIM_OpenWindow(g_SmallMapUINames.root)
	g_smallMapVisible = true
	MainUI_SetSmallMapVisible(true)
end

function CloseSmallMapUI()
	--UIM_CloseWindow(g_SmallMapUINames.root)
	g_smallMapVisible = false
	MainUI_SetSmallMapVisible(false)
end

function MainUI_SHSmallMap()
	if g_smallMapVisible then
		CloseSmallMapUI()
	else
		OpenSmallMapUI()
		
		UIM_DoOpenAndCloseUI(g_SmallMapUINames.root)
	end
end

function MainUI_hideWinChild(wname, cname)
	local name = wname..cname
	local win = g_WinMgr:getWindow(name)
	
	if win then
		win:setVisible(false)
	end
end

function MainUI_hideMFFrame(fname)
	MainUI_hideWinChild(fname, "__auto_staticimage__")
end

function MainUI_hideMFCloseBtn(fname)
	MainUI_hideWinChild(fname, "__auto_closebutton__")
end

function MainUI_hideMFHelpBtn(fname)
	MainUI_hideWinChild(fname, "__auto_button__")
end

MainUI_hideMFFrame("PlayerProperty")
MainUI_hideMFFrame("OtherPlayerProperty")
MainUI_hideMFFrame("Goto/Windows")
MainUI_hideMFFrame("mail/open_mailbox1")
MainUI_hideMFFrame("Marketroot/Market")
MainUI_hideMFFrame("housedata")
MainUI_hideMFFrame("houseshop")
MainUI_hideMFFrame("liwuhe/biaoqianmianban/liwuhechuangkou")
MainUI_hideMFFrame("liwuka/biaoqianmianban/liwukachuangkou")
MainUI_hideMFFrame("Npcshoproot/Npcshop")

MainUI_hideMFCloseBtn("Shoplistroot/Shoplist")
MainUI_hideMFHelpBtn("Shoplistroot/Shoplist")

function MainUI_skipSliderInput(sname)
	local wslider = g_WinMgr:getWindow(sname)
	local wslthumb = g_WinMgr:getWindow(sname.."__auto_thumb__")
	local pname = "MousePassThroughEnabled"
	local pvalue = "true"
	
	if wslider then
		--wslider:setProperty(pname, pvalue)
		wslthumb:setProperty(pname, pvalue)
	end
end

MainUI_skipSliderInput("PlayerAttribute_Root/Player/Player/slider1")
MainUI_skipSliderInput("PlayerAttribute_Root/Player/Player/slider2")
MainUI_skipSliderInput("PlayerAttribute_Root/Player/Player/slider3")
MainUI_skipSliderInput("PlayerAttribute_Root/Player/Player/slider4")
MainUI_skipSliderInput("PlayerAttribute_Root/Player/Player/slider5")

function Resolution_Change()
	--g_Loger:logDebug("Resolution Changed")
	setNPCPosition(g_WinMgr:getWindow(g_NpcDlgUINames.root))	--	分辨率改变更改npc对话框位置(hezhr)
	BroadCast_SetPostion()	--	分辨率改变更改公告位置(hezhr)
end

function DayNight_Change()
	if EQGlobaFunc.getDayNight() == true then
		g_WinMgr:getWindow("DFwnd_MainRoot/3"):setProperty("Image", "set:Button5 image:NewImage349")
	else
		g_WinMgr:getWindow("DFwnd_MainRoot/3"):setProperty("Image", "set:Button5 image:NewImage351")
	end
end

--显示时间
function MainUI_onShowTime(timer, param1, param2)
	local lbTime = "DFwnd_MainRoot/2/1"
	
	--local year = os.date("%Y")
	--local month = os.date("%m")
	--local day = os.date("%d")
	local hour = os.date("%H")
	local minute = os.date("%M")
	--local second = os.date("%S")
	
	--FSetText(lbTime, hour..":"..minute)
	g_WinMgr:getWindow(lbTime):setText(hour..":"..minute)
end
local showTimeTimer = createTimer(990, 0, 0)
addTimerListener(showTimeTimer, TE_TIMER, MainUI_onShowTime)
startTimer(showTimeTimer)

function HandleOpenUIMsg(args)
	if args == g_openUiType.sysshop then
		OnOpenShopCenter()
	--elseif args == g_openUiType.npcshop then
	--	NpcShop_Open()
	elseif args == g_openUiType.farmdepot then
		if g_GameStateMgr:isFunnyFarm() ~= true then
			IIT_openIITUI()
		end
	elseif args == g_openUiType.depot then
		PopStorageUI()
	elseif args == g_openUiType.bag then
		PopPackUI()
	elseif args == g_openUiType.housetransaction then
		PopHouseSalesUI()
	end
end
-----------------------------------------
-- 室外角色隐藏(2011/6/17 add by hezhr)
-----------------------------------------
function clickHidePlayerBtn(args)
	Player_ShowHide()
end
local m_showHideType = 1	-- (1.全部显示,2.显示名字,3.全部隐藏)
function Player_ShowHide()
	if EQGlobaClass.EQGameSceneMgr.SCENE_COMMON ~= g_sceneMgr:getSceneType() then
		return
	end
	if 3 == m_showHideType then
		m_showHideType = 1
	else
		m_showHideType = m_showHideType + 1
	end
	g_objmgr:setVisibleType(m_showHideType)
end
-----------------------------------------

--鲜花鸡蛋
function updateFlowerEggToday()
	local flower = g_mainplayer:getFlowerToday()
	local egg = g_mainplayer:getEggToday()

	g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/txt1"):setText(flower)
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/txt2"):setText(egg)
end

function reqFlowerLog()
	EQGlobaFunc.reqFlowerLogs()
end

function reqEggLog()
	EQGlobaFunc.reqEggLogs()
end

function showFlowerAnimate()
	--local img = g_WinMgr:getWindow("Root/fnepreview/img")
	--local rtt = g_WinMgr:getWindow("Root/fnepreview/rtt")

	--local flower = g_WinMgr:getWindow("Root/fnepreview/flower")
	--if nil == flower then
	--	local ani = EQGlobaClass.createAnimate()
	--	ani:initAnimate(80, 120, 120)
	--	for i=1, 32 do
	--		if i >= 10 then
	--			image = "flower"..i
	--		else
	--			image = "flower".."0"..i
	--		end
	--		ani:addAnimateFrame("flower", image)
	--	end	
		
	--	flower = g_WinMgr:createWindow("EQHomeLook/AnimateBox", "Root/fnepreview/flower")
	--	EQGlobaFunc.toAnimateBox(flower:getName()):addAnimate(ani)
	--end
	
	--g_MainUIWindows:addChildWindow(g_FnePreview)
	--Common_setModalState(g_FnePreview:getName(), true)
	
	--img:addChildWindow(flower)
	--img:moveToFront()
	
	--closeOtherPlayerInfoUI()
	--EQGlobaFunc.RTTSetActive(g_rttOtherPlayerWinName, true)
	--rtt:addChildWindow(g_rttOtherPlayerWinName)
	
	--EQGlobaFunc.toAnimateBox(flower:getName()):play(false)
	--EQGlobaFunc.toAnimateBox(flower:getName()):setPlayOverHandle("closeAnimate")
end

function closeAnimate()
	g_MainUIWindows:removeChildWindow(g_FnePreview)
	Common_setModalState(g_FnePreview:getName(), false)
	
	EQGlobaFunc.RTTSetActive(g_rttOtherPlayerWinName, false)
	g_WinMgr:getWindow("Root/fnepreview/rtt"):removeChildWindow(g_rttOtherPlayerWinName)
	
	local img = g_WinMgr:getWindow("Root/fnepreview/img")
	img:removeChildWindow(img:getChildAtIdx(0))
end

function showEggAnimate()
	--local img = g_WinMgr:getWindow("Root/fnepreview/img")
	--local rtt = g_WinMgr:getWindow("Root/fnepreview/rtt")
	
	--local egg = g_WinMgr:getWindow("Root/fnepreview/egg")
	--if nil == egg then
	--	local ani = EQGlobaClass.createAnimate()
	--	ani:initAnimate(80, 120, 120)
	--	for i=1, 14 do
	--		if i >= 10 then
	--			image = "egg"..i
	--		else
	--			image = "egg".."0"..i
	--		end
	--		ani:addAnimateFrame("egg", image)
	--	end	
		
	--	egg = g_WinMgr:createWindow("EQHomeLook/AnimateBox", "Root/fnepreview/egg")
	--	EQGlobaFunc.toAnimateBox(egg:getName()):addAnimate(ani)
	--end
	
	--g_MainUIWindows:addChildWindow(g_FnePreview)
	--Common_setModalState(g_FnePreview:getName(), true)
		
	--img:addChildWindow(egg)
	--img:moveToFront()
	
	--closeOtherPlayerInfoUI()
	--EQGlobaFunc.RTTSetActive(g_rttOtherPlayerWinName, true)
	--rtt:addChildWindow(g_rttOtherPlayerWinName)
	
	--EQGlobaFunc.toAnimateBox(egg:getName()):play(false)
	--EQGlobaFunc.toAnimateBox(egg:getName()):setPlayOverHandle("closeAnimate")
end
-----------------------------------------
function MainUI_quickGoHome()
	if DBS_getDropingFlag() or DBS_getPickingFlag() then
		DBS_cantChangeSceneTip()
	else
		g_sceneMgr:reqChangeScene(EQGlobaClass.EQGameSceneMgr.SCENE_HOUSE, g_mainplayer:getPlayerAccount())
	end
end
-----------------------------------------

function MainUI_startLoading()
	local mainwin = g_UIRoot:getChildAtIdx(0)
	mainwin:addChildWindow("Loading")
	
	local loading = g_WinMgr:getWindow("Loading/img")
	local row = g_logictableMgr:getRandomLoadingRow()
	loading:setProperty("Image", "set:" .. row.set .. " image:" .. row.image)
	
	local win = g_WinMgr:getWindow("Loading/tip")
	if win == nil then
		win = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Loading/tip");
		loading:addChildWindow(win);
		win:setArea( CEGUI.URect( CEGUI.UDim(0,0), CEGUI.UDim(1,-60), CEGUI.UDim(1,0), CEGUI.UDim(1,30) ) )
	end
	
	local tip = EQGlobaFunc.toMultiLineBox(win:getName())
	tip:setAlign("AT_CENTER_HOR")
	tip:setSpecSide("#FFFFFF", 1)
	tip:clearUp()
	tip:addElementText("simsun-12", g_logictableMgr:getRandomLoadingTips(), "#000000", true)
end

function MainUI_endLoading()
	if g_sceneMgr:getIsLoading() == true then
		local loading = g_WinMgr:getWindow("Loading")
		loading:getParent():removeChildWindow(loading)
	end
end

-----------------------------------------
function MainUI_showWelcome(text)
	local welcome = g_WinMgr:getWindow("Root/wd")
	welcome:setAlpha(1)
	g_MainUIWindows:addChildWindow(welcome);

	local win = g_WinMgr:getWindow("Root/wd/txt1")
	if win == nil then
		win = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "Root/wd/txt1");
		welcome:addChildWindow(win);
		win:setArea( CEGUI.URect( CEGUI.UDim(0.5, -100), CEGUI.UDim(0.5, -90), CEGUI.UDim(0.5, 100), CEGUI.UDim(0.5, 15) ) )
	end
	
	local textwin = EQGlobaFunc.toMultiLineBox(win:getName())
	textwin:clearUp();
	textwin:addElementText("simsun-10", text, "#000000", true)
	
	g_dynamicUIInfo.type = EQGlobaClass.DynamicUIInfo.EDUI_COMMON
	g_dynamicUIInfo.winname = "Root/wd";
	g_dynamicUIInfo.life = 5000;
	g_dynamicUIInfo.autoRemove = true;
	EQGlobaClass.getDynamicUIMgr():createCommon(g_dynamicUIInfo)
	
	g_uiAnimationInfo.type = EQGlobaClass.UIAnimationInfo.EUIA_FADE
	g_uiAnimationInfo.delayTime = 4000
	g_uiAnimationInfo.playTime = 1000
	g_uiAnimationInfo.startAlpha = 1
	g_uiAnimationInfo.endAlpha = 0
	EQGlobaFunc.playUIAnimation("Root/wd", g_uiAnimationInfo)
end