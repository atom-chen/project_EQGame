-----------------------------------------
-- 文件说明	：预览
-- 作者		：lf
-- 修改		：
-- 日期		：11/03/25
-----------------------------------------

local goods_row
local rtt_type = 0
local previewwin = g_WinMgr:getWindow("Root/RTT")

-----------------------------------------
--脚本加载后执行------------------------------------------------------------------------
-----------------------------------------
function Preview_Onload()
	MainUI_ShieldClick(previewwin)

	local left_rotate = g_WinMgr:getWindow("Root/RTT/Btn_left")
	left_rotate:setMouseAutoRepeatEnabled(true)
	left_rotate:setAutoRepeatRate(0.1)
	left_rotate:setWantsMultiClickEvents(false)
	left_rotate:subscribeEvent("MouseButtonDown", "Preview_LeftRotate")
	
	local right_rotate = g_WinMgr:getWindow("Root/RTT/Btn_right")
	right_rotate:setMouseAutoRepeatEnabled(true)
	right_rotate:setAutoRepeatRate(0.1)
	right_rotate:setWantsMultiClickEvents(false)
	right_rotate:subscribeEvent("MouseButtonDown", "Preview_RightRotate")
	
	g_WinMgr:getWindow("Root/RTT/Btn_close"):subscribeEvent("Clicked", "Preview_Close")
	g_WinMgr:getWindow("Root/RTT/Btn_zoomin"):subscribeEvent("Clicked", "Preview_ZoomIn")
	g_WinMgr:getWindow("Root/RTT/Btn_zoomout"):subscribeEvent("Clicked", "Preview_ZoomOut")
	g_WinMgr:getWindow("Root/RTT/Btn_reset"):subscribeEvent("Clicked", "Preview_Reset")
	g_WinMgr:getWindow("Root/RTT/Btn_payfor"):subscribeEvent("Clicked", "Preview_Buy")
end

-----------------------------------------
--显示预览
-----------------------------------------
function Preview_OpenByItem(item_id, pos)
	g_WinMgr:getWindow("Root/RTT/Btn_payfor"):setVisible(false)
	
	local row = g_logictableMgr:getShopCenterRowByItemID(item_id)
	if row ~= nill then
		Preview_Open(row, pos)
	end
	
	--local row = g_logictableMgr:getNpcShopRowByItemID(item_id)
	--if row ~= nill then
	--	Preview_Open(row, pos)
	--end
end

local noinitRtt = 0
function Preview_Open(row, pos)
	Preview_Close()
	
	if row == nil then
		return
	end
	
	if (g_mainplayer:isMale()==true and row.sex==2) or
		(g_mainplayer:isMale()==false and row.sex==1) then
		g_MainUIWindows:removeChildWindow(g_Preview)
		MsgBox_createMsgBox(g_MainUIWindows,0,"","无法预览不同性别的服装!", "MsgBox_cancelFunc", "")
		return
	else
		g_MainUIWindows:addChildWindow(g_Preview)
	end
	
	goods_row = row
	g_WinMgr:getWindow("Root/RTT/Btn_payfor"):setVisible(true)
	previewwin:setPosition(pos)
	
	local itemRow = g_itemtableMgr:getItemRow(row.item_id)
	if itemRow == nil then 
		return
	end

	if itemRow.type == 1 then
		--衣服
		rtt_type = 1
		
		if noinitRtt == 0 then
			EQGlobaFunc.EQStartRTT(0,50000,0)
			EQGlobaFunc.RTTReset()
			local curSex = g_rtt:getAvatar():getAdjuster():getCurrentVal(g_atSex)
	
			if curSex == 1 then
				g_Loger:logDebug("RTTScale Male NpcSHOP****************************************************")
				EQGlobaFunc.RTTScale(0.93,0.93,0.93)--(1,1,1)------
			else
				g_Loger:logDebug("RTTScale FeMale nPCSHOP****************************************************")
				EQGlobaFunc.RTTScale(1.03,1.03,1.03)--(0.9,0.9,0.9)
			end
			EQGlobaFunc.RTTLookAt(0,50082,0)
			noinitRtt = 1
		end
	
		local itemRowEquip = g_itemtableMgr:getItemEquipRow(itemRow.subID)
		if itemRowEquip == nil then
			return
		end

		EQGlobaFunc.RTTSetActive(g_rttAvatarWinName, true)
		previewwin:addChildWindow(g_WinMgr:getWindow(g_rttAvatarWinName))
		EQGlobaFunc.RTTSetAspectRatio(previewwin:getWidth():asAbsolute(640), previewwin:getHeight():asAbsolute(480))
		
		local slot = EQGlobaFunc.posToSlot(itemRowEquip.equip_pos)
		EQGlobaFunc.RTTequipOn(slot, itemRowEquip.equip_disp_id, itemRowEquip.equip_placeholder)
	elseif itemRow.type == 2 then
		--家具
		rtt_type = 2
		
		EQGlobaFunc.RTTFurniture(row.item_id)
		EQGlobaFunc.RTTFurniReset()
		
		EQGlobaFunc.RTTFurniSetAspectRatio(previewwin:getWidth():asAbsolute(640), previewwin:getHeight():asAbsolute(480))
		EQGlobaFunc.RTTSetActive(g_rttFurnitureWinName, true)
		previewwin:addChildWindow(g_WinMgr:getWindow(g_rttFurnitureWinName))
	end
end

-----------------------------------------
--关闭预览
-----------------------------------------
function Preview_Close()
	if previewwin:getChild(g_rttFurnitureWinName) ~= nil then
		EQGlobaFunc.RTTSetActive(g_rttFurnitureWinName, false)
		previewwin:removeChildWindow(g_rttFurnitureWinName)
	end
	
	if previewwin:getChild(g_rttAvatarWinName) ~= nil then
		EQGlobaFunc.RTTSetActive(g_rttAvatarWinName, false)
		previewwin:removeChildWindow(g_rttAvatarWinName)
	end
	
	g_MainUIWindows:removeChildWindow(g_Preview)
end

-----------------------------------------
--左旋
-----------------------------------------
function Preview_LeftRotate()
	if rtt_type == 1 then
		EQGlobaFunc.RTTRotate(18,2)
	elseif rtt_type == 2 then
		EQGlobaFunc.RTTFurniRotate(18)
	end
end

-----------------------------------------
--右旋
-----------------------------------------
function Preview_RightRotate()
	if rtt_type == 1 then
		EQGlobaFunc.RTTRotate(-18,2)
	elseif rtt_type == 2 then
		EQGlobaFunc.RTTFurniRotate(-18)
	end
end

-----------------------------------------
--预览放大
-----------------------------------------
function Preview_ZoomIn()
	if rtt_type == 1 then
		EQGlobaFunc.RTTZoom(0.1)
	elseif rtt_type == 2 then
		EQGlobaFunc.RTTFurniZoom(0.1)
	end
end

-----------------------------------------
--预览缩小
-----------------------------------------
function Preview_ZoomOut()
	if rtt_type == 1 then
		EQGlobaFunc.RTTZoom(-0.1)
	elseif rtt_type == 2 then
		EQGlobaFunc.RTTFurniZoom(-0.1)
	end
end

-----------------------------------------
--预览重置
-----------------------------------------
function Preview_Reset()
	if rtt_type == 1 then
		EQGlobaFunc.RTTReset()
		EQGlobaFunc.RTTSync()
	elseif rtt_type == 2 then
		EQGlobaFunc.RTTFurniReset()
	end
end

-----------------------------------------
--预览购买
-----------------------------------------
function Preview_Buy()
	local str
	if goods_row.money_type == 1 then
		str = "金"
	else
		str = "EQ"
	end

	local itemrow = g_itemtableMgr:getItemRow(goods_row.item_id)
	MsgBox_ShowBuyOneConfirm(itemrow.name, goods_row.price, str, "Preview_ConfirmBuy")
end

-----------------------------------------
--确认购买
-----------------------------------------
function Preview_ConfirmBuy()
	--if goods_row.money_type == 1 then
		--g_req_buy_npc_shop_goods.goods_id = goods_row.good_id
		--g_msgmgr:send(g_req_buy_npc_shop_goods)
	--else
		g_req_buy_sys_shop_goods.goods_id = goods_row.good_id
		g_msgmgr:send(g_req_buy_sys_shop_goods)
	--end
end
