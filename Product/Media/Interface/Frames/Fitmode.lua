-----------------------------------------
-- 文件说明	：家装模式相关逻辑处理
-- 作者		：麦
-- 修改		:
-- 日期		：10/03/25
-----------------------------------------
------ChangeFitmentMode----------
--1 游戏模式
--2 添加模式
--3 编辑模式

local PutFunitureFlag = false --家装模式是否开启
local mSelfBag = {}
local SELF_BAG_NUM = 19
local select_slot = -1

---进入退出家装模式
local OtherUIOpenflag = false    --进入家装模式前其他界面是否打开，false：没有打开，true：打开

--
Common_injectEmptyEvt("upRoot")
Common_injectEmptyEvt("up1Root")
local firstInFitEdit = true
function StartFitEdit()
	if PutFunitureFlag == false then
		--家装模式默认改成打开商城
		--g_MainUIWindows:addChildWindow(g_FitmodeStorage)
		FitModeShopCenter_Open()
		SetFitToMarketAttr()
		if g_WinMgr:getWindow("Task/TipAnimate") then
			g_WinMgr:getWindow("Task/TipAnimate"):setVisible(false)
		end
		if g_WinMgr:getWindow("MailAnimation") then
			g_WinMgr:getWindow("MailAnimation"):setVisible(false)
		end
		g_WinMgr:getWindow("DFwnd_MainRoot/RunBtn"):setVisible(false)
		g_WinMgr:getWindow("MainRoot/telephone"):setVisible(false)
		g_WinMgr:getWindow("MainRoot/liwu"):setVisible(false)
		g_WinMgr:getWindow("DFwnd_MainRoot/broadcast"):setVisible(false)
		g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):setVisible(false)	--隐藏竖直工具栏
		g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):setVisible(false)	--隐藏水平工具栏
		SetUnViewMailGiftUIVisible(false)	--隐藏礼物图标
		g_MainUIWindows:removeChildWindow(g_WinMgr:getWindow("BroadCast/BE/SpeakerName"))	--移除公告小喇叭按钮
		g_MainUIWindows:removeChildWindow(g_WinMgr:getWindow("BroadCast/BE/PareName"))	--移除玩家行为公告界面
		g_MainUIWindows:removeChildWindow(g_WinMgr:getWindow("MainRoot/MTxt_ShowChatbox"))	--移除聊天框
		g_MainUIWindows:removeChildWindow(g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"))	--移除查看任务按钮
		FitModeShopCenter_closeIntroUI()	-- 关闭家具介绍界面(2011/4/15 add by hezhr)

		EQGlobaFunc.ChangeGridMode(2)            --增加房屋网格
		EQGlobaFunc.ChangeFitmentMode(3)
		PutFunitureFlag =true
		
		UIM_DoOpenAndCloseUI(g_FitModeMktUINames.root)
		
		if g_isOpenIITUI == true then
			IIT_closeIITUI()
		end
		if firstInFitEdit then
			do_task_script("script10")	-- (打开家装系统)执行新手任务脚本10(2011/6/15 add by hezhr)
			firstInFitEdit = false
		end
	else
	--	EQGlobaFunc.ChangeGridMode(1)
	--	g_MainUIWindows:removeChildWindow(g_FitModeShopCenter)
	--	g_MainUIWindows:removeChildWindow(g_FitmodeStorage)
	--	EQGlobaFunc.ChangeFitmentMode(1)
	--	PutFunitureFlag = false
	end	
end
-----------------------------------------
--
function Fitmode_getPutFunitureFlag()
	return PutFunitureFlag
end
---主界面家具摆放按钮界面点出---------------------------------------------------
function Fitmode_OnPutFuniture()
	g_msgmgr:send(g_req_start_edit_house)
end

-----------------------------------------
--退出
-----------------------------------------
function FitMode_Exit()
		if FitModeShopCenter_ExitCheck("FitModeShopCenter_ConfirmExit") == true then
				Fitmode_ClosedFitmodeStorage()
		end
end
-----------------------------------------
--确认退出
-----------------------------------------
function FitMode_ConfirmExit()
		Fitmode_ClosedFitmodeStorage()
	
		FitModeShopCenter_CancelExit()
end

function AbortFitMode()
	if -1 ~= select_slot then
		Shop_RemoveSelectBox()
		FitMode_CancelSelecting()
	end
	EQGlobaFunc.OnMouseFitmentDro(-1)
	FitModeShopCenter_HideRecycle()
	FitModeShopCenter_HideRotate()
	
	--g_Loger:logMessage("AbortFitMode")
end

----家装仓库界面下点击关闭-----------------------------------------------------------
function Fitmode_ClosedFitmodeStorage()
	AbortFitMode()
	EQGlobaFunc.ChangeGridMode(1)
	g_MainUIWindows:removeChildWindow(g_FitmodeStorage)
	EQGlobaFunc.ChangeFitmentMode(1)
	if g_WinMgr:getWindow("Task/TipAnimate") and PlayerGuide_getNewTaskFlag() then
		g_WinMgr:getWindow("Task/TipAnimate"):setVisible(true)
	end
	if g_WinMgr:getWindow("MailAnimation") then
		g_WinMgr:getWindow("MailAnimation"):setVisible(true)
	end
	g_WinMgr:getWindow("DFwnd_MainRoot/RunBtn"):setVisible(true)
	g_WinMgr:getWindow("MainRoot/telephone"):setVisible(true)
	g_WinMgr:getWindow("MainRoot/liwu"):setVisible(true)
	g_WinMgr:getWindow("DFwnd_MainRoot/broadcast"):setVisible(true)
	g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):setVisible(true)   --显示竖直工具栏
	g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):setVisible(true)   --显示水平工具栏	
	SetUnViewMailGiftUIVisible(true)	--显示礼物图标
	g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("BroadCast/BE/SpeakerName"))	--添加公告小喇叭按钮
	if 1 == g_WinMgr:getWindow("BroadCast/BE/PareName"):getID() then
		g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("BroadCast/BE/PareName"))	--添加玩家行为公告界面
	end
	g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("MainRoot/MTxt_ShowChatbox"))	--添加聊天框
	g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"))	--添加查看任务按钮
	
	PutFunitureFlag = false
	
	FitModeShopCenter_RestoreDayNight() --还原日夜
	
	g_msgmgr:send(g_req_end_edit_house)
end
local firstOutFitEdit = true
function EndFitEdit()
	--if FurStatusFlag == false then
	--	return
	--end
	AbortFitMode()
	EQGlobaFunc.ChangeGridMode(1)
	g_MainUIWindows:removeChildWindow(g_FitModeShopCenter)
	--g_MainUIWindows:removeChildWindow(g_FitmodeStorage)
	EQGlobaFunc.ChangeFitmentMode(1)
	
	g_WinMgr:getWindow("MainRoot/telephone"):setVisible(true)
	g_WinMgr:getWindow("MainRoot/liwu"):setVisible(true)
	g_WinMgr:getWindow("DFwnd_MainRoot/broadcast"):setVisible(true)
	g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):setVisible(true)   --显示竖直工具栏
	g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):setVisible(true)   --显示水平工具栏	
	g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("BroadCast/BE/SpeakerName"))	--添加公告小喇叭按钮
	if 1 == g_WinMgr:getWindow("BroadCast/BE/PareName"):getID() then
		g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("BroadCast/BE/PareName"))	--添加玩家行为公告界面
	end
	g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("MainRoot/MTxt_ShowChatbox"))	--添加聊天框
	g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"))	--添加查看任务按钮
	
	PutFunitureFlag = false
	if firstOutFitEdit then
		do_task_script("script27")
		firstOutFitEdit = false
	end
	--g_msgmgr:send(g_req_end_edit_house)
end

------家装商场界面下点击关闭------------------------------------------------------
function Fitmode_ClosedFitmodeMkt()
	g_msgmgr:send(g_req_end_edit_house)
end
-----------------------------------------
-- 家具摆放处理
-----------------------------------------
function fitEditEventReg()
	g_MainUIWindows:subscribeEvent("MouseEnter", "FitmodeEnterMainUI")
	g_MainUIWindows:subscribeEvent("MouseLeave", "FitmodeLeaveMainUI")
end

function FitmodeClickSlot(args)
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window

	if SlotWin:getChildCount() == 0 then
		return
	end
	
	local itemwin   =  SlotWin:getChildAtIdx(0)
	if itemwin ~= nil then
		select_slot = getFitStorIdx(itemwin)
		
		bagidx = mSelfBag[select_slot]
		
		--播放抓取音效
		if nil ~= bagidx then
			local itemid = g_mainplayer:getItemIDByGlobaIdx(bagidx,EQGlobaClass.ItemPack.PT_WAREHOUSE)
			local itemRow = g_itemtableMgr:getItemRow(itemid)
			
			if itemRow then
				EQGlobaClass.getVoiceMgr():play(itemRow.uvoiceid, false)
				--EQGlobaClass.getVoiceMgr():play(1, false)
				--g_Loger:logMessage("Item uvoiceid: " ..tostring(itemRow.uvoiceid))
			end
		end
	end
end

local bagBasename = "FitmodeStorageRoot/Btn_Bag"
--
function FitmodeClickBag(args)
	local win 	= CEGUI.toWindowEventArgs(args).window
	local winname   =  win:getName()
		
	for bagidx=1,4 do
		if winname == bagBasename..bagidx then
			AbortFitMode()
			ChangePage(bagidx)
		end
	end
end

--
function FitmodeUpdateBagUI()

	local curpage = g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
	local pagenum = g_mainplayer:getPackPageCount(g_PT_WAREHOUSE)
	
	for bagui=1,4 do
		local win = g_WinMgr:getWindow(bagBasename..bagui)
		
		if nil ~= win then
			if bagui <= pagenum then
				win:setVisible(true)
			else
				win:setVisible(false)
			end
		end
		
		if bagui == curpage then
			win:setProperty("NormalImage",g_StoragePageActive)
			win:setProperty("HoverImage",g_StoragePageActive)
			win:setProperty("PushedImage",g_StoragePageActive)
		else
			win:setProperty("NormalImage",g_StoragePageOpenNormal)
			win:setProperty("HoverImage",g_StoragePageOpenHover)
		end
	end
end

function FitMode_CancelSelecting()
	select_slot = -1
end

----家具UI拖进入主界面上----------------------------------------------------------- 
function FitmodeEnterMainUI(args)
	if select_slot == -1 then
		return
	end

	if EQGlobaFunc.getPickState() == 2 then
		return
	end

	local idx = select_slot
	local fitid = getFitmentID(idx)
	
	if -1 ~= fitid then
		--g_Loger:logDebug("FitmodeEnterMainUI ")
		EQGlobaFunc.OnMouseFitmentDro(fitid)
		g_sceneMgr:getHouse():setCurUsingBagIdx(mSelfBag[idx])
	end
end

-----家具UI拖放离开主界面-------------------------------------------------
function FitmodeLeaveMainUI(args)
	if select_slot == -1 then
		return
	end

	if EQGlobaFunc.getPickState() == 2 then
		return
	end

	EQGlobaFunc.OnMouseFitmentDro(-1)
end

------得到家具UI在家装仓库界面里的索引--------------------------------------
function getFitStorIdx(ContainerWin)
	local name = ContainerWin:getName()
	local tmpcontianer
	
	for idx=1,40 do
		tmpcontianer = g_fitstorageslot.none..idx.."/DragContainer"..idx
		
		if nil ~= string.find(name, tmpcontianer) then
			return (idx-1)
		end
	end
end

----通过家装仓库索引找到仓库里对应物品的家具ID--------------------------------
function getFitmentID(idx)
	local bagidx = mSelfBag[idx] 
	if nil ~= bagidx then
		local itemid = g_mainplayer:getItemIDByGlobaIdx(bagidx,EQGlobaClass.ItemPack.PT_WAREHOUSE)
		local itemrow = g_itemtableMgr:getItemRow(itemid)
		
		if itemrow ~= nil then
			--家装				
			if itemrow.type == 2 then
				return itemrow.subID
			end
		end
	end
	return -1
end
----切换到家装仓库---------------------------------------
function SetFitToStoreAttr()
	g_WinMgr:getWindow("FitmodeMktRoot/MarketBtn"):setProperty("NormalImage", "set:Button5 image:NewImage306")
	g_WinMgr:getWindow("FitmodeStorageRoot/toMarket"):setProperty("NormalImage", "set:Button5 image:NewImage306")
	g_WinMgr:getWindow("FitmodeMktRoot/StoreBtn"):setProperty("NormalImage", "set:Button5 image:NewImage311")
	g_WinMgr:getWindow("FitmodeStorageRoot/toStorage"):setProperty("NormalImage", "set:Button5 image:NewImage311")
end
function Fitmode_OnFitmodeStorage()
	g_MainUIWindows:removeChildWindow(g_FitModeShopCenter)
	g_MainUIWindows:addChildWindow(g_FitmodeStorage)
	
	SetFitToStoreAttr()
end
----切换到家装商场---------------------------------------
function SetFitToMarketAttr()
	g_WinMgr:getWindow("FitmodeMktRoot/MarketBtn"):setProperty("NormalImage", "set:Button5 image:NewImage308")
	g_WinMgr:getWindow("FitmodeStorageRoot/toMarket"):setProperty("NormalImage", "set:Button5 image:NewImage308")
	g_WinMgr:getWindow("FitmodeMktRoot/StoreBtn"):setProperty("NormalImage", "set:Button5 image:NewImage309")
	g_WinMgr:getWindow("FitmodeStorageRoot/toStorage"):setProperty("NormalImage", "set:Button5 image:NewImage309")
end
function Fitmode_OnFitmodeMkt()
	g_MainUIWindows:removeChildWindow(g_FitmodeStorage)
	g_MainUIWindows:addChildWindow(g_FitModeShopCenter)
	
	CEGUI.toTabControl(g_WinMgr:getWindow("FitmodeUI/SelectTabctrl")):setTabTextPadding(CEGUI.UDim(0,13.5))
	
	SetFitToMarketAttr()
	
	AbortFitMode()
end
----客户端添加家具后的通知（是否还有需要？）-----------------------------------
function RecvFitAddedNotify()           
	--EQGlobaFunc.ChangeFitmentMode(3)
	--EQGlobaFunc.OnMouseFitmentDro(-1)
	--FurStatusFlag = true
end
-----从仓库界面更新后（也一起更新家装界面）后调用，保存对应仓库的位置
function fitAdd( selfidx, bagidx)
	mSelfBag[selfidx] = bagidx
end

--脚本加载后执行----------
function FitMod_Onload()
		
	for i=0,SELF_BAG_NUM-1 do
		mSelfBag[i] = -1
	end 
	-----------------------------------------
	--  开启关闭家装模式
	-----------------------------------------
	MainUI_ShieldClick(g_FitModeShopCenter)
	MainUI_ShieldClick(g_FitmodeStorage)
	
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):subscribeEvent("Clicked", "Fitmode_OnPutFuniture")
	--g_WinMgr:getWindow("FitmodeMktRoot/StoreBtn"):subscribeEvent("Clicked", "Fitmode_OnFitmodeStorage")
	g_WinMgr:getWindow("FitmodeStorageRoot/toMarket"):subscribeEvent("Clicked", "Fitmode_OnFitmodeMkt")
	
	--家装商城关闭按钮	--家装仓库关闭按钮  
	g_WinMgr:getWindow("FitmodeStorageRoot/Close"):subscribeEvent("Clicked", "FitMode_Exit") 
	
	fitEditEventReg()
	--slotEventreg()
	g_GameEventMgr:subscribeEvent("FIT_NEW_ADDED","RecvFitAddedNotify")
	g_GameEventMgr:subscribeEvent("START_FIT_EDIT","StartFitEdit")
	g_GameEventMgr:subscribeEvent("END_FIT_EDIT","EndFitEdit")
	
	g_WinMgr:getWindow("upRoot"):setProperty("MousePassThroughEnabled", "true")
	
	g_GameEventMgr:subscribeEvent("UPDATE_PLAYER_GOLD", "FitMod_UpdateGold")
	
	for i = C_STORAGE_GIRD_NAME_START,C_STORAGE_GIRD_NUM do
		local SlotName = "FitmodeStorageRoot/Slot"..i

		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "FitmodeClickSlot")
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "Shop_SetSelectBox")
		g_WinMgr:getWindow(SlotName):removeEvent("MouseEnter")
		g_WinMgr:getWindow(SlotName):removeEvent("MouseLeave")
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseEnter", "FitmodeEnterSlot")
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseLeave", "FitmodeLeaveSlot")
	end
	
	for i = 1,4 do
		local SlotName = "FitmodeStorageRoot/Btn_Bag"..i

		g_WinMgr:getWindow(SlotName):
				subscribeEvent("MouseClick", "FitmodeClickBag")

	end
	
end

function FitMod_UpdateGold()
	g_WinMgr:getWindow("FitmodeStorageRoot/Txt6"):setText(g_mainplayer:getEQGold())
	g_WinMgr:getWindow("FitmodeStorageRoot/Txt8"):setText(g_mainplayer:getGameGold())
end

-----------------------------------------
--显示itemhint
-----------------------------------------
function FitmodeEnterSlot(args)
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	if SlotWin:getChildCount() == 0 then
		return
	end
	local idxtar = nil
	local bagidx = nil
	local itemid = nil
	local itemwin = SlotWin:getChildAtIdx(0)
	if nil ~= itemwin then
		idxtar = getFitStorIdx(itemwin)
	end
	if nil ~= idxtar then
		bagidx = mSelfBag[idxtar]
	end
	if nil ~= bagidx then
		itemid = g_mainplayer:getItemIDByGlobaIdx(bagidx, EQGlobaClass.ItemPack.PT_WAREHOUSE)
	end
	if nil ~= itemid then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_mainplayer:getItemDelTimeByGlobalIdx(bagidx, EQGlobaClass.ItemPack.PT_WAREHOUSE)
		createItemhint_shop(itemid, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end

-----------------------------------------
--隐藏itemhint
-----------------------------------------
function FitmodeLeaveSlot(args)
	destroyItemhint(g_MainUIWindows)
end

function OnFitmodePlaceFurniture(args)
	--播放使用音效
	local bagidx = mSelfBag[select_slot] 
	if nil ~= bagidx then
		local itemid = g_mainplayer:getItemIDByGlobaIdx(bagidx,EQGlobaClass.ItemPack.PT_WAREHOUSE)
		local ir = g_itemtableMgr:getItemRow(itemid)
		
		if ir then
			EQGlobaClass.getVoiceMgr():play(ir.voiceid, false)
			--EQGlobaClass.getVoiceMgr():play(32, false)
			--g_Loger:logMessage("Item voiceid: " ..tostring(ir.voiceid))
		else
			--g_Loger:logError("ItemRow null, itemid:"..tostring(itemid))
		end
	end
	
	FitMode_CancelSelecting()
end
--------------------------------------
--
function Fitmode_doPlayerGuideScript(type)
	if type == 1 then --摆放下去的是灯具
		do_task_script("script28")
	else	--其他的是普通家具
		do_task_script("script17")
	end
	PlayerGuide_do_NewerTeachTaskScript(15014) --新手教学任务(提示玩家使用照相功能)
end