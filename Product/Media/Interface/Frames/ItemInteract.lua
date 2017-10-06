--乐园和农场物品交互
-----------------------------------------------------------------------
--
g_isOpenIITUI = false

--
local mStartUseFlag = true
--
local mSlotsCount = 30
local mTotalpage = 1
local interactState = 0  -- 0代表不在交互状态，1代表正在交互状态
local removeMode = 0	--0为初始化值，无意义；1代表从农场到背包；2代表从背包的农场
-----------------------------------------------------------------------

----------------------------------------------------------
--
function IIT_onLoadInit()
	IIT_subscribeEvent()
	IIT_windowInit()
end
----------------------------------------------------------
--
function IIT_windowInit()
	MainUI_hideMFFrame(g_ItemInteractUIName.root)
	Common_injectEmptyEvt(g_ItemInteractUIName.root)
	MainUI_ShieldClick(g_ItemInteractUI)
	
	Common_injectEmptyEvt("store_money")
	MainUI_ShieldClick(g_storageInputMoneyUI)
end
----------------------------------------------------------
--
function IIT_subscribeEvent()
	g_GameEventMgr:subscribeEvent("UPDATE_IITDATA_UI","IIT_notifyIITData")
	g_GameEventMgr:subscribeEvent("NOTIFY_FARM_ISACTIVE","IIT_reqOpenIITUI")
	g_GameEventMgr:subscribeEvent("NOTIFY_FARM_NOACTIVE","IIT_notifyFarmNoActive")
	
	g_WinMgr:getWindow(g_ItemInteractUIName.closeBtn):subscribeEvent("Clicked","IIT_closeIITUI")
	--
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/jiaohu"):subscribeEvent("Clicked","IIT_reqStartInteract")
	--
	g_WinMgr:getWindow(g_ItemInteractUIName.prePageBtn):subscribeEvent("Clicked","IIT_prePage")
	g_WinMgr:getWindow(g_ItemInteractUIName.nextPageBtn):subscribeEvent("Clicked","IIT_nextPage")
	g_WinMgr:getWindow(g_ItemInteractUIName.moneyImgBtn):subscribeEvent("MouseClick","IIT_openRemoveMoneyUI")
	IIT_subscribeSlotEvent()
end
----------------------------------------------------------
--
function IIT_subscribeSlotEvent()
	local slotBaseName = g_ItemInteractUIName.slotBaseName
	for idx = 1,mSlotsCount do
		local slotWin = g_WinMgr:getWindow(g_ItemInteractUIName.slotBaseName..tostring(idx))
		slotWin:subscribeEvent("MouseClick", "IIT_onClickSlot")
		slotWin:removeEvent("MouseEnter")
		slotWin:removeEvent("MouseLeave")
		slotWin:subscribeEvent("MouseEnter", "IIT_onMouseEnterSlot")
		slotWin:subscribeEvent("MouseLeave", "IIT_onMouseLeaveSlot")
	end
end
----------------------------------------------------------
--
function IIT_notifyIITData()
--	g_Loger:logError("notify_IITData_OK")
	IIT_setIITState(1)
	IIT_resetAllGrids()
	IIT_setGridsData()
	IIT_setMoneyTxt()
	IIT_setTotalPage()
	IIT_setPageToTxtbox()
end
-----------------------------------------------------------
--
function IIT_setMoneyTxt()
	local money = g_ItemInteractMgr:getmoney()
	local moneyBox = g_WinMgr:getWindow(g_ItemInteractUIName.moneyBox)
	moneyBox:setText(tostring(money))
end
-----------------------------------------------------------
--
function IIT_setTotalPage()
	mTotalpage = g_ItemInteractMgr:getPackPageCount()
end
-----------------------------------------------------------
function IIT_setPageToTxtbox()
	local tbox = g_WinMgr:getWindow(g_ItemInteractUIName.pageTxtbox)
	local curPage = g_ItemInteractMgr:getPackCurPage()
--	g_Loger:logError("HIUFGIUHL" ..tostring(curPage))
	tbox:setText(tostring(curPage) .."/" ..tostring(mTotalpage))
end
-----------------------------------------------------------
--
function IIT_isIITBtnStartUsing()
	if mStartUseFlag == false then
		MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","该功能暂未开通.", "MsgBox_cancelFunc", "")
		return false
	else
		return true
	end
end
-----------------------------------------------------------
--
function IIT_reqStartInteract()
	local isStartUse = IIT_isIITBtnStartUsing()
	if isStartUse == false then
		return
	end
	IIT_checkItemCanInteract()
	IIT_reqIsActiveFarm()
end
-----------------------------------------------------------
--
function IIT_checkItemCanInteract()
	if g_DragItem:isDraging() then
		if g_PT_WAREHOUSE == g_DragItem:getPacktype() then
			local slotIdx = g_DragItem:getIndex()
			local itemid = g_mainplayer:getItemIDByGlobaIdx(slotIdx,g_PT_WAREHOUSE)
			local itemrow 	= g_itemtableMgr:getItemRow(itemid)
			local itemBind = itemrow.bind
			if itemBind ~= nil and itemBind ~= 5 then
				g_DragItem:undo()
			end
		end
	end
end
-----------------------------------------------------------
--
function IIT_reqIsActiveFarm()
	g_IITConn:reqFarmisActive()
end
-----------------------------------------------------------
--
function IIT_reqOpenIITUI()
	IIT_submitReqOpenUI()
	
end
-----------------------------------------------------------
--
function IIT_reqFarmMoney()
	
end
-----------------------------------------------------------
--
function IIT_submitReqOpenUI()
	g_req_open_ui.type = g_uitype.uit_farmdepot
	g_msgmgr:send(g_req_open_ui)
end
-----------------------------------------------------------
--
function IIT_openRelatUI()
	UIM_DoOpenAndCloseUI(g_ItemInteractUIName.root)
end
-----------------------------------------------------------
--
function IIT_openIITUI()
	--g_MainUIWindows:addChildWindow(g_ItemInteractUI)
	IIT_openRelatUI()
	UIM_OpenWindow(g_ItemInteractUIName.root)
	
	if g_isOpenIITUI == false then
		IIT_setUIPos()
		IIT_subscribeRelateEvent()
		g_isOpenIITUI = true
	end
end
-----------------------------------------------------------
--
function IIT_subscribeRelateEvent()
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/qian"):subscribeEvent("MouseClick","IIT_openRemoveMoneyUI")
end
-----------------------------------------------------------
--
function IIT_removeRelateEvent()
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/Txt_GoldBox"):removeEvent("MouseClick")
end
-----------------------------------------------------------
--
function IIT_closeIITUI()
	--g_MainUIWindows:removeChildWindow(g_ItemInteractUI)
	UIM_CloseWindow(g_ItemInteractUIName.root)
	IIT_closeRemoveMoneyUI()
	g_isOpenIITUI = false
	IIT_removeRelateEvent()
	IIT_setIITState(0)
	g_GameEventMgr:fireEvent("UPDATE_STORAGE_UI")
end
-----------------------------------------------------------
--
function IIT_setPackPos()
	
	local h, w
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	h  = g_Storage:getHeight():asAbsolute(screenH)
	w  = g_Storage:getWidth():asAbsolute(screenW)
	g_Storage:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,0),	CEGUI.UDim(0.5, -h/2)))
end
-----------------------------------------------------------
--
function IIT_setUIPos()
	local win = g_WinMgr:getWindow(g_ItemInteractUIName.root)
	local h, w
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	h  = win:getHeight():asAbsolute(screenH)
	w  = win:getWidth():asAbsolute(screenW)
	win:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-w),	CEGUI.UDim(0.5, -h/2)))
	
	IIT_setPackPos()
end
-----------------------------------------------------------
--
function IIT_setGridsData()
	for packIndex = 0, mSlotsCount - 1 do
		local gridIndex = packIndex + 1	
		local itemTemplateId = g_ItemInteractMgr:getItemID(packIndex)
		if 0 < itemTemplateId then
			local packInfo = g_ItemInteractMgr:getItemGird(packIndex)
			local itemRow = g_itemtableMgr:getItemRow(itemTemplateId)

			if nil ~= itemRow then
				local overLay = packInfo.mOverlay
				
				--无法叠加
				if itemRow.overlap == 1 then
					overLay = -1
				end
				local itemEnabled = "True"
				if itemRow.bind == 5 then
					itemEnabled = "True"
				else
					itemEnabled = "False"
				end
				StorageUI_DynamicLoadItem(g_ItemInteractUIName.slotBaseName..tostring(gridIndex),
											"newImg",
											gridIndex,
											itemRow.imageset,
											itemRow.icon,"True",overLay,itemEnabled)
			else
				--为空提示“加载资源失败”
			end
			--加载物品锁定状态
			--if packInfo.mLock ~= 1 then
			--	lockunlockGird(true, gridIndex, EQGlobaClass.ItemPack.PT_STORAGE)
			--else
			--	lockunlockGird(false, gridIndex, EQGlobaClass.ItemPack.PT_STORAGE)
			--end
		end
	end
end
-----------------------------------------------------------
--
function IIT_resetAllGrids()
	for i = 1,mSlotsCount do
		IIT_resetOneGrid(i)
	end
end
--------------------------------------------------------------
--
function IIT_resetOneGrid(gridIdx)
	local slotWin = g_WinMgr:getWindow(g_ItemInteractUIName.slotBaseName..tostring(gridIdx))
	
	if slotWin ~= nil then
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			TempSource:removeEvent("DragDropItemDropped")
			slotWin:removeChildWindow(TempSource)
		end
	end
end
--------------------------------------------------------------
--
function IIT_onClickSlot(args)
--	g_Loger:logError("on_Click_IITslot")
	--获取点击的格子索引
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local childNum = CEGUI.toWindowEventArgs(args).window:getChildCount()
	local gridIndex = GetClickWindowIndex(winName, g_ItemInteractUIName.slotBaseName, 1 ,mSlotsCount)
	
	local curPagePackIndex = gridIndex - 1
	local itemTemplateId = g_ItemInteractMgr:getItemID(curPagePackIndex)
		local item_instance_id = g_ItemInteractMgr:getItemInsIdStr(curPagePackIndex)
--		g_Loger:logError(tostring(item_instance_id))
	local curPage = g_ItemInteractMgr:getPackCurPage()
	local totalPackIndex = curPagePackIndex + (curPage - 1) * mSlotsCount
	
	--判断格子是否已锁定
	
	--区分左键点击和右键点击 0 为左键 1为右键
	local mouseDownState = CEGUI.toMouseEventArgs(args).button
	
	if 0 == mouseDownState  then
		--	超链接(hezhr)
		if g_CS_HYPERLINK == g_CursorMgr:getState() then
			ClickHyperLinkItem(itemTemplateId)
			return
		end
		
		if g_DragItem:isDraging() then
		
			if g_PT_WAREHOUSE == g_DragItem:getPacktype() then
				
				--if	g_DragItem:getSplitNum() > 0 then
					--MsgBox_createMsgBox(g_MainUIWindows,10601,"","无法拆分物品.", "MsgBox_cancelFunc", "")
					--return
				--else
					--ReqMoveItem(g_DragItem:getIndex(),  g_PT_WAREHOUSE, -1, g_PT_FARMDEPOT)
					--
					local index = g_DragItem:getIndex()
					local item_instance_id = g_mainplayer:getItemInsIdStr(index,g_PT_WAREHOUSE)
--					g_Loger:logError(tostring(item_instance_id))
					--
					IIT_reqSwitchItem(g_PT_WAREHOUSE,item_instance_id,g_PT_FARMDEPOT)
					IIT_submitReqOpenUI()
				--end
			elseif g_PT_FARMDEPOT == g_DragItem:getPacktype() then
				--拆分
				if	g_DragItem:getSplitNum() > 0 then
					--拆分的是仓库的物品
					--ReqSplitItem(g_PT_IITSLOTS, g_DragItem:getIndex(), totalPackIndex, g_DragItem:getSplitNum())
					
				else
					--目标格子不为原始格子
					if g_DragItem:getIndex() ~= totalPackIndex then
						--ReqSwapItem(g_DragItem:getIndex(), totalPackIndex, g_PT_FARMDEPOT)
					end
				end
			end	
			
			g_DragItem:undo()
		else
			--g_Loger:logError("*****DRAGING*****")
			local itemBind = 1
			if 0 < itemTemplateId then
				local itemRow = g_itemtableMgr:getItemRow(itemTemplateId)

				if itemRow ~= nil then
					itemBind = itemRow.bind
					--g_Loger:logError("itemRow.id*******" ..tostring(itemTemplateId))
					--g_Loger:logError("itemRow.id*******" ..tostring(itemRow.id))
					--g_Loger:logError("itemRow.bind*****" .. tostring(itemBind))
				else
					--加载资源失败
				end
				if childNum > 0  then
					if itemBind == 5 then
						g_DragItem:drag(winName, totalPackIndex, g_PT_FARMDEPOT)
						g_dratItemID = itemTemplateId	--保存吸起来的物品模板id(hezhr)
					else
						--g_Loger:logError("can't interact this item!")
						MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","此物品无法移动。", "MsgBox_cancelFunc", "")

					end
				end
			end
		end
	else
		if g_DragItem:isDraging() then
			--删除鼠标粘滞物
			if 0 < itemTemplateId then
				--if 背包打开
					--请求将仓库物品放入背包
			end
		else
			--使用物品
			UseItem(itemTemplateId)
		end
	end
end
--------------------------------------------------------------
--
function IIT_onMouseEnterSlot(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local gridIndex = GetClickWindowIndex(winName, g_ItemInteractUIName.slotBaseName, 1 ,mSlotsCount)
	
	local curPagePackIndex = gridIndex - 1
	local itemTemplateId = g_ItemInteractMgr:getItemID(curPagePackIndex)
	if 0 ~= itemTemplateId then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_ItemInteractMgr:getItemDelTime(curPagePackIndex)
		createItemhint_shop(itemTemplateId, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
--------------------------------------------------------------
--
function IIT_onMouseLeaveSlot(args)
	destroyItemhint(g_MainUIWindows)
end
--------------------------------------------------------------
--
function IIT_changePage(page)
	g_ItemInteractMgr:setPackCurPage(page)
	IIT_setPageToTxtbox()
	
end
---------------------------------------------------------------
--
function IIT_prePage()
	local pageCount = g_ItemInteractMgr:getPackPageCount()
	local pageIdx	= IIT_getCurPageNumfromTxtBox() -1
	--g_Loger:logDebug("pageIdx= "..pageIdx)
	--g_Loger:logDebug("pageCount= "..pageCount)
	if pageIdx < 1 then
		
	else
		IIT_changePage(pageIdx)
	end
end
---------------------------------------------------------------
--
function IIT_nextPage()
	local pageCount = g_ItemInteractMgr:getPackPageCount()
	local pageIdx	= IIT_getCurPageNumfromTxtBox() + 1
--	g_Loger:logDebug("pageIdx= "..pageIdx)
--	g_Loger:logDebug("pageCount= "..pageCount)
	if pageIdx > pageCount then
		
	else
		IIT_changePage(pageIdx)
	end
end
---------------------------------------------------------------
--
function IIT_getCurPageNumfromTxtBox()
	local tbox = g_WinMgr:getWindow(g_ItemInteractUIName.pageTxtbox)
	local num_str = tbox:getText()
	local num_tab = {}
	num_tab = Split(num_str,"/")
	--g_Loger:logError("O*^%*%*^((&&&&&" ..num_tab[1])
	return tonumber(num_tab[1])
end
---------------------------------------------------------------
--
function IIT_notifyFarmNoActive()
	MsgBox_createMsgBox(g_MainUIWindows,106645,"消息提示","农场未激活，没有相应数据", "IIT_MsgBoxCorfirmEvent", "MsgBox_cancelFunc","激活")
end
---------------------------------------------------------------
--
function IIT_MsgBoxCorfirmEvent(args)
	OpenFunnyUI()
	--MsgBox_cancelFunc(args)
end
---------------------------------------------------------------
--
function IIT_setIITState(state)
	interactState = state
end
---------------------------------------------------------------
--
function IIT_getIITState()
	return interactState
end
---------------------------------------------------------------
--
function IIT_startRemoveMoney(money,fromType,toType)
	g_IITConn:reqMoveMoney(money,fromType,toType)
	IIT_closeRemoveMoneyUI()
end
---------------------------------------------------------------
--
function IIT_openRemoveMoneyUI(args)
	EQGlobaFunc.SetText(g_storageInputMoneyUI:getName(),"金币导入")
	local win = CEGUI.toWindowEventArgs(args).window
	local winName = win:getName()
	local bag_goldBox = "CkFrmWnd/BgPanel/qian"
	local farm_goldBox = "ItemInteractWnd/BgPanel/qian"
	if winName == farm_goldBox then
		EQGlobaFunc.SetText("store_money/background/tishi","农场->家园")
		IIT_setRemoveMoneyMode(1)
		g_MainUIWindows:addChildWindow(g_storageInputMoneyUI)
		IIT_subcribeMoneyBoxEvent()
	else
		if g_isOpenIITUI == true then
			EQGlobaFunc.SetText("store_money/background/tishi","家园->农场")
			IIT_setRemoveMoneyMode(2)
			g_MainUIWindows:addChildWindow(g_storageInputMoneyUI)
			IIT_subcribeMoneyBoxEvent()
		end
	end
end
---------------------------------------------------------------
--
function IIT_setRemoveMoneyMode(mode)
	removeMode = mode
end
---------------------------------------------------------------
--
function IIT_subcribeMoneyBoxEvent()
	g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):removeEvent("TextChanged")
	g_WinMgr:getWindow(g_storageUIName.inputBoxSureBtn):removeEvent("Clicked")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCancleBtn):removeEvent("Clicked")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCloseBtn):removeEvent("Clicked")
	g_WinMgr:getWindow(g_storageUIName.inputBoxSureBtn):subscribeEvent("Clicked", "IIT_submitRemoveMoneyReq")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCancleBtn):subscribeEvent("Clicked", "IIT_closeRemoveMoneyUI")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCloseBtn):subscribeEvent("Clicked", "IIT_closeRemoveMoneyUI")
	g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):subscribeEvent("TextChanged", "IIT_moveMoneyChanged")
end
---------------------------------------------------------------
--
function IIT_submitRemoveMoneyReq()
	local moneyBox = g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox)
	local moveMoney = tonumber(moneyBox:getText())
	local moneyMax = 0
	if removeMode == 1 then
		moneyMax = g_ItemInteractMgr:getmoney()
	elseif removeMode == 2 then
		moneyMax = g_mainplayer:getGameGold()
	end
	if moneyMax == 0 then
		MsgBox_createMsgBox(g_MainUIWindows,106645,"消息提示","您已无金币可转.", "MsgBox_cancelFunc", "")
		return
	end
	if moveMoney ~= nil and moveMoney ~= 0 then
		if	moveMoney > moneyMax then
			moveMoney = moneyMax
		end
		if removeMode == 1 then
			IIT_startRemoveMoney(moveMoney,g_PT_FARMDEPOT,g_PT_WAREHOUSE)
		elseif removeMode == 2 then
			IIT_startRemoveMoney(moveMoney,g_PT_WAREHOUSE,g_PT_FARMDEPOT)
		else
			--removeMode金币转移模式值出错
		end
		IIT_closeRemoveMoneyUI()
	else
		--请输入数值
		MsgBox_createMsgBox(g_MainUIWindows,106645,"消息提示","请输入金币.", "MsgBox_cancelFunc", "")
	end
	
end
---------------------------------------------------------------
--
function IIT_closeRemoveMoneyUI()
	g_MainUIWindows:removeChildWindow(g_storageInputMoneyUI)
	g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):setText("")
end
---------------------------------------------------------------
--
function IIT_moveMoneyChanged(args)
	local moneyMax = 0
	if removeMode == 1 then
		moneyMax = g_ItemInteractMgr:getmoney()
	elseif removeMode == 2 then
		moneyMax = g_mainplayer:getGameGold()
	else
		--removeMode值出错
	end
	if moneyMax == 0 then
		--MsgBox_createMsgBox(g_MainUIWindows,106645,"消息提示","您已无金币可转.", "MsgBox_cancelFunc", "")
		
	end
	local inputMoney = g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):getText()
	local removeMoney = tonumber(inputMoney)
	if removeMoney == nil then
		removeMoney = 0
	end
	if removeMoney > moneyMax then
		g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):setText(tostring(moneyMax))
	else
	
	end
end
----------------------------------------------------------------
--
function IIT_reqSwitchItem(soucreType,insid,targetType)
	if(g_DragItem:isDraging()) then
		g_IITConn:reqSwithItem(soucreType,insid,targetType)
		g_DragItem:undo()
	end
end