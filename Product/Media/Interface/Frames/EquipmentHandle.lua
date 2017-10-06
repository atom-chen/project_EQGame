-----------------------------------------
-- 文件说明	：装备栏事件相关处理
-- 作者		：麦
-- 修改		:
-- 日期		：10/03/25
-----------------------------------------
local m_quick_click_equipPos = -1
--
----------------------------------------
--
function Equipment_setEquipPos(slot_id)
	m_quick_click_equipPos = slot_id
end
------------------------------------------
--
local function _Equip(SlotWin,args)
	local idxtar 	= getEquipIdx(SlotWin)
	local itemid = g_mainplayer:getItemID(idxtar,EQGlobaClass.ItemPack.PT_BODY)
	local num 		= SlotWin:getChildCount()
	local button 	= CEGUI.toMouseEventArgs(args).button

	if button == 1 then
		return
	end
	
	--	超链接(hezhr)
	if g_CS_HYPERLINK == g_CursorMgr:getState() then
		ClickHyperLinkItem(itemid)
		return
	end
	
	if num == 0 then--目标没物品 
  --{
		if(g_DragItem:isDraging()) then--穿上装备
			if g_DragItem:getPacktype() == g_PT_WAREHOUSE then
				equipItem(g_DragItem:getIndex(),idxtar)
			end
		end
  --}	
	elseif num > 0 then--目标有物品
  --{
		local isDrag = false
		if(g_DragItem:isDraging()) then--
	  --{
			if g_DragItem:getPacktype() == g_PT_WAREHOUSE then--换装
				equipItem(g_DragItem:getIndex(),idxtar)
				do_task_script("script7")	-- (換裝)执行新手任务脚本7(add by hezhr)
				PlayerGuide_do_NewerTeachTaskScript(15014) --新手教学任务(提示玩家使用照相功能)
			elseif g_DragItem:getPacktype() == g_PT_BODY then--另外吸一个
				Storage_ClickCloseEquipTips()	--	关闭装备提示(hezhr)
				g_DragItem:undo()
				if idxtar ~= g_DragItem:getIndex() then
					g_DragItem:drag(SlotWin:getName(),idxtar,g_PT_BODY)
					isDrag = true
				end
			end
	  --}	
		else--吸起来
			g_DragItem:drag(SlotWin:getName(),idxtar,g_PT_BODY)
			g_dratItemID = itemid	--保存吸起来的物品模板id(hezhr)
			isDrag = true
		end
		
		if isDrag then
			--播放抓取音效
			local itemRow = g_itemtableMgr:getItemRow(itemid)
			if itemRow then
				EQGlobaClass.getVoiceMgr():play(itemRow.uvoiceid, false)
				--EQGlobaClass.getVoiceMgr():play(1, false)
				--g_Loger:logMessage("Item uvoiceid: " ..tostring(itemRow.uvoiceid))
			end
		end
  --}	
	end	
	
	g_CursorMgr:setState(g_CS_NORMAL)
end
-----------------------------------------
--点击RTT的位置快速换装
function Equipment_clickRttEquip(args)
	local slotName = "PlayerProperty/PropertyTabCtrl/Property/Slot"
	local slotWin = g_WinMgr:getWindow(slotName..m_quick_click_equipPos)
	if slotWin then
		_Equip(slotWin,args)
		m_quick_click_equipPos = -1
	end
end
------物品----------------------------------------
function EQuipClickSlot(args)
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	_Equip(SlotWin,args)
end

--左键仓库往人物属性装备栏拖拽处理---------------------------------------------
function DragDropItemToEquip(args)
	--g_Loger:logMessage("DragDropItemToEquip ")
	local DragItem 		= CEGUI.toDragDropEventArgs(args).dragDropItem
	local SlotWin 		= CEGUI.toWindowEventArgs(args).window
	local SlotName 		= SlotWin:getName()

	--g_Loger:logMessage(" CEGUI.toWindowEventArgs(args).window =  "..SlotName)

	if SlotWin:getChildCount() == 0 then
		--得到物品在仓库的IDX
		local sidx 			= getStorageIdx(DragItem:getChildAtIdx(0))
		local idxbody 		= getEquipIdx(CEGUI.toWindowEventArgs(args).window)

		if nil ~= sidx then
			equipItem(sidx,idxbody)
		else
			--g_Loger:logDebug("no found item:"..DragItem:getChildAtIdx(0))
		end

	end
end
-------------------------------------------------------------------
function equipItem(sidx,idxbody)
	local isOK = false
	--客户端检验---抽出方法------------------------------------------------------
	local itemid 	= g_mainplayer:getItemIDByGlobaIdx(sidx,EQGlobaClass.ItemPack.PT_WAREHOUSE)
	--local  equipslot= EQGlobaFunc.posToSlot(idxbody)
	if itemid > 0 then -- 有物品时

		local itemrow 	= g_itemtableMgr:getItemRow(itemid)
		if itemrow ~= nil then
			if itemrow.type == 1 then
				local equiprow 	= g_itemtableMgr:getItemEquipRow(itemrow.subID)
				if equiprow ~= nil then
					if equiprow.equip_pos ~= idxbody then
						--g_Loger:logError("equiprow.slot: "..equiprow.equip_pos.." idxbody: "..idxbody )
						--g_Loger:logError("err slot. try again1! itemid = "..itemid)
					else 
						--锁格子
						local itemgrid = g_mainplayer:getItemGird(idxbody,EQGlobaClass.ItemPack.PT_BODY)
			--g_Loger:logError("idxbody =="..idxbody)
						if itemgrid.mLock == 1 then
							--g_Loger:logError("target grid is locked!")
						else
							isOK = true
						end
					end
				else
					--g_Loger:logError("1装备配置表没这个！")
				end
			else
				--g_Loger:logError("2不是装备物品！")
			end
		else
			--g_Loger:logError("3仓库没这个物品！")
		end
	end
	--客户端检验通过了-------------------------------------------------------------
	if isOK then
		Storage_ClickCloseEquipTips()	--	关闭装备提示(hezhr)
		g_DragItem:undo()
		--发消息给服务器
		g_req_use_item.index			= sidx
		--g_req_move_item.index2 		= idxbody
		--g_req_move_item.pack1_type 	= EQGlobaClass.ItemPack.PT_WAREHOUSE
		--g_req_move_item.pack2_type 	= EQGlobaClass.ItemPack.PT_BODY
		g_msgmgr:send(g_req_use_item)
		
		--播放使用音效
		local ir = g_itemtableMgr:getItemRow(itemid)
		if ir then
			EQGlobaClass.getVoiceMgr():play(ir.voiceid, false)
			--EQGlobaClass.getVoiceMgr():play(32, false)
			--g_Loger:logMessage("Item voiceid: " ..tostring(ir.voiceid))
		else
			--g_Loger:logError("ItemRow null, itemid:"..tostring(itemid))
		end
	else
		g_Loger:logError("4仓库没这个物品！")
	end
end
--右键自动装备上---------------------------------------------------------------
function StorageRButtonToEquip(args)
	if g_DragItem:isDraging()==true or g_CS_HYPERLINK==g_CursorMgr:getState() or g_CS_SPLIT_ITEM==g_CursorMgr:getState() then
		return
	end

	--用keyvalue有个问题会出现：当使用了组合键后，将不会恢复，（如：只点右键，key值为2，若按下了ctrl+alt+a等组合键，再点右键，返回的key值将不是2）暂先记录，不深入分析，改用另种方法
	--g_Loger:logError("StorageRButtonToEquip")
	--local keyvalue 	= CEGUI.toMouseEventArgs(args).sysKeys
	local button 	= CEGUI.toMouseEventArgs(args).button
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	local num 		= SlotWin:getChildCount()
	--print(keyvalue)
	--g_Loger:logError(tostring(keyvalue))
	if button == 1 then--右键
		if num == 1 then
			local ItemContainer 	= SlotWin:getChildAtIdx(0)
			local DragItemName 		= ItemContainer:getChildAtIdx(0):getName()
			local curPage 		= g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
			local sidx  			= getStorageIdx(ItemContainer)
			sidx = sidx + (curPage - 1) * C_STORAGE_GIRD_NUM
			local itemid = g_mainplayer:getItemIDByGlobaIdx(sidx,g_PT_WAREHOUSE)
			if itemid ~= nil then
				local itemrow 	= g_itemtableMgr:getItemRow(itemid)
				if itemrow ~= nil then
					local itemType = itemrow.type
					if itemType == 1 then
						--发消息给服务器
						g_req_use_item.index = sidx
						g_msgmgr:send(g_req_use_item)
						do_task_script("script7")	-- (換裝)执行新手任务脚本7(add by hezhr)
						PlayerGuide_do_NewerTeachTaskScript(15014) --新手教学任务(提示玩家使用照相功能)
						--播放使用音效
						local ir = g_itemtableMgr:getItemRow(itemid)
						if ir then
							EQGlobaClass.getVoiceMgr():play(ir.voiceid, false)
							--EQGlobaClass.getVoiceMgr():play(32, false)
							--g_Loger:logMessage("Item voiceid: " ..tostring(ir.voiceid))
						else
							--g_Loger:logError("ItemRow null, itemid:"..tostring(itemid))
						end
					end
				end
			end
			--g_Loger:logError("****" ..tostring(sidx))
		end
	end
end
--  物品右键卸下处理----------------------------------------------------------
function EquipRButtonTakeOff(args)
	local keyvalue 	= CEGUI.toMouseEventArgs(args).sysKeys
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	local num 		= SlotWin:getChildCount()
	local RemoveItem

	if keyvalue == 0x0002 then
		if num ~= 0 then

			RemoveItem 		= SlotWin:getChildAtIdx(0)
			local eqidx 	= getEquipIdx(SlotWin)
			local stidx 	= EquipmentHandle_SearchEmptySlot(RemoveItem)

			if stidx ~= -1 then
				--发消息给服务器
				--g_Loger:logDebug("eqidx..."..eqidx)
				--g_Loger:logDebug("stidx..."..stidx)

				g_req_move_item.index1 = eqidx
				g_req_move_item.index2 = stidx
				g_req_move_item.pack1_type = EQGlobaClass.ItemPack.PT_BODY
				g_req_move_item.pack2_type = EQGlobaClass.ItemPack.PT_WAREHOUSE
				g_msgmgr:send(g_req_move_item)
			else
				g_Loger:logError("EquipRButtonTakeOff--EquipmentHandle_SearchEmptySlot-1")
			end
		else
			--error
		end
	end
end
---------------------------------------------------------------------------
function getEquipIdx(conwin)
	local name = conwin:getName()
	local tmpcontianer

	for idx=1,8 do
		tmpcontianer = g_bodyslot.none..idx

		--g_Loger:logDebug("name = "..name..". tmpcontianer = "..tmpcontianer)
		if nil ~= string.find(name, tmpcontianer) then
			--g_Loger:logDebug("getEquipIdx = "..idx - 1)
			return (idx-1)
		end
	end
end
---------------------------------------------------
function toEquiptype(idx)
	return idx
end
----------------------------------------------------------------------
function EquipSlotHasChild(args)
	local SlotWin = CEGUI.toWindowEventArgs(args).window

	--g_Loger:logDebug("function EquipSlotHasChild(args)")
	--g_Loger:logDebug("window name = "..SlotWin:getName())

	if SlotWin:getChildCount() == 0 then
		--g_Loger:logDebug("window name = "..SlotWin:getName().."has no child.")
		g_ExchgFlag = false
	else
		--g_Loger:logDebug("window name = "..SlotWin:getName().."has ".. SlotWin:getChildCount().." child.")

		g_ExchgFlag = true
		g_ExchTargetWin = SlotWin:getChildAtIdx(0)

		--g_Loger:logDebug("()g_ExchTargetWin name = "..SlotWin:getChildAtIdx(0):getName())
		--调试信息
		--g_Loger:logDebug("g_ExchTargetWin childCount = "..g_ExchTargetWin:getChildCount())
		for idx=0,g_ExchTargetWin:getChildCount() - 1 do
			if idx >= 1 then
				--g_Loger:logError("g_ExchTargetWin: "..idx.." = "
				--					..g_ExchTargetWin:getChildAtIdx(idx):getName())
			else
				--g_Loger:logDebug("g_ExchTargetWin: "..idx.." = "
				--					..g_ExchTargetWin:getChildAtIdx(idx):getName())
			end
		end
	end
end

-----------------------------------------
--  物品拖拽卸下处理
-----------------------------------------
--拖拽卸下
function takeoff(args)
	local SlotWin = CEGUI.toWindowEventArgs(args)
	local SlotName = "PlayerProperty/PropertyTabCtrl/Property/Slot"
	local i
	for i=1,10 do
		--local name = SlotName..i
		--local num = g_WinMgr:getWindow(name):getChildCount()
		--if num == 0 then
		--	if i == 1 then
		--		EQGlobaFunc.RTTtakeOff(0x0001)
		--	end
		--	if i == 2 then
		--		EQGlobaFunc.RTTtakeOff(0x0002)
		--	end
		--	if i == 4 then
		--		EQGlobaFunc.RTTtakeOff(0x0004)
		--	end
		--	if i == 6 then
		--		EQGlobaFunc.RTTtakeOff(0x0008)
		--	end
		--end
	end
end
----------------------------------------------------------------------------
--  查询仓库空白的格子
function EquipmentHandle_SearchEmptySlot(TakeOffItem)
	local str = "CkFrmWnd/BgPanel/Slot"
	local i
	for i = iC_STORAGE_GIRD_NAME_START,C_STORAGE_GIRD_NUM do
		local SlotName = str..i--.."/DragContainer"..i
		local Empty = g_WinMgr:getWindow(SlotName):getChildCount()
		if Empty == 0 then
			--g_WinMgr:getWindow(SlotName):addChildWindow(TakeOffItem)
			return i
		end
	end

	return -1
end
----------------------------------------------------------------------------------
function OnUpdateBodyUI()
	--g_Loger:logDebug("OnUpdateBodyUI...")
	for i=1,8 do
		--锁或解锁格子
		local itemgrid = g_mainplayer:getItemGird(i-1,EQGlobaClass.ItemPack.PT_BODY)
		
		--g_Loger:logDebug("["..i.."] itemgrid.mLock = "..itemgrid.mLock)
		if itemgrid.mLock == 1 then
			lockunlockGird(false,i,EQGlobaClass.ItemPack.PT_BODY)
		else
			lockunlockGird(true,i,EQGlobaClass.ItemPack.PT_BODY)
		end

		local itemid = g_mainplayer:getItemID(i-1,EQGlobaClass.ItemPack.PT_BODY)

		local itemrow

		--g_Loger:logDebug("["..i.."] itemgrid = "..itemid)

		if itemid ~= 0 then
			itemrow = g_itemtableMgr:getItemRow(itemid)
		end

		if itemrow ~= nil then --and itemgrid.mLock ~= 1 then
			id = itemrow.id
			local iconimage = itemrow.icon
			local imageset = itemrow.imageset
			--g_Loger:logDebug("iconimagename..."..iconimage)
			--g_Loger:logDebug("iconimageset..."..imageset)

			StorageUI_DynamicLoadItem(g_bodyslot.none..i,
									"newImg", i,
									imageset,
									iconimage, "True")
		else
			dropbodyItem(i)
		end
	end
end
---
function lockunlockGird(lock,idx, type)
	if type == EQGlobaClass.ItemPack.PT_BODY then
		local slotName = g_bodyslot.none..idx
		local slotWin = g_WinMgr:getWindow(slotName)
		slotWin:setEnabled(lock)

	--锁定格子中的Container	而不是格子
	elseif type == EQGlobaClass.ItemPack.PT_WAREHOUSE then
	
		local slotName 
		
		if 1 == idx then
			 slotName = g_storageslot.none.."1" .."/DragContainer".."1"
		else
		
			 slotName = g_storageslot.none..tostring(idx) .."/DragContainer"..tostring(idx)
		end
	
		local slotWin = g_WinMgr:getWindow(slotName)

		if nil ~= slotWin then
			slotWin:setEnabled(lock)
			if false == lock then
				slotWin:setAlpha(0.4)
			else
				local curPage 	= g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
			
				local conidx = idx + (curPage - 1) * C_STORAGE_GIRD_NUM
				if StorageGridOnDrag(conidx-1) == true then
					slotWin:setAlpha(0.4)
					--g_Loger:logMessage("setAlpha = 0.4 idx="..idx-1)
				else
					slotWin:setAlpha(1)
				end
			end
		end
		
		local testWin = g_WinMgr:getWindow(g_storageslot.none.."1" .."/DragContainer".."1")
		if nil ~= testWin then
			local alpha = testWin:getAlpha()
		end	
	end
end
-------------------------------------------------------------------
function dropbodyItem(index)
	--g_Loger:logDebug("dropbodyItem...idx = "..index - 1)
	local slotName = g_bodyslot.none..index
	local slotWin = g_WinMgr:getWindow(slotName)

	if slotWin ~= nil then
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
		end
	end
end
-------------------------------------------------------------------------

--脚本装载后执行-----------------------------------------------------
function EquipmentHandle_Onload()

	g_GameEventMgr:subscribeEvent("UPDATE_BODY_UI","OnUpdateBodyUI")
	g_WinMgr:getWindow("PlayerProperty/PropertyTabCtrl/Property/PlayerShow"):subscribeEvent("MouseClick","Equipment_clickRttEquip")
	local win
	for idx=1,8 do
		win = g_WinMgr:getWindow(g_bodyslot.none..idx)
		win:subscribeEvent("DragDropItemDropped", "DragDropItemToEquip")
		win:subscribeEvent("RemovedChild", "takeoff")
		win:subscribeEvent("DragDropItemEnters", "EquipSlotHasChild")

		win:subscribeEvent("MouseClick","EQuipClickSlot")
		--win:subscribeEvent("MouseButtonDown", "EquipRButtonTakeOff")
		win:removeEvent("MouseEnter")
		win:removeEvent("MouseLeave")
		win:subscribeEvent("MouseEnter", "Equip_MouseEnter")
		win:subscribeEvent("MouseLeave", "Equip_MouseLeave")
	end
	-----------------------------------------
	--  物品右键装备处理
	-----------------------------------------
	local str = "CkFrmWnd/BgPanel/Slot"
	local i
	local SlotName
	for i = 1,30 do
		SlotName = str..i
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseButtonDown", "StorageRButtonToEquip")
	end

	g_PlayerProUI:subscribeEvent("DragDropItemLeaves", "RetSet_gExchgFlag")
end

-----------------------------------------
--显示人物装备itemhint
-----------------------------------------
function Equip_MouseEnter(args)
	local SlotWin 	= CEGUI.toWindowEventArgs(args).window
	local idxtar 	= getEquipIdx(SlotWin)
	
	local itemid = g_mainplayer:getItemID(idxtar,EQGlobaClass.ItemPack.PT_BODY)
	if 0 ~= itemid then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_mainplayer:getItemDelTime(itemid,EQGlobaClass.ItemPack.PT_BODY)
		createItemhint_shop(itemid, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end

-----------------------------------------
--隐藏itemhint
-----------------------------------------
function Equip_MouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end