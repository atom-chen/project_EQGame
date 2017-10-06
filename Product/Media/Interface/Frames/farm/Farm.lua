--农场脚本
g_FarmUIFlag = false
g_nextLv = nil
g_FarmEqMoney = 0
g_FarmMoney = 0
--
g_GameEventMgr:subscribeEvent("UPDATE_UI_FARM_DATA","Farm_onLoadFarmUI")
g_GameEventMgr:subscribeEvent("EXIT_FUNNY_FARM","Farm_onExitFarm")
g_GameEventMgr:subscribeEvent("UPDATE_FARMGOLD", "Farm_updateFarmGold")
g_GameEventMgr:subscribeEvent("UPDATE_FARMEQGOLD", "Farm_updateFarmEqGold")

--
function Farm_updateFarmGold()
--更新农场中的金币显示
	g_FarmMoney = g_FarmMgr:getFarmMoney()
	Farm_setFarmGold(g_FarmMgr:getFarmMoney())
end
-----------------------------------------------------
--
function Farm_setFarmGold(money)
	local moneyWin = g_WinMgr:getWindow("FarmmainUI/PlayerInfo/money")
	if moneyWin ~= nil then
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):clearUp()
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):setSpecSide("#FFFFFF", 1)
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):addElementText("金币：" ..tostring(money), "#000000", false)		
		--农场个人信息中的金币显示
		EQGlobaFunc.SetText(g_farmPlayerInfo.gameGold,tostring(money).."")
	end
end
-----------------------------------------------------
--
function Farm_updateFarmEqGold()
--更新农场中的EQ币显示
	g_FarmEqMoney = g_FarmMgr:getFarmEqMoney()
	Farm_setFarmEqGold(g_FarmMgr:getFarmEqMoney())
end
-----------------------------------------------------
--
function Farm_setFarmEqGold(eqmoney)
	local moneyWin = g_WinMgr:getWindow("FarmmainUI/PlayerInfo/eqmoney")
	if moneyWin ~= nil then
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):clearUp()
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):setSpecSide("#FFFFFF", 1)
		EQGlobaFunc.toMultiLineBox(moneyWin:getName()):addElementText("EQ币：" ..tostring(eqmoney), "#000000", false)		
		--农场个人信息中的eq币显示
		--EQGlobaFunc.SetText(g_farmPlayerInfo.gameGold,tostring(eqmoney).."")
	end
end
-----------------------------------------------------
--
function Farm_onLoadInitial()
	g_WinMgr:getWindow("FarmmainUI"):activate()
	MainUI_ShieldClick(g_FarmRecordUI)
	MainUI_ShieldClick(g_FarmMarketUI)
	MainUI_ShieldClick(g_RemainDogFoodUI)
	MainUI_ShieldClick(g_TUTSRewardUI)
	MainUI_ShieldClick(g_redLandRewardUI)
	MainUI_ShieldClick(g_BuyDogFood_ConfirmUI)
	--MainUI_ShieldClick(g_FarmBoardUI)
	MainUI_ShieldClick(g_FarmStorage)
	MainUI_ShieldClick(g_FarmSystemSetUI)
	g_WinMgr:getWindow("FarmmainUI/msg"):removeEvent("Clicked")
	--g_WinMgr:getWindow("FarmmainUI/FarmSystem/weather"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/pasture"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/farm_stock"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/farm_store"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/decoration"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/levelup"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/move"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/turn_soil"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/watering"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/pick_insect"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/weed"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/harvest"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/all_harvest"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/Farm1"):removeEvent("Clicked")
	g_WinMgr:getWindow("farm_friendlist/left_button"):removeEvent("Clicked")
	g_WinMgr:getWindow("farm_friendlist/right_button"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/seedbag"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):removeEvent("MouseClick")
	g_WinMgr:getWindow("FarmmainUI/Note"):removeEvent("MouseClick")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/put_insect"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/put_weed"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/harvest1"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/farm_system_button"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo1/Note1"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayerImg1"):removeEvent("MouseClick")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/Garden"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/pasture"):removeEvent("Clicked")
	g_WinMgr:getWindow("FarmmainUI/infor"):removeEvent("Clicked")

	g_WinMgr:getWindow("FarmmainUI/msg"):subscribeEvent("Clicked", "Farm_openFarmRecordUI")
	g_WinMgr:getWindow("FarmmainUI/infor"):subscribeEvent("Clicked", "Farm_openFarmNotice")
	--g_WinMgr:getWindow("FarmmainUI/FarmSystem/weather"):subscribeEvent("Clicked", "Farm_weatherChange")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/pasture"):subscribeEvent("Clicked", "Farm_returnSelfPasture")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/farm_stock"):subscribeEvent("Clicked", "Farm_openFarmStorage")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/farm_store"):subscribeEvent("Clicked", "PopFarmShopUI")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/decoration"):subscribeEvent("Clicked", "Dec_openDecUI")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/seedbag"):subscribeEvent("Clicked", "Farm_openSeedUI")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/move"):subscribeEvent("Clicked", "Farm_moveScreenView")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/turn_soil"):subscribeEvent("Clicked", "Farm_turnUpSoil")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/watering"):subscribeEvent("Clicked", "Farm_watering")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/pick_insect"):subscribeEvent("Clicked", "Farm_clearPest")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/weed"):subscribeEvent("Clicked", "Farm_clearWeed")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/harvest"):subscribeEvent("Clicked", "Farm_harvest")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/all_harvest"):subscribeEvent("Clicked", "Farm_harvestAll")

	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/all_harvest1"):subscribeEvent("Clicked", "Farm_harvestAll")
	


	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/put_insect"):subscribeEvent("Clicked", "Farm_putBug")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/put_weed"):subscribeEvent("Clicked", "Farm_putGrass")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/harvest1"):subscribeEvent("Clicked", "Farm_steal")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainmen/move1"):subscribeEvent("Clicked", "Farm_moveScreenView")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/watering1"):subscribeEvent("Clicked", "Farm_watering")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/weed1"):subscribeEvent("Clicked", "Farm_clearWeed")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/pick_insect1"):subscribeEvent("Clicked", "Farm_clearPest")
	g_WinMgr:getWindow("farm_friendlist/right_button"):subscribeEvent("Clicked", "nextPage")
	g_WinMgr:getWindow("farm_friendlist/left_button"):subscribeEvent("Clicked", "prePage")
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayImg"):subscribeEvent("MouseClick", "FPopMainPlayerInfo")
	g_WinMgr:getWindow("FarmmainUI/Note"):subscribeEvent("MouseClick", "FarmPlayerComment")
	g_WinMgr:getWindow("FarmmainUI/farm_system_button"):subscribeEvent("Clicked", "Farm_openFarmSystemSet")
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo1/Note1"):subscribeEvent("Clicked", "FPopOtherPlayerLeaveWord")
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/PlayerImg1"):subscribeEvent("MouseClick", "FPopOtherPlayerInfo")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/Farm1"):subscribeEvent("Clicked", "reqEnterFarm")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/Garden"):subscribeEvent("Clicked", "Farm_EnterGarden")
	g_WinMgr:getWindow("FarmmainUI/FarmSystem/pasture"):subscribeEvent("Clicked", "Farm_EnterPasture")
	g_WinMgr:getWindow("FarmmainUI/FarmEntertainment/levelup"):subscribeEvent("Clicked", "Farm_landLevUp")
	g_WinMgr:getWindow("FarmmainUI/farm_exit_button"):subscribeEvent("Clicked", "Farm_exitFarm")
end

function Farm_openFarmUI()
	--if g_FarmUIFlag == false then
		--g_MainUIWindows:addChildWindow(g_FarmUI)
		--g_FarmUIFlag = true
	--end

	--if g_FarmUIFlag == true then
	--	Farm_OnCloseFarmUI()
	--end

end

function Farm_OnCloseFarmUI()
	--g_MainUIWindows:removeChildWindow(g_FarmUI)
	--g_FarmUIFlag = false
end

--农场记录信息界面
function Farm_openFarmRecordUI()
	if false == g_FPlayerInfoFlag then
		g_FarmUI:addChildWindow(g_FPlayerInfoUI)
		Common_injectEmptyEvt("FarmPlayerInfo")
		g_FPlayerInfoFlag = true
		SetFarmPlayerType(1)
		InitFarmPlayerInfo()
	end
	CEGUI.toTabControl(g_WinMgr:getWindow(g_farmPlayerInfo.tabName)):setSelectedTabAtIndex(0)
	ReqFarmPlayerInfo()
end
--其他玩家农场留言界面
function FPopOtherPlayerLeaveWord()
	if false == g_FPlayerInfoFlag then
		g_FarmUI:addChildWindow(g_FPlayerInfoUI)
		Common_injectEmptyEvt("FarmPlayerInfo")
		g_FPlayerInfoFlag = true
	end
	InitFarmPlayerInfo(g_FarmMgr.mAccount)
	CEGUI.toTabControl(g_WinMgr:getWindow(g_farmPlayerInfo.tabName)):setSelectedTabAtIndex(1)
	SetFarmPlayerType(2)
	ReqFarmPlayerInfo()
end

--系统公告界面
function Farm_openFarmNotice()
	if true == g_FarmUI:isChild(g_BroadCastUI) then
		g_FarmUI:removeChildWindow(g_BroadCastUI)
	else
		BroadCast_addFarmUI()
	end
end
--
function Farm_PopBulletinBoard()

end
--天气界面按钮
function Farm_weatherChange()

end

--返回自己农场的按钮
function Farm_toSelfFarm()
	g_FarmMgr:reqEnterFarm("")
	--FarmFL_reqFL()
end

--返回自己牧场的按钮
function Farm_returnSelfPasture()

end

--农场留言
function FarmPlayerComment()
	if false == g_FPlayerInfoFlag then
		g_FarmUI:addChildWindow(g_FPlayerInfoUI)
		g_FPlayerInfoFlag = true
		Common_injectEmptyEvt("FarmPlayerInfo")
		InitFarmPlayerInfo()
		SetFarmPlayerType(1)
	end
	CEGUI.toTabControl(g_WinMgr:getWindow(g_farmPlayerInfo.tabName)):setSelectedTabAtIndex(1)
	ReqFarmPlayerInfo()
end

--仓库界面
function Farm_openFarmStorage()
	FPopStoragePanel()
end

--添加农场商品
function Farm_addItemToStore()

end

--系统设置
function Farm_openFarmSystemSet()
	FReqSetting()
end

--仓库种子页的系统快速通道按钮
function Farm_openSeedUI()
	PopSeedBag()
end

--移动农场画面的按钮
function Farm_moveScreenView()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_MOVE)
	--- test
	-- FarmTask_setCompleteFarmTaskUI(1003)
	-- FarmTask_setGetFarmTaskUI(1008)
	--
end

--翻地按钮
function Farm_turnUpSoil()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_TILL)
end

--浇水按钮
function Farm_watering()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_WATER)
end

--除虫按钮
function Farm_clearPest()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_PICKBUG)
end

--放虫按钮
function Farm_putBug()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_PUTBUG)
end
--放草按钮
function Farm_putGrass()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_PUTGRASS)
end

--偷菜按钮
function Farm_steal()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_HARVEST)
end

--除草按钮
function Farm_clearWeed()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_WEED)
end

--收获按钮
function Farm_harvest()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_HARVEST)
end

--一键收获按钮
function Farm_harvestAll()
	g_FarmMgr:setPushFlag(1)
	EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_ONEKEYHARVEST)
end

--请求进入农场(进入自己的农场)
enterFarmFlag = 0
function reqEnterFarm()
	--g_Loger:logDebug("req Enter Self Farm...")
	CloseAllFarmUI()
	g_FarmMgr:setPushFlag(0)
	g_FarmMgr:reqEnterFarm("")
	--FarmFL_reqFL() 			--请求农场好友列表
	enterFarmFlag = 1
end
--请求退出农场
function Farm_reqExitFarm()
	Farm_closeFarmUI()
	Farm_OnCloseFarmUI()
	g_GameEventMgr:fireEvent("EXIT_FUNNY_FARM")
end
--
function Farm_onExitFarm()
	farmFriendInfo_farmStatus_Map ={}
	FarmFL_resetFLpos()
	g_sceneMgr:reqLeaveFarm()
	enterFarmFlag = 0
end

--加载农场界面
	--要区分自己的农场和其他人的农场界面上的区别
function Farm_onLoadFarmUI()
	g_Loger:logDebug("Farm_onLoadFarmUI...")
	EQGlobaFunc.loadFarmScence()
	Farm_openFarmUI()
	Farm_onLoadInitial()
	local playerAcc = g_mainplayer:getPlayerAccount()
	if g_objmgr:isMainPlayer(g_FarmMgr.mAccount) then
		g_Loger:logDebug("have Entered Self Farm...")
		g_WinMgr:getWindow("FarmmainUI/FarmEntertainment"):setVisible(true)
		g_WinMgr:getWindow("FarmmainUI/infor"):setVisible(true)
		g_WinMgr:getWindow("FarmmainUI/Note"):setVisible(true)
		g_WinMgr:getWindow("FarmmainUI/msg"):setVisible(true)
		g_WinMgr:getWindow("FarmmainUI/FarmEntertainment1"):setVisible(false)
		g_WinMgr:getWindow("FarmmainUI/PlayerInfo1"):setVisible(false)
		Common_injectEmptyEvt("FarmmainUI/FarmEntertainment")
		
		Farm_showSelfFarmInfo()
	else
		g_Loger:logDebug("have Entered Friend Farm...")
		--EQGlobaFunc.setFarmCurState(farmCurStaEnum.CS_FARM_MOVE)
		g_WinMgr:getWindow("FarmmainUI/FarmEntertainment1"):setVisible(true)
		g_WinMgr:getWindow("FarmmainUI/PlayerInfo1"):setVisible(true)
		g_WinMgr:getWindow("FarmmainUI/FarmEntertainment"):setVisible(false)
		g_WinMgr:getWindow("FarmmainUI/infor"):setVisible(false)
		g_WinMgr:getWindow("FarmmainUI/Note"):setVisible(false)
		g_WinMgr:getWindow("FarmmainUI/msg"):setVisible(false)
		Common_injectEmptyEvt("FarmmainUI/FarmEntertainment1")
		--清空原来的文字
		getFriendInfoCtrl()
		--
		Farm_showPlayerInfo()
	end
	--g_WinMgr:getWindow("DFwnd_MainRoot/tip/tip1"):setVisible(false)
	Common_injectEmptyEvt("FarmmainUI/FarmSystem")
	Common_injectEmptyEvt("FarmmainUI/PlayerInfo")
	Common_injectEmptyEvt("FarmmainUI/PlayerInfo1")
	--
	--FarmFL_reqFL() 
	FarmFL_openFL()	--打开农场好友列表

end
--显示玩家的农场信息
--
local motherName = ""
------------------------------------------------------
--
function Farm_clearOtherName()
	motherName = ""
end
------------------------------------------------------
--
function Farm_showPlayerInfo()
	local acc = g_FarmMgr.mAccount
	local name = ""
	if motherName == "" then
		name = acc
	else
		name = motherName
	end
	local level = g_FarmMgr.mLev
	local exp = g_FarmMgr.mExp
	local farmLevelRow = g_FarmTableMgr:getFarmLevelRow(level)
	local nextExp = farmLevelRow.next_exp
	local nameWin = g_WinMgr:getWindow("FarmmainUI/PlayerInfo1/name1")
	if nameWin ~= nil then
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):clearUp()
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
		local len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", tostring(name))
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):setWidth(len)
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText(tostring(name), "#000000", false)	
	end
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/LevelImage/Level1"):setText(level)
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp/number1"):setText(exp .."/" ..nextExp)
	CEGUI.toProgressBar(g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp1")):setProgress (exp/nextExp)
	Farm_setFriendPhoto("FarmmainUI/PlayerInfo/PlayerImg1", g_farmSelectAccount)	-- (2011/8/2 add by hezhr)
end

--显示本人农场信息
local farmOwner  = nil
function Farm_showSelfFarmInfo()
	selfInfo = g_FarmMgr:getSelfFarmInfo()
	farmOwner = selfInfo.mSusername
	local level = selfInfo.mSlev
	local gold = g_FarmMgr:getFarmMoney()--g_FarmMgr.mGameGold
	local exp = selfInfo.mSexp
	local farmLevelRow = g_FarmTableMgr:getFarmLevelRow(level)
	local nextExp = farmLevelRow.next_exp
	g_exp = exp
	g_nextLv = nextExp
	--local nameWin, moneyWin, titleWin = getPlayerInfoCtrl()
	--EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
	--local len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", tostring(farmOwner))
	--EQGlobaFunc.toMultiLineBox(nameWin:getName()):setWidth(len)
	--EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText(tostring(farmOwner), "#000000", false)
	--EQGlobaFunc.toMultiLineBox(moneyWin:getName()):setSpecSide("#FFFFFF", 1)
	--EQGlobaFunc.toMultiLineBox(moneyWin:getName()):addElementText("金币：" ..tostring(gold), "#000000", false)	
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/LevelImage/Level"):setText(level)
	g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp/number"):setText(exp .."/" ..nextExp)
	CEGUI.toProgressBar(g_WinMgr:getWindow("FarmmainUI/PlayerInfo/Exp")):setProgress (exp/nextExp)
end
-------------------------------------------------------
--退出后关闭农场内的子UI
function Farm_closeFarmUI()
	if true== g_FStorageFlag then
		FRemoveStoragePanel()
	end
	if true == g_FShopFlag then
		CloseFarmShopUI()
	end
	if true == g_FSettingFlag then
		FSettingClose()
	end
	if true == g_FarmDecoFlag then
		Dec_closeDecUI()
	end
	if true == g_FPlayerInfoFlag then
		FClosePlayerInfoUI()
	end
	if true == g_seedBagFlag then
		CloseSeedBag()
	end
end

-- 获取农场玩家信息相关控件：name，money，title控件(hezhr)
function getPlayerInfoCtrl()
	local playerInfo = g_WinMgr:getWindow("FarmmainUI/PlayerInfo")
	local name = g_WinMgr:getWindow("FarmmainUI/PlayerInfo/name")
	local money = g_WinMgr:getWindow("FarmmainUI/PlayerInfo/money")
	local eqmoney = g_WinMgr:getWindow("FarmmainUI/PlayerInfo/eqmoney")
	local title = g_WinMgr:getWindow("FarmmainUI/PlayerInfo/title")

	if nil == name then
		name = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "FarmmainUI/PlayerInfo/name")
		EQGlobaFunc.toMultiLineBox(name:getName()):setSize(100, 15)
		EQGlobaFunc.toMultiLineBox(name:getName()):setPosition(52, 1)
		EQGlobaFunc.toMultiLineBox(name:getName()):setFont("simsun-10")
		playerInfo:addChildWindow(name)
	end
	if nil == money then
		money = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "FarmmainUI/PlayerInfo/money")
		EQGlobaFunc.toMultiLineBox(money:getName()):setSize(116, 15)
		EQGlobaFunc.toMultiLineBox(money:getName()):setPosition(64, 32)
		EQGlobaFunc.toMultiLineBox(money:getName()):setFont("simsun-10")
		playerInfo:addChildWindow(money)
	end
	if nil == eqmoney then
		eqmoney = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "FarmmainUI/PlayerInfo/eqmoney")
		EQGlobaFunc.toMultiLineBox(eqmoney:getName()):setSize(116, 15)
		EQGlobaFunc.toMultiLineBox(eqmoney:getName()):setPosition(180, 32)--(64, 48)
		EQGlobaFunc.toMultiLineBox(eqmoney:getName()):setFont("simsun-10")
		playerInfo:addChildWindow(eqmoney)
	end
	if nil == title then
		title = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "FarmmainUI/PlayerInfo/title")
		EQGlobaFunc.toMultiLineBox(title:getName()):setSize(98, 15)
		EQGlobaFunc.toMultiLineBox(title:getName()):setPosition(151, 14)
		EQGlobaFunc.toMultiLineBox(title:getName()):setFont("simsun-10")
		playerInfo:addChildWindow(title)
	end
	EQGlobaFunc.toMultiLineBox(name:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(money:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(eqmoney:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(title:getName()):clearUp()
	return name, money, title,eqmoney
end
-- 获取农场玩家好友信息相关控件：name1，title1控件(hezhr)
function getFriendInfoCtrl()
	local playerInfo1 = g_WinMgr:getWindow("FarmmainUI/PlayerInfo1")
	local name1 = g_WinMgr:getWindow("FarmmainUI/PlayerInfo1/name1")
	local title1 = g_WinMgr:getWindow("FarmmainUI/PlayerInfo1/title1")

	if nil == name1 then
		name1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "FarmmainUI/PlayerInfo1/name1")
		EQGlobaFunc.toMultiLineBox(name1:getName()):setSize(100, 15)
		EQGlobaFunc.toMultiLineBox(name1:getName()):setPosition(52, 1)
		EQGlobaFunc.toMultiLineBox(name1:getName()):setFont("simsun-10")
		playerInfo1:addChildWindow(name1)
	end
	if nil == title1 then
		title1 = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "FarmmainUI/PlayerInfo1/title1")
		EQGlobaFunc.toMultiLineBox(title1:getName()):setSize(98, 15)
		EQGlobaFunc.toMultiLineBox(title1:getName()):setPosition(151, 14)
		EQGlobaFunc.toMultiLineBox(title1:getName()):setFont("simsun-10")
		playerInfo1:addChildWindow(title1)
	end
	EQGlobaFunc.toMultiLineBox(name1:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(title1:getName()):clearUp()
	return name1, title1
end
--
function Farm_EnterGarden()
	if g_WinMgr:isWindowPresent("FarmmainUI/FarmSystem/Garden/useless") then
		Farm_unEnabledMsg()
	else
		Farm_reqEnterGarden()
	end
end
--
function Farm_reqEnterGarden()

end
--
function Farm_EnterPasture()
	if g_WinMgr:isWindowPresent("FarmmainUI/FarmSystem/pasture/useless2") then
		Farm_unEnabledMsg()
	else
		Farm_reqEnterPasture()
	end
end
--
function Farm_reqEnterPasture()

end
--
function Farm_landLevUp()
	--RedLand.Lua
	ReqCanUpgradeLand()
end
--
function Farm_reqLandLevUp()

end
--
function Farm_unEnabledMsg()
	MsgBox_createMsgBox(g_FarmUI
						,000
						,"消息提示"
						,"该功能还没有正式推出，敬请期待！"
						,"MsgBox_cancelFunc"
						,"")
end
--

--关闭农场里所有界面
function CloseAllFarmUI()
	CloseSeedBag()
	FClosePlayerInfoUI()
	CloseFarmShopUI()
	Dec_closeDecUI()
	FSettingClose()
	FRemoveStoragePanel()
	CloseUpgradeLandUI()
end
-----------------------------------------------------------
--
local GMBox = g_WinMgr:getWindow("FarmmainUI/tempEdit")
GMBox:setVisible(false)
GMBox:subscribeEvent("KeyDown","Farm_addGMcommand")
g_WinMgr:getWindow("FarmmainUI"):subscribeEvent("KeyDown","Farm_UIRootKeyDown")
--
function Farm_addGMcommand(args)
	local keyvalue = CEGUI.toKeyEventArgs(args).scancode 
	if keyvalue == 28 or keyvalue == 156 then   -- 回车键
		Farm_addChat()
	end
end
--
function Farm_addChat()
	local bool
	local text =  GMBox:getText()	
	if text == "" then
		if GMBox:isVisible() then
			GMBox:setVisible(false)
		else
			GMBox:setVisible(true)
			GMBox:activate()
		end
		return
	end
	ChatSet_Push(text)
	bool = ChatSet_LoadScript(text)	
	if bool == 1 then
		--ChatBox:setText("Command Succeed!")
	end
	GMBox:setText("")
end
--
function Farm_UIRootKeyDown(args)
	local keyvalue = CEGUI.toKeyEventArgs(args).scancode 
	if keyvalue == 28 or keyvalue == 156 then   -- 回车键
		Farm_setEditboxShow()
	end	
end
--
function Farm_setEditboxShow()
	local text =  GMBox:getText()	
	if text == "" then
		if GMBox:isVisible() then
			GMBox:setVisible(false)
		else
			GMBox:setVisible(true)
			GMBox:activate()
		end
	end
end
-----------------------------------------------------------
--更新玩家信息
function UpdatePlayerEqInfo(account, info)
	--为农场个人信息面板设置相关信息
	setDataForPlayerInfoUI(account, info)
	--设置名字
	local nameWin, moneyWin, _,eqmoneyWin = getPlayerInfoCtrl()
	--
	Farm_setFarmEqGold(g_FarmEqMoney)
	Farm_setFarmGold(g_FarmMoney)

	--
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
	--
	local len
	if "" ~= info.nick_name then
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText( info.nick_name, "#000000", false)
		len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", info.nick_name)
	elseif "" ~= info.full_name then
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText( info.full_name, "#000000", false)
		len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", info.full_name)
	else
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText( account, "#000000", false)
		len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", account)
	end
	
	EQGlobaFunc.toMultiLineBox(nameWin:getName()):setWidth(len)
	--保存并设置玩家头像(add by hezhr)
	if "image/gif" ~= info.photo_type then
		Farm_insertFriendPhoto(account, info.photo_data)
		if Farm_needUpdateFriendPhoto(account) then
			Farm_setNeeddUpdateFriendPhoto(account, false)
			Farm_setFriendPhoto("FarmmainUI/PlayerInfo/PlayImg", account)
			local index = Farm_findAccountIndex(account)
			if 0 ~= index then
				Farm_setFriendPhoto("farm_friendlist/F"..index.."/icon"..index, account)
			end
		end
	end
	--打开的是主玩家信息面板(add by hezhr)
	if true == g_farmMainPlayerInfoUI then
		Farm_setFriendPhoto("FarmPlayerInfo/bg/photo", account)
	end
	--
end
--------------------------------------------------------------------------
--
function UpdateOtherPlayerEqInfo(account, info)
	--设置名字
	motherName = ""
	if account == g_farmSelectAccount then	--(2011/8/2 modify by hezhr)
		setDataForOtherPlayerInfoUI(account, info)
		local nameWin = getFriendInfoCtrl()	
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):setSpecSide("#FFFFFF", 1)
		local len
		if "" ~= info.nick_name then
			motherName = info.nick_name
			EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText( info.nick_name, "#000000", false)
			len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", info.nick_name)
		elseif "" ~= info.full_name then
			motherName = info.full_name
			EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText( info.full_name, "#000000", false)
			len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", info.full_name)
		else
			motherName = account
			EQGlobaFunc.toMultiLineBox(nameWin:getName()):addElementText( account, "#000000", false)
			len = EQGlobaFunc.toMultiLineBox(nameWin:getName()):getFitWidth("simsun-10", account)
		end
		EQGlobaFunc.toMultiLineBox(nameWin:getName()):setWidth(len)
	end
	--设置好友头像(add by hezhr)
	if "image/gif" ~= info.photo_type then
		if Farm_needUpdateFriendPhoto(account) then
			Farm_setNeeddUpdateFriendPhoto(account, false)
			Farm_insertFriendPhoto(account, info.photo_data)
			local index = Farm_findAccountIndex(account)
			if 0 ~= index then
				Farm_setFriendPhoto("farm_friendlist/F"..index.."/icon"..index, account)
			end
		end
	end
	--当前选中的不是主玩家账号(add by hezhr)
	if false == g_objmgr:isMainPlayer(g_farmSelectAccount) then
		Farm_setFriendPhoto("FarmmainUI/PlayerInfo/PlayerImg1", g_farmSelectAccount)
	end
	--打开的不是主玩家信息面板(add by hezhr)
	if false == g_farmMainPlayerInfoUI then
		Farm_setFriendPhoto("FarmPlayerInfo/bg/photo", g_farmSelectAccount)
	end
end
---------------------------------------------------------------
--
function Farm_exitFarm(args)
	EQGlobaFunc.ExitClient()
end
