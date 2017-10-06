-----------------------------------------
-- 文件说明	：家装模式商城相关逻辑处理
-- 作者		：lf
-- 修改		：
-- 日期		：10/07/10
-----------------------------------------

local daynight
local type1 = 0
local type2 = 0
local type3 = 0
local page = 0
local goods_row = nil
local total_count = 0
local total_eq = 0
local total_gold = 0
local type_slot = "FitmodeMktRoot/smslot"
local goods_slot = "FitmodeMktRoot/Slot"
local select_slot = -1
local show_mark = false
local cart_index = 0
local cart_slot = "Shoplistroot/Shoplist/Slot_"
local cart_slotname = "Shoplistroot/Shoplist/SL_Name"
local cart_slotprice = "Shoplistroot/Shoplist/SL_Price"
local cart_slotmoney = "Shoplistroot/Shoplist/Selling_img"
local recycle_button = g_WinMgr:createWindow("EQHomeLook/ImageButton", "FitModeShopCenter_Recycle")
local leftrotate_button = g_WinMgr:createWindow("EQHomeLook/ImageButton", "FitModeShopCenter_LeftRotate")
local rightrotate_button = g_WinMgr:createWindow("EQHomeLook/ImageButton", "FitModeShopCenter_RightRotate")
local pickup_button = g_WinMgr:createWindow("EQHomeLook/ImageButton", "FitModeShopCenter_Pickup")
local putdown_button = g_WinMgr:createWindow("EQHomeLook/ImageButton", "FitModeShopCenter_Putdown")
local playing_ani = false

-----------------------------------------
--脚本加载后执行
-----------------------------------------
function FitModeShopCenterUI_Onload()
		--商城
		MainUI_ShieldClick(g_FitModeShopCenter)
		MainUI_ShieldClick(g_FitModeShoppingCart)
		--g_WinMgr:getWindow("up1Root"):setProperty("MousePassThroughEnabled", "true")
		
		g_WinMgr:getWindow("FitmodeUI/img1"):subscribeEvent("MouseClick", "FitModeShopCenter_ChangeDayNight")
		g_WinMgr:getWindow("FitmodeUI1/img1"):subscribeEvent("MouseClick", "FitModeShopCenter_ChangeDayNight")
		g_WinMgr:getWindow("FitmodeMktRoot/Btn1"):subscribeEvent("Clicked", "FitModeShopCenter_Purchase")
		g_WinMgr:getWindow("FitmodeMktRoot/MarketBtn"):subscribeEvent("Clicked", "FitModeShopCenter_OpenShopCenter")
		g_WinMgr:getWindow("FitmodeMktRoot/StoreBtn"):subscribeEvent("Clicked", "FitModeShopCenter_OpenStore")
		g_WinMgr:getWindow("FitmodeMktRoot/CloseBtn"):subscribeEvent("Clicked", "FitModeShopCenter_Exit")
		g_WinMgr:getWindow("FitmodeMktRoot/checkbox"):subscribeEvent("CheckStateChanged", "FitModeShopCenter_Mark")
		
		g_WinMgr:getWindow("FitmodeUI/SelectTabctrl__auto_TabPane__Buttons__auto_btnTab 1"):subscribeEvent("Clicked", "FitModeShopCenter_ClickPage")
		g_WinMgr:getWindow("FitmodeUI/SelectTabctrl__auto_TabPane__Buttons__auto_btnTab 2"):subscribeEvent("Clicked", "FitModeShopCenter_ClickPage")
		g_WinMgr:getWindow("FitmodeUI/SelectTabctrl__auto_TabPane__Buttons__auto_btnTab 3"):subscribeEvent("Clicked", "FitModeShopCenter_ClickPage")
		g_WinMgr:getWindow("FitmodeUI/img2"):setProperty("MousePassThroughEnabled", "true")
		g_WinMgr:getWindow("FitmodeUI/img3"):setProperty("MousePassThroughEnabled", "true")
		g_WinMgr:getWindow("FitmodeUI/img4"):setProperty("MousePassThroughEnabled", "true")
		
		local SlotName
		
		for i=1,9 do
				SlotName = type_slot..i
				g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "FitModeShopCenter_ClickType")	
		end
		
		g_WinMgr:getWindow("FitmodeMktRoot/LeftGo"):subscribeEvent("Clicked", "FitModeShopCenter_LastPage")
		g_WinMgr:getWindow("FitmodeMktRoot/RightGo"):subscribeEvent("Clicked", "FitModeShopCenter_NextPage")
		
		for i=1,24 do
			SlotName = goods_slot..i
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "FitModeShopCenter_ClickGoods")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "Shop_SetSelectBox")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseEnter")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseLeave")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseEnter", "FitModeShopCenter_GoodsMouseEnter")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseLeave", "FitModeShopCenter_GoodsMouseLeave")
		end
		
		--购物车
		
		g_WinMgr:getWindow("Shoplistroot/Shoplist/Btn_1"):subscribeEvent("Clicked", "FitModeShopCenter_CartDel")
		g_WinMgr:getWindow("Shoplistroot/Shoplist/Btn_2"):subscribeEvent("Clicked", "FitModeShopCenter_CartPay")
		g_WinMgr:getWindow("Shoplistroot/Shoplist/Btn_3"):subscribeEvent("Clicked", "FitModeShopCenter_CartClose")
		g_WinMgr:getWindow("Shoplistroot/Shoplist__auto_closebutton__"):subscribeEvent("Clicked", "FitModeShopCenter_CartClose")
		g_WinMgr:getWindow("Shoplistroot/Shoplist/VerticalScrollbar"):subscribeEvent("ScrollPosChanged", "FitModeShopCenter_CartScrollbar")
		
		for i=1,12 do
			SlotName = cart_slot..i
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "FitModeShopCenter_CartSel")	
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "Shop_SetSelectBox")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseEnter")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseLeave")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseEnter", "FitModeShopCenter_CartMouseEnter")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseLeave", "FitModeShopCenter_CartMouseLeave")
		end
		
		CEGUI.toTabControl(g_WinMgr:getWindow("FitmodeUI/SelectTabctrl")):setTabTextPadding(CEGUI.UDim(0,13.5))
		
		g_MainUIWindows:subscribeEvent("MouseEnter", "FitEnterMainUI")
		g_MainUIWindows:subscribeEvent("MouseLeave", "FitLeaveMainUI")
		
		--更新EQ币
		g_GameEventMgr:subscribeEvent("UPDATE_PLAYER_GOLD", "FitModeShopCenter_UpdateGold")
		
		--回收按钮
		recycle_button:setProperty("HoverImage", "set:Button1 image:imagebutton93_hover")
		recycle_button:setProperty("NormalImage", "set:Button1 image:imagebutton93_normal")
		recycle_button:setProperty("PushedImage", "set:Button1 image:imagebutton93_normal")
		g_MainUIWindows:addChildWindow(recycle_button)
		recycle_button:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))
		recycle_button:subscribeEvent("Clicked", "FitModeShopCenter_ClickRecycle")
		
		--左旋按钮
		leftrotate_button:setProperty("HoverImage", "set:Button5 image:NewImage361")
		leftrotate_button:setProperty("NormalImage", "set:Button5 image:NewImage359")
		leftrotate_button:setProperty("PushedImage", "set:Button5 image:NewImage361")
		g_MainUIWindows:addChildWindow(leftrotate_button)
		leftrotate_button:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))
		leftrotate_button:subscribeEvent("Clicked", "FitModeShopCenter_ClickLeftRotate")
		
		--右旋按钮
		rightrotate_button:setProperty("HoverImage", "set:Button5 image:NewImage362")
		rightrotate_button:setProperty("NormalImage", "set:Button5 image:NewImage360")
		rightrotate_button:setProperty("PushedImage", "set:Button5 image:NewImage362")
		g_MainUIWindows:addChildWindow(rightrotate_button)
		rightrotate_button:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))
		rightrotate_button:subscribeEvent("Clicked", "FitModeShopCenter_ClickRightRotate")
		
		--拿起按钮
		pickup_button:setProperty("HoverImage", "set:Button5 image:NewImage403")
		pickup_button:setProperty("NormalImage", "set:Button5 image:NewImage401")
		pickup_button:setProperty("PushedImage", "set:Button5 image:NewImage403")
		g_MainUIWindows:addChildWindow(pickup_button)
		pickup_button:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))
		pickup_button:subscribeEvent("Clicked", "FitModeShopCenter_ClickPickup")
		
		--放下按钮
		putdown_button:setProperty("HoverImage", "set:Button5 image:NewImage404")
		putdown_button:setProperty("NormalImage", "set:Button5 image:NewImage402")
		putdown_button:setProperty("PushedImage", "set:Button5 image:NewImage404")
		g_MainUIWindows:addChildWindow(putdown_button)
		putdown_button:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))
		putdown_button:subscribeEvent("Clicked", "FitModeShopCenter_ClickPutdown")
		
		FitModeShopCenter_HideRecycle()
end

-----------------------------------------
--更新EQ币
-----------------------------------------
function FitModeShopCenter_UpdateGold()
	g_WinMgr:getWindow("FitmodeMktRoot/Txt6"):setText(g_mainplayer:getEQGold())
	g_WinMgr:getWindow("FitmodeMktRoot/Txt8"):setText(g_mainplayer:getGameGold())
end

-----------------------------------------
--打开
-----------------------------------------
function FitModeShopCenter_Open()
		g_MainUIWindows:addChildWindow(g_FitModeShopCenter)
		--g_WinMgr:getWindow("FitmodeMktRoot"):setVisible(false)
	
		type1 = 2
		type2 = 1
		type3 = 1
		page = 1
		goods_row = nil
		total_count = 0
 		total_eq = 0
 		total_gold = 0
		CEGUI.toTabControl(g_WinMgr:getWindow("FitmodeUI/SelectTabctrl")):setSelectedTab(0)
		FitModeShopCenter_UpdateTypeIcon()
		g_mainplayer:setShopCenterType(type1*100+type2*10+type3, 3, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER, 0)
		FitModeShopCenter_UpdateGoodsIcon()
		FitModeShopCenter_SelectType(type_slot..type3)
		FitModeShopCenter_HideRecycle()
		
		g_sceneMgr:getHouse():setTempMark(show_mark)
		--daynight = EQGlobaFunc.getDayNight()
		
		g_WinMgr:getWindow("FitmodeMktRoot/Txt2"):setText(0)
		g_WinMgr:getWindow("FitmodeMktRoot/Selling_img"):setProperty("Image", "set: image:")
		
		if EQGlobaFunc.getDayNight() == true then
				g_WinMgr:getWindow("FitmodeUI/img1"):setProperty("Image", "set:Button5 image:NewImage349")
				g_WinMgr:getWindow("FitmodeUI1/img1"):setProperty("Image", "set:Button5 image:NewImage349")
		else
				g_WinMgr:getWindow("FitmodeUI/img1"):setProperty("Image", "set:Button5 image:NewImage351")
				g_WinMgr:getWindow("FitmodeUI1/img1"):setProperty("Image", "set:Button5 image:NewImage351")
		end
end

-----------------------------------------
--切换检查
-----------------------------------------
function FitModeShopCenter_OpenStore()
		if EQGlobaFunc.getPickState() == 2 then
			MsgBox_createMsgBox(g_MainUIWindows,0,"","请先放下物品", "MsgBox_cancelFunc", "")
			return
		end

		if FitModeShopCenter_ExitCheck("FitModeShopCenter_ConfirmStore", "购物车里还有商品，确定切换？") == true then
				FitModeShopCenter_ConfirmStore()
		end
		
		FitModeShopCenter_HideRecycle()
end
-----------------------------------------
--确认切换仓库
-----------------------------------------
function FitModeShopCenter_ConfirmStore()
		--g_Loger:logDebug("FitModeShopCenter_ConfirmStore ")
		g_MainUIWindows:removeChildWindow(g_FitModeShopCenter)
		g_MainUIWindows:addChildWindow(g_FitmodeStorage)
		
		SetFitToStoreAttr()
		
		FitModeShopCenter_CancelSelecting()
		
		g_mainplayer:clearShoppingCart()
		FitModeShopCenter_CancelExit()
		total_count = 0
		FitModeShopCenter_recoverAllRoomComps()	-- (2011/7/27 add by hezhr)
end

-----------------------------------------
--打开购物车
-----------------------------------------
function FitModeShopCenter_OpenShopCenter()
		g_WinMgr:getWindow("FitmodeMktRoot"):setVisible(true)
		
		SetFitToMarketAttr()
end


-----------------------------------------
--退出
-----------------------------------------
function FitModeShopCenter_Exit()
		if FitModeShopCenter_ExitCheck("FitModeShopCenter_ConfirmExit", "购物车里还有商品，确定退出？") == true then
				FitModeShopCenter_ConfirmExit()
		end
		
		FitModeShopCenter_HideRecycle()
end

-----------------------------------------
--退出检查
-----------------------------------------
function FitModeShopCenter_ExitCheck(confirmFunc, confirmMsg)
		if total_count > 0 then
				g_MainUIWindows:addChildWindow(g_ExitWindow)
						
				EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit",confirmMsg)
				g_ExitWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,230), CEGUI.UDim(0,117)))
				g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-100), CEGUI.UDim(0.5,-30)))
				g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setSize(CEGUI.UVector2(CEGUI.UDim(0,200), CEGUI.UDim(0,30)))
			
				g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):removeEvent("Clicked")
				g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):subscribeEvent("Clicked", confirmFunc)
				g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):removeEvent("Clicked")
				g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):subscribeEvent("Clicked", "FitModeShopCenter_CancelExit")
				g_ExitWindow:activate()
				
				return false
		else
				return true
		end
end

-----------------------------------------
--确认退出
-----------------------------------------
function FitModeShopCenter_ConfirmExit()
		g_MainUIWindows:removeChildWindow(g_FitModeShopCenter)
		Fitmode_ClosedFitmodeStorage()
		
		g_mainplayer:clearShoppingCart()
		FitModeShopCenter_RestoreDayNight()

		FitModeShopCenter_CancelSelecting()
		
		--g_msgmgr:send(g_req_end_edit_house)
		--g_Loger:logMessage("g_req_end_edit_house on FitModeShopCenter_ConfirmExit")
		FitModeShopCenter_CancelExit()
		FitModeShopCenter_recoverAllRoomComps()	-- (2011/7/27 add by hezhr)
end

-----------------------------------------
--取消退出
-----------------------------------------
function FitModeShopCenter_CancelExit()
		g_MainUIWindows:removeChildWindow(g_ExitWindow)
end

-----------------------------------------
--手动日夜切换
-----------------------------------------
function FitModeShopCenter_ChangeDayNight()
		EQGlobaFunc.switchDayNight(true)
		
		if EQGlobaFunc.getDayNight() == true then
				g_WinMgr:getWindow("FitmodeUI/img1"):setProperty("Image", "set:Button5 image:NewImage349")
				g_WinMgr:getWindow("FitmodeUI1/img1"):setProperty("Image", "set:Button5 image:NewImage349")
		else
				g_WinMgr:getWindow("FitmodeUI/img1"):setProperty("Image", "set:Button5 image:NewImage351")
				g_WinMgr:getWindow("FitmodeUI1/img1"):setProperty("Image", "set:Button5 image:NewImage351")
		end
end

-----------------------------------------
--自动日夜切换
-----------------------------------------
function FitModeShopCenter_DayNightTimer(dn)
	g_Loger:logMessage("---dn----" ..tostring(dn))
		daynight = dn
end
-----------------------------------------
--还原日夜
-----------------------------------------
function FitModeShopCenter_RestoreDayNight()
	g_Loger:logMessage("-------" ..tostring(daynight))
	if not CreatePlayer_getFirstInflag() then
		g_Loger:logError("---daynight is ----")
		if daynight == nil then
			g_Loger:logError("---daynight is nil----")
		else
			EQGlobaFunc.changeDayNight(daynight, true)
		end
	elseif CreatePlayer_getFirstInflag() and not g_SceneMgr:inMyHouse() then
		EQGlobaFunc.changeDayNight(daynight, true)
	else
		EQGlobaFunc.changeDayNight(true, true)
	end
end
-----------------------------------------
--点击页
-----------------------------------------
function FitModeShopCenter_ClickPage(args)
		local winname = CEGUI.toWindowEventArgs(args).window:getName()
		page = tonumber(string.sub(winname, string.len("FitmodeUI/SelectTabctrl__auto_TabPane__Buttons__auto_btnTab ")+1, string.len(winname)))
		type1 = page + 1
		type2 = 1
		type3 = 0
		FitModeShopCenter_UpdateTypeIcon()
		g_mainplayer:setShopCenterType(0, 3, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER, 0)
		FitModeShopCenter_UpdateGoodsIcon()
		FitModeShopCenter_CancelSelecting()
end

-----------------------------------------
--设置选中状态
-----------------------------------------
function FitModeShopCenter_SelectType(winname)
	local slotWin = g_WinMgr:getWindow(winname)
	if slotWin == nil then
		--g_Loger:logDebug("FitModeShopCenter_SelectType winname = "..winname)
		return
	end
	
	local tempstr = slotWin:getProperty("PushedImage")
	slotWin:setProperty("NormalImage", tempstr)
	slotWin:setProperty("HoverImage", tempstr)
	--g_Loger:logDebug("ShopCenter_SelectType "..winname.." "..tempstr)
end

-----------------------------------------
--更新分类icon
-----------------------------------------
function FitModeShopCenter_UpdateTypeIcon()
		local iconrow
		if type3 == 0 then
				--第一层
				for i=1,9 do
						local slotWin = g_WinMgr:getWindow(type_slot..i)
						slotWin:setVisible(false)
						slotWin:setProperty("NormalImage", "set: image:")
						slotWin:setProperty("PushedImage", "set: image:")
						slotWin:setProperty("HoverImage", "set: image:")
			
						local iconrow = g_logictableMgr:getFitModeShopCenterIconRow(i+type1*10)
						if iconrow ~= nil then
								slotWin:setVisible(true)
								slotWin:setProperty("NormalImage", "set:" ..iconrow.imageset .." image:" ..iconrow.normal)
								slotWin:setProperty("PushedImage", "set:" ..iconrow.imageset .." image:" ..iconrow.push)
								slotWin:setProperty("HoverImage", "set:" ..iconrow.imageset .." image:" ..iconrow.hover)
								EQGlobaFunc.setTooltipProperty(slotWin:getName(), iconrow.tooltip)
						end
				end
		else
				--第二层
				for i=1,9 do
						local slotWin = g_WinMgr:getWindow(type_slot..i)
						slotWin:setVisible(false)
						slotWin:setProperty("NormalImage", "set: image:")
						slotWin:setProperty("PushedImage", "set: image:")
						slotWin:setProperty("HoverImage", "set: image:")
					
						local iconrow = g_logictableMgr:getFitModeShopCenterIconRow(i+type1*100+type2*10)
						if iconrow ~= nil then
								slotWin:setVisible(true)
								slotWin:setProperty("NormalImage", "set:" ..iconrow.imageset .." image:" ..iconrow.normal)
								slotWin:setProperty("PushedImage", "set:" ..iconrow.imageset .." image:" ..iconrow.push)
								slotWin:setProperty("HoverImage", "set:" ..iconrow.imageset .." image:" ..iconrow.hover)
								EQGlobaFunc.setTooltipProperty(slotWin:getName(), iconrow.tooltip)
						end
						
						g_WinMgr:getWindow("FitmodeMktRoot"):setVisible(true)
				end
		end
end

-----------------------------------------
--点击类型
-----------------------------------------
function FitModeShopCenter_ClickType(args)
		local winname = CEGUI.toWindowEventArgs(args).window:getName()
		local index = tonumber(string.sub(winname, string.len(type_slot)+1, string.len(winname)))
		
		if index == 9 then
				--第9个是返回按钮
				type3 = 0
				FitModeShopCenter_UpdateTypeIcon()
				g_mainplayer:setShopCenterType(0, 3, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER, 0)
				FitModeShopCenter_initWallFloorMode()	-- (2011/7/27 add by hezhr)
		else
				if type3 == 0 then
						type2 = index
						type3 = 1
						FitModeShopCenter_UpdateTypeIcon()
						FitModeShopCenter_SelectType(type_slot..type3)
				else
							type3 = index
							FitModeShopCenter_UpdateTypeIcon()
							FitModeShopCenter_SelectType(winname)
				end
				
				g_mainplayer:setShopCenterType(type1*100+type2*10+type3, 3, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER, 0)
				FitModeShopCenter_chooseWallFloorMode()	-- (2011/7/25 add by hezhr)
		end		
		
		FitModeShopCenter_UpdateGoodsIcon()
		goods_row = nil
		FitModeShopCenter_CancelSelecting()
end

-----------------------------------------
--更新商品icon
-----------------------------------------
function FitModeShopCenter_UpdateGoodsIcon()
		Shop_RemoveSelectBox()
		
		for i=1,24 do
				local slotWin = g_WinMgr:getWindow(goods_slot..i)
				if slotWin:getChildCount() > 0 then
						slotWin:removeChildWindow(slotWin:getChildAtIdx(0))
						slotWin:removeChildWindow(goods_slot..i.."/money")
				end
		end
			
		for i=1,24 do
				local itemid 	= g_mainplayer:getItemID(i-1,EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER)
				if itemid > 0 then
						local itemrow = g_itemtableMgr:getItemRow(itemid)	
						if itemrow ~= nil then
								StorageUI_DynamicLoadItem(goods_slot..i, "item", i, itemrow.imageset, itemrow.icon, "False")
								
								local row = g_mainplayer:getShopCenterRow(i-1, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER)
								if row ~= nil then
									local win = g_WinMgr:getWindow(goods_slot..i.."/money")
									if win == nil then
										win = g_WinMgr:createWindow("EQHomeLook/StaticImage", goods_slot..i.."/money")
										win:setProperty("MousePassThroughEnabled", "true")
									end
									g_WinMgr:getWindow(goods_slot..i):addChildWindow(win)
									if row.money_type == 1 then
										win:setProperty("Image", "set:Button5 image:NewImage444")
									else
										win:setProperty("Image", "set:Button5 image:NewImage443")
									end
								end
						end	
				end
		end
end

-----------------------------------------
--点击物品
-----------------------------------------
function FitModeShopCenter_ClickGoods(args)
		local winname = CEGUI.toWindowEventArgs(args).window:getName()
		local slotindex = tonumber(string.sub(winname, string.len(goods_slot)+1, string.len(winname)))
		
		local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER)
		if row ~= nil then
			goods_row = row
			g_WinMgr:getWindow("FitmodeMktRoot/Txt2"):setText(goods_row.price)
			if goods_row.money_type == 1 then
				g_WinMgr:getWindow("FitmodeMktRoot/Selling_img"):setProperty("Image", "set:Button5 image:NewImage428")
			else
				g_WinMgr:getWindow("FitmodeMktRoot/Selling_img"):setProperty("Image", "set:Button5 image:NewImage427")
			end

			select_slot = slotindex-1

			--播放抓取音效
			local itemRow = g_itemtableMgr:getItemRow(goods_row.item_id)
			if itemRow then
				EQGlobaClass.getVoiceMgr():play(itemRow.uvoiceid, false)
				--EQGlobaClass.getVoiceMgr():play(1, false)
				--g_Loger:logMessage("Item uvoiceid: " ..tostring(itemRow.uvoiceid))
				FitModeShopCenter_selectRoomMaterail(row.good_id, itemRow.subID)	-- (2011/7/22 add by hezhr)
			end
		end
end

function OnFitModeShopCenterPlaceFurniture(args)
	--播放使用音效
	if nil ~= goods_row then
		local ir = g_itemtableMgr:getItemRow(goods_row.item_id)
		
		if ir then
			EQGlobaClass.getVoiceMgr():play(ir.voiceid, false)
			--EQGlobaClass.getVoiceMgr():play(32, false)
			--g_Loger:logMessage("Item voiceid: " ..tostring(ir.voiceid))
		else
			--g_Loger:logError("ItemRow null, itemid:"..tostring(itemid))
		end
	end
end

-----------------------------------------
--上页
-----------------------------------------
function FitModeShopCenter_LastPage()
		g_mainplayer:setPackLastPage(EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER)
		FitModeShopCenter_UpdateGoodsIcon()
		goods_row = nil
end

-----------------------------------------
--下页
-----------------------------------------
function FitModeShopCenter_NextPage()
		g_mainplayer:setPackNextPage(EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER)
		FitModeShopCenter_UpdateGoodsIcon()
		goods_row = nil
end

-----------------------------------------
--购买(放入购物车)
-----------------------------------------
function FitModeShopCenter_Purchase()
		FitModeShopCenter_CartOpen()
end

-----------------------------------------
--购物车开启
-----------------------------------------
function FitModeShopCenter_CartOpen()
		if total_count > 0 then
			local scroll = CEGUI.toScrollbar(g_WinMgr:getWindow("Shoplistroot/Shoplist/VerticalScrollbar" ))
			scroll:setDocumentSize(g_mainplayer:getPackPageCount(EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART))
			scroll:setScrollPosition(0)
			scroll:setPageSize(1)
			scroll:setStepSize(1)
			EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt2", tostring(total_count).."件商品")
			EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt4", tostring(total_gold))
			EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt6", tostring(total_eq))
			
			FitModeShopCenter_CartUpdate()
			g_MainUIWindows:addChildWindow(g_FitModeShoppingCart)
		else
			MsgBox_createMsgBox(g_MainUIWindows,0,"","购物车内没商品.", "MsgBox_cancelFunc", "")
		end
end

-----------------------------------------
--购物车刷新
-----------------------------------------
function FitModeShopCenter_CartUpdate()
		Shop_RemoveSelectBox()
		
		for i=1,12 do
				local slotWin = g_WinMgr:getWindow(cart_slot..i)
				if slotWin:getChildCount() > 0 then
						slotWin:removeChildWindow(slotWin:getChildAtIdx(0))
				end
				g_WinMgr:getWindow(cart_slotname..i):setText("")
				g_WinMgr:getWindow(cart_slotprice..i):setText("")
				g_WinMgr:getWindow(cart_slotmoney..i):setProperty("Image", "set: image:")
		end
			
		for i=1,12 do
				local itemid 	= g_mainplayer:getItemID(i-1,EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART)
				if itemid > 0 then
						local itemrow = g_itemtableMgr:getItemRow(itemid)	
						local shoprow = g_mainplayer:getShopCenterRow(i-1, EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART)
						if itemrow ~= nil and shoprow ~= nil then
								local slotWin = g_WinMgr:getWindow(cart_slot..i)
								StorageUI_DynamicLoadItem(cart_slot..i, "item", i, itemrow.imageset, itemrow.icon, "False")
								EQGlobaFunc.SetText(cart_slotname..i, itemrow.name)
								g_WinMgr:getWindow(cart_slotprice..i):setText(shoprow.price)
								if shoprow.money_type == 1 then
									g_WinMgr:getWindow(cart_slotmoney..i):setProperty("Image", "set:Button5 image:NewImage428")
								else
									g_WinMgr:getWindow(cart_slotmoney..i):setProperty("Image", "set:Button5 image:NewImage427")
								end
						end	
				end
		end
end

-----------------------------------------
--购物车滚动条
-----------------------------------------
function FitModeShopCenter_CartScrollbar()
		local scroll = CEGUI.toScrollbar(g_WinMgr:getWindow("Shoplistroot/Shoplist/VerticalScrollbar" ))
		local pos = scroll:getScrollPosition()
		g_mainplayer:setPackCurPage(EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART, pos+1)
		FitModeShopCenter_CartUpdate()

		cart_index = 0
		
		--g_Loger:logDebug("FitModeShopCenter_CartScrollbar "..pos)
end

-----------------------------------------
--购物车点击
-----------------------------------------
function FitModeShopCenter_CartSel(args)
		local winname = CEGUI.toWindowEventArgs(args).window:getName()
		local slotindex = tonumber(string.sub(winname, string.len(cart_slot)+1, string.len(winname)))
		
		local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART)
		if row ~= nil then
				cart_index = slotindex-1
		end
end

-----------------------------------------
--购物车删除
-----------------------------------------
function FitModeShopCenter_CartDel()
	local row = g_mainplayer:getShopCenterRow(cart_index, EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART)
	if row ~= nil then
		local res = g_mainplayer:delShoppingCart(cart_index)
		if 0 == res then
			return
		end
		FitModeShopCenter_recoverByCartNum(res)	-- (2011/7/27 add by hezhr)
		FitModeShopCenter_CartUpdate()
		
		total_count = total_count - 1
		total_gold = g_mainplayer:getShoppingCartPrice(1)
		total_eq = g_mainplayer:getShoppingCartPrice(2)
		EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt2", tostring(total_count).."件商品")
		EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt4", tostring(total_gold))
		EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt6", tostring(total_eq))
		
		FitModeShopCenter_HideRecycle()
	end
end

-----------------------------------------
--购物车关闭
-----------------------------------------
function FitModeShopCenter_CartClose()
		g_MainUIWindows:removeChildWindow(g_FitModeShoppingCart)
end

-----------------------------------------
--购物车购买
-----------------------------------------
function FitModeShopCenter_CartPay()
		if total_count > 0 then
				MsgBox_ShowBuyMultiConfirm(total_eq, total_gold, "FitModeShopCenter_ConfirmBuy")
		else
				MsgBox_createMsgBox(g_MainUIWindows,0,"","购物车内没商品.", "MsgBox_cancelFunc", "")
		end
end

-----------------------------------------
--确认购买
-----------------------------------------
function FitModeShopCenter_ConfirmBuy()
	--发购买包
	g_mainplayer:sendShoppingCart()
	FitModeShopCenter_CartClose()
end

-----------------------------------------
--取消购买
-----------------------------------------
function FitModeShopCenter_CancelBuy()
	g_MainUIWindows:removeChildWindow(g_ExitWindow)
end

-----------------------------------------
--购买成功
-----------------------------------------
function FitModeShopCenter_BuySucess()
	total_count = 0
	total_eq = 0
	total_gold = 0
	FitModeShopCenter_resetChangeMaterialData()	-- (2011/7/29 add by hezhr)
end

-----------------------------------------
--家具UI拖动到主界面
-----------------------------------------
function FitModeShopCenter_IsSelecting()
		if select_slot == -1 then
				return false
		else
				return true
		end
end

function FitModeShopCenter_CancelSelecting()
	select_slot = -1
end

-----------------------------------------
--家具UI拖动到主界面
-----------------------------------------
function FitEnterMainUI()
		if PutFunitureFlag == false then
				return
		end

		if select_slot == -1 then
				return
		end
		
		if EQGlobaFunc.getPickState() == 2 then
			return
		end

		local itemid 	= g_mainplayer:getItemID(select_slot, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER)
		if itemid > 0 then
				local itemrow = g_itemtableMgr:getItemRow(itemid)
				if itemrow ~= nil and itemrow.type == 2 then			
						EQGlobaFunc.OnMouseFitmentDro(itemrow.subID)
				end
		end
end

-----------------------------------------
--家具UI拖动离开主界面
-----------------------------------------
function FitLeaveMainUI()
		if PutFunitureFlag == false then
				return
		end

		if select_slot == -1 then
				return
		end
		
		if EQGlobaFunc.getPickState() == 2 then
			return
		end
		
		EQGlobaFunc.OnMouseFitmentDro(-1)
end

-----------------------------------------
--添加商品到购物车
-----------------------------------------
function FitModeShopCenter_AddShoppingCart(uid)
		if goods_row ~= nil then
			g_mainplayer:addShoppingCart(goods_row, uid)
			FitModeShopCenter_CartUpdate()
			
			total_count = total_count + 1
			total_gold = g_mainplayer:getShoppingCartPrice(1)
			total_eq = g_mainplayer:getShoppingCartPrice(2)
			EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt2", tostring(total_count).."件商品")
			EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt4", tostring(total_gold))
			EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt6", tostring(total_eq))
			
			FitModeShopCenter_CancelSelecting()
		end
end

-----------------------------------------
--删除购物车商品
-----------------------------------------
function FitModeShopCenter_DelShoppingCart(uid)
	local res = g_mainplayer:delShoppingCartByData(uid)
	if 0 ~= res then
		FitModeShopCenter_recoverByCartNum(res)	-- (2011/7/27 add by hezhr)
		FitModeShopCenter_CartUpdate()
		
		local scroll = CEGUI.toScrollbar(g_WinMgr:getWindow("Shoplistroot/Shoplist/VerticalScrollbar" ))
		scroll:setDocumentSize(g_mainplayer:getPackPageCount(EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART))
		
		total_count = total_count - 1
		total_gold = g_mainplayer:getShoppingCartPrice(1)
		total_eq = g_mainplayer:getShoppingCartPrice(2)
		EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt2", tostring(total_count).."件商品")
		EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt4", tostring(total_gold))
		EQGlobaFunc.SetText("Shoplistroot/Shoplist/txt6", tostring(total_eq))
		
		FitModeShopCenter_HideRecycle()
	end
end

-----------------------------------------
--购物车商品标记
-----------------------------------------
function FitModeShopCenter_Mark()
		show_mark = CEGUI.toCheckbox(g_WinMgr:getWindow("FitmodeMktRoot/checkbox")):isSelected()
		g_sceneMgr:getHouse():setTempMark(show_mark)
		
		if show_mark == true then
				g_sceneMgr:getHouse():showAllTempMark()
		else
				g_sceneMgr:getHouse():hideAllTempMark()
		end				
end

-----------------------------------------
--点击回收
-----------------------------------------
function FitModeShopCenter_ClickRecycle()
		EQGlobaFunc.OnMouseFitmentDro(-1)
		FitModeShopCenter_HideRecycle()
end

-----------------------------------------
--点击左旋
-----------------------------------------
function FitModeShopCenter_ClickLeftRotate()
		EQGlobaFunc.OnMouseFitmentRotate(90)
end

-----------------------------------------
--点击右旋
-----------------------------------------
function FitModeShopCenter_ClickRightRotate()
		EQGlobaFunc.OnMouseFitmentRotate(-90)
end

-----------------------------------------
--点击拿起
-----------------------------------------
function FitModeShopCenter_ClickPickup()
		EQGlobaFunc.OnMouseFitmentPickup()
end

-----------------------------------------
--点击放下
-----------------------------------------
function FitModeShopCenter_ClickPutdown()
		EQGlobaFunc.OnMouseFitmentPutdown()
end

-----------------------------------------
--弹出按钮回调
-----------------------------------------
function FitModeShopCenter_EnableButton(winname)
	g_WinMgr:getWindow(winname):enable()
	playing_ani = false
end

-----------------------------------------
--弹出回收按钮
-----------------------------------------
function FitModeShopCenter_ShowRecycle(x, y)
		local dx = CEGUI.UDim(0,x-16)
		local dy = CEGUI.UDim(0,y-16)
		recycle_button:disable()
		recycle_button:setVisible(true)
		recycle_button:setPosition(CEGUI.UVector2(dx, dy))
		
		local oy = 50
		if y > 50 then
			oy = -50
		end
		
		g_uiAnimationInfo.luaPlayEnd = "FitModeShopCenter_EnableButton"
		g_uiAnimationInfo.type = EQGlobaClass.UIAnimationInfo.EUIA_MOVE
		g_uiAnimationInfo.delayTime = 0
		g_uiAnimationInfo.playTime = 100
		g_uiAnimationInfo.startOffsetX = 0
		g_uiAnimationInfo.startOffsetY = 0
		g_uiAnimationInfo.endOffsetX = 0
		g_uiAnimationInfo.endOffsetY = oy
		EQGlobaFunc.playUIAnimation(recycle_button:getName(), g_uiAnimationInfo)
		
		playing_ani = true
end

-----------------------------------------
--隐藏回收按钮
-----------------------------------------
function FitModeShopCenter_HideRecycle()
		recycle_button:setVisible(false)
end

-----------------------------------------
--弹出旋转按钮
-----------------------------------------
function FitModeShopCenter_ShowRotate(x, y)
		local dx = CEGUI.UDim(0,x-16)
		local dy = CEGUI.UDim(0,y-16)
		pickup_button:disable()
		pickup_button:setVisible(true)
		pickup_button:setPosition(CEGUI.UVector2(dx, dy))
		leftrotate_button:disable()
		leftrotate_button:setVisible(true)
		leftrotate_button:setPosition(CEGUI.UVector2(dx, dy))
		rightrotate_button:disable()
		rightrotate_button:setVisible(true)
		rightrotate_button:setPosition(CEGUI.UVector2(dx, dy))
		putdown_button:disable()
		putdown_button:setVisible(true)
		putdown_button:setPosition(CEGUI.UVector2(dx, dy))
		
		g_uiAnimationInfo.luaPlayEnd = "FitModeShopCenter_EnableButton"
		g_uiAnimationInfo.type = EQGlobaClass.UIAnimationInfo.EUIA_MOVE
		g_uiAnimationInfo.delayTime = 0
		g_uiAnimationInfo.playTime = 100
		g_uiAnimationInfo.startOffsetX = 0
		g_uiAnimationInfo.startOffsetY = 0
		g_uiAnimationInfo.endOffsetX = 0
		g_uiAnimationInfo.endOffsetY = -50
		EQGlobaFunc.playUIAnimation(pickup_button:getName(), g_uiAnimationInfo)
		
		g_uiAnimationInfo.luaPlayEnd = "FitModeShopCenter_EnableButton"
		g_uiAnimationInfo.type = EQGlobaClass.UIAnimationInfo.EUIA_MOVE
		g_uiAnimationInfo.delayTime = 0
		g_uiAnimationInfo.playTime = 100
		g_uiAnimationInfo.startOffsetX = 0
		g_uiAnimationInfo.startOffsetY = 0
		g_uiAnimationInfo.endOffsetX = -50
		g_uiAnimationInfo.endOffsetY = 0
		EQGlobaFunc.playUIAnimation(leftrotate_button:getName(), g_uiAnimationInfo)
		
		g_uiAnimationInfo.luaPlayEnd = "FitModeShopCenter_EnableButton"
		g_uiAnimationInfo.type = EQGlobaClass.UIAnimationInfo.EUIA_MOVE
		g_uiAnimationInfo.delayTime = 0
		g_uiAnimationInfo.playTime = 100
		g_uiAnimationInfo.startOffsetX = 0
		g_uiAnimationInfo.startOffsetY = 0
		g_uiAnimationInfo.endOffsetX = 50
		g_uiAnimationInfo.endOffsetY = 0
		EQGlobaFunc.playUIAnimation(rightrotate_button:getName(), g_uiAnimationInfo)
		
		g_uiAnimationInfo.luaPlayEnd = "FitModeShopCenter_EnableButton"
		g_uiAnimationInfo.type = EQGlobaClass.UIAnimationInfo.EUIA_MOVE
		g_uiAnimationInfo.delayTime = 0
		g_uiAnimationInfo.playTime = 100
		g_uiAnimationInfo.startOffsetX = 0
		g_uiAnimationInfo.startOffsetY = 0
		g_uiAnimationInfo.endOffsetX = 0
		g_uiAnimationInfo.endOffsetY = 50
		EQGlobaFunc.playUIAnimation(putdown_button:getName(), g_uiAnimationInfo)
		
		playing_ani = true
end

-----------------------------------------
--隐藏旋转按钮
-----------------------------------------
function FitModeShopCenter_HideRotate()
		pickup_button:setVisible(false)
		leftrotate_button:setVisible(false)
		rightrotate_button:setVisible(false)
		putdown_button:setVisible(false)
end

-----------------------------------------
--弹出旋转按钮
-----------------------------------------
function FitModeShopCenter_SetRotatePos(x, y)
	if playing_ani == true then
		return
	end

	local dx = CEGUI.UDim(0,x-16)
	local dy = CEGUI.UDim(0,y-16)
	local offset = CEGUI.UDim(0,50)
	
	pickup_button:setPosition(CEGUI.UVector2(dx, dy-offset))
	leftrotate_button:setPosition(CEGUI.UVector2(dx-offset, dy))
	rightrotate_button:setPosition(CEGUI.UVector2(dx+offset, dy))
	putdown_button:setPosition(CEGUI.UVector2(dx, dy+offset))
end

-----------------------------------------
--显示itemhint
-----------------------------------------
function FitModeShopCenter_GoodsMouseEnter(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(goods_slot)+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_FITMODE_SHOPCENTER)
	if nil ~= row then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = ""
		local itemrow = g_itemtableMgr:getItemRow(row.item_id)
		if 1 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
		elseif 2 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
		end
		createItemhint_shop(row.item_id, rect, g_MainUIWindows, effectTime, 0, row)
	end
end

function FitModeShopCenter_CartMouseEnter(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(cart_slot)+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_FITMODE_SHOPPINGCART)
	if nil ~= row then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = ""
		local itemrow = g_itemtableMgr:getItemRow(row.item_id)
		if 1 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
		elseif 2 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
		end
		createItemhint_shop(row.item_id, rect, g_MainUIWindows, effectTime, 0, row)
	end
end

-----------------------------------------
--隐藏itemhint
-----------------------------------------
function FitModeShopCenter_GoodsMouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end

function FitModeShopCenter_CartMouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
-----------------------------------------
-- 局部变量(add by hezhr)
-----------------------------------------
local m_furIntroVec = EQGlobaClass.createDynamicCtrlVector()	-- 家具介绍面板容器
local m_screenW = g_GuiSystem:getRenderer():getRect():getWidth()
local m_screenH = g_GuiSystem:getRenderer():getRect():getHeight()
local m_showEffectTimeFlag = -1	-- -1.不显示时效;其他值则显示
-----------------------------------------
--生成家具介绍界面(add by hezhr)
-----------------------------------------
function FitModeShopCenter_introUI()
	-- 主界面
	local frameUIName = "Furniture_introduction/frame"
	local frameUI = g_WinMgr:getWindow(frameUIName)
	if nil == frameUI then
		frameUI = g_WinMgr:createWindow("NewEQHomeLook/NewFrameWindow3", frameUIName)
		frameUI:setWidth(CEGUI.UDim(0, 252))
		EQGlobaFunc.SetText(frameUIName, "家具资料")
		g_WinMgr:getWindow(frameUIName.."__auto_closebutton__"):subscribeEvent("Clicked", "FitModeShopCenter_clickCloseBtn")		
	end
	-- 介绍界面
	local introUIName = "Furniture_introduction/frame/intro"
	local introUI = g_WinMgr:getWindow(introUIName)
	if nil == introUI then
		introUI = g_WinMgr:createWindow("EQHomeLook/StaticImage", introUIName)
		introUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 1), CEGUI.UDim(0, 15)))
		introUI:setWidth(CEGUI.UDim(0, 250))	-- 控件18个汉字宽度
		m_furIntroVec:initialise(introUIName)
		frameUI:addChildWindow(introUI)
	end
	-- 家具名
	local furniName = "Furniture_introduction/frame/intro/name"
	local furniWin = g_WinMgr:getWindow(furniName)
	if nil == furniWin then
		furniWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", furniName)
		FitModeShopCenter_setIntroUIWin(furniName, "#FF9C00", "#000000")
	end
	-- 图标
	local iconName = "Furniture_introduction/frame/intro/icon"
	local iconWin = g_WinMgr:getWindow(iconName)
	if nil == iconWin then
		iconWin = g_WinMgr:createWindow("EQHomeLook/StaticImage6", iconName)
		iconWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, 32), CEGUI.UDim(0, 32)))
	end
	-- 类型
	local typeName = "Furniture_introduction/frame/intro/type"
	local typeWin = g_WinMgr:getWindow(typeName)
	if nil == typeWin then
		typeWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", typeName)
		FitModeShopCenter_setIntroUIWin(typeName, "#FFFFFF", "#045098")
	end
	-- 状态
	local stateName = "Furniture_introduction/frame/intro/state"
	local stateWin = g_WinMgr:getWindow(stateName)
	if nil == stateWin then
		stateWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", stateName)
		FitModeShopCenter_setIntroUIWin(stateName, "#FFFFFF", "#045098")
	end
	-- 描述
	local describeName = "Furniture_introduction/frame/intro/describe"
	local describeWin = g_WinMgr:getWindow(describeName)
	if nil == describeWin then
		describeWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", describeName)
		FitModeShopCenter_setIntroUIWin(describeName, "#FFFFFF", "#045098")			
	end
	-- 时效
	local effTimeName = "Furniture_introduction/frame/intro/effTime"
	local effTimeWin = g_WinMgr:getWindow(effTimeName)
	if nil == effTimeWin then
		effTimeWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", effTimeName)
		FitModeShopCenter_setIntroUIWin(effTimeName, "#FFFFFF", "#045098")
	end
	return frameUIName, introUIName, furniName, iconName, typeName, stateName, describeName, effTimeName
end
-----------------------------------------
--设置家具介绍界面控件属性(add by hezhr)
-----------------------------------------
function FitModeShopCenter_setIntroUIWin(winname, cols, side)
	EQGlobaFunc.toMultiLineBox(winname):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(winname):setCols(cols)
	EQGlobaFunc.toMultiLineBox(winname):setSide(side, 1)
	EQGlobaFunc.toMultiLineBox(winname):setAlign("AT_CENTER_VER")
end
-----------------------------------------
--设置家具介绍界面文字(add by hezhr)
-----------------------------------------
function FitModeShopCenter_setIntroUIText(winname, text, needParse)
	EQGlobaFunc.toMultiLineBox(winname):clearUp()
	-- 添加文字
	if needParse then	-- 说明控件
		EQGlobaFunc.toMultiLineBox(winname):setWidth(240)	-- 18个汉字宽度
		g_mlbParse:getData(text, winname)
	else	-- 非说明控件
		local width = EQGlobaFunc.toMultiLineBox(winname):getFitWidth("simsun-10", text)
		g_WinMgr:getWindow(winname):setWidth(CEGUI.UDim(0, width))
		EQGlobaFunc.toMultiLineBox(winname):addElementText(text, false)
		EQGlobaFunc.toMultiLineBox(winname):setAlign("AT_CENTER_HOR")
	end
	-- 设置控件高度
	local height = EQGlobaFunc.toMultiLineBox(winname):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(winname):setHeight(height)	
end
-----------------------------------------
--家具介绍界面控件排版(add by hezhr)
-----------------------------------------
function FitModeShopCenter_typesetIntroUIWin()
	local frame, intro, name, icon, typ, state, describe, effTime = FitModeShopCenter_introUI()
	-- 添加控件
	m_furIntroVec:setAlign("AT_CENTER")
	m_furIntroVec:setVerInterval(10)
	m_furIntroVec:addControl(name)		-- 家具名称
	m_furIntroVec:setHorInterval(10)
	m_furIntroVec:addControl(icon)		-- 家具icon
	m_furIntroVec:setAtRight()
	m_furIntroVec:addControl(typ)		-- 家具类型
	m_furIntroVec:setHorInterval(48)
	m_furIntroVec:setVerInterval(-13)
	m_furIntroVec:addControl(state)		-- 家具状态
	m_furIntroVec:addControl(describe)	-- 家具说明
	if -1 ~= m_showEffectTimeFlag then
		m_furIntroVec:addControl(effTime)	-- 家具时效
	end
	-- 排版
	m_furIntroVec:controlTypeset()
end
-----------------------------------------
--打开家具介绍界面(add by hezhr)
-----------------------------------------
function FitModeShopCenter_openIntroUI(id, rect, delTime)
	-- 获取商城物品信息
	local itemrow = g_itemtableMgr:getItemRow(id)
	if nil == itemrow then
		return
	end
	m_furIntroVec:removeAllControl()
	-- 获取控件
	local frame, intro, name, icon, typ, state, describe, effTime = FitModeShopCenter_introUI()
	-- 家具名称
	FitModeShopCenter_setIntroUIText(name, itemrow.name, false)
	-- 家具icon
	g_WinMgr:getWindow(icon):setProperty("Image", "set:" ..itemrow.imageset .." image:" ..itemrow.icon)
	-- 家具类型
	FitModeShopCenter_setIntroUIText(typ, "家具", false)
	-- 家具状态
	local days = checkEffectTime(itemrow.effectTimeID, delTime, g_WinMgr:getWindow(effTime))
	m_showEffectTimeFlag = days
	FitModeShopCenter_setIntroUIText(state, checkItemhintState(itemrow.bind, days, 1), false)
	-- 家具描述
	local introrow = g_logictableMgr:getGoodsDescribeRow(itemrow.tooltip)
	if nil == introrow then
		FitModeShopCenter_setIntroUIText(describe, "", true)
	else
		FitModeShopCenter_setIntroUIText(describe, introrow.value, true)
	end
	-- 控件排版
	FitModeShopCenter_typesetIntroUIWin()
	-- 设置界面高度
	local pos = {x, y}
	pos = g_WinMgr:getWindow(intro):getPosition()
	yPos = pos.y:asAbsolute(m_screenH)	
	local height = g_WinMgr:getWindow(intro):getHeight():asAbsolute(m_screenH)
	g_WinMgr:getWindow(frame):setHeight(CEGUI.UDim(0, yPos + height))
	-- 设置界面位置
	local xPos, yPos = checkItemhintPos(rect, g_MainUIWindows, g_WinMgr:getWindow(frame))
	g_WinMgr:getWindow(frame):setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos), CEGUI.UDim(0, yPos)))		
	g_MainUIWindows:addChildWindow(frame)	
end
-----------------------------------------
--点击关闭家具介绍界面按钮(add by hezhr)
-----------------------------------------
function FitModeShopCenter_clickCloseBtn(args)
	local closeBtn = CEGUI.toWindowEventArgs(args).window
	FitModeShopCenter_closeIntroUI()
end
-----------------------------------------
--关闭家具介绍界面(add by hezhr)
-----------------------------------------
function FitModeShopCenter_closeIntroUI()
	local frame = FitModeShopCenter_introUI()
	g_MainUIWindows:removeChildWindow(frame)
	m_furIntroVec:removeAllControl()	
end
-----------------------------------------
-- 更换墙壁地板(2011/7/21 add by hezhr)
-----------------------------------------
local m_isChangeWallMode = false
local m_isChangeFloorMode = false
local m_roomMaterialTable = {}	-- 保存家装模式下有替换的房屋材质
local m_curGoodsID = 0	-- 当前材质id,对应到Item_tplt.xml
local m_curMaterialID = 0	-- 当前材质id,对应到change_wall_floor_mat.xml
local m_materialCartNum = 100000	-- 材质在购物车里的编号,初始这么大的值是为了避免重复
local m_materialCartNumTable = {}	-- 保存材质在购物车里的编号与材质id的对应关系表
local m_floorId = 0
local m_roomId = 0
local m_compId = 0
local m_tex = ""
-----------------------------------------
-- 初始模式(2011/7/27 add by hezhr)
function FitModeShopCenter_initWallFloorMode()
	m_isChangeFloorMode = false
	m_isChangeWallMode = false
	m_curGoodsID = 0
	m_curMaterialID = 0
end
-----------------------------------------
-- 监听更换墙壁地板模式(2011/7/21 add by hezhr)
function FitModeShopCenter_listenWallFloorMode()
	-- 不在模式下
	if g_MainUIWindows:isChild(g_FitModeShopCenter) and 4==type1 then
		if 1==type2 or 2==type2 then
			FitModeShopCenter_chooseWallFloorMode()
			return
		end
	end
	FitModeShopCenter_initWallFloorMode()
end
-----------------------------------------
-- 选择模式(2011/7/27 add by hezhr)
function FitModeShopCenter_chooseWallFloorMode()
	if 4 ~= type1 then	-- 显示第三页
		FitModeShopCenter_initWallFloorMode()
		return
	end
	if 1 == type2 then	-- 地板
		m_isChangeFloorMode = true
		m_isChangeWallMode = false
	end
	if 2 == type2 then	-- 墙壁
		m_isChangeFloorMode = false
		m_isChangeWallMode = true
	end
end
-----------------------------------------
-- 选择了房屋部件材料(2011/7/21 add by hezhr)
function FitModeShopCenter_selectRoomMaterail(goods_id, material_id)
	if m_isChangeFloorMode or m_isChangeWallMode then
		m_curGoodsID = goods_id
		m_curMaterialID = material_id
	end
end
-----------------------------------------
-- 设置鼠标状态(2011/7/21 add by hezhr)
function FitModeShopCenter_setChangeWallFloorCursorState()
	if 0 == m_curMaterialID then
		return false
	end
	if m_isChangeFloorMode then
		g_CursorMgr:setState(g_CS_CHANGE_FLOOR)
		return true
	end
	if m_isChangeWallMode then
		g_CursorMgr:setState(g_CS_CHANGE_WALL)
		return true
	end
	return false
end
-----------------------------------------
-- 是否可以更换房屋部件材料(2011/7/27 add by hezhr)
function FitModeShopCenter_canChangeRoomMaterial(comp)
	if m_isChangeFloorMode and 2==comp then	-- 更换地板,点击是地板
		return true
	end
	if m_isChangeWallMode then -- 更换墙纸,点击的是墙
		if 3==comp or 4==comp then
			return true
		end
	end
	return false
end
-----------------------------------------
-- 更换房屋部件材料(2011/7/21 add by hezhr)
function FitModeShopCenter_changeRoomMaterial(floor, room, comp)
	--g_Loger:logMessage("--- floor: "..floor.." room: "..room.." comp: "..comp.." materialID: "..m_curMaterialID.." ---")
	
	if room == 0 then -- 室外墙壁地板不更换
		return
	end
	
	if false == FitModeShopCenter_canChangeRoomMaterial(comp) then	-- 不能更换材料
		return
	end
	local row = g_logictableMgr:getChangeWallFloorMaterialRow(m_curMaterialID)
	m_curMaterialID = 0
	if nil == row then
		return
	end
	-- g_Loger:logMessage("--- materail: "..row.material.." ---")
	local res = FitModelShowCenter_saveChangeRoomComp(floor, room, comp, row.material, m_curGoodsID)
	if 0 == res then	-- 什么都不做
		return
	end
	if 1 == res then	-- 添加到购物车,更换新的材料
		FitModeShopCenter_saveRoomCompCartNum(floor, room, comp, m_materialCartNum)
		FitModeShopCenter_AddShoppingCart(m_materialCartNum)
		g_sceneMgr:getHouse():changeRoomMaterial(floor, room, comp, row.material)
	elseif 2 == res then	-- 从购物车删除,恢复为旧的材料
		local num = FitModeShopCenter_getRoomCompCartNum(floor, room, comp)
		if nil ~= num then
			FitModeShopCenter_DelShoppingCart(num)
		end
		FitModeShopCenter_recoverRoomComp(floor, room, comp)
	elseif 3 == res then	-- 先从购物车删除,再添加到购物车,更换新的材料
		local num = FitModeShopCenter_getRoomCompCartNum(floor, room, comp)
		if nil ~= num then
			FitModeShopCenter_DelShoppingCart(num)
		end
		FitModelShowCenter_saveChangeRoomComp(floor, room, comp, row.material, m_curGoodsID)
		FitModeShopCenter_saveRoomCompCartNum(floor, room, comp, m_materialCartNum)
		FitModeShopCenter_AddShoppingCart(m_materialCartNum)
		g_sceneMgr:getHouse():changeRoomMaterial(floor, room, comp, row.material)
	end
	m_materialCartNum = m_materialCartNum + 1
	g_CursorMgr:setState(g_CS_NORMAL)
end
-----------------------------------------
-- 获取已更改的房间部件(2011/7/26 add by hezhr)
function FitModeShopCenter_getChangeRoomComp(floor, room, comp)
	for i, v in pairs(m_roomMaterialTable) do
		if floor==v.floor and room==v.room and comp==v.comp then
			return i	-- 有找到,返回在表中的id
		end
	end
	return 0	-- 未找到
end
-----------------------------------------
-- 保存已更改的房间部件(2011/7/25 add by hezhr)
function FitModelShowCenter_saveChangeRoomComp(floor, room, comp, new, goods_id)
	local i = FitModeShopCenter_getChangeRoomComp(floor, room, comp)
	-- 已保存
	if 0 ~= i then
		if new == m_roomMaterialTable[i].old then	-- 恢复到旧的材料
			return 2	-- 从购物车删除,恢复为旧的材料
		end
		if new ~= m_roomMaterialTable[i].new then	-- 使用新的材料
			m_roomMaterialTable[i].new = new
			return 3	-- 先从购物车删除,再添加到购物车,更换新的材料
		end
		return 0	-- 什么都不做
	end
	-- 未保存
	local mat = {}
	mat["floor"] = floor	-- 楼层id
	mat["room"] = room	-- 房屋id
	mat["comp"] = comp	-- 房屋部件类型
	mat["old"] = g_sceneMgr:getHouse():getRoomMaterial(floor, room, comp)	-- 原始材料
	mat["new"] = new	-- 新的材料
	mat["goods_id"] = goods_id	-- 新材料对应的物品id
	-- 检查材质是否一样
	if mat["old"] == new then
		return 0	-- 什么都不做
	end
	table.insert(m_roomMaterialTable, mat)
	return 1	-- 添加到购物车,更换新的材料
end
-----------------------------------------
-- 恢复房屋部件(2011/7/26 add by hezhr)
function FitModeShopCenter_recoverRoomComp(floor, room, comp)
	for i, v in pairs(m_roomMaterialTable) do
		if floor==v.floor and room==v.room and comp==v.comp then
			g_sceneMgr:getHouse():changeRoomMaterial(floor, room, comp, v.old)
			table.remove(m_roomMaterialTable, i)
			return
		end
	end
end
-----------------------------------------
-- 恢复所有房屋部件(2011/7/26 add by hezhr)
function FitModeShopCenter_recoverAllRoomComps()
	for i, v in pairs(m_roomMaterialTable) do
		g_sceneMgr:getHouse():changeRoomMaterial(v.floor, v.room, v.comp, v.old)
	end
	FitModeShopCenter_resetChangeMaterialData()
end
-----------------------------------------
-- 保存房屋部件对应到购物车的编号(2011/7/26 add by hezhr)
function FitModeShopCenter_saveRoomCompCartNum(floor, room, comp, num)
	for i, v in pairs(m_materialCartNumTable) do
		if floor==v.floor and room==v.room and comp==v.comp then
			v.num = num
			return
		end
	end
	local row = {}
	row["floor"] = floor
	row["room"] = room
	row["comp"] = comp
	row["num"] = num
	table.insert(m_materialCartNumTable, row)
end
-----------------------------------------
-- 获取房屋部件对应到购物车的编号(2011/7/26 add by hezhr)
function FitModeShopCenter_getRoomCompCartNum(floor, room, comp)
	for i, v in pairs(m_materialCartNumTable) do
		if floor==v.floor and room==v.room and comp==v.comp then
			return v.num
		end
	end
	return nil
end
-----------------------------------------
-- 房屋部件对应到购物车的编号恢复部件(2011/7/26 add by hezhr)
function FitModeShopCenter_recoverByCartNum(num)
	for i, v in pairs(m_materialCartNumTable) do
		if num == v.num then
			FitModeShopCenter_recoverRoomComp(v.floor, v.room, v.comp)
		end
	end
end
-----------------------------------------
-- 重置数据(2011/7/26 add by hezhr)
function FitModeShopCenter_resetChangeMaterialData()
	m_roomMaterialTable = {}
	m_materialCartNumTable = {}
	m_floorId, m_roomId, m_compId, m_tex = 0, 0, 0, ""
end
-----------------------------------------
-- 根据购物车里的id获取数据(2011/9/21 add by hezhr)
function FitModeShopCenter_hasDataByCartNum(num)
	for i, v in pairs(m_materialCartNumTable) do
		if num == v.num then
			for j, w in pairs(m_roomMaterialTable) do
				if w.floor==v.floor and w.room==v.room and w.comp==v.comp then
					m_floorId, m_roomId, m_compId, m_tex = w.floor, w.room, w.comp, w.new
					return true
				end
			end
			break
		end
	end
	return false
end
-----------------------------------------
-- 获取楼层编号(2011/9/21 add by hezhr)
function FitModeShopCenter_getFloorIdByCartNum(num)
	return m_floorId
end
-----------------------------------------
-- 获取房屋编号(2011/9/21 add by hezhr)
function FitModeShopCenter_getRoomIdByCartNum(num)
	return m_roomId
end
-----------------------------------------
-- 获取部件类型(2011/9/21 add by hezhr)
function FitModeShopCenter_getCompByCartNum(num)
	return m_compId
end
-----------------------------------------
-- 获取材质(2011/9/21 add by hezhr)
function FitModeShopCenter_getTexByCartNum(num)
	return m_tex
end
-----------------------------------------