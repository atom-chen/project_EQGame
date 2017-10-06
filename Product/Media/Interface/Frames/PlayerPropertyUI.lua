-----------------------------------------
--  文件说明：处理人物属性界面相关的逻辑
-----------------------------------------
--加载人物属性界面
--
g_playerPropertyBtnClicked = false
local mPlayerType = {
					mainPlayer  = 1,
					otherPlayer = 2,
					}

g_playerInfoName = {
					[mPlayerType.mainPlayer] = {
							[""] = "",
							["closeBtn"] = "PlayerProperty__auto_closebutton__",
							["name"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/5",
							["level"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/level",
							["clean"] = "PlayerProperty/PropertyTabCtrl/Property/QJDPro",
							["healthy"] = "PlayerProperty/PropertyTabCtrl/Property/JKZ",
							["charm"] = "PlayerProperty/PropertyTabCtrl/Property/MLZ",
							["furClean"] = "PlayerProperty/PropertyTabCtrl/Property/FWQJD",
							["active"] = "PlayerProperty/PropertyTabCtrl/Property/hyz",
							["cleanValue"] = "PlayerProperty/PropertyTabCtrl/Property/shuzhi1",
							["healthyValue"] = "PlayerProperty/PropertyTabCtrl/Property/shuzhi2",
							["charmValue"] = "PlayerProperty/PropertyTabCtrl/Property/shuzhi3",
							["furCleanValue"] = "PlayerProperty/PropertyTabCtrl/Property/shuzhi4",
							["activeValue"] = "PlayerProperty/PropertyTabCtrl/Property/shuzhi5",
							["diseaseTxt"] = "PlayerProperty/PropertyTabCtrl/Property/zhuangtaiTXT",
							["diseaseWindow"] = "PlayerProperty/PropertyTabCtrl/Property/zhuangtaiT",
							["eqName"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/5",
							["eqNickName"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/51",
							["eqBirthday"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/52",
							["eqTel"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/53",
							["eqSignature"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/55",
							["eqRemark"] = "PlayerProperty/PropertyTabCtrl/BasicInfo/551",
							["flower"] = "PlayerProperty/PropertyTabCtrl/Property/txt1",
							["decoration"] = "PlayerProperty/PropertyTabCtrl/Property/txt2",
					},
					[mPlayerType.otherPlayer] = {
							["closeBtn"] = "OtherPlayerProperty__auto_closebutton__",
							["name"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/5",
							["level"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/level",
							["clean"] = "OtherPlayerProperty/PropertyTabCtrl/Property/QJDPro",
							["healthy"] = "OtherPlayerProperty/PropertyTabCtrl/Property/JKZ",
							["charm"] = "OtherPlayerProperty/PropertyTabCtrl/Property/MLZ",
							["furClean"] = "OtherPlayerProperty/PropertyTabCtrl/Property/FWQJD",
							["active"] = "OtherPlayerProperty/PropertyTabCtrl/Property/hyz",
							["cleanValue"] = "OtherPlayerProperty/PropertyTabCtrl/Property/shuzhi1",
							["healthyValue"] = "OtherPlayerProperty/PropertyTabCtrl/Property/shuzhi2",
							["charmValue"] = "OtherPlayerProperty/PropertyTabCtrl/Property/shuzhi3",
							["furCleanValue"] = "OtherPlayerProperty/PropertyTabCtrl/Property/shuzhi4",
							["activeValue"] = "OtherPlayerProperty/PropertyTabCtrl/Property/shuzhi5",
							["diseaseTxt"] = "OtherPlayerProperty/PropertyTabCtrl/Property/zhuangtaiTXT",
							["diseaseWindow"] = "OtherPlayerProperty/PropertyTabCtrl/Property/zhuangtaiT",
							["eqSlot"] = "OtherPlayerProperty/PropertyTabCtrl/Property/Slot",
							["eqName"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/5",
							["eqNickName"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/51",
							["eqBirthday"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/52",
							["eqTel"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/53",
							["eqSignature"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/55",
							["eqRemark"] = "OtherPlayerProperty/PropertyTabCtrl/BasicInfo/551",
					}
					}


-----------------------------------------
--预览相关
local preview_checkbox = CEGUI.toCheckbox(g_WinMgr:getWindow("OtherPlayerProperty/PropertyTabCtrl/Property/Checkbox"))
local preview_pos = CEGUI.UVector2(CEGUI.UDim(0,0), CEGUI.UDim(0,74))
local preview_itemid = 0

-----------------------------------------
local function skipSliderInput(sname)
	local wslider = g_WinMgr:getWindow(sname)
	local wslthumb = g_WinMgr:getWindow(sname.."__auto_thumb__")
	local pname = "MousePassThroughEnabled"
	local pvalue = "true"

	if wslider then
		--wslider:setProperty(pname, pvalue)
		wslthumb:setProperty(pname, pvalue)
	end
end

local function _InitSlider()

	skipSliderInput(g_playerInfoName[mPlayerType.mainPlayer].clean)
	skipSliderInput(g_playerInfoName[mPlayerType.mainPlayer].healthy)
	skipSliderInput(g_playerInfoName[mPlayerType.mainPlayer].charm)
	skipSliderInput(g_playerInfoName[mPlayerType.mainPlayer].active)

	skipSliderInput(g_playerInfoName[mPlayerType.otherPlayer].clean)
	skipSliderInput(g_playerInfoName[mPlayerType.otherPlayer].healthy)
	skipSliderInput(g_playerInfoName[mPlayerType.otherPlayer].charm)
	skipSliderInput(g_playerInfoName[mPlayerType.otherPlayer].active)

	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.mainPlayer].clean)):setMaxValue(g_max_pro_player_clean)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.mainPlayer].healthy)):setMaxValue(g_max_pro_player_health)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.mainPlayer].charm)):setMaxValue(g_max_pro_player_charm)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.mainPlayer].active)):setMaxValue(g_max_pro_player_active)


	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].clean)):setMaxValue(g_max_pro_player_clean)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].healthy)):setMaxValue(g_max_pro_player_health)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].charm)):setMaxValue(g_max_pro_player_charm)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].active)):setMaxValue(g_max_pro_player_active)

	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].clean)):setMaxValue(g_max_pro_player_clean)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].healthy)):setMaxValue(g_max_pro_player_health)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].charm)):setMaxValue(g_max_pro_player_charm)
	CEGUI.toSlider(g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].active)):setMaxValue(g_max_pro_player_active)

	CEGUI.toSlider(g_WinMgr:getWindow("PlayerAttribute_Root/Player/Player/slider1")):setMaxValue(g_max_pro_player_clean)
	CEGUI.toSlider(g_WinMgr:getWindow("PlayerAttribute_Root/Player/Player/slider2")):setMaxValue(g_max_pro_player_health)
	CEGUI.toSlider(g_WinMgr:getWindow("PlayerAttribute_Root/Player/Player/slider3")):setMaxValue(g_max_pro_player_charm)
	CEGUI.toSlider(g_WinMgr:getWindow("PlayerAttribute_Root/Player/Player/slider5")):setMaxValue(g_max_pro_player_active)
end
-----------------------------------------
function PlayerPropertyUIOnLoad()
	_InitSlider()

	g_WinMgr:getWindow("PlayerProperty__auto_titlebar__"):setWantsMultiClickEvents(false)

	g_GameEventMgr:subscribeEvent("ON_CLICKED_RECV_PROPERTY","PlayerPropertyUI_OnOpenPlayerProperty")
	g_PlayerAttrUI:subscribeEvent("MouseClick", "GiftSys_setWinActive")
	
	preview_checkbox:subscribeEvent("CheckStateChanged", "OtherPlayerProperty_ClickPreview")

	--屏蔽界面鼠标穿透
	MainUI_ShieldClick(g_PlayerProUI)
	Common_injectEmptyEvt("PlayerProperty")
	MainUI_ShieldClick(g_OtherPlayerProUI)
end
-----------------------------------------
--  开启关闭界面处理
-----------------------------------------
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_PlayerProperty"):subscribeEvent("Clicked", "ReqPlayerBasicData")
g_WinMgr:getWindow("PlayerProperty__auto_closebutton__"):subscribeEvent("Clicked", "PlayerPropertyUI_OnClosePlayerProperty")
g_WinMgr:getWindow(g_playerInfoName[2].closeBtn):subscribeEvent("Clicked", "closeOtherPlayerInfoUI")

g_PlayerPropertyFlag = false
--初始化其他玩家RTT
g_smallModules:initOtherPlayerRTT()

----------------------------------------------------------------------------------
function ReqPlayerBasicData()
	g_playerPropertyBtnClicked = true
	if false == g_PlayerPropertyFlag then
		ReqOpenPlayerPropertyUI()
	else
		PlayerPropertyUI_OnClosePlayerProperty()
	end
end

----------------------------------------------------------------------------------
function PlayerPropertyUI_SetPlayerAttr(player)
	local property = player:getProperty()
	
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/sp_txt"):setText(tostring(property.active_value))
	Scale("PlayerAttribute_Root/Player/Player/slider1", property.player_clean)
	Scale("PlayerAttribute_Root/Player/Player/slider2", property.player_health)
	Scale("PlayerAttribute_Root/Player/Player/slider3", property.player_charm)
	Scale("PlayerAttribute_Root/Player/Player/slider5", property.active_value)
end
----------------------------------------------------------------------------------
--local noinitRtt = 0
function PlayerPropertyUI_OnOpenPlayerProperty()
	PlayerPropertyUI_SetPlayerAttr(g_mainplayer)
	if g_reqPlayDataByPlayerAttr then
		g_reqPlayDataByPlayerAttr = false
		return
	end
	if not g_playerPropertyBtnClicked then
		return
	end
	--if noinitRtt == 0 then
		EQGlobaFunc.EQStartRTT(0,50000,0)
		EQGlobaFunc.RTTReset()
		local curSex = g_rtt:getAvatar():getAdjuster():getCurrentVal(g_atSex)
		if curSex == 1 then
			EQGlobaFunc.RTTScale(0.96,0.96,0.96)--(1,1,1)--------(0.9,0.9,0.9)
		else
			EQGlobaFunc.RTTScale(1.05,1.05,1.05)--(0.9,0.9,0.9)
		end
		EQGlobaFunc.RTTLookAt(0,50082,0)--(0,50075,0)
		--noinitRtt = 1
	--end

	--同步身上装备
	EQGlobaFunc.RTTSync()
	local sX = g_WinMgr:getWindow("PlayerProperty/PropertyTabCtrl/Property/PlayerShow"):getWidth():asAbsolute(640)
	local sY = g_WinMgr:getWindow("PlayerProperty/PropertyTabCtrl/Property/PlayerShow"):getHeight():asAbsolute(480)
	EQGlobaFunc.RTTSetAspectRatio(sX,sY)--(98,232)

	--PlayerPloop()
	PlayerPropertyUI_startRTT()                 --人物窗口渲染
	OpenPlayerPropertyUI()
	g_PlayerProUI:activate()
	--人物属性值
	ShowPlayerInfo(g_mainplayer, mPlayerType.mainPlayer)
	--人物基本信息
	SetPlayerBaseInfo(g_mainplayer, mPlayerType.mainPlayer)

end

function PlayerPropertyUI_OnClosePlayerProperty()
	ClosePlayerPropertyUI()
	EQGlobaFunc.RTTSetActive(g_rttAvatarWinName, false)
end

function ReqOpenPlayerPropertyUI()
	EQGlobaFunc.reqPlayerBasicData()
end

function OpenPlayerPropertyUI()
	UIM_DoOpenAndCloseUI(g_PlayerPropertyUINames.root)
	UIM_OpenWindow(g_PlayerPropertyUINames.root)
	if not g_StorageUIFlag then
		SetWindowCenter(g_WinMgr:getWindow(g_PlayerPropertyUINames.root))
	end
	g_PlayerPropertyFlag = true
end

function ClosePlayerPropertyUI()
	UIM_CloseWindow(g_PlayerPropertyUINames.root)
	g_PlayerPropertyFlag = false
	g_playerPropertyBtnClicked = false
end

function PlayerPropertyUI_startRTT()
	local win = g_WinMgr:getWindow(g_rttAvatarWinName);
	win:setMousePassThroughEnabled(true)
	--print(win);
	local window = g_WinMgr:getWindow("PlayerProperty/PropertyTabCtrl/Property/PlayerShow");
	window:addChildWindow(win);

	EQGlobaFunc.RTTSetActive(g_rttAvatarWinName, true)
end

function OtherPlayerStartRTT()
	local win = g_WinMgr:getWindow(g_rttOtherPlayerWinName);
	local window = g_WinMgr:getWindow("OtherPlayerProperty/PropertyTabCtrl/Property/PlayerShow")
	window:addChildWindow(win);
	
	EQGlobaFunc.RTTSetActive(g_rttOtherPlayerWinName, true)
end

-----------------------------------------
--  人物旋转处理
-----------------------------------------
local Btn_TurnRight = g_WinMgr:getWindow("PlayerProperty/PropertyTabCtrl/Property/Btn_TurnRight")
local Btn_TurnLeft = g_WinMgr:getWindow("PlayerProperty/PropertyTabCtrl/Property/Btn_TurnLeft")

--连续旋转
Btn_TurnRight:setMouseAutoRepeatEnabled(true)
Btn_TurnRight:setAutoRepeatRate(0.1)

Btn_TurnLeft:setMouseAutoRepeatEnabled(true)
Btn_TurnLeft:setAutoRepeatRate(0.1)

--屏蔽双击按键
Btn_TurnRight:setWantsMultiClickEvents(false)
Btn_TurnLeft:setWantsMultiClickEvents(false)

Btn_TurnRight:subscribeEvent("MouseButtonDown", "PlayerPropertyUI_TurnCounterclockwise")
Btn_TurnLeft:subscribeEvent("MouseButtonDown", "PlayerPropertyUI_TurnClockwise")

function PlayerPropertyUI_TurnCounterclockwise(args)
	EQGlobaFunc.RTTRotate(18,2)
--	print ("TurnCounterclockwise")
end

function PlayerPropertyUI_TurnClockwise(args)
--	print ("TurnClockwise")
	EQGlobaFunc.RTTRotate(-18,2)
end

--其他玩家RTT旋转
local Other_TurnRight = g_WinMgr:getWindow("OtherPlayerProperty/PropertyTabCtrl/Property/Btn_TurnRight")
local Other_TurnLeft = g_WinMgr:getWindow("OtherPlayerProperty/PropertyTabCtrl/Property/Btn_TurnLeft")

--连续旋转
Other_TurnRight:setMouseAutoRepeatEnabled(true)
Other_TurnRight:setAutoRepeatRate(0.1)

Other_TurnLeft:setMouseAutoRepeatEnabled(true)
Other_TurnLeft:setAutoRepeatRate(0.1)

--屏蔽双击按键
Other_TurnRight:setWantsMultiClickEvents(false)
Other_TurnLeft:setWantsMultiClickEvents(false)

Other_TurnRight:subscribeEvent("MouseButtonDown", "OtherPlayerRTTRatateR")
Other_TurnLeft:subscribeEvent("MouseButtonDown", "OtherPlayerRTTRatateL")

function OtherPlayerRTTRatateR()
	g_smallModules:otherPlayerRttRotate(18, 2)
end

function OtherPlayerRTTRatateL()
	g_smallModules:otherPlayerRttRotate(-18, 2)
end

-----------------------------------------
--  人物属性处理  xiaoMing
-----------------------------------------
-----------------------------------------
--基本资料
function SetPlayerBaseInfo(player, type)
	local value = player:getLevel()
	--g_Loger:logError(tostring(value))
	SetBaseInfoValue(g_playerInfoName[type].level,  value )
end
-----------------------------------------
function SetBaseInfoValue(caseName, value)
	FSetText(caseName, tostring(value))
end
-----------------------------------------
--设置进度条长度
function Scale(winName, curValue)
	local window = g_WinMgr:getWindow(winName)
	CEGUI.toSlider(window):setCurrentValue(curValue)
end
-----------------------------------------
--查看其他玩家信息
g_otherPlayerInfoFlag = false

function PopOtherPlayerInfo()
	PlayerPropertyUI_clearOldEqInfo()
	if false == g_otherPlayerInfoFlag then
		g_MainUIWindows:addChildWindow(g_OtherPlayerProUI)
		g_otherPlayerInfoFlag = true
		--g_OtherPlayerProUI:setProperty("AlwaysOnTop", "true")
		preview_checkbox:setSelected(false)
	end
end
-----------------------------------------
function closeOtherPlayerInfoUI()
	g_OtherPlayerProUI:setProperty("AlwaysOnTop", "false")
	g_MainUIWindows:removeChildWindow(g_OtherPlayerProUI)
	g_otherPlayerInfoFlag = false
	EQGlobaFunc.RTTSetActive(g_rttOtherPlayerWinName, false)
	
	preview_checkbox:setSelected(false)
end
-----------------------------------------
function ShowOtherPlayerInfo()
	g_smallModules:reqOtherPlayer(1)
	g_MainUIWindows:removeChildWindow(g_circleMenuUI)
end
-----------------------------------------
--查看玩家信息 1代表主玩家 2代表其他玩家
function ShowPlayerInfo(player,  type)

	if mPlayerType.otherPlayer == type then
		PopOtherPlayerInfo()
		OtherPlayerStartRTT()
		SetOtherPlayerEquipment(player)
	else
		g_WinMgr:getWindow(g_playerInfoName[type].flower):setText(g_mainplayer:getFlowerCount())
		g_WinMgr:getWindow(g_playerInfoName[type].decoration):setText(g_mainplayer:getDecoration())
	end

	local property = player:getProperty()
	Scale(g_playerInfoName[type].clean, property.player_clean)
	Scale(g_playerInfoName[type].healthy, property.player_health)
	Scale(g_playerInfoName[type].charm, property.player_charm)
	Scale(g_playerInfoName[type].active, property.active_value)

	UpdateDiseaseState(player:getHealtyState(), type)

	--基本资料中的名字由此设置
	SetPlayerBaseInfo(player, type)

	g_WinMgr:getWindow(g_playerInfoName[type].cleanValue):setText(property.player_clean.."/"..g_max_pro_player_clean)
	g_WinMgr:getWindow(g_playerInfoName[type].healthyValue):setText(property.player_health.."/"..g_max_pro_player_health)
	g_WinMgr:getWindow(g_playerInfoName[type].charmValue):setText(property.player_charm.."/"..g_max_pro_player_charm)
	g_WinMgr:getWindow(g_playerInfoName[type].activeValue):setText(property.active_value.."/"..g_max_pro_player_active)
end
--存储不在线玩家的装备表
local equipTable = {}
function lua_getEquipParam(equipIndex,param)
	equipTable[equipIndex] = param
end
--不在线玩家的人物信息
function lua_ShowOffLinePlayerInfo(playerProperty,basicData,type)
	if mPlayerType.otherPlayer == type then
		PopOtherPlayerInfo()
		OtherPlayerStartRTT()
		SetOtherPlayerEquipment1(equipTable)
	end
	local property = playerProperty
	Scale(g_playerInfoName[type].clean, property.player_clean)
	Scale(g_playerInfoName[type].healthy, property.player_health)
	Scale(g_playerInfoName[type].charm, property.player_charm)
	Scale(g_playerInfoName[type].active, property.active_value)
	--基本资料中的名字由此设置
	SetPlayerBaseInfo1(basicData, type)

	g_WinMgr:getWindow(g_playerInfoName[type].cleanValue):setText(property.player_clean.."/"..g_max_pro_player_clean)
	g_WinMgr:getWindow(g_playerInfoName[type].healthyValue):setText(property.player_health.."/"..g_max_pro_player_health)
	g_WinMgr:getWindow(g_playerInfoName[type].charmValue):setText(property.player_charm.."/"..g_max_pro_player_charm)
	g_WinMgr:getWindow(g_playerInfoName[type].activeValue):setText(property.active_value.."/"..g_max_pro_player_active)

end
--
--不在线玩家的基本资料
function SetPlayerBaseInfo1(basicData, type)
	local value = basicData.level
	--g_Loger:logError(tostring(value))
	SetBaseInfoValue(g_playerInfoName[type].level,  value )
end

--不在线玩家的装备信息
function SetOtherPlayerEquipment1(equipArr)
	for i=1,8 do
		local itemid = equipArr[i-1]


		local itemrow

		if itemid ~= 0 then
			itemrow = g_itemtableMgr:getItemRow(itemid)
		end

		if itemrow ~= nil then
			--id = itemrow.id
			local iconimage = itemrow.icon
			local imageset = itemrow.imageset

			StorageUI_DynamicLoadItem(g_playerInfoName[2].eqSlot..i,
									"newImg", i,
									imageset,
									iconimage, "True")
		else
			local slotName = g_playerInfoName[2].eqSlot..i
			local slotWin = g_WinMgr:getWindow(slotName)

			if slotWin ~= nil then
				if slotWin:getChildCount() > 0 then
					local TempSource = slotWin:getChildAtIdx(0)
					slotWin:removeChildWindow(TempSource)
				end
			end
		end
	end

end
-----------------------------------------
function SetOtherPlayerEquipment(Player)
	for i=1,8 do
		--锁或解锁格子
		local itemgrid = Player:getItemGird(i-1,EQGlobaClass.ItemPack.PT_BODY)
		local itemid = Player:getItemID(i-1,EQGlobaClass.ItemPack.PT_BODY)


		local itemrow

		if itemid ~= 0 then
			itemrow = g_itemtableMgr:getItemRow(itemid)
		end

		if itemrow ~= nil then
			id = itemrow.id
			local iconimage = itemrow.icon
			local imageset = itemrow.imageset

			StorageUI_DynamicLoadItem(g_playerInfoName[2].eqSlot..i,
									"newImg", i,
									imageset,
									iconimage, "True")
			local slotName = g_playerInfoName[2].eqSlot..i
			g_WinMgr:getWindow(slotName):setID(id)
			g_WinMgr:getWindow(slotName):subscribeEvent("MouseClick", "OtherPlayer_SlotClick")
			g_WinMgr:getWindow(slotName):subscribeEvent("MouseClick", "Shop_SetSelectBox")
			g_WinMgr:getWindow(slotName):removeEvent("MouseEnter")
			g_WinMgr:getWindow(slotName):removeEvent("MouseLeave")
			g_WinMgr:getWindow(slotName):subscribeEvent("MouseEnter", "OtherPlayer_SlotEnter")
			g_WinMgr:getWindow(slotName):subscribeEvent("MouseLeave", "OtherPlayer_SlotLeave")
		else
			local slotName = g_playerInfoName[2].eqSlot..i
			local slotWin = g_WinMgr:getWindow(slotName)

			if slotWin ~= nil then
				if slotWin:getChildCount() > 0 then
					local TempSource = slotWin:getChildAtIdx(0)
					slotWin:removeChildWindow(TempSource)
				end
			end
		end
	end
end
-----------------------------------------
--点击其他玩家装备
function OtherPlayer_SlotClick(args)
	local slotWin 	= CEGUI.toWindowEventArgs(args).window
	local id = slotWin:getID()
	if 1 == CEGUI.toMouseEventArgs(args).button then	--	鼠标右键
		return
	end
	--	超链接(hezhr)
	if g_CS_HYPERLINK == g_CursorMgr:getState() then
		ClickHyperLinkItem(id)
	end
	
	local itemrow = g_itemtableMgr:getItemRow(id)
	if itemrow ~= nil then
		preview_itemid = itemrow.id
		--g_Loger:logDebug("OtherPlayer_SlotClick "..preview_itemid)	
		if preview_checkbox:isSelected() == true then
			Preview_OpenByItem(preview_itemid, preview_pos)
		end
	end
end
-----------------------------------------
--鼠标移进其他玩家装备
function OtherPlayer_SlotEnter(args)
	local slotWin 	= CEGUI.toWindowEventArgs(args).window
	local id = slotWin:getID()
	local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
	--时效相关
	local effectTime = ""
	local itemrow = g_itemtableMgr:getItemRow(id)
	if 1 == itemrow.effectTimeID then
		effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
	elseif 2 == itemrow.effectTimeID then
		effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
	end
	createItemhint_shop(id, rect, g_MainUIWindows, effectTime, 1, nil)
end
-----------------------------------------
--鼠标移出其他玩家装备
function OtherPlayer_SlotLeave(args)
	destroyItemhint(g_MainUIWindows)
end
-----------------------------------------
function OnLoadPlayerInfo()
	--图文混排框--
	--备注框
	local eqRemark = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "EqMainPlayerRemark")
	local xOffset = 8
	local yOffset = 8
	EQGlobaFunc.toMultiLineBox(eqRemark:getName()):setPosition(xOffset, yOffset)
	EQGlobaFunc.toMultiLineBox(eqRemark:getName()):setSize(326 - xOffset, 60 - yOffset)
	EQGlobaFunc.toMultiLineBox(eqRemark:getName()):setFont("simsun-10")
	g_WinMgr:getWindow(g_playerInfoName[mPlayerType.mainPlayer].eqRemark):addChildWindow(eqRemark)

	eqRemark = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "EqOtherPlayerRemark")
	EQGlobaFunc.toMultiLineBox(eqRemark:getName()):setPosition(xOffset, yOffset)
	EQGlobaFunc.toMultiLineBox(eqRemark:getName()):setSize(326 - xOffset, 60 - yOffset)
	EQGlobaFunc.toMultiLineBox(eqRemark:getName()):setFont("simsun-10")
	g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].eqRemark):addChildWindow(eqRemark)
	--个人签名框
	local eqSignature = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "EqMainPlayerSignature")
	EQGlobaFunc.toMultiLineBox(eqSignature:getName()):setSize(340,40)
	EQGlobaFunc.toMultiLineBox(eqSignature:getName()):setFont("simsun-10")
	g_WinMgr:getWindow(g_playerInfoName[mPlayerType.mainPlayer].eqSignature):addChildWindow(eqSignature)

	eqSignature = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "EqOtherPlayerSignature")
	EQGlobaFunc.toMultiLineBox(eqSignature:getName()):setSize(340, 40)
	EQGlobaFunc.toMultiLineBox(eqSignature:getName()):setFont("simsun-10")
	g_WinMgr:getWindow(g_playerInfoName[mPlayerType.otherPlayer].eqSignature):addChildWindow(eqSignature)

end
-----------------------------------------
--设置EQ里面的信息
function SetEqInfo(account, info)
	if g_otherPlayerInfoFlag or g_PlayerPropertyFlag then
		local playerType
		if g_objmgr:isMainPlayer(account) then
			playerType = mPlayerType.mainPlayer
			EQGlobaFunc.toMultiLineBox("EqMainPlayerRemark"):clearUp()
			EQGlobaFunc.toMultiLineBox("EqMainPlayerRemark"):addElementText(info.desc, "#00FFFC", false)
		else
			playerType = mPlayerType.otherPlayer
			EQGlobaFunc.toMultiLineBox("EqOtherPlayerRemark"):clearUp()
			EQGlobaFunc.toMultiLineBox("EqOtherPlayerRemark"):addElementText( info.desc, "#00FFFC", false)
		end
		--g_Loger:logError(tostring(playerType))
		--g_Loger:logError(tostring(info.full_name))
		--g_Loger:logError(tostring(info.nick_name))
		EQGlobaFunc.SetText(g_playerInfoName[playerType].eqName, info.full_name)
		EQGlobaFunc.SetText(g_playerInfoName[playerType].eqNickName, info.nick_name)
		EQGlobaFunc.SetText(g_playerInfoName[playerType].eqBirthday, info.birthday)
		EQGlobaFunc.SetText(g_playerInfoName[playerType].eqTel, info.phone)
		
	end
	if g_myDriftBottleUIopened == true then
		DBS_setEqInfo(account, info)
		return
	elseif mfuckFlag == true then
		DBS_quickSetEqInfo(account, info)
		mfuckFlag = false
	end
	-- 保存并设置从服务端接收的好友头像(add by hezhr)
	if "image/gif" ~= info.photo_type then
		g_FriendSys:insertFriendPhoto(account, info.photo_data)
		g_FriendSys:setCurListPhotos()
	end
end
-----------------------------------------
--
function PlayerPropertyUI_clearOldEqInfo()
	--EQGlobaFunc.toMultiLineBox("EqMainPlayerRemark"):clearUp()
	EQGlobaFunc.toMultiLineBox("EqOtherPlayerRemark"):clearUp()
	EQGlobaFunc.SetText(g_playerInfoName[mPlayerType.otherPlayer].eqName, "")
	EQGlobaFunc.SetText(g_playerInfoName[mPlayerType.otherPlayer].eqNickName, "")
	EQGlobaFunc.SetText(g_playerInfoName[mPlayerType.otherPlayer].eqBirthday, "")
	EQGlobaFunc.SetText(g_playerInfoName[mPlayerType.otherPlayer].eqTel, "")
	EQGlobaFunc.SetText(g_playerInfoName[mPlayerType.otherPlayer].level, "")
end
-----------------------------------------
--设置EQ里面的个性签名(主玩家，其他玩家的暂时不做)
function SetMainPlayerSignature(signature)
	EQGlobaFunc.toMultiLineBox("EqMainPlayerSignature"):clearUp()
	EQGlobaFunc.toMultiLineBox("EqMainPlayerSignature"):addElementText( signature, "#000000", false)
end
-----------------------------------------

-----------------------------------------
--预览
-----------------------------------------
function OtherPlayerProperty_ClickPreview()
	if preview_checkbox:isSelected() == true then
		Preview_OpenByItem(preview_itemid, preview_pos)
	else
		Preview_Close()
	end
end

local mPropertyItemHitMsg =
{
	{"清洁度","代表人物清洁的指数，清洁度会影响健康值，所以要经常洗澡洗手哦！"},
	{"健康值","代表人物的健康指数，健康值下降到50后就有可能生病，生病是一件很可怕的事情哦！偶尔上上厕所，洗洗澡就能恢复少量健康值呢！健康值的下降可能会直接影响到魅力值，所以务必小心维！"},
	{"魅力值","代表人物的魅力指数，这个值是非常重要的，只有一些特殊途径可以获得，魅力值会影响活力值获得的多少，所以一定要好好维护魅力值哦！"},
	{"活力值","代表人物的活力指数，打工、捡垃圾等等行为都会用到活力值，而活力值唯一获得的途径就是经常在线，每过一段时间系统都会给你加一定的活力值！"}
}
local mPropertyShowItemHitWins =
{
	{"PlayerAttribute_Root/Player/Player/slider1",1},
	{"PlayerAttribute_Root/Player/Player/slider2",2},
	{"PlayerAttribute_Root/Player/Player/slider3",3},
	{"PlayerAttribute_Root/Player/Player/slider5",4},
	{"PlayerProperty/PropertyTabCtrl/Property/QJDPro",1},
	{"PlayerProperty/PropertyTabCtrl/Property/JKZ",2},
	{"PlayerProperty/PropertyTabCtrl/Property/MLZ",3},
	{"PlayerProperty/PropertyTabCtrl/Property/hyz",4}
}

function InitPropertyItemHit()
	local i
	for i = 1, table.getn(mPropertyShowItemHitWins) do
		g_WinMgr:getWindow(mPropertyShowItemHitWins[i][1]):subscribeEvent("MouseEnter","OnPropertyShowItemHit")
		g_WinMgr:getWindow(mPropertyShowItemHitWins[i][1]):subscribeEvent("MouseLeave","OnPropertyHideItemHit")
		g_WinMgr:getWindow(mPropertyShowItemHitWins[i][1]):subscribeEvent("MouseClick", "OnPropertyHideItemHit")
	end
	
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/jibingzhuangtai"):subscribeEvent("MouseEnter","OnPropertyShowDiseaseItemHit")
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/jibingzhuangtai"):subscribeEvent("MouseLeave","OnPropertyHideItemHit")
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/jibingzhuangtai"):subscribeEvent("MouseClick", "OnPropertyHideItemHit")
end

InitPropertyItemHit()

function OnPropertyShowItemHit(args)
	local wn = CEGUI.toWindowEventArgs(args).window:getName()
	local mp = {x, y}
	mp = g_MouseCur:getPosition()
	mp.x = mp.x + g_MouseCur:getImage():getWidth()
	mp.y = mp.y + g_MouseCur:getImage():getHeight()
	
--	g_Loger:logMessage("mp("..tostring(mp.x)..","..tostring(mp.y)..")")
	
	local i
	for i = 1, table.getn(mPropertyShowItemHitWins) do
		if wn == mPropertyShowItemHitWins[i][1] then
			Itemhint_showType1(g_MainUIWindows,mPropertyItemHitMsg[mPropertyShowItemHitWins[i][2]][1],mPropertyItemHitMsg[mPropertyShowItemHitWins[i][2]][2],mp.x,mp.y)
		end
	end
end

function OnPropertyShowDiseaseItemHit(args)
	local healtyState = g_mainplayer:getHealtyState()
	local mp = {x, y}
	mp = g_MouseCur:getPosition()
	mp.x = mp.x + g_MouseCur:getImage():getWidth()
	mp.y = mp.y + g_MouseCur:getImage():getHeight()
	local title
	local context
	
	if 1 == healtyState then
		title = "健康"
		context = "您现在非常健康"
	else
		title = "疾病"
		context = "疾病分为四个等级，白、黄、紫、红，白色最轻红色最重，如果生病了一定要去找护士治疗，如果不治疗可能会加重病情，还会触发一些倒霉的事情，赶快去治疗吧！"
	end
	
	Itemhint_showType1(g_MainUIWindows,title,context,mp.x,mp.y)
end

function OnPropertyHideItemHit(args)
	Itemhint_hideType1()
end
