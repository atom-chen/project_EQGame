-----------------------------------------
-- 文件说明	：背包相关逻辑处理
-- 作者		：麦
-- 修改		：
-- 日期		：10/03/25
-----------------------------------------

g_WinMgr:getWindow("KMainRoot/HMenubar/Btn_HyperLink"):subscribeEvent("Clicked", "ClickHyperLinkBtn")	--	(hezhr)
g_dratItemID = 0	-- 物品吸起来的模板id，全局变量
local tipsPare = nil	-- 装备提示父窗口
local tipsAni = EQGlobaClass.createAnimate()	-- 装备提示动画

function GetGlobalBagidx(curpageidx)
	local curPage 	= g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
	local globalidx = curpageidx + (curPage - 1) * C_STORAGE_GIRD_NUM 
	return globalidx
end
------请求交换物品----------------------------------------
function ReqSplitItem(packType, idxsrc,idxtar,num )
	--类型不一致不允许拆分
	if g_DragItem:getPacktype() == packType then
		g_req_split_item.type			= packType
		g_req_split_item.src_gindex		= idxsrc
		g_req_split_item.target_gindex 	= idxtar
		g_req_split_item.count			= num
		g_msgmgr:send(g_req_split_item)
	end

	g_DragItem:undo()

end
------请求交换物品----------------------------------------
function ReqSwapItem(idxsrc,idxtar, packType)

	if(g_DragItem:isDraging()) then

		if idxsrc ~= idxtar then
			g_req_swap_item.index1 = idxsrc
			g_req_swap_item.index2 = idxtar
			g_req_swap_item.type   = packType
			g_msgmgr:send(g_req_swap_item)
--			g_Loger:logDebug("g_req_swap_item")

			g_DragItem:undo()
		else
			g_DragItem:undo()
		end

	else--什么也没有

	end
end
------请求移动物品----------------------------------------
--合并为通用函数 in storage.lua
--function ReqMoveItem(idxsrc,idxtar)

	--if(g_DragItem:isDraging()) then

		--g_req_move_item.index1 	= idxsrc
		--g_req_move_item.index2 	= idxtar
		--g_req_move_item.pack1_type= g_PT_BODY
		--g_req_move_item.pack2_type= g_PT_WAREHOUSE
		--g_msgmgr:send(g_req_move_item)

		--g_DragItem:undo()
	--end
--end
--
function ReqResizePack()
	g_msgmgr:send(g_req_resize_player_pack)
	NormalCancel()
end
--
function GetResizeWndIndex(wndname)
	for i=1,4 do
		if g_StoragePage[i] == wndname then
			return i
		end
	end
	return nil
end
--
function ChangePage(pageIdx)
	g_mainplayer:setPackCurPage(g_PT_WAREHOUSE,pageIdx)
end
------背包扩容，切换点击事件----------------------------------------
function BagResizeBtnClicked(args)
	local wndTarget	= CEGUI.toWindowEventArgs(args).window
	local curPage 	= g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
	local pageCount = g_mainplayer:getPackPageCount(g_PT_WAREHOUSE)
	local pageIdx	= GetResizeWndIndex(wndTarget:getName())
--	g_Loger:logDebug("pageIdx= "..pageIdx)
--	g_Loger:logDebug("pageCount= "..pageCount)
	if pageIdx > pageCount then
		MessageBox("确定使用10EQ点进行扩容吗？","ReqResizePack","NormalCancel")
	else
		ChangePage(pageIdx)
	end
end
------背包格子点击事件----------------------------------------
function StorageClickSlot(args)
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	local idxtar 	= getStorageIdxByslot(SlotWin)
	local curPage 	= g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
	idxtar = idxtar + (curPage - 1) * C_STORAGE_GIRD_NUM
	local itemid = g_mainplayer:getItemIDByGlobaIdx(idxtar,g_PT_WAREHOUSE)
	--g_Loger:logError("Itemid: " ..tostring(itemid))
	local itemRow	= g_itemtableMgr:getItemRow(itemid)
	local num 		= SlotWin:getChildCount()
	local button 	= CEGUI.toMouseEventArgs(args).button
	if button == 1 then	--	鼠标右键
		if g_DragItem:isDraging()==false then
			--使用物品
			UseItem(itemid)
		end
		return
	end
	
	--	装备提示(hezhr)
	local equipPos = Storage_GetClothesEquipPos(itemid)
	
	if g_CS_HYPERLINK~=g_CursorMgr:getState() and g_CS_SPLIT_ITEM~=g_CursorMgr:getState() then
		if -1 ~= equipPos then
			Equipment_setEquipPos(equipPos + 1)
			Storage_ClickOpenEquipTips(equipPos+1)
		end
	end
	
	--	超链接(hezhr)
	if g_CS_HYPERLINK == g_CursorMgr:getState() then
		ClickHyperLinkItem(itemid)
		return
	end
	--物品拖动操作
	if(g_DragItem:isDraging()) then
		if g_DragItem:getPacktype() == g_PT_WAREHOUSE then
			--背包背包操作
			if g_DragItem:getSplitNum() > 0 then
				if g_dratItemID ~= itemid and 0 ~= itemid then
					MsgBox_createMsgBox(g_MainUIWindows,0,"","不同物品无法拆分！", "MsgBox_cancelFunc", "")
				else
					--请求拆分
					ReqSplitItem(g_PT_WAREHOUSE, g_DragItem:getIndex(),idxtar,g_DragItem:getSplitNum())
				end
			else
				--目标格子已锁，返回
				if true == IsLockedGrid(idxtar, EQGlobaClass.ItemPack.PT_WAREHOUSE) then
					return
				end
				--请求交换
				ReqSwapItem(g_DragItem:getIndex(),idxtar, g_PT_WAREHOUSE)
				playdropsnd = true
			end
		elseif g_DragItem:getPacktype() == g_PT_BODY then
			--装备栏操作
			if num == 0 then--装备拖到空位
				ReqMoveItem(g_DragItem:getIndex(), g_PT_BODY, idxtar , g_PT_WAREHOUSE)
			else --交换装备
				equipItem(idxtar,g_DragItem:getIndex())
			end
		elseif g_PT_TRADE == g_DragItem:getPacktype() then
			--是交易栏的拖过来的物品
			g_smallModules:sendUnputItem(g_DragItem:getIndex(), idxtar)
			g_DragItem:undo()
		elseif g_PT_OUTBOX == g_DragItem:getPacktype() then
			--发件箱
			g_mailManager:backSendItem(g_DragItem:getIndex(), idxtar)
			g_DragItem:undo()
			UpdateSendItem()
		elseif g_PT_STORAGE == g_DragItem:getPacktype() then
			--背包托过来的物品 若在拆分状态提示无法拆分
			if g_DragItem:getSplitNum() > 0 then
				MsgBox_createMsgBox(g_MainUIWindows,10601,"","无法这样拆分物品.", "MsgBox_cancelFunc", "")
				return
			end
			
			ReqMoveItem(g_DragItem:getIndex(), g_PT_STORAGE, idxtar, g_PT_WAREHOUSE)
		elseif g_PT_FARMDEPOT == g_DragItem:getPacktype() then
			--ReqMoveItem(-1, g_PT_WAREHOUSE, g_DragItem:getIndex(), g_PT_FARMDEPOT)
			--
			local index = g_DragItem:getIndex()
			local item_instance_id = g_ItemInteractMgr:getItemInsIdStr(index)
			--g_Loger:logError(tostring(item_instance_id))
			--
			IIT_reqSwitchItem(g_PT_FARMDEPOT,item_instance_id,g_PT_WAREHOUSE)
			IIT_submitReqOpenUI()
		end
		Storage_ClickCloseEquipTips()	--	关闭装备提示(hezhr)
	
		--播放放下音效
		local ir = g_itemtableMgr:getItemRow(g_dratItemID)
		if ir then
			EQGlobaClass.getVoiceMgr():play(ir.dvoiceid, false)
			--EQGlobaClass.getVoiceMgr():play(2, false)
--			g_Loger:logMessage("Item dvoiceid: " ..tostring(ir.dvoiceid))
		else
--			g_Loger:logError("ItemRow null, DragItemId:"..tostring(g_dratItemID))
		end
	elseif num > 0 then
	--物品吸起来
		--目标格子已锁，返回
		if true == IsLockedGrid(idxtar, EQGlobaClass.ItemPack.PT_WAREHOUSE) then
			return
		end
		
		local itemrow 	= g_itemtableMgr:getItemRow(itemid)
		local canDrag = true
		local IITState = IIT_getIITState()
		
		if IITState == 1 then
			if itemrow ~= nil then
				if itemrow.bind == 5 then
					canDrag = true
				else
					canDrag = false
				end
			else
				--加载资源失败
			end
		else
			canDrag = true
		end
		--g_Loger:logError("itemRow.id*******" ..tostring(itemid))
		--g_Loger:logError("canDrag*******" ..tostring(canDrag))
		--g_Loger:logError("itemRow.bind*******" ..tostring(itemrow.bind))
		if canDrag == true then
			g_DragItem:drag(SlotWin:getName(),idxtar,g_PT_WAREHOUSE)
			g_dratItemID = itemid	--保存吸起来的物品模板id(hezhr)
			--g_Loger:logDebug("idxtar = "..idxtar)
			--播放抓取音效
			if itemRow then
				EQGlobaClass.getVoiceMgr():play(itemRow.uvoiceid, false)
				--EQGlobaClass.getVoiceMgr():play(1, false)
--				g_Loger:logMessage("Item uvoiceid: " ..tostring(itemRow.uvoiceid))
			end
		else
			--g_Loger:logError("Can't darg this item!")
			MsgBox_createMsgBox(g_MainUIWindows,0,"消息提示","此物品无法移动。", "MsgBox_cancelFunc", "")
		end
	end

	--准备拆分物品
	if g_isKeydown(g_KC_LSHIFT) or (g_CursorMgr:getState() == g_CS_SPLIT_ITEM) then
		if num > 0 then
			if g_DragItem:getSplitNum() == 0 then
				gird = g_mainplayer:getItemGird(idxtar % C_STORAGE_GIRD_NUM,EQGlobaClass.ItemPack.PT_WAREHOUSE)
				SplitItem(gird.mOverlay)
				
				EQGlobaFunc.setKeyFlag(g_KC_LSHIFT, false)
			end
		else
			if(g_CursorMgr:getState() == g_CS_SPLIT_ITEM) then
				g_CursorMgr:backtoLastState()
			end
		end
	end
end
--
function ReqDeleltItem()
	g_DragItem:undo()
	g_req_delete_item.grid_index = g_DragItem:getIndex()
	g_msgmgr:send(g_req_delete_item)
	NormalCancel()
end
function CancelDelete()
	g_DragItem:undo()
	NormalCancel()
end
------物品拖放到删除界面上----------------------------------------
function DragItemToDestoryBtn(args)
	Storage_ClickCloseEquipTips()
	if g_DragItem:isDraging() and g_DragItem:getSplitNum() == 0 then
		if g_DragItem:getPacktype() == g_PT_WAREHOUSE then
			local itemid 	= g_mainplayer:getItemID(g_DragItem:getIndex(),g_PT_WAREHOUSE)
			local itemrow 	= g_itemtableMgr:getItemRow(itemid)
			g_DragItem:setVisible(false)
			MessageBox("丢弃该物品后将无法恢复，确定\n要丢弃吗？","ReqDeleltItem","CancelDelete")
		else
			g_Loger:logError("不是背包物品")
		end
	end
end
------物品拖放到删除界面上----------------------------------------
function DragDropToDestoryBtn(args)
--	g_Loger:logMessage("DragDropToDestoryBtn")
	local DragItem 	= CEGUI.toDragDropEventArgs(args).dragDropItem
	local idxsrc 	= getStorageIdx(DragItem)

	if idxsrc ~= nil then
		g_req_delete_item.grid_index = idxsrc
		g_msgmgr:send(g_req_delete_item)
	else
		g_Loger:logError("DragDropToDestoryBtn:idxsrc == nil")
	end

end
------判断物品是否已锁-------------------------------------------
function IsLockedGrid(index, type)
	index = index % C_STORAGE_GIRD_NUM
	local grid = g_mainplayer:getItemGird(index, type)
	if 1 == grid.mLock then
		return true
	else
		return false
	end
end


-----物品拖放到背包slot里处理，slot里没有子窗口
-----目前有背包内部移动和装备栏拖来物品的处理
function DroppedToStorageSlot(args)
--	g_Loger:logMessage("DroppedToStorageSlot")
	Storage_ClickCloseEquipTips()
	local DragItem 	= CEGUI.toDragDropEventArgs(args).dragDropItem
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	local num 		= SlotWin:getChildCount()

	if num == 0 then --背包slot里有子窗口，这步是否有必要？
--		g_Loger:logMessage("DroppedToStorageSlot: num == 0")
		local idxsrc = getStorageIdx(DragItem)
		local idxtar = getStorageIdxByslot(SlotWin)

		if idxsrc == nil then--不是背包内拖过来的物品，目前就是装备
			--bmove = true
			idxsrc = getEquipIdx(DragItem)

			if idxsrc ~= nil then
				g_req_move_item.index1 	= idxsrc
				g_req_move_item.index2 	= idxtar
				g_req_move_item.pack1_type= EQGlobaClass.ItemPack.PT_BODY
				g_req_move_item.pack2_type= EQGlobaClass.ItemPack.PT_WAREHOUSE

				g_msgmgr:send(g_req_move_item)
--				g_Loger:logDebug("g_req_move_item")
			else--从交易栏拖到背包中
				--发送消息给服务器
				--取消一个交易栏中的物品
				--g_Loger:logDebug("***********UPPUT************"..idxTrade.."to"..idxtar)

				idxTrade = getTradeIdxByDropItem(DragItem)
				if nil ~= idxTrade then
					g_smallModules:sendUnputItem(idxTrade, idxtar)

				else
					g_Loger:logError("dx1 == nil. in DroppedToStorageSlot()")
				end
			end
		else
			--背包内交换物品存放格子
			g_req_swap_item.index1 = idxsrc
			g_req_swap_item.index2 = idxtar

			g_msgmgr:send(g_req_swap_item)
--			g_Loger:logDebug("g_req_swap_item")
		end
		--g_Loger:logMessage("idxsrc = "..idxsrc.."idxtar = "..idxtar)
	else
		g_Loger:logError("DroppedToStorageSlot: num ~= 0")
	end
end
-----物品拖放到背包slot下子窗口container里的处理，表示本slot内有物品
-----包括背包内拖来的物品，装备栏拖过来的物品应该也在这里处理合理
function ItemExchWithStorage(args)
--	g_Loger:logDebug("DragEnded: ItemExchWithStorage()")
	--local DragItem 	= CEGUI.toDragDropEventArgs(args).dragDropItem
	local DragItem = CEGUI.toWindowEventArgs(args).window
--	g_Loger:logDebug(" CEGUI.toWindowEventArgs(args).window	 name: "..DragItem:getName())
	if g_ExchgFlag == true then
--		g_Loger:logDebug("g_ExchTargetWin name: "..g_ExchTargetWin:getName())

		local idxsrc = getStorageIdx(DragItem)
		local idxtar = getStorageIdx(g_ExchTargetWin)


		if nil == idxsrc then--从装备栏拖到背包

			idxsrc = getEquipIdx(DragItem)
			local idxTrade = getTradeIdxByDropItem(DragItem) --交易栏Index

			if idxsrc ~= nil then
				if idxtar ~= nil then
					EquipToStorage(idsrc,idtar)
				else
					g_Loger:logError("idxsrc = getEquipIdx(DragItem) == nil")
				end
			else	--从交易栏拖到背包中
				--if true ==  g_dropEnterStorageUI then
					if nil ~= idxTrade then
						if nil ~= idxtar then
							--发送消息给服务器
							--取消一个交易栏中的物品
							g_smallModules:sendUnputItem(idxTrade, idxtar)
						else
							g_Loger:logError("idxsrc == nil. in ItemExchWithStorage()")
						end
				  end
			  --  else
					--g_Loger:logError("idxsrc == nil. 1in ItemExchWithStorage()")
				--end
			end

		elseif idxtar == nil then--从背包拖到装备栏

			idxtar = getEquipIdx(g_ExchTargetWin)

			if idxtar ~= nil then
				equipItem(idxsrc,idxtar)
			else
				g_Loger:logError("idxtar == nil. in ItemExchWithStorage()")
			end

		else--背包内部交换
--			g_Loger:logMessage("idxsrc = "..idxsrc.."idxtar = "..idxtar)
			SwapItem(idxsrc,idxtar)
		end
	else
			g_Loger:logError("ItemExchWithStorage: g_ExchgFlag ~= true")
	end

	RetSet_gExchgFlag()
end


-----背包slot是否有子窗口（物品）---------------------------------
function StorageSlotHasChild(args)
	local SlotWin = CEGUI.toWindowEventArgs(args).window
	local DragItem 	= CEGUI.toDragDropEventArgs(args).dragDropItem
	DragItem:setZOrderingEnabled(true)
	DragItem:moveToFront()

--	g_Loger:logDebug("function StorageSlotHasChild(args)")
--	g_Loger:logDebug("window name = "..SlotWin:getName())

	g_ExchTargetWin = nil

	if SlotWin:getChildCount() == 0 then
--		g_Loger:logDebug("window name = "..SlotWin:getName().."has no child.")
		g_ExchgFlag = false
	else
--		g_Loger:logDebug("window name = "..SlotWin:getName().."has ".. SlotWin:getChildCount().." child.")
		g_ExchgFlag = true
		g_ExchTargetWin = SlotWin:getChildAtIdx(0)

--		g_Loger:logDebug("()g_ExchTargetWin name = "..SlotWin:getChildAtIdx(0):getName())
		--调试信息
--		g_Loger:logDebug("g_ExchTargetWin childCount = "..g_ExchTargetWin:getChildCount())
		for idx=0,g_ExchTargetWin:getChildCount() - 1 do
			if idx >= 1 then
--				g_Loger:logError("g_ExchTargetWin: "..idx.." = "
--									..g_ExchTargetWin:getChildAtIdx(idx):getName())
			else
--				g_Loger:logDebug("g_ExchTargetWin: "..idx.." = "
--									..g_ExchTargetWin:getChildAtIdx(idx):getName())
			end
		end
	end
end
----通过slot返回背包的索引----------------------------------------
function getStorageIdxByslot(slot)
	local name = slot:getName()
	local tmpcontianer

	for idx=C_STORAGE_GIRD_NAME_START,C_STORAGE_GIRD_NUM do
		--g_Loger:logMessage("name = "..name)
		tmpcontianer = g_storageslot.none..idx
		if nil == string.find(tmpcontianer,name) then

		else
--			g_Loger:logMessage("idx = "..idx)
			return (idx-1)
		end
	end
end
-----通过containerwin返回背包的索引---------------------------------
function getStorageIdx(ContainerWin)
	local name = ContainerWin:getName()
	local tmpcontianer

	for idx=C_STORAGE_GIRD_NAME_START,C_STORAGE_GIRD_NUM do
		--g_Loger:logMessage("name = "..name)
		tmpcontianer = g_storageslot.none..idx.."/DragContainer"..idx
		if nil == string.find(name, tmpcontianer) then

		else
--			g_Loger:logMessage("idx = "..idx)
			return (idx-1)
		end
	end
--	g_Loger:logDebug("no found ContainerWin:"..name.."at storage.")
end
-----通过containerwin返回背包的索引---------------------------------
function getStorageIdxByName(name)
	local tmpcontianer

	for idx=C_STORAGE_GIRD_NAME_START,C_STORAGE_GIRD_NUM do
		--g_Loger:logMessage("name = "..name)
		tmpcontianer = g_storageslot.none..idx.."/DragContainer"..idx
		if nil == string.find(name, tmpcontianer) then

		else
--			g_Loger:logMessage("idx = "..idx)
			return (idx-1)
		end
	end
--	g_Loger:logDebug("no found ContainerWin:"..name.."at storage.")
end
--避免物体托出背包界面外时产生的bug-------------------------------------
function RetSet_gExchgFlag()
--	g_Loger:logMessage("RetSet_gExchgFlag")
	g_ExchgFlag = false
end
--
function StorageGridOnDrag(idx)
	local dragidx = g_DragItem:getIndex()
	if idx == dragidx and g_DragItem:isDraging() then
		return true
	end
	
	return false
end
---ConHasChild
function ConHasChild(args)

	local conwin = CEGUI.toWindowEventArgs(args).window

--	g_Loger:logDebug("function ConHasChild(args)")
--	g_Loger:logDebug("conwin name = "..conwin:getName())

	if conwin:getChildCount() == 0 then
--		g_Loger:logDebug("window name = "..conwin:getName().."has no child.")
		g_ExchgFlag = false
	else
--		g_Loger:logDebug("window name = "..conwin:getName().."has ".. conwin:getChildCount().." child.")

		g_ExchgFlag = true
		g_ExchTargetWin = conwin

--		g_Loger:logDebug("()g_ExchTargetWin name = "..conwin:getName())
		--调试信息
--		g_Loger:logDebug("g_ExchTargetWin childCount = "..g_ExchTargetWin:getChildCount())
		for idx=0,g_ExchTargetWin:getChildCount() - 1 do
			if idx >= 1 then
--				g_Loger:logError("g_ExchTargetWin: "..idx.." = "
--									..g_ExchTargetWin:getChildAtIdx(idx):getName())
			else
--				g_Loger:logDebug("g_ExchTargetWin: "..idx.." = "
--									..g_ExchTargetWin:getChildAtIdx(idx):getName())
			end
		end
	end

end

-------------------------------------------------------------------------------------
--设置背包中的钱
function SetStorageMoney(money)
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/Txt_GoldBox"):setText(money)
end
-------更新背包相关UI显示------------------------------------------------------------------------------
function OnUpdateUI()

--	g_Loger:logDebug("UPDATE_STORAGE_UI...")
	--消除背包家装的红框
	AbortFitMode()
	--更新页面状态显示
	UpdatePageState()

	--更新背包中的钱
	local money  = g_mainplayer:getGameGold()
	SetStorageMoney(money)

	local fitstorIdx = 1
	-----
	for i=C_STORAGE_GIRD_NAME_START,C_STORAGE_GIRD_NUM do
		dropfitItem(i)
	end



	for i=0,C_STORAGE_GIRD_NUM - 1 do
		local itemid 	= g_mainplayer:getItemID(i,EQGlobaClass.ItemPack.PT_WAREHOUSE)

		------
		local gird
		--mLock == 1 锁定格子中的容器 否则解锁
		gird = g_mainplayer:getItemGird(i,EQGlobaClass.ItemPack.PT_WAREHOUSE)

		if itemid > 0 then -- 有物品时

			local itemrow 	= g_itemtableMgr:getItemRow(itemid)

			if itemrow ~= nil then

				local overlay = gird.mOverlay
				if itemrow.overlap == 1 then
					overlay = -1
				end
				local itemEnabled = "True"
				local IITState = IIT_getIITState()
				if IITState == 1 then
					--g_Loger:logError("$**********itemInteracting*********&")
					if itemrow.bind == 5 then
						--g_Loger:logError("$**********this item can interact*********&")
						itemEnabled = "True"
					else
						--g_Loger:logError("$**********this item can't interact*********&")
						itemEnabled = "False"
					end
				else
					--g_Loger:logError("$**********not in itemInteract state*********&")
					itemEnabled = "True"
				end

				StorageUI_DynamicLoadItem(g_storageslot.none..(i+1),
											"newImg",
											(i+1),
											itemrow.imageset,
											itemrow.icon,"True",overlay,itemEnabled)
				--家装模式下显示的背包家具
				if itemrow.type == 2 then
					StorageUI_DynamicLoadItem(g_fitstorageslot.none..(fitstorIdx),
											"newImg",
											(fitstorIdx),
											itemrow.imageset,
											itemrow.icon, "True",overlay,itemEnabled)


					--调用fitmode lua的接口设置家装背包索引对应到背包的索引
					fitAdd(fitstorIdx-1,GetGlobalBagidx(i))
					fitstorIdx = fitstorIdx + 1
				end

			else
				--dropItem(i)
			end
		else
			dropItem(i+1)
		end

		if gird.mLock ~= 1 then
			--g_Loger:logDebug("UNLockStorageGrid..."..i)
			lockunlockGird(true, i + C_STORAGE_GIRD_NAME_START, EQGlobaClass.ItemPack.PT_WAREHOUSE)
		else
			--g_Loger:logDebug("LockStorageGrid..."..i)
			lockunlockGird(false, i + C_STORAGE_GIRD_NAME_START, EQGlobaClass.ItemPack.PT_WAREHOUSE)
		end
	end
	--最后一个标识为-1
	fitAdd(fitstorIdx,-1)
	--更新礼物背包界面
	UpdateGiftStorage()
end
-------去掉slot下的容器-------------------------------------------------------------------------------
function dropItem(index)

	--g_Loger:logDebug("dropItem...idx = "..index)

	local str 		= "CkFrmWnd/BgPanel/Slot"
	local slotName 	= str..index
	local slotWin 	= g_WinMgr:getWindow(slotName)

	if slotWin ~= nil then
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
			--TempSource:unSubscribeEvent("DragEnded")
			--TempSource:unSubscribeEvent("DragDropItemEnters", "SlotHasChild")
			--TempSource:unSubscribeEvent("DragDropItemLeaves","ReSetEquipExchangeFlag")
		end
	end
end
-------去掉slot下的容器-------------------------------------------------------------------------------
function dropfitItem(index)
	if index > C_STORAGE_GIRD_NUM then
		return
	end
	--g_Loger:logDebug("dropfitItem = "..index)

	local slotName 	= g_fitstorageslot.none..index
	local slotWin 	= g_WinMgr:getWindow(slotName)

	if slotWin ~= nil then
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
		end
	end
end
--背包打开---------------------------------------
local firstOpenStorageUI= true
function StorageUI_OnOpenStorage()
	if false == g_StorageUIFlag then
		ReqOpenPackUI()
		g_playerPropertyBtnClicked = true
		if not g_openDealFlag then
			UIM_DoOpenAndCloseUI(g_PackUINames.root)
		end
		if firstOpenStorageUI then
			do_task_script("script6")	-- (打开背包)执行新手任务脚本6(add by hezhr)
			firstOpenStorageUI = false
		end
	else
		StorageUI_OnCloseStorage()
	end
end

function PopPackUI()
	OpenPackUI()
	if g_isOpenStorage then
		g_Storage:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5, 0), CEGUI.UDim(0.2, 0)))
	end
end

function ReqOpenPackUI()
	g_req_open_ui.type = g_uitype.uit_bag
	g_msgmgr:send(g_req_open_ui)
end

function OpenPackUI()
	UIM_OpenWindow(g_PackUINames.root)
	g_StorageUIFlag = true
	SetTwoUIPosition("PlayerProperty",g_PackUINames.root)
end
---------------------
--
function SetTwoUIPosition(first_name,second_name)
	local first_win = g_WinMgr:getWindow(first_name)
	local second_win = g_WinMgr:getWindow(second_name)
	local first_win_width = first_win:getWidth():asAbsolute(800)
	local first_win_height = first_win:getHeight():asAbsolute(600)
	first_win:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-first_win_width),CEGUI.UDim(0.5,-first_win_height/2)))
	second_win:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,0),CEGUI.UDim(0.5,-first_win_height/2)))
end
function ClosePackUI()
	UIM_CloseWindow(g_PackUINames.root)
	UIM_CloseMutexUI(g_PackUINames.root)
	IIT_closeRemoveMoneyUI()
	g_StorageUIFlag = false
end
--背包关闭-----------------------------------------
function StorageUI_OnCloseStorage()
	ClosePackUI()
	
	g_CursorMgr:setState(g_CS_NORMAL)
	if g_DragItem:isDraging() then
		g_DragItem:undo()
	end
end
--脚本加载后执行------------------------------------------------------------------------
function StorageUI_Onload()
	--回收箱
	g_Btn_Destroy 	= g_WinMgr:getWindow("CkFrmWnd/BgPanel/Btn_Destroy")
	--g_Btn_Destroy:subscribeEvent("DragDropItemDropped","DragDropToDestoryBtn")
	g_Btn_Destroy:subscribeEvent("MouseClick", "DragItemToDestoryBtn")


	--  屏蔽界面鼠标穿透处理
	g_WinMgr:getWindow("CkFrmWnd__auto_titlebar__"):setWantsMultiClickEvents(false)
	MainUI_ShieldClick(g_Storage)
	Common_injectEmptyEvt("CkFrmWnd")
	local StorageSize = g_Storage:getSize()
	g_Storage:setMaxSize(StorageSize)
	g_Storage:setMinSize(StorageSize)
	--  开启关闭界面
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Storage"):
						subscribeEvent("Clicked", "StorageUI_OnOpenStorage")
	g_WinMgr:getWindow("CkFrmWnd__auto_closebutton__"):
						subscribeEvent("Clicked", "StorageUI_OnCloseStorage")

	--g_Storage:subscribeEvent("DragDropItemLeaves", "RetSet_gExchgFlag")

	-----绑定UI更新事件----------------------------------------------
	g_GameEventMgr:subscribeEvent("UPDATE_STORAGE_UI","OnUpdateUI")
	-- SlotLoop 背包20个格子(SLot)
	local str = "CkFrmWnd/BgPanel/Slot"
	local i
	for i = C_STORAGE_GIRD_NAME_START,C_STORAGE_GIRD_NUM do
		local SlotName = str..i
		--g_WinMgr:getWindow(SlotName):
		--	subscribeEvent("DragDropItemDropped", "DroppedToStorageSlot")
		--g_WinMgr:getWindow(SlotName):
		--	subscribeEvent("DragDropItemEnters", "StorageSlotHasChild")

		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "StorageClickSlot")--"selectItemFram")
		g_WinMgr:getWindow(SlotName):removeEvent("MouseEnter")
		g_WinMgr:getWindow(SlotName):removeEvent("MouseLeave")
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseEnter", "Storage_MouseEnter")
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseLeave", "Storage_MouseLeave")

		--g_WinMgr:getWindow(SlotName):
		--subscribeEvent("DragDropItemLeaves", "RetSet_gExchgFlag")
	end

	--包裹扩容，切换
	for i=1,4  do
		g_WinMgr:getWindow(g_StoragePage[i]):
			subscribeEvent("MouseClick", "BagResizeBtnClicked")	
	end

	g_WinMgr:getWindow("CkFrmWnd/BgPanel/chaifen"):
		subscribeEvent("Clicked", "chaifenClicked")
	--更新EQ币
	g_GameEventMgr:subscribeEvent("UPDATE_PLAYER_GOLD", "updateGameGoldUI")
end
--
function chaifenClicked()
	if g_DragItem:isDraging() then
		return
	end

	if(g_CursorMgr:getState() == g_CS_SPLIT_ITEM) then
		g_CursorMgr:backtoLastState()
	else
		g_CursorMgr:setState(g_CS_SPLIT_ITEM)
	end
end
--
function updateGameGoldUI()
--	g_Loger:logDebug("gamegold......" ..g_mainplayer:getGameGold())
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/eq_txt"):setText(g_mainplayer:getEQGold())
	g_WinMgr:getWindow("DFwnd_MainRoot/tip/touxiang3/gold_txt"):setText(g_mainplayer:getGameGold())
	g_WinMgr:getWindow("CkFrmWnd/BgPanel/Txt_GoldBox"):setText(g_mainplayer:getGameGold())
end
--
function UpdatePageState()
	local curpage = g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
	local pagenum = g_mainplayer:getPackPageCount(g_PT_WAREHOUSE)

	for page=1,pagenum do
		--if page == curpage then
			--g_WinMgr:getWindow(g_StoragePage[curpage]):
				--setProperty("NormalImage",g_StoragePageCur)
			--g_WinMgr:getWindow(g_StoragePage[curpage]):
				--setProperty("HoverImage",g_StoragePageCur)
		--else
			--g_WinMgr:getWindow(g_StoragePage[page]):
				--setProperty("NormalImage",g_StoragePageActive)
			--g_WinMgr:getWindow(g_StoragePage[page]):
				--setProperty("HoverImage",g_StoragePageActive)
		--end
		
		if page == curpage then
			g_WinMgr:getWindow(g_StoragePage[curpage]):setProperty("NormalImage",g_StoragePageActive)
			g_WinMgr:getWindow(g_StoragePage[curpage]):setProperty("HoverImage",g_StoragePageActive)
			g_WinMgr:getWindow(g_StoragePage[curpage]):setProperty("PushedImage",g_StoragePageActive)
		else
			g_WinMgr:getWindow(g_StoragePage[page]):setProperty("NormalImage",g_StoragePageOpenNormal)
			g_WinMgr:getWindow(g_StoragePage[page]):setProperty("HoverImage",g_StoragePageOpenHover)
		end
	end
	
	FitmodeUpdateBagUI()
end

function SwapItem(idxsrc,idxtar)
	local gird = g_mainplayer:getItemGird(idxtar,EQGlobaClass.ItemPack.PT_WAREHOUSE)
	if gird.mLock ~= 1 then
		g_req_swap_item.index1 = idxsrc
		g_req_swap_item.index2 = idxtar
		g_msgmgr:send(g_req_swap_item)
	else
		--gLoger:
	end
end

-----------------------------------------
--显示我的背包itemhint
-----------------------------------------
function Storage_MouseEnter(args)
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	local idxtar 	= getStorageIdxByslot(SlotWin)
	local curPage 	= g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
	idxtar = idxtar + (curPage - 1) * C_STORAGE_GIRD_NUM
	
	local itemid 	= g_mainplayer:getItemIDByGlobaIdx(idxtar,g_PT_WAREHOUSE)
	--g_Loger:logError("itemid***" ..tostring(itemid))
	if 0 ~= itemid then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_mainplayer:getItemDelTimeByGlobalIdx(idxtar,g_PT_WAREHOUSE)
		createItemhint_shop(itemid, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
-----------------------------------------
--隐藏itemhint
-----------------------------------------
function Storage_MouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
-----------------------------------------
--根据id获取类型为服饰的物品的装备位置
-----------------------------------------
function Storage_GetClothesEquipPos(itemid)
	local itemrow = g_itemtableMgr:getItemRow(itemid)
	if nil == itemrow then
		return -1
	end
	if 1 == itemrow.type then	-- 物品为服饰类型
		local itemEquitRow = g_itemtableMgr:getItemEquipRow(itemrow.subID)	-- 获取物品服装表
		return itemEquitRow.equip_pos
	end
	return -1
end
-----------------------------------------
--获取装备提示动画宽、高
-----------------------------------------
local function _EquipmentTips_GetWH()
	local slotName = "PlayerProperty/PropertyTabCtrl/Property/Slot1"
	local slotWin = g_WinMgr:getWindow(slotName)
	if nil == slotWin then
		return
	end
	local width = slotWin:getWidth():asAbsolute(g_GuiSystem:getRenderer():getRect():getWidth())
	local height = slotWin:getHeight():asAbsolute(g_GuiSystem:getRenderer():getRect():getHeight())
	return width, height
end
-----------------------------------------
--装备提示动画初始化
-----------------------------------------
local function _EquipmentTips_AnimateInit(animate)
	local w, h = _EquipmentTips_GetWH()
	animate:initAnimate(40, w, h)
	local imageset = "LightCue"
	for i=1, 20 do
		local image
		if i >= 10 then
			image = imageset..i
		else
			image = imageset.."0"..i
		end
		animate:addAnimateFrame(imageset, image)
	end
end
-- 该调用放在函数_EquipmentTips_AnimateInit()定义后面
_EquipmentTips_AnimateInit(tipsAni)
-----------------------------------------
--点击类型为服饰的物品，在装备栏相应位置提示
-----------------------------------------
function Storage_ClickOpenEquipTips(id)
	local slotName = "PlayerProperty/PropertyTabCtrl/Property/Slot"
	tipsPare = g_WinMgr:getWindow(slotName..id)
	if nil == tipsPare then
		return
	end
	local width, height = _EquipmentTips_GetWH()
	-- 相应的提示表现形式(暂定)
	local tips = g_WinMgr:getWindow(slotName.."/tips")
	if nil == tips then
		tips = g_WinMgr:createWindow("EQHomeLook/AnimateBox", slotName.."/tips")
		tips:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
		tips:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 0), CEGUI.UDim(0, 0)))
		EQGlobaFunc.toAnimateBox(tips:getName()):addAnimate(tipsAni)
		EQGlobaFunc.toAnimateBox(tips:getName()):play(true)	-- 循环播放
	end
	tipsPare:addChildWindow(tips)
end
-----------------------------------------
--放下物品，隐藏装备提示
-----------------------------------------
function Storage_ClickCloseEquipTips()
	local slotName = "PlayerProperty/PropertyTabCtrl/Property/Slot"
	local tips = g_WinMgr:getWindow(slotName.."/tips")
	if nil == tips then
		return
	end
	tipsPare:removeChildWindow(tips)
end
-----------------------------------------
--点击超链接按钮
-----------------------------------------
function ClickHyperLinkBtn(args)
	do_task_script("script8")	-- (超链接按钮使用帮助,不属于新手任务)执行新手任务脚本8
	if(g_CS_HYPERLINK == g_CursorMgr:getState()) then
		g_CursorMgr:setState(g_CS_NORMAL)
	else
		g_CursorMgr:setState(g_CS_HYPERLINK)
	end
end
-----------------------------------------
--点击超链接物品
-----------------------------------------
function ClickHyperLinkItem(itemid)
	local itemrow = g_itemtableMgr:getItemRow(itemid)
	if nil == itemrow then
		return
	end
	local link = "["..itemrow.name.."]"
	local ChatBox = g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat")
	limitCtrlTextLen(ChatBox, CEGUI.toEditbox(ChatBox):getMaxTextLength(), link)
	g_CursorMgr:setState(g_CS_NORMAL)
	--激活聊天输入栏，并设置光标位置
	g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):activate()
	local len = string.len(g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):getText())
	g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):setCaratIndex(len)
end
-----------------------------------------
--物品时效已到，处理点击时的情况
-----------------------------------------
function handleEffTimeWhenClicked()
	if(g_DragItem:isDraging()) then
		if false == g_mainplayer:isInItemPack(g_dratItemID) then
			g_DragItem:undo()
		end
	end
end
