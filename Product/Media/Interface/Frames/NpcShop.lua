-----------------------------------------
-- 文件说明	：NPC商店相关逻辑处理
-- 作者		：lf
-- 修改		：
-- 日期		：10/06/29
-----------------------------------------
local sale_type
local goods_type1 = 0
local goods_type2 = 0
local goods_type3 = 0
local page = 0
local sex = 0
local goods_row = nil
local type2_slot = "Npcshoproot/Npcshop/Btn_type1_"
local type3_slot = "Npcshoproot/Npcshop/Btn_type2_"
local goods_tab = "Npcshoproot/Npcshop/Mkt_tab/Tab"
local	goods_slot = 
{
	[1] = "Npcshoproot/Npcshop/Mkt_tab/Tab 1/SlotBg/Slot_",
	[2] = "Npcshoproot/Npcshop/Mkt_tab/Tab 2/SlotBg/Slot_",
	[3] = "Npcshoproot/Npcshop/Mkt_tab/Tab 3/SlotBg/Slot_",
	[4] = "Npcshoproot/Npcshop/Mkt_tab/Tab 4/SlotBg/Slot_",
	[5] = "Npcshoproot/Npcshop/Mkt_tab/Tab 5/SlotBg/Slot_",
}
local preview_checkbox = CEGUI.toCheckbox(g_WinMgr:getWindow("Npcshoproot/Npcshop/Checkbox"))
local preview_pos = g_NpcShop:getPosition() + CEGUI.UVector2(g_NpcShop:getWidth(), CEGUI.UDim(0,22))

local goods_slot_size = 50

--脚本加载后执行------------------------------------------------------------------------
function NpcShop_Onload()

	MainUI_ShieldClick(g_NpcShop)
	Common_injectEmptyEvt("Npcshoproot/Npcshop")
	g_WinMgr:getWindow("Npcshoproot/Npcshop__auto_closebutton__"):subscribeEvent("Clicked", "NpcShop_Close")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Btn_buy"):subscribeEvent("Clicked", "NpcShop_Buy")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Pageup"):subscribeEvent("Clicked", "NpcShop_LastPage")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Pagedown"):subscribeEvent("Clicked", "NpcShop_NextPage")
	g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_male"):subscribeEvent("Clicked", "NpcShop_SelMale")
	g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_famale"):subscribeEvent("Clicked", "NpcShop_SelFemale")
	preview_checkbox:subscribeEvent("CheckStateChanged", "NpcShop_ClickPreview")
	
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab1"):subscribeEvent("Clicked", "NpcShop_ClickPage")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab2"):subscribeEvent("Clicked", "NpcShop_ClickPage")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab3"):subscribeEvent("Clicked", "NpcShop_ClickPage")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab4"):subscribeEvent("Clicked", "NpcShop_ClickPage")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab5"):subscribeEvent("Clicked", "NpcShop_ClickPage")
	
	--CEGUI.toTabControl(g_WinMgr:getWindow ("Npcshoproot/Npcshop/Mkt_tab")):setTabHeight(CEGUI.UDim(0, 28))
	
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Tab_clother"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Tab_furniture"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Tab_electric"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Tab_building"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Tab_item"):setProperty("MousePassThroughEnabled", "true")

	local SlotName
	
	for i=1,8 do
		SlotName = type2_slot..i
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "NpcShop_ClickType2")	
	end
	
	for i=1,8 do
		SlotName = type3_slot..i
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "NpcShop_ClickType3")	
	end
	
	local j
	for j=1,5 do
		for i=1,goods_slot_size do
			SlotName = goods_slot[j]..i
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "NpcShop_ClickGoods")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "Shop_SetSelectBox")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseEnter")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseLeave")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseEnter", "NpcShop_GoodsMouseEnter")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseLeave", "NpcShop_GoodsMouseLeave")
		end
	end
	
	--更新EQ币
	g_GameEventMgr:subscribeEvent("UPDATE_PLAYER_GOLD", "NpcShop_UpdateGold")
end

-----------------------------------------
--更新金币
-----------------------------------------
function NpcShop_UpdateGold()
	g_WinMgr:getWindow("Npcshoproot/Market/Have_background/txt1"):setText(g_mainplayer:getGameGold())
end

-----------------------------------------
--打开
-----------------------------------------
g_NpcFlag = false
function NpcShop_Open(arg)
	if false == g_NpcFlag then
		sale_type = arg
		local row = g_logictableMgr:getNpcShopMgrRow(sale_type)
		if row == nil then
			return
		end
		EQGlobaFunc.SetText("Npcshoproot/Npcshop", row.shop_name)
	
		NpcShop_otherBtnEnbled(false)
		
		type1 = 1
		type2 = 1
		type3 = 1
		page = 1
		goods_row = nil
		
		CEGUI.toTabControl(g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab")):setSelectedTab(0)
		g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab1"):setVisible(row.page1)
		g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab2"):setVisible(row.page2)
		g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab3"):setVisible(row.page3)
		g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab4"):setVisible(row.page4)
		g_WinMgr:getWindow("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab5"):setVisible(row.page5)
		
		page = row.default_page
		NpcShop_SelectPage()
		
		preview_checkbox:setSelected(false)
		Preview_Close()
		NpcShop_UpdatePage()
		
		if g_mainplayer:isMale() == true then
			sex = 1
			NpcShop_SelMale()
		else
			sex = 2
			NpcShop_SelFemale()
		end
		
		g_WinMgr:getWindow("Npcshoproot/Npcshop/Selling_price1"):setText(0)
		
		EQGlobaFunc.RTTSync()
		
		OpenNpcShopUI()
		
		UIM_DoOpenAndCloseUI(g_NPCMarketUINames.root)
	else
		NpcShop_Close()
	end
end

-----------------------------------------
--关闭
-----------------------------------------
function NpcShop_Close()
	CloseNpcShopUI()
	NpcShop_otherBtnEnbled(true)
	
	preview_checkbox:setSelected(false)
	Preview_Close()
end

function OpenNpcShopUI()
	UIM_OpenWindow(g_NPCMarketUINames.root)
	g_NpcFlag = true
end

function CloseNpcShopUI()
	UIM_CloseWindow(g_NPCMarketUINames.root)
	g_NpcFlag = false
end

-----------------------------------------
--选择性别
-----------------------------------------
function NpcShop_SelMale()
	sex = 1
	
	local slotWin = g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_male") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage110")
	
	slotWin = g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_famale") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage109")
	
	NpcShop_SelectType("Npcshoproot/Market/Background3/Btn_male")
	NpcShop_UpdateType3Icon()
	NpcShop_SelectType(type3_slot..type3)
	
	if g_mainplayer:isMale() == false then
		preview_checkbox:setSelected(false)
		Preview_Close()
	end
end

function NpcShop_SelFemale()
	sex = 2
	
	local slotWin = g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_male") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage110")
	
	slotWin = g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_famale") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage109")
	
	NpcShop_SelectType("Npcshoproot/Market/Background3/Btn_famale")
	NpcShop_UpdateType3Icon()
	NpcShop_SelectType(type3_slot..type3)
	
	if g_mainplayer:isMale() == true then
		preview_checkbox:setSelected(false)
		Preview_Close()
	end
end

-----------------------------------------
--预览
-----------------------------------------
function NpcShop_ClickPreview()
	if preview_checkbox:isSelected() == true then
		Preview_Open(goods_row, preview_pos)
	else
		Preview_Close()
	end
end

-----------------------------------------
--点击分页
-----------------------------------------
function NpcShop_ClickPage(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	page = tonumber(string.sub(winname, string.len("Npcshoproot/Npcshop/Mkt_tab__auto_TabPane__Buttons__auto_btnTab")+1, string.len(winname)))
	NpcShop_SelectPage()
end

-----------------------------------------
--选择分页
-----------------------------------------
function NpcShop_SelectPage()
	for i=1,5 do
		g_WinMgr:getWindow(goods_tab..i):setVisible(false)
	end
	g_WinMgr:getWindow(goods_tab..page):setVisible(true)
	
	if page == 1 then
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_male"):setVisible(true)
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Male"):setVisible(true)
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_famale"):setVisible(true)
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Famale"):setVisible(true)
	else
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_male"):setVisible(false)
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Male"):setVisible(false)
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Btn_famale"):setVisible(false)
		g_WinMgr:getWindow("Npcshoproot/Market/Background3/Famale"):setVisible(false)
	end
	
	type1 = page
	type2 = 1
	type3 = 1
	NpcShop_UpdateType2Icon()
	NpcShop_UpdateType3Icon()
	
	if page > 1 then
		NpcShop_SelectType(type2_slot..type2)
		NpcShop_SelectType(type3_slot..type3)
	end
end

-----------------------------------------
--点击子分类
-----------------------------------------
function NpcShop_ClickType2(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	type2 = tonumber(string.sub(winname, string.len(type2_slot)+1, string.len(winname)))
	type3 = 1
	NpcShop_UpdateType3Icon()
	goods_row = nil
	
	NpcShop_UpdateType2Icon()
	NpcShop_SelectType(winname)
	NpcShop_SelectType(type3_slot..type3)
end

-----------------------------------------
--点击子子分类
-----------------------------------------
function NpcShop_ClickType3(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	type3 = tonumber(string.sub(winname, string.len(type3_slot)+1, string.len(winname)))
	g_mainplayer:setShopCenterType(type1*100+type2*10+type3, sex, EQGlobaClass.ItemPack.PT_NPCSHOP, sale_type)
	NpcShop_UpdateGoods()
	goods_row = nil
	
	NpcShop_UpdateType3Icon()
	NpcShop_SelectType(winname)
end

-----------------------------------------
--设置选中状态
-----------------------------------------
function NpcShop_SelectType(winname)
	local slotWin = g_WinMgr:getWindow(winname)
	if slotWin == nil then
		return
	end
	
	local tempstr = slotWin:getProperty("PushedImage")
	slotWin:setProperty("NormalImage", tempstr)
	slotWin:setProperty("HoverImage", tempstr)
end

-----------------------------------------
--更新子分类
-----------------------------------------
function NpcShop_UpdateType2Icon()
	if type1 == 0 then
		return
	end
		
	for i=1,8 do
		local slotWin = g_WinMgr:getWindow(type2_slot..i)
		slotWin:setVisible(false)
		slotWin:setProperty("NormalImage", "set: image:")
		slotWin:setProperty("PushedImage", "set: image:")
		slotWin:setProperty("HoverImage", "set: image:")
			
		local iconrow = g_logictableMgr:getShopCenterIconRow(i+type1*10)
		if iconrow ~= nil then
			slotWin:setVisible(true)
			slotWin:setProperty("NormalImage", "set:" ..iconrow.imageset .." image:" ..iconrow.normal)
			slotWin:setProperty("PushedImage", "set:" ..iconrow.imageset .." image:" ..iconrow.push)
			slotWin:setProperty("HoverImage", "set:" ..iconrow.imageset .." image:" ..iconrow.hover)
			EQGlobaFunc.setTooltipProperty(slotWin:getName(), iconrow.tooltip)
		end
	end
end

-----------------------------------------
--更新子子分类
-----------------------------------------
function NpcShop_UpdateType3Icon()
	if type1 == 0 or type2 == 0 then
		return
	end
	
	for i=1,8 do
		local slotWin = g_WinMgr:getWindow(type3_slot..i)
		slotWin:setVisible(false)
		slotWin:setProperty("NormalImage", "set: image:")
		slotWin:setProperty("PushedImage", "set: image:")
		slotWin:setProperty("HoverImage", "set: image:")
			
		local iconrow = g_logictableMgr:getShopCenterIconRow(i+type1*100+type2*10)
		if iconrow ~= nil then
			slotWin:setVisible(true)
			slotWin:setProperty("NormalImage", "set:" ..iconrow.imageset .." image:" ..iconrow.normal)
			slotWin:setProperty("PushedImage", "set:" ..iconrow.imageset .." image:" ..iconrow.push)
			slotWin:setProperty("HoverImage", "set:" ..iconrow.imageset .." image:" ..iconrow.hover)
			EQGlobaFunc.setTooltipProperty(slotWin:getName(), iconrow.tooltip)
		end
	end
	
	g_mainplayer:setShopCenterType(type1*100+type2*10+type3, sex, EQGlobaClass.ItemPack.PT_NPCSHOP, sale_type)
	NpcShop_UpdateGoods()
end

-----------------------------------------
--刷新商城物品
-----------------------------------------
function NpcShop_UpdateGoods()
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Selling_price1"):setText(0)	
	Shop_RemoveSelectBox()
	
	for i=1,goods_slot_size do
		local slotWin = g_WinMgr:getWindow(goods_slot[page]..i)
		if slotWin:getChildCount() > 0 then
			slotWin:removeChildWindow(slotWin:getChildAtIdx(0))
		end
	end
		
	for i=1,goods_slot_size do
		local itemid 	= g_mainplayer:getItemID(i-1,EQGlobaClass.ItemPack.PT_NPCSHOP)
		if itemid > 0 then
			local itemrow = g_itemtableMgr:getItemRow(itemid)	
			if itemrow ~= nil then
				local slotWin = g_WinMgr:getWindow(goods_slot[page]..i)
				StorageUI_DynamicLoadItem(goods_slot[page]..i, "item", i, itemrow.imageset, itemrow.icon, "False")
			end	
		end
	end
	
	NpcShop_UpdatePage()
end

-----------------------------------------
--点击物品
-----------------------------------------
function NpcShop_ClickGoods(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(goods_slot[page])+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_NPCSHOP)
	if row ~= nil then
		goods_row = row
		g_WinMgr:getWindow("Npcshoproot/Npcshop/Selling_price1"):setText(goods_row.price)
		
		if preview_checkbox:isSelected() == true then
			Preview_Open(goods_row, preview_pos)
		end
		
		--if g_MainUIWindows:isChild(g_ExitWindow) == true then
			--EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit","共需"..goods_row.price.."EQ币，是否购买？")
		--end
	end
end

-----------------------------------------
--上页
-----------------------------------------
function NpcShop_LastPage()
	g_mainplayer:setPackLastPage(EQGlobaClass.ItemPack.PT_NPCSHOP)
	NpcShop_UpdateGoods()
	goods_row = nil
end

-----------------------------------------
--下页
-----------------------------------------
function NpcShop_NextPage()
	g_mainplayer:setPackNextPage(EQGlobaClass.ItemPack.PT_NPCSHOP)
	NpcShop_UpdateGoods()
	goods_row = nil
end

-----------------------------------------
--页数
-----------------------------------------
function NpcShop_UpdatePage()
	local curpage = 0
	local pagecount = 0

	curpage = g_mainplayer:getPackCurPage(EQGlobaClass.ItemPack.PT_NPCSHOP)
	pagecount = g_mainplayer:getPackPageCount(EQGlobaClass.ItemPack.PT_NPCSHOP)
		
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Page"):setText(curpage)
	g_WinMgr:getWindow("Npcshoproot/Npcshop/Page1"):setText("/"..pagecount)
end

-----------------------------------------
--购买
-----------------------------------------
function NpcShop_Buy()
	if goods_row ~= nil then
		--g_MainUIWindows:addChildWindow(g_ExitWindow)
		--
		--EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit","共需"..goods_row.price.."金币，是否购买？")
		--g_ExitWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,230), CEGUI.UDim(0,117)))
		--g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-100), CEGUI.UDim(0.5,-30)))
		--g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setSize(CEGUI.UVector2(CEGUI.UDim(0,200), CEGUI.UDim(0,30)))
	--
		--g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):removeEvent("Clicked")
		--g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):subscribeEvent("Clicked", "NpcShop_ConfirmBuy")
		--g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):removeEvent("Clicked")
		--g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):subscribeEvent("Clicked", "NpcShop_CancelBuy")
		--g_ExitWindow:activate()
		
		MsgBox_ShowBuyConfirm(goods_row.price, "金", "NpcShop_ConfirmBuy")
	end
end

-----------------------------------------
--确认购买
-----------------------------------------
function NpcShop_ConfirmBuy()
	--发购买包
	g_req_buy_npc_shop_goods.goods_id = goods_row.good_id
	g_msgmgr:send(g_req_buy_npc_shop_goods)
	
	NpcShop_CancelBuy()
	
	--g_Loger:logDebug("req_buy_npc_shop_goods "..goods_row.good_id)
end

-----------------------------------------
--取消购买
-----------------------------------------
function NpcShop_CancelBuy()
	g_MainUIWindows:removeChildWindow(g_ExitWindow)
end

--使能其他按钮
function NpcShop_otherBtnEnbled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_PlayerProperty"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_House"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Storage"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Mall"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Action"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Friendlist"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_System"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Navigate"):setEnabled(bl)
	if g_sceneMgr:getSceneType() ~= EQGlobaClass.EQGameSceneMgr.SCENE_COMMON and g_isInFriendHouse == false then
		g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):setEnabled(bl)
		g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Storage"):setEnabled(bl)
	end
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PlayGame"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Email"):setEnabled(bl)
	
end

-----------------------------------------
--显示itemhint
-----------------------------------------
function NpcShop_GoodsMouseEnter(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(goods_slot[page])+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_NPCSHOP)
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
function NpcShop_GoodsMouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
