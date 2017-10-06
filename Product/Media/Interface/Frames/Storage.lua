-----------------------------------------
-- 文件说明	：仓库相关逻辑处理
-- 作者		：xiaoMing
-- 日期		：2010-10-25
-----------------------------------------
-- ReqOpenStorage PopStorageUI RemoveStorageUI
-- ExpendStorage 扩展仓库
-- PutItemInStorage 存入仓库
-- GetItemFromStorage 从仓库取出
-- SplitStorageItem 拆分仓库物品 
-- PutMoneyInStorage
-- GetMoneyFromStorage
-- StorageItemTplId(gridIndex)
-- ClickStorageGrid 点击仓库格子
-- LockStorageGrid
-- SetStorageGrid
-- ClearStorageGrid
-- ReqSwapStorageItem
-- RepMoveStorageItem
-- UpdateStorage
-- StorageOnLoad
-----------------------------------------
local HandleMoneyState = {
						None = 0,
						Get  = 1,
						Save = 2,
							}
-----------------------------------------
local mStorageGridCount = 40
g_isOpenStorage = false
local mIsOpenMoneyInputBox = false					
local mCurHandleMoneyState = HandleMoneyState.None
local mNoHandleMoneyChange = false
-----------------------------------------
function StorageOnload()
	g_GameEventMgr:subscribeEvent("UPDATE_STORAGE","UpdateStorage")
	
	g_WinMgr:getWindow(g_storageUIName.closeBtn):subscribeEvent("Clicked", "RemoveStorageUI")
	
	for gridIndex = 1, mStorageGridCount do
		g_WinMgr:getWindow(g_storageUIName.slotBaseName..gridIndex):subscribeEvent("MouseClick", "ClickStorageGrid")
		g_WinMgr:getWindow(g_storageUIName.slotBaseName..gridIndex):removeEvent("MouseEnter")
		g_WinMgr:getWindow(g_storageUIName.slotBaseName..gridIndex):removeEvent("MouseLeave")
		g_WinMgr:getWindow(g_storageUIName.slotBaseName..gridIndex):subscribeEvent("MouseEnter", "EnterStorageGrid")
		g_WinMgr:getWindow(g_storageUIName.slotBaseName..gridIndex):subscribeEvent("MouseLeave", "LeaveStorageGrid")
	end
	
	
	g_WinMgr:getWindow(g_storageUIName.saveMoneyBtn):subscribeEvent("Clicked", "PopSaveMoneyBox")
	g_WinMgr:getWindow(g_storageUIName.getMoneyBtn):subscribeEvent("Clicked", "PopGetMoneyBox")	
	
	local window = g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox)
	--只让输入框输入正整数
	CEGUI.toEditbox(window):setValidationString("^$|^[0-9]*[1-9][0-9]*$")
	
	MainUI_ShieldClick(g_storageUI)
end
-----------------------------------------
function UpdateStorage()
	if true == mIsOpenMoneyInputBox then
		RemoveMoneyInputBox()
	end
	
	for packIndex = 0, mStorageGridCount - 1 do
		local gridIndex = packIndex + 1	
		ClearStorageGrid(gridIndex)
		--获取当前页每个格子对应的物品ID
		local itemTemplateId = g_mainplayer:getItemID(packIndex,EQGlobaClass.ItemPack.PT_STORAGE)
		
		if 0 < itemTemplateId then
			local packInfo = g_mainplayer:getItemGird(packIndex,EQGlobaClass.ItemPack.PT_STORAGE)
			local itemRow = g_itemtableMgr:getItemRow(itemTemplateId)

			if nil ~= itemRow then
				local overLay = packInfo.mOverlay
				
				--无法叠加
				if itemRow.overlap == 1 then
					overLay = -1
				end
			
				StorageUI_DynamicLoadItem(g_storageUIName.slotBaseName..gridIndex,
											"newImg",
											gridIndex,
											itemRow.imageset,
											itemRow.icon,"True",overLay)
			else
				--为空提示“加载资源失败”
			end
		
			--加载物品锁定状态
			if packInfo.mLock ~= 1 then
				lockunlockGird(true, gridIndex, EQGlobaClass.ItemPack.PT_STORAGE)
			else
				lockunlockGird(false, gridIndex, EQGlobaClass.ItemPack.PT_STORAGE)
			end
		end
	end
	--加载金钱
	totalMoney = g_mainplayer:getStorageCoin()
	g_WinMgr:getWindow(g_storageUIName.moneyTextBox):setText(tostring(totalMoney))
	
end
-----------------------------------------
function ReqPopStorageUI()
	--判断是不是在自己家
	local isInMyHouse = g_SceneMgr:inMyHouse()
	if false == isInMyHouse then
		return
	end
	
	if g_DragItem:isDraging() then
		g_DragItem:undo()
	end
	
	if false == g_isOpenStorage then
		ReqOpenStorageUI()
		
		UIM_DoOpenAndCloseUI(g_storageUIName.root)
	else
		RemoveStorageUI()
	end
end
function ReqOpenStorageUI()
	g_req_open_ui.type = g_uitype.uit_depot
	g_msgmgr:send(g_req_open_ui)
end
-----------------------------------------
function PopStorageUI()
	OpenStorageUI()
	g_storageUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.05, 0), CEGUI.UDim(0.2, 0)))
end
-----------------------------------------
function OpenStorageUI()
	UIM_OpenWindow(g_storageUIName.root)
	g_isOpenStorage = true
	PlayerGuide_do_NewerTeachTaskScript(15006) --新手教学任务
end

function RemoveStorageUI()
	UIM_CloseWindow(g_storageUIName.root)
	g_isOpenStorage = false
	
	g_CursorMgr:setState(g_CS_NORMAL)
	if g_DragItem:isDraging() then
		g_DragItem:undo()
	end
end
-----------------------------------------
--点击仓库格子
function ClickStorageGrid(args)
	Storage_ClickCloseEquipTips()
	--获取点击的格子索引
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local gridIndex = GetClickWindowIndex(winName, g_storageUIName.slotBaseName, 1 ,mStorageGridCount)
	
	local curPagePackIndex = gridIndex - 1
	local itemTemplateId = g_mainplayer:getItemID(curPagePackIndex,EQGlobaClass.ItemPack.PT_STORAGE)
	
	local curPage = g_mainplayer:getPackCurPage(g_PT_STORAGE)
	local totalPackIndex = curPagePackIndex + (curPage - 1) * mStorageGridCount
	
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
				
				if	g_DragItem:getSplitNum() > 0 then
					MsgBox_createMsgBox(g_MainUIWindows,10601,"","无法这样拆分物品.", "MsgBox_cancelFunc", "")
					return
				else
					ReqMoveItem(g_DragItem:getIndex(), g_PT_WAREHOUSE, totalPackIndex , g_PT_STORAGE)
				end
					
				
			elseif g_PT_STORAGE == g_DragItem:getPacktype() then
				--拆分
				if	g_DragItem:getSplitNum() > 0 then
					if g_dratItemID ~= itemTemplateId and 0 ~= itemTemplateId then
						MsgBox_createMsgBox(g_MainUIWindows,0,"","不同物品无法拆分！", "MsgBox_cancelFunc", "")
					else
						--拆分的是仓库的物品
						ReqSplitItem(g_PT_STORAGE, g_DragItem:getIndex(), totalPackIndex, g_DragItem:getSplitNum())
					end
				else
					--目标格子不为原始格子
					if g_DragItem:getIndex() ~= totalPackIndex then
						ReqSwapItem(g_DragItem:getIndex(), totalPackIndex, g_PT_STORAGE)
					end
				end
			end	
			
			g_DragItem:undo()
			
			--播放放下音效
			local ir = g_itemtableMgr:getItemRow(g_dratItemID)
			if ir then
				EQGlobaClass.getVoiceMgr():play(ir.dvoiceid, false)
				--EQGlobaClass.getVoiceMgr():play(2, false)
--				g_Loger:logMessage("Item dvoiceid: " ..tostring(ir.dvoiceid))
			else
				g_Loger:logError("ItemRow null, DragItemId:"..tostring(g_dratItemID))
			end
		else
			if 0 < itemTemplateId then
				g_DragItem:drag(winName, totalPackIndex, g_PT_STORAGE)
				g_dratItemID = itemTemplateId	--保存吸起来的物品模板id(hezhr)
				
				--播放抓取音效
				local itemRow = g_itemtableMgr:getItemRow(itemTemplateId)
				if itemRow then
					EQGlobaClass.getVoiceMgr():play(itemRow.uvoiceid, false)
					--EQGlobaClass.getVoiceMgr():play(1, false)
--					g_Loger:logMessage("Item uvoiceid: " ..tostring(itemRow.uvoiceid))
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
	
	--准备拆分物品
	if 0 < itemTemplateId  then
		if g_isKeydown(g_KC_LSHIFT) or (g_CursorMgr:getState() == g_CS_SPLIT_ITEM) then
			if g_DragItem:getSplitNum() == 0 then
				gird = g_mainplayer:getItemGird(curPagePackIndex,EQGlobaClass.ItemPack.PT_STORAGE)
				SplitItem(gird.mOverlay)
				
				EQGlobaFunc.setKeyFlag(g_KC_LSHIFT, false)
			end
		end
	end
			
	return
end
-----------------------------------------
function ClearStorageGrid(gridIndex)
	local slotWin = g_WinMgr:getWindow(g_storageUIName.slotBaseName..gridIndex)
	
	if slotWin ~= nil then
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			TempSource:removeEvent("DragDropItemDropped")
			slotWin:removeChildWindow(TempSource)
		end
	end
end
-------------------------------------------
function PopMoneyInputBox()
	g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):setText("")
	g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):activate()
	mNoHandleMoneyChange = false
	local window = g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox)
	window:removeEvent("TextChanged")
	window:subscribeEvent("TextChanged", "OnPuttingMoney")
	g_WinMgr:getWindow(g_storageUIName.inputBoxSureBtn):removeEvent("Clicked")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCancleBtn):removeEvent("Clicked")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCloseBtn):removeEvent("Clicked")
	g_WinMgr:getWindow(g_storageUIName.inputBoxSureBtn):subscribeEvent("Clicked", "HandleStorageMoney")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCancleBtn):subscribeEvent("Clicked", "RemoveMoneyInputBox")
	g_WinMgr:getWindow(g_storageUIName.inputBoxCloseBtn):subscribeEvent("Clicked", "RemoveMoneyInputBox")

	g_MainUIWindows:addChildWindow(g_storageInputMoneyUI)
	mIsOpenMoneyInputBox = true
	
	Common_setModalState(g_storageInputMoneyUI:getName(), true)
end
-------------------------------------------
function RemoveMoneyInputBox()
	g_MainUIWindows:removeChildWindow(g_storageInputMoneyUI)
	mIsOpenMoneyInputBox = false
	
	RecoverMoneyChange()
	
	Common_setModalState(g_storageInputMoneyUI:getName(), false)
end
-------------------------------------------
function PopSaveMoneyBox()
	if false == mIsOpenMoneyInputBox then
		PopMoneyInputBox()
	elseif mCurHandleMoneyState == HandleMoneyState.Save then
		RemoveMoneyInputBox()
		return
	end
	
	mCurHandleMoneyState = HandleMoneyState.Save
end
-------------------------------------------
function PopGetMoneyBox()
	if false == mIsOpenMoneyInputBox then
		PopMoneyInputBox()
	elseif mCurHandleMoneyState == HandleMoneyState.Get then
		RemoveMoneyInputBox()
		return
	end
	--设置金钱处理状态
	mCurHandleMoneyState = HandleMoneyState.Get
end
-------------------------------------------
function HandleStorageMoney()
	local moneyCount = g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):getText()
	moneyCount = tonumber(moneyCount)
	
	if 0 == moneyCount or nil == moneyCount then
		return
	end
	
	--获得当前处理状态
	if HandleMoneyState.Save == mCurHandleMoneyState then
		g_mainplayer:reqSaveMoneyToStorage(moneyCount)
	else
		g_mainplayer:reqGetMoneyFromStorage(moneyCount)
	end
end
-------------------------------------------
function OnPuttingMoney()
	--由于某种条件，对文本输入框进行文本设置引起
	--的该函数的回调不处理
	if true == mNoHandleMoneyChange then
		mNoHandleMoneyChange = false
		return 
	end
	
	local changeMoneyStr = g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):getText()
	local changeMoneyCount = tonumber(changeMoneyStr)
	
	if nil == changeMoneyCount then
		changeMoneyCount = 0
	end
	
	local bagMoneyCount = g_mainplayer:getGameGold()
	local storageMoneyCount = g_mainplayer:getStorageCoin()
	local leavingsBagMoney, leavingsStorageMoney
	--存钱
	if HandleMoneyState.Save == mCurHandleMoneyState then
		if changeMoneyCount > bagMoneyCount then
			changeMoneyCount = bagMoneyCount
			
			mNoHandleMoneyChange = true
			g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):setText(changeMoneyCount)	
		else
			mNoHandleMoneyChange = false
		end
		
		leavingsStorageMoney = storageMoneyCount + changeMoneyCount
		leavingsBagMoney = bagMoneyCount - changeMoneyCount	
	--取钱	
	elseif HandleMoneyState.Get == mCurHandleMoneyState then
		if changeMoneyCount > storageMoneyCount then
			changeMoneyCount = storageMoneyCount
			
			mNoHandleMoneyChange = true
			g_WinMgr:getWindow(g_storageUIName.moneyInputEditBox):setText(changeMoneyCount)
		else
			mNoHandleMoneyChange = false
		end
		
		leavingsStorageMoney = storageMoneyCount - changeMoneyCount
		leavingsBagMoney = bagMoneyCount + changeMoneyCount
	else
		--HandleMoneyState.None == mCurHandleMoneyState
		return
	end
	
	g_WinMgr:getWindow(g_storageUIName.moneyTextBox):setText(leavingsStorageMoney)
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/Txt_GoldBox"):setText(leavingsBagMoney)
end
-------------------------------------------
function ReqMoveItem(formIndex, formPackType, toIndex, toPackType)
	if(g_DragItem:isDraging()) then
		g_req_move_item.index1 	= formIndex
		g_req_move_item.index2 	= toIndex
		g_req_move_item.pack1_type= formPackType
		g_req_move_item.pack2_type= toPackType
		g_msgmgr:send(g_req_move_item)
		
		g_DragItem:undo()
	end
end
-------------------------------------------
function RecoverMoneyChange()
	local bagMoneyCount = g_mainplayer:getGameGold()
	local storageMoneyCount = g_mainplayer:getStorageCoin()
	g_WinMgr:getWindow(g_storageUIName.moneyTextBox):setText(storageMoneyCount)
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/Txt_GoldBox"):setText(bagMoneyCount)
	
	mCurHandleMoneyState = HandleMoneyState.None
end
-----------------------------------------
--显示仓库itemhint
function EnterStorageGrid(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local gridIndex = GetClickWindowIndex(winName, g_storageUIName.slotBaseName, 1 ,mStorageGridCount)
	local curPagePackIndex = gridIndex - 1
	local itemTemplateId = g_mainplayer:getItemID(curPagePackIndex,EQGlobaClass.ItemPack.PT_STORAGE)
	if 0 ~= itemTemplateId then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_mainplayer:getItemDelTime(itemTemplateId,EQGlobaClass.ItemPack.PT_STORAGE)
		createItemhint_shop(itemTemplateId, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
-----------------------------------------
--隐藏itemhint
function LeaveStorageGrid(args)
	destroyItemhint(g_MainUIWindows)
end