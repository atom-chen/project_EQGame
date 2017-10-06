-----------------------------------------
-- 文件说明	：红土地
-- 作者		：xiaoMing
-- 日期		：2010-11-24
-----------------------------------------
--
mOpenFlag = false
g_GameEventMgr:subscribeEvent("END_ALLPICK", "RedLand_AllpickAward")
g_GameEventMgr:subscribeEvent("END_ALLPICK", "popHintData")

-----------------------------------------
local mTexts = {
					[1] = "您的土地升级后，将变成肥沃的红土地:",
					[2] = "（1）、珍贵的",
					[3] = "高级作物",
					[4] = "，只能种在红土地上！",
					[5] = "（2）、普通作物种在红土地上增产",
					[6] = "10%",
					[7] = "！",
					[8] = "（3）、红土地上进行摘取，挖地有更丰富的",
					[9] = "奖励",
					[10] = "，更多有趣的悲喜剧！",
					}

local function _InitUI()
	--红土地作用提示
	tips = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "redLand_tips")
	tips:setSize(CEGUI.UVector2(CEGUI.UDim(0, 415), CEGUI.UDim(0,65)))

	g_WinMgr:getWindow(g_upgradeLandUIName.tips):addChildWindow(tips)
	
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-11", mTexts[1], "#000000", true)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[2], "#000000", true)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[3], "#FF6600", false)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[4], "#000000", false)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[5], "#000000", true)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[6], "#FF6600", false)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[7], "#000000", false)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[8], "#000000", true)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[9], "#FF6600", false)
	EQGlobaFunc.toMultiLineBox(tips:getName()):addElementText("simsun-10", mTexts[10], "#000000", false)
	
	
	--满足条件
	condition = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "redLand_condition")
	EQGlobaFunc.toMultiLineBox("redLand_condition"):setFont("simsun-10")	-- 设置字体：新宋10号
	EQGlobaFunc.toMultiLineBox("redLand_condition"):setCols("#FFFFFF")	-- 设置颜色：白色	
	condition:setSize(CEGUI.UVector2(CEGUI.UDim(0, 380), CEGUI.UDim(0,110)))
	g_WinMgr:getWindow(g_upgradeLandUIName.condition):addChildWindow(condition)
end

-----------------------------------------
function ReqUpgradeLand()
	g_FarmMgr:reqUpgradeLand()
	CloseUpgradeLandUI()
end
-----------------------------------------
-----------------------------------------
-----------------------------------------
-----------------------------------------
function OnLoadRedLand()
	_InitUI()
	
	g_WinMgr:getWindow(g_upgradeLandUIName.sureBtn):subscribeEvent("Clicked", "ReqUpgradeLand")
	g_WinMgr:getWindow(g_upgradeLandUIName.closeBtn):subscribeEvent("Clicked", "CloseUpgradeLandUI")
	
	MainUI_ShieldClick(g_upgradeLandUI)
end
-----------------------------------------
function ReqCanUpgradeLand()
	if false == mOpenFlag then
		g_FarmMgr:reqCanUpgradeLand() 
	else
		CloseUpgradeLandUI()
	end
end
-----------------------------------------
function OpenUpgradeLandUI()
	RedLand_closeOtherUI()
	if false == mOpenFlag then
		g_FarmUI:addChildWindow(g_upgradeLandUI)
		Common_injectEmptyEvt(g_upgradeLandUI:getName())
		
		mOpenFlag = true
	end
	--RedLand_popHavestRewardUI()
end
-----------------------------------------
function CloseUpgradeLandUI()
	g_FarmUI:removeChildWindow(g_upgradeLandUI)
	mOpenFlag = false
end
-----------------------------------------
--
function RedLand_closeOtherUI()
	if true== g_FStorageFlag then
		FRemoveStoragePanel()
	end
	if true == g_FShopFlag then
		CloseFarmShopUI()
	end
	if true == g_FSettingFlag then
		FSettingClose()
	end
	if true == g_FPlayerInfoFlag then
		FClosePlayerInfoUI()
	end
	if true == g_FarmDecoFlag then
		Dec_closeDecUI()
	end
end
--------------------------------------------------
--红土地收获奖励
g_WinMgr:getWindow("red_farmland_harvest/bgimg/button"):subscribeEvent("Clicked","RedLand_closeHavestRewardUI")
g_WinMgr:getWindow("red_farmland_harvest__auto_closebutton__"):subscribeEvent("Clicked","RedLand_closeHavestRewardUI")
g_WinMgr:getWindow("red_farmland_harvest/bgimg/button"):subscribeEvent("KeyDown","RedLand_closeHavestRewardUI")
g_WinMgr:getWindow("red_farmland_harvest"):subscribeEvent("KeyDown","RedLand_closeHavestRewardUI")

g_WinMgr:getWindow("turn_soil_attain__auto_closebutton__"):subscribeEvent("Clicked","RedLand_closeTUTSRewardUI")
g_WinMgr:getWindow("turn_soil_attain/button"):subscribeEvent("Clicked","RedLand_closeTUTSRewardUI")
g_WinMgr:getWindow("turn_soil_attain/button"):subscribeEvent("KeyDown","RedLand_closeTUTSRewardUI")
g_WinMgr:getWindow("turn_soil_attain"):subscribeEvent("KeyDown","RedLand_closeTUTSRewardUI")

g_GameEventMgr:subscribeEvent("NOTIFY_PICKAWARD", "RedLand_popHavestRewardUI")
g_GameEventMgr:subscribeEvent("NOTIFY_TUTSAWARD", "RedLand_popTUTSRewardUI")

local RedLandRewardTable = {}
local awardMoney = 0
local allAwardName = ""
---------------------------------------------------------
--
function RedLand_popHavestRewardUI()
	g_FarmUI:addChildWindow(g_redLandRewardUI)
	Common_injectEmptyEvt("red_farmland_harvest")
	RedLand_setRewardUiPos()
	Common_setModalState(g_redLandRewardUI:getName(), true)
	RedLand_setRewardSlotUIPos()
end
----------------------------------------------------------
--
function RedLand_setRewardUiPos()
	local width = g_redLandRewardUI:getWidth():asAbsolute(1024)
	local height = g_redLandRewardUI:getHeight():asAbsolute(1024)
	g_redLandRewardUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-(width/2)), CEGUI.UDim(0.5,-(height/2))))
end
------------------------------------------------------------
--
function RedLand_closeHavestRewardUI()
	for i = 1,4 do
		g_WinMgr:getWindow("red_farmland_harvest/bgimg/bgicon" .."/icon" ..i):setProperty("Image","")
		local txtWin = "red_farmland_harvest/bgimg/bgicon/count"
		g_WinMgr:getWindow(txtWin ..i):setText("")

	end
	Common_setModalState(g_redLandRewardUI:getName(), false)
	g_FarmUI:removeChildWindow(g_redLandRewardUI)
	RedLand_clearRewardTable()
end
-------------------------------------------------------------------
--
function RedLand_clearRewardTable()
	RedLandRewardTable = {}
end
-------------------------------------------------------------------
--
function RedLand_saveRewardSerData(i,money,item,count)

	allAwardName = "你在收获自己家的过程中，获得了"
	awardMoney = money
	table.insert(RedLandRewardTable,
				 {
				  idx = item,
				  c = count,
				 }
				)
end
------------------------------------------------------------------
--
function RedLand_setRewardSlotUIPos()
	local rewardCount = table.getn(RedLandRewardTable)
	if rewardCount == 0 then
		return
	end
	--g_Loger:logError("**rewardCount*" ..rewardCount)
	local baseSlotName = "red_farmland_harvest/bgimg/bgicon"
	for i,v in ipairs(RedLandRewardTable) do
		--g_Loger:logError("***" ..RedLandRewardTable[i].idx .."***" ..RedLandRewardTable[i].c)
		local itemRow = g_itemtableMgr:getItemRow(v.idx)
		local name = itemRow.name
		local imageset = itemRow.imageset
		local icon = itemRow.icon
		--g_Loger:logError("***" ..imageset .."**" ..icon)
		local SlotWin = g_WinMgr:getWindow(baseSlotName ..1)
		local slotWidth = SlotWin:getWidth():asAbsolute(1024)
		local gapWidth = 5
		local posX = -(slotWidth*rewardCount)/2 + (slotWidth) * (i-1)
		g_WinMgr:getWindow(baseSlotName ..i):setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,posX), CEGUI.UDim(0,95)))
		if imageset ~= nil and icon ~= nil then
			g_WinMgr:getWindow(baseSlotName .."/icon" ..i):setProperty("Image", "set:" ..imageset .." image:" ..icon)
		end
		local txtWin = "red_farmland_harvest/bgimg/bgicon/count"
		g_WinMgr:getWindow(txtWin ..i):setText(v.c)
		allAwardName = allAwardName ..tostring(v.c) ..name .." "
		--g_Loger:logError("**allAwardName*" ..allAwardName)
	end
	if awardMoney ~= 0 then
		allAwardName = allAwardName ..tostring(awardMoney) .."金币"
	end
	--EQGlobaFunc.SetText("red_farmland_harvest/bgimg/sysmsg", allAwardName)
	local msgTxtBox = g_WinMgr:getWindow("red_farmland_harvest/bgimg/sysmsg1")
	if msgTxtBox == nil then
		msgTxtBox = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "red_farmland_harvest/bgimg/sysmsg1")
		msgTxtBox:setSize(CEGUI.UVector2(CEGUI.UDim(0,280), CEGUI.UDim(0,40)))	-- 设置控件13个汉字宽度
		msgTxtBox:setPosition(CEGUI.UVector2(CEGUI.UDim(0,1), CEGUI.UDim(0,74)))
		EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setAlign("AT_CENTER_HOR")	-- 设置水平居中
		EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setCols("#FF0000")
		g_redLandRewardUI:addChildWindow(msgTxtBox)
	end
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):addElementText(allAwardName, false)	-- 添加文本
	local height = EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setHeight(height)
end
--------------------------------------------------------------
--
function RedLand_AllpickAward()
	g_FarmMgr:checkAllpickAward()
end
--------------------------------------------------------------
--铲地奖励
local TUTSRewardsTable = {}
--------------------------------------------------------------
--
function RedLand_saveTUTSRewards(i,money,item,count)
	TUTSRewardsTable = {}
	table.insert(TUTSRewardsTable,
				{
				  idx = item,
				  c = count,
				}
				)
end
--------------------------------------------------------
--
function RedLand_popTUTSRewardUI()
	g_FarmUI:addChildWindow(g_TUTSRewardUI)
	Common_injectEmptyEvt("turn_soil_attain")
	RedLand_setTUTSRewardUiPos()
	Common_setModalState(g_TUTSRewardUI:getName(), true)
	RedLand_setTUTSRewardsData()
end
----------------------------------------------------------
--
function RedLand_setTUTSRewardUiPos()
	local width = g_TUTSRewardUI:getWidth():asAbsolute(1024)
	local height = g_TUTSRewardUI:getHeight():asAbsolute(1024)
	g_TUTSRewardUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-(width/2)), CEGUI.UDim(0.5,-(height/2))))
end
--------------------------------------------------------
--
function RedLand_setTUTSRewardsData()
	local slotbaseName = "turn_soil_attain/iconbackground/icon"
	local countWinName = "turn_soil_attain/iconbackgroundcount"
	local tabN = table.getn(TUTSRewardsTable)
	if tabN == 0 then
		return
	end
	for i,v in ipairs(TUTSRewardsTable) do
		local itemRow = g_itemtableMgr:getItemRow(v.idx)
		local name = itemRow.name
		local imageset = itemRow.imageset
		local icon = itemRow.icon
		g_WinMgr:getWindow(slotbaseName ..i):setProperty("Image","set:" ..imageset .." image:" ..icon)
		g_WinMgr:getWindow(countWinName ..i):setText(tostring(v.c))
	end
end
--------------------------------------------------------
--
function RedLand_closeTUTSRewardUI()
	local slotName = "turn_soil_attain/iconbackground/icon1"
	local countWinName = "turn_soil_attain/iconbackgroundcount1"
	g_WinMgr:getWindow(slotName):setProperty("Image","")
	g_WinMgr:getWindow(countWinName):setText("")
	Common_setModalState(g_TUTSRewardUI:getName(), false)
	g_FarmUI:removeChildWindow(g_TUTSRewardUI)
end
--------------------------------------------------------
--红土地偷菜
function RedLand_popStealAwardUI()
	g_FarmUI:addChildWindow(g_redLandStealRewardUI)
	Common_injectEmptyEvt("red_farmland_Steal")
	Common_setModalState(g_redLandStealRewardUI:getName(), true)
	RedLand_setStealAwardUIPos()
	RedLand_setStealAwardData()
end
--------------------------------------------------------
--
function RedLand_setStealAwardUIPos()
	local width = g_redLandStealRewardUI:getWidth():asAbsolute(1024)
	local height = g_redLandStealRewardUI:getHeight():asAbsolute(1024)
	g_redLandStealRewardUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-(width/2)), CEGUI.UDim(0.5,-(height/2))))

end
--------------------------------------------------------
--
function RedLand_setStealAwardData()

end
--------------------------------------------------------
--
function RedLand_closeStealAwardUI()
	Common_setModalState(g_redLandStealRewardUI:getName(), false)
	g_FarmUI:removeChildWindow(g_redLandStealRewardUI)
end
-------------------------------------------------------
--
--
--
-------------------------------------------------------
--------------------------------------------------------
--
function popHintData()
	g_FarmMgr:checkAllHintData()
end
--------------------------------------------------------
--
function popHintDataUI(msg_id,hint_count, loseGold)
	local content = "你在采摘的过程中被狗发现%s次，在逃跑的时候损失了%s金币。"
	if hint_count ~= 0 then
		content = string.format(content, hint_count,loseGold)
		MsgSys_showStyle5(content)
	end
end

