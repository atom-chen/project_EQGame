 -----------------------------------------
-- 文件说明	交易模块
-- 作者		：xiaoMing
-- 日期		：10/05/26
-----------------------------------------
--本模块的Deal与C++中的Trade对应

--服务器端通知自己所交易的钱
g_openDealFlag = false

local g_setMoney = false
local g_dealGridSize = 9

function myGold_Onclicked(args)
	g_WinMgr:getWindow(g_deal.myGold):activate()	
end

 ----------------------------------------------------------------------
--脚本装载后执行
--给每个格子注册监听事件
function DealPanleOnLoad()
	g_WinMgr:getWindow(g_deal.myGold):subscribeEvent("TextChanged", "OnMoneyChanged")
	g_WinMgr:getWindow(g_deal.myGold):subscribeEvent("MouseClick", "myGold_Onclicked")

	local window = g_WinMgr:getWindow(g_deal.myGold)
	--设置输入框长度
	CEGUI.toEditbox(window):setMaxTextLength(10)
	--只让输入框输入正整数
	CEGUI.toEditbox(window):setValidationString("^$|^[0-9]*[1-9][0-9]*$")

	--设置阻挡面板允许鼠标穿透
	local win
	win = g_WinMgr:getWindow(g_deal.LBlock)
	win:setProperty("MousePassThroughEnabled","True")
	win = g_WinMgr:getWindow(g_deal.RBlock)
	win:setProperty("MousePassThroughEnabled","True")

	local win
	--左边格子
	for i = 1, g_dealGridSize do
		win = g_WinMgr:getWindow(g_deal.leftNone..i)
		win:subscribeEvent("MouseClick", "OnClickTradeSlot")
		win:subscribeEvent("MouseEnter", "LeftTradeSlot_MouseEnter")
		win:subscribeEvent("MouseLeave", "LeftTradeSlot_MouseLeave")
		--win:subscribeEvent("RightButtonDown", "OnClickTradeSlot")
	end

	--右边格子
	for i = 1, g_dealGridSize do
		win = g_WinMgr:getWindow(g_deal.rightNone..i)
		win:subscribeEvent("MouseClick", "OnClickTradeRightSlot")
		win:subscribeEvent("MouseEnter", "RightTradeSlot_MouseEnter")
		win:subscribeEvent("MouseLeave", "RightTradeSlot_MouseLeave")
		--win:subscribeEvent("DragDropItemEnters", "DealGridDragEnter")
	end

	window = g_WinMgr:getWindow(g_deal.sureBtn)
	window:subscribeEvent("Clicked", "ReqSureTrade")
	--window:setProperty("NormalTextColour","FF000000")

	window = g_WinMgr:getWindow(g_deal.cancleBtn)
	window:subscribeEvent("Clicked", "ReqCancelTrade")
	--window:setProperty("NormalTextColour","FF000000")
	
	g_WinMgr:getWindow(g_deal.closeBtn):subscribeEvent("Clicked", "ReqCancelTrade")

	--把物品托在MainUI上
	--g_MainUIWindows:subscribeEvent("MouseClick", "TradeDroppedOnMainUI")

	g_GameEventMgr:subscribeEvent("TRADE_START", "PopTradePanel")
	g_GameEventMgr:subscribeEvent("OTHER_TRADE_MONEY", "UpdateOtherMoney")
	g_GameEventMgr:subscribeEvent("MY_TRADE_MONEY",        "UpdateMyMoney")

	g_GameEventMgr:subscribeEvent("MY_TRADE_SURE", "LockMyTrade")
	g_GameEventMgr:subscribeEvent("OTHER_TRADE_SURE", "LockOtherTrade")

	g_GameEventMgr:subscribeEvent("TRADE_CANCEL", "CancelTrade")
	g_GameEventMgr:subscribeEvent("TRADE_SUCCESS", "TradeSuccess")

	g_GameEventMgr:subscribeEvent("UPDATE_MY_TRADE", "OnUpdateMyTrade")
	g_GameEventMgr:subscribeEvent("UPDATE_OTHER_TRADE", "OnUpdateOtherTrade")

	MainUI_ShieldClick(g_DealUI)

end
-----------------------------------------------------------------------
--左右边栏遮挡
function SetBlockVisible(isVisible , side)
	local win
	if 0 == side then
		win = g_WinMgr:getWindow(g_deal.LBlock)
	else
		win = g_WinMgr:getWindow(g_deal.RBlock)
	end
	win:setVisible(isVisible)
end
-----------------------------------------------------------------------
--返回交易栏索引 0-14 格子的索引从1开始 ，所以应该减1
function GetLeftGridIdx(winName)
	local gridName
	local baseName = g_deal.leftNone

	for i = 1, g_dealGridSize do
		gridName = baseName..i

		if  nil ~= string.find(winName, gridName) then
			return i - 1
		end

	end
	-- 非交易左边栏格子
	return nil
end
-----------------------------------------------------------------------
function getTradeIdxByDropItem(dropItem)
	local dropName = dropItem:getName()
	local cmpName
	local baseName = g_deal.leftNone

	for i = 1, g_dealGridSize do
		cmpName =  baseName..i.."/DragContainer"..i
		if  nil ~= string.find(dropName, cmpName) then
			return i - 1
		end
	end
	return nil
end
-----------------------------------------------------------------------
--初始化交易栏界面
function InitTradePanel()
	UpdateMyMoney()
	UpdateOtherMoney()

	-- 清空交易栏
	OnUpdateMyTrade()
	OnUpdateOtherTrade()
end

-----------------------------------------------------------------------
--弹出交易框
function PopTradePanel()

	if g_isOpenIITUI == true then
		IIT_closeIITUI()
	end
	
	InitTradePanel()

	--名字设置
	local myName    = g_mainplayer:getName()
	local otherName = g_smallModules:getTraderName()

	myName   = myName.."的物品"
	otherName = otherName.."的物品"

	EQGlobaFunc.SetText(g_deal.myName,myName)
	EQGlobaFunc.SetText(g_deal.otherName,otherName)
	g_openDealFlag = true
	g_MainUIWindows:addChildWindow(g_DealUI)

	if g_StorageUIFlag == false then
		StorageUI_OnOpenStorage()
	end
	UIM_CloseMutexUI("DealWnd")
	SetBlockVisible(false, 0)
	SetBlockVisible(false, 1)
	
 end

-----------------------------------------------------------------------
--删除交易框
 function DeleteTradePanel()
	g_MainUIWindows:removeChildWindow(g_DealUI)
 end

 -----------------------------------------------------------------------
 --请求确认交易
 function ReqSureTrade()
	g_smallModules:reqSureTrade()
 end

 -----------------------------------------------------------------------
 --请求取消交易
 function ReqCancelTrade()
	g_smallModules:reqCancelTrade()
 end
 -----------------------------------------------------------------------
 --更新交易方输入的金钱数
 function UpdateOtherMoney()
	SetBlockVisible(false, 0)
	SetBlockVisible(false, 1)
	--获得钱
	local money = g_smallModules:getAcquireMoney()
	local window = g_WinMgr:getWindow(g_deal.otherGold):setText(money)

 end
  -----------------------------------------------------------------------
 function UpdateMyMoney()
   	--获得玩家money 但不改变它 只是在界面的表现做变化
	--暂时通过界面的文本获得
	local TotalValue = g_mainplayer:getGameGold()

	local money = g_smallModules:getLoseMoney()
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/Txt_GoldBox"):setText(TotalValue - money)

	--在setText之前调用
	g_setMoney = true

	if 0 == money then
		local window = g_WinMgr:getWindow(g_deal.myGold):setText("")
	else
		local window = g_WinMgr:getWindow(g_deal.myGold):setText(money)
	end
 end
 -----------------------------------------------------------------------
 --交易栏锁定
 function LockMyTrade()
	SetBlockVisible(true, 0)
 end
 ------------------------------------------------------------------------
 --交易栏锁定
 function LockOtherTrade()
	SetBlockVisible(true, 1)
 end
 -----------------------------------------------------------------------
 --对发取消交易
 function CancelTrade()
	--获得玩家的钱
	DeleteTradePanel()
 end
 -----------------------------------------------------------------------
 --交易成功
 function TradeSuccess()
	DeleteTradePanel()
 end
  ------------------------------------------------------------------------
  --交易金币的修改（主玩家）
function OnMoneyChanged()
	if g_setMoney then
		g_setMoney = false
		return
	end

	local mwin = g_WinMgr:getWindow(g_deal.myGold)
	local smoney = mwin:getText()
	if 0 == string.len(smoney) then
		smoney = "0"
	end
	local nmoney = tonumber(smoney)
	local tmoney = g_mainplayer:getGameGold()

	if nmoney > tmoney then
		nmoney = tmoney
		mwin:setText(tostring(nmoney))
		return
	end

	SetBlockVisible(false, 0)
	SetBlockVisible(false, 1)

	g_smallModules:sendUpdateMoney(nmoney)
end

-------------------------------------------------------------------------
--交易栏物品拖入Main_UI
function TradeDroppedOnMainUI(args)
	--g_Loger:logMessage("TradeDroppedOnMainUI")
	if(g_DragItem:isDraging()) then
		--仓库拖到交易栏
		if g_PT_TRADE == g_DragItem:getPacktype() then
			g_smallModules:sendUnputItem(g_DragItem:getIndex(), -1)
			g_DragItem:undo()
		end
	end
end
-------------------------------------------------------------------------
--此函数暂时封装StorageUI_DynamicLoadItem来实现交易栏Item的动态
--加载。更好的做法是提供一个统一的Item的动态加载
function TradeUI_DynamicLoadItem(slotName, itemWinName, index, imagesetName, imgName, dragFlag, count)
	StorageUI_DynamicLoadItem(slotName,
									itemWinName, index,
									imagesetName,
									imgName,
									dragFlag,
									count)
	local container = g_WinMgr:getWindow(slotName):getChildAtIdx(0)
	--此事件要取消 否则会产生错误
	container:removeEvent("DragDropItemDropped")
	--container:removeEvent("DragEnded")
	--if "True" == dragFlag then
		--注册自己的监听函数
		--container:subscribeEvent("DragDropItemDropped", "ItemExchWithTrade")
	--end
end

-------------------------------------------------------------------------
--根据服务器消息更新交易栏
--更新自己的交易栏
function OnUpdateMyTrade()
	--如果交易栏锁定面板存在 则隐藏锁定面板
	OnUpdateTrade(0)
	SetBlockVisible(false, 0)
	SetBlockVisible(false, 1)
end
--更新对方的交易栏
function OnUpdateOtherTrade()
	--如果交易栏锁定面板存在 则隐藏锁定面板
	SetBlockVisible(false, 1)
	SetBlockVisible(false, 0)
	OnUpdateTrade(1)
end
-------------------------------------------------------------------------
--side == 0 表示交易左边栏格子
--side == 1 表示交易右边栏格子
function OnUpdateTrade(side)
		local itemId
		for i = 1, g_dealGridSize do
				itemId = g_smallModules:getTradeItemId(i - 1, side) -- 0代表左边交易栏，即自己的交易物品
				count  = g_smallModules:getTradeItemCount(i - 1, side) -- 0代表左边交易栏，即自己的交易物品
				if itemId > 0 then
					local itemrow 	= g_itemtableMgr:getItemRow(itemId)
					if nil ~= itemrow then
							local iconimage = itemrow.icon
							local imageset = itemrow.imageset
							local slotName, dragFlag
							if 0 == side then
								slotName = g_deal.leftNone..i
								dragFlag = "True"
							else
								slotName = g_deal.rightNone..i
								dragFlag = "False"
							end
							TradeUI_DynamicLoadItem(slotName,
												"newImg", i,
												imageset,
												iconimage,
												dragFlag,
												count)
					else
						--g_Loger:logMessage("the item no belong the itemTable")
					end
				else
					DropItemFromTrade(i, side)
				end
		end
end

-------------------------------------------------------------------------
--side == 0 表示交易左边栏格子
--side == 1 表示交易右边栏格子
function DropItemFromTrade(gridIndex, side)
	local slotName
	local slotWin
	if side == 0 then
		slotName = g_deal.leftNone..gridIndex
		slotWin    =  g_WinMgr:getWindow(slotName)
	else
		slotName = g_deal.rightNone..gridIndex
		slotWin    = g_WinMgr:getWindow(slotName)
	end

	if slotWin ~= nil then
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			TempSource:removeEvent("DragDropItemDropped")
			slotWin:removeChildWindow(TempSource)
		end
	end
end
-------------------------------------------------------------------------
function DealGridDragEnter(args)
	local DragItem 	= CEGUI.toDragDropEventArgs(args).dragDropItem
	DragItem:setZOrderingEnabled(true)
	DragItem:moveToFront()
end
-------------------------------------------------------------------------
-------------------------------------------------------------------------
-------------------------------------------------------------------------
-------------------------------------------------------------------------
-------------------------------------------------------------------------
--交易栏拖动改粘吸
function OnClickTradeSlot(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_deal.leftNone, 1, g_dealGridSize)

	index = index - 1
	local indexItemID = g_smallModules:getTradeItemId(index , 0)
	
	--区分左键点击和右键点击 0 为左键 1为右键
	local state = CEGUI.toMouseEventArgs(args).button
	
	if 0 == state then
		--	超链接(hezhr)
		if g_CS_HYPERLINK == g_CursorMgr:getState() then
			ClickHyperLinkItem(indexItemID)
			return
		end	
		
		Storage_ClickCloseEquipTips()	-- 隐藏装备提示(hezhr)
		if(g_DragItem:isDraging()) then
			--仓库拖到交易栏
			if g_PT_WAREHOUSE == g_DragItem:getPacktype() then
				
				--若为拆分状态提示无法拆分
				if g_DragItem:getSplitNum() > 0 then
					MsgBox_createMsgBox(g_MainUIWindows,10601,"","无法这样拆分物品.", "MsgBox_cancelFunc", "")
					return
				end
			
				g_smallModules:sendPutItem(g_DragItem:getIndex(), index)
				g_DragItem:undo()
			--交易栏拖到交易栏
			elseif g_PT_TRADE == g_DragItem:getPacktype() then
				ReqSwapTradeItem(g_DragItem:getIndex(), index)
			elseif g_PT_STORAGE == g_DragItem:getPacktype() then
				--若为拆分状态提示无法拆分
				if g_DragItem:getSplitNum() > 0 then
					MsgBox_createMsgBox(g_MainUIWindows,10601,"","无法这样拆分物品.", "MsgBox_cancelFunc", "")	
				else
					MsgBox_createMsgBox(g_MainUIWindows,10602,"","无法直接将仓库中的物品进行交易.", "MsgBox_cancelFunc", "")	
				end
				
				return
			end
		elseif -1 ~= indexItemID then--点击的格子有物品
			g_DragItem:drag(winName,index,g_PT_TRADE)
			g_dratItemID = indexItemID	--保存吸起来的物品模板id(hezhr)
		end
	else
		if -1 ~= indexItemID then
			g_smallModules:sendUnputItem(index, -1)
		end
	end
end
-------------------------------------------------------------------------
function ReqSwapTradeItem(fromIndex, toIndex)
	if(g_DragItem:isDraging()) then
		if fromIndex ~= toIndex then
			g_smallModules:sendSwap(fromIndex, toIndex)
		end
		g_DragItem:undo()
	end
end
-------------------------------------------------------------------------
-- 点击右边交易格子(hezhr)
function OnClickTradeRightSlot(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_deal.rightNone, 1, g_dealGridSize)
	index = index - 1
	local indexItemID = g_smallModules:getTradeItemId(index , 1)
	--区分左键点击和右键点击 0 为左键 1为右键
	local state = CEGUI.toMouseEventArgs(args).button
	if 1 == state then
		return
	end
	if -1 == indexItemID then
		return
	end
	--	超链接(hezhr)
	if g_CS_HYPERLINK == g_CursorMgr:getState() then
		ClickHyperLinkItem(indexItemID)
	end	
end
-------------------------------------------------------------------------
-- 打开左边交易格子itemhint(hezhr)
function LeftTradeSlot_MouseEnter(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_deal.leftNone, 1, g_dealGridSize)
	index = index - 1
	local indexItemID = g_smallModules:getTradeItemId(index , 0)
	if -1 ~= indexItemID then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_smallModules:getTradeItemEffectTime(index, 0)
		createItemhint_shop(indexItemID, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
-------------------------------------------------------------------------
-- 关闭左边交易格子itemhint(hezhr)
function LeftTradeSlot_MouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
-------------------------------------------------------------------------
-- 打开右边交易格子itemhint(hezhr)
function RightTradeSlot_MouseEnter(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_deal.rightNone, 1, g_dealGridSize)
	index = index - 1
	local indexItemID = g_smallModules:getTradeItemId(index , 1)
	local effectTime = ""

	if -1 ~= indexItemID then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_smallModules:getTradeItemEffectTime(index, 1)
		createItemhint_shop(indexItemID, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
-------------------------------------------------------------------------
-- 关闭右边交易格子itemhint(hezhr)
function RightTradeSlot_MouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
