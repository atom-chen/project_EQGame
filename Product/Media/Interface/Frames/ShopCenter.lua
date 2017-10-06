-----------------------------------------
-- 文件说明	：商城相关逻辑处理
-- 作者		：lf
-- 修改		：
-- 日期		：10/05/25
-----------------------------------------
local goods_type1 = 0
local goods_type2 = 0
local goods_type3 = 0
local page = 0
local sex = 0
local goods_row = nil
local type2_slot = "Marketroot/Market/Btn_type1_"
local type3_slot = "Marketroot/Market/Btn_type2_"
local goods_tab = "Marketroot/Market/Mkt_tab/Tab"
local recommend_slot = "Marketroot/Market/Mkt_tab/Tab 1/SlotBg/Slot_"
local goods_slot = 
{
	[2] = "Marketroot/Market/Mkt_tab/Tab 2/SlotBg/Slot_",
	[3] = "Marketroot/Market/Mkt_tab/Tab 3/SlotBg/Slot_",
	[4] = "Marketroot/Market/Mkt_tab/Tab 4/SlotBg/Slot_",
	[5] = "Marketroot/Market/Mkt_tab/Tab 5/SlotBg/Slot_",
	[6] = "Marketroot/Market/Mkt_tab/Tab 6/SlotBg/Slot_",
}
local preview_checkbox = CEGUI.toCheckbox(g_WinMgr:getWindow("Marketroot/Market/Checkbox"))
local preview_pos = g_ShopCenterUI:getPosition() + CEGUI.UVector2(g_ShopCenterUI:getWidth(), CEGUI.UDim(0,22))
local moneytype_combobox = CEGUI.toCombobox(g_WinMgr:getWindow("Marketroot/Market/box"))
local money_type = 0

local recommand_slot_count = 20
local goods_slot_count = 36
local type2_slot_count = 8
local type3_slot_count = 7
local goods_tab_count = 6

-----------------------------------------
--脚本加载后执行------------------------------------------------------------------------
-----------------------------------------
function ShopCenterUI_Onload()
	--推荐界面控件
	MainUI_ShieldClick(g_ShopCenterUI)
	Common_injectEmptyEvt("Marketroot/Market")
	g_WinMgr:getWindow("Marketroot/Market__auto_closebutton__"):subscribeEvent("Clicked", "ShopCenter_Close")
	g_WinMgr:getWindow("Marketroot/Market/Btn_buy"):subscribeEvent("Clicked", "ShopCenter_Buy")
	g_WinMgr:getWindow("Marketroot/Market/Pageup"):subscribeEvent("Clicked", "ShopCenter_LastPage")
	g_WinMgr:getWindow("Marketroot/Market/Pagedown"):subscribeEvent("Clicked", "ShopCenter_NextPage")
	g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_male"):subscribeEvent("Clicked", "ShopCenter_SelMale")
	g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_famale"):subscribeEvent("Clicked", "ShopCenter_SelFemale")
	preview_checkbox:subscribeEvent("CheckStateChanged", "ShopCenter_ClickPreview")
	
	g_WinMgr:getWindow("Marketroot/Market/Mkt_tab__auto_TabPane__Buttons__auto_btnTab1"):subscribeEvent("Clicked", "ShopCenter_ClickPage")
	g_WinMgr:getWindow("Marketroot/Market/Mkt_tab__auto_TabPane__Buttons__auto_btnTab2"):subscribeEvent("Clicked", "ShopCenter_ClickPage")
	g_WinMgr:getWindow("Marketroot/Market/Mkt_tab__auto_TabPane__Buttons__auto_btnTab3"):subscribeEvent("Clicked", "ShopCenter_ClickPage")
	g_WinMgr:getWindow("Marketroot/Market/Mkt_tab__auto_TabPane__Buttons__auto_btnTab4"):subscribeEvent("Clicked", "ShopCenter_ClickPage")
	g_WinMgr:getWindow("Marketroot/Market/Mkt_tab__auto_TabPane__Buttons__auto_btnTab5"):subscribeEvent("Clicked", "ShopCenter_ClickPage")
	g_WinMgr:getWindow("Marketroot/Market/Mkt_tab__auto_TabPane__Buttons__auto_btnTab6"):subscribeEvent("Clicked", "ShopCenter_ClickPage")
	
	CEGUI.toTabControl(g_WinMgr:getWindow ("Marketroot/Market/Mkt_tab")):setTabHeight(CEGUI.UDim(0, 28))
	
	g_WinMgr:getWindow("Marketroot/Market/Tab_commend"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Marketroot/Market/Tab_clother"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Marketroot/Market/Tab_furniture"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Marketroot/Market/Tab_electric"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Marketroot/Market/Tab_building"):setProperty("MousePassThroughEnabled", "true")
	g_WinMgr:getWindow("Marketroot/Market/Tab_item"):setProperty("MousePassThroughEnabled", "true")

	local SlotName
	
	for i=1,type2_slot_count do
		SlotName = type2_slot..i
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "ShopCenter_ClickType2")	
	end
	
	for i=1,type3_slot_count do
		SlotName = type3_slot..i
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "ShopCenter_ClickType3")	
	end
	
	for i=1,recommand_slot_count do
		SlotName = recommend_slot..i
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "ShopCenter_ClickRecommend")	
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "Shop_SetSelectBox")
		g_WinMgr:getWindow(SlotName):removeEvent("MouseEnter")
		g_WinMgr:getWindow(SlotName):removeEvent("MouseLeave")
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseEnter", "ShopCenter_RecommendMouseEnter")
		g_WinMgr:getWindow(SlotName):subscribeEvent("MouseLeave", "ShopCenter_RecommendMouseLeave")
	end
	
	local j
	for j=2,6 do
		for i=1,goods_slot_count do
			SlotName = goods_slot[j]..i
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "ShopCenter_ClickGoods")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseClick", "Shop_SetSelectBox")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseEnter")
			g_WinMgr:getWindow(SlotName):removeEvent("MouseLeave")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseEnter", "ShopCenter_GoodsMouseEnter")
			g_WinMgr:getWindow(SlotName):subscribeEvent("MouseLeave", "ShopCenter_GoodsMouseLeave")
		end
	end
	
	--更新EQ币
	g_GameEventMgr:subscribeEvent("UPDATE_PLAYER_GOLD", "ShopCenter_UpdateGold")
	
	EQGlobaFunc.createComboBoxTextItem(moneytype_combobox:getName(), "全部商品", 0,"")
	EQGlobaFunc.createComboBoxTextItem(moneytype_combobox:getName(), "金币商品", 1,"")
	EQGlobaFunc.createComboBoxTextItem(moneytype_combobox:getName(), "EQ币商品", 2,"")
	moneytype_combobox:setItemSelectState(0,true)
	moneytype_combobox:setReadOnly(true)
	moneytype_combobox:subscribeEvent("ListSelectionAccepted", "ShopCenter_SelectMoneyType")
end

-----------------------------------------
--更新EQ币
-----------------------------------------
function ShopCenter_UpdateGold()
	g_WinMgr:getWindow("Marketroot/Market/Have_background/txt1"):setText(g_mainplayer:getEQGold())
end

-----------------------------------------
--打开
-----------------------------------------
g_ShopFlag = false
function ShopCenter_Open()
	if false == g_ShopFlag then
		ReqOpenShopCenter()
		
		UIM_DoOpenAndCloseUI(g_MarketUINames.root)
	else
		ShopCenter_Close()
	end
end

function OnOpenShopCenter()
	type1 = 0
	type2 = 0
	type3 = 0
	page = 1
	goods_row = nil
	ShopCenter_UpdateRecommend()
	CEGUI.toTabControl(g_WinMgr:getWindow("Marketroot/Market/Mkt_tab")):setSelectedTab(0)
	ShopCenter_SetTypeIconVisible(false)
	preview_checkbox:setSelected(false)
	Preview_Close()
	ShopCenter_UpdatePage()
	
	if g_mainplayer:isMale() == true then
		sex = 1
		ShopCenter_SelMale()
	else
		sex = 2
		ShopCenter_SelFemale()
	end
	
	g_WinMgr:getWindow("Marketroot/Market/Selling_price1"):setText(0)
	
	g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_male"):setVisible(false)
	g_WinMgr:getWindow("Marketroot/Market/Background3/Male"):setVisible(false)
	g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_famale"):setVisible(false)
	g_WinMgr:getWindow("Marketroot/Market/Background3/Famale"):setVisible(false)
	
	Shop_otherBtnEnbled(false)
	
	EQGlobaFunc.RTTSync()
	
	OpenShopCenter()
end

function ReqOpenShopCenter()
	g_req_open_ui.type = g_uitype.sysshop
	g_msgmgr:send(g_req_open_ui)
end

function OpenShopCenter()
	UIM_OpenWindow(g_MarketUINames.root)
	g_ShopFlag = true
	PlayerGuide_do_NewerTeachTaskScript(15001) --新手教学任务
end

function CloseShopCenter()
	UIM_CloseWindow(g_MarketUINames.root)
	g_ShopFlag = false
end

-----------------------------------------
--关闭
-----------------------------------------
function ShopCenter_Close()
	CloseShopCenter()
	Shop_otherBtnEnbled(true)
	preview_checkbox:setSelected(false)
	Preview_Close()
end

-----------------------------------------
--更新货币类型
-----------------------------------------
function ShopCenter_SelectMoneyType()
	money_type = moneytype_combobox:getSelectedItem():getID()
	if page == 1 then
		g_mainplayer:setShopCenterType(0, 3, EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND, money_type)
		ShopCenter_UpdateRecommend()
	else
		ShopCenter_UpdateType3Icon()
		ShopCenter_SelectType(type3_slot..type3)
	end
	
	goods_row = nil
end

-----------------------------------------
--选择性别
-----------------------------------------
function ShopCenter_SelMale()
	sex = 1
	
	local slotWin = g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_male") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage110")
	
	slotWin = g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_famale") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage109")
	
	ShopCenter_SelectType("Marketroot/Market/Background3/Btn_male")
	ShopCenter_UpdateType3Icon()
	ShopCenter_SelectType(type3_slot..type3)
	
	if g_mainplayer:isMale() == false then
		preview_checkbox:setSelected(false)
		Preview_Close()
	end
	
	goods_row = nil
end

function ShopCenter_SelFemale()
	sex = 2
	
	local slotWin = g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_male") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage108")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage110")
	
	slotWin = g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_famale") 
	slotWin:setProperty("NormalImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("PushedImage", "set:Button6 image:NewImage107")
	slotWin:setProperty("HoverImage", "set:Button6 image:NewImage109")
	
	ShopCenter_SelectType("Marketroot/Market/Background3/Btn_famale")
	ShopCenter_UpdateType3Icon()
	ShopCenter_SelectType(type3_slot..type3)
	
	if g_mainplayer:isMale() == true then
		preview_checkbox:setSelected(false)
		Preview_Close()
	end
end

-----------------------------------------
--预览
-----------------------------------------
function ShopCenter_ClickPreview()
	if preview_checkbox:isSelected() == true then
		Preview_Open(goods_row, preview_pos)
	else
		Preview_Close()
	end
end

-----------------------------------------
--点击分页
-----------------------------------------
function ShopCenter_ClickPage(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	page = tonumber(string.sub(winname, string.len("Marketroot/Market/Mkt_tab__auto_TabPane__Buttons__auto_btnTab")+1, string.len(winname)))

	for i=1,goods_tab_count do
		g_WinMgr:getWindow(goods_tab..i):setVisible(false)
	end
	g_WinMgr:getWindow(goods_tab..page):setVisible(true)

	if page == 1 then
		ShopCenter_SetTypeIconVisible(false)
		ShopCenter_UpdateRecommend()
	else
		ShopCenter_SetTypeIconVisible(true)
	end
	
	if page == 2 then
		g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_male"):setVisible(true)
		g_WinMgr:getWindow("Marketroot/Market/Background3/Male"):setVisible(true)
		g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_famale"):setVisible(true)
		g_WinMgr:getWindow("Marketroot/Market/Background3/Famale"):setVisible(true)
	else
		g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_male"):setVisible(false)
		g_WinMgr:getWindow("Marketroot/Market/Background3/Male"):setVisible(false)
		g_WinMgr:getWindow("Marketroot/Market/Background3/Btn_famale"):setVisible(false)
		g_WinMgr:getWindow("Marketroot/Market/Background3/Famale"):setVisible(false)
	end
	
	type1 = page - 1
	type2 = 0
	type3 = 0
	ShopCenter_UpdateType2Icon()
	ShopCenter_UpdateType3Icon()
	
	if page > 1 then
		ShopCenter_SelectType(type2_slot..type2)
		ShopCenter_SelectType(type3_slot..type3)
	end
	
	--g_Loger:logDebug("ShopCenter_ClickPage = "..page.." "..type1.." "..type2.." "..type3)
end

-----------------------------------------
--点击商城子分类
-----------------------------------------
function ShopCenter_ClickType2(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	type2 = tonumber(string.sub(winname, string.len(type2_slot)+1, string.len(winname)))
	type3 = 0
	ShopCenter_UpdateType3Icon()
	goods_row = nil
	
	ShopCenter_UpdateType2Icon()
	ShopCenter_SelectType(winname)
	ShopCenter_SelectType(type3_slot..type3)
end

-----------------------------------------
--点击商城子子分类
-----------------------------------------
function ShopCenter_ClickType3(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	type3 = tonumber(string.sub(winname, string.len(type3_slot)+1, string.len(winname)))
	g_mainplayer:setShopCenterType(type1*100+type2*10+type3, sex, EQGlobaClass.ItemPack.PT_SHOPCENTER, money_type)
	ShopCenter_UpdateGoods()
	goods_row = nil
	
	ShopCenter_UpdateType3Icon()
	ShopCenter_SelectType(winname)
end

-----------------------------------------
--设置选中状态
-----------------------------------------
function ShopCenter_SelectType(winname)
	local slotWin = g_WinMgr:getWindow(winname)
	if slotWin == nil then
		return
	end
	
	local tempstr = slotWin:getProperty("PushedImage")
	slotWin:setProperty("NormalImage", tempstr)
	slotWin:setProperty("HoverImage", tempstr)
	--g_Loger:logDebug("ShopCenter_SelectType "..winname.." "..tempstr)
end
-----------------------------------------
--显示隐藏分类
-----------------------------------------
function ShopCenter_SetTypeIconVisible(visible)
	for i=1,type2_slot_count do
		g_WinMgr:getWindow(type2_slot..i):setVisible(visible)
	end
	
	for i=1,type3_slot_count do
		g_WinMgr:getWindow(type3_slot..i):setVisible(visible)
	end
end

-----------------------------------------
--更新子分类
-----------------------------------------
function ShopCenter_UpdateType2Icon()
	--if type1 == 0 then
	--	return
	--end
		
	for i=1,type2_slot_count do
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
			
			if type2 == 0 then
				type2 = 1
			end
		end
	end
end

-----------------------------------------
--更新子子分类
-----------------------------------------
function ShopCenter_UpdateType3Icon()
	--if type1 == 0 or type2 == 0 then
	--	return
	--end
	
	for i=1,type3_slot_count do
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
			
			if type3 == 0 then
				type3 = 1
			end
		end
	end
	
	g_mainplayer:setShopCenterType(type1*100+type2*10+type3, sex, EQGlobaClass.ItemPack.PT_SHOPCENTER, money_type)
	ShopCenter_UpdateGoods()
end

-----------------------------------------
--刷新推荐物品
-----------------------------------------
function ShopCenter_UpdateRecommend()
	Shop_RemoveSelectBox()
	
	for i=1,recommand_slot_count do
		local slotWin = g_WinMgr:getWindow(recommend_slot..i)
		if slotWin:getChildCount() > 0 then
			slotWin:removeChildWindow(slotWin:getChildAtIdx(0))
			slotWin:removeChildWindow(recommend_slot..i.."/money")
		end
	end

	for i=1,recommand_slot_count do
		local itemid 	= g_mainplayer:getItemID(i-1,EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
		if itemid > 0 then
			local itemrow 	= g_itemtableMgr:getItemRow(itemid)	
			if itemrow ~= nil then
				StorageUI_DynamicLoadItem(recommend_slot ..i, "item", i, itemrow.imageset, itemrow.icon, "False")
				
				local row = g_mainplayer:getShopCenterRow(i-1, EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
				if row ~= nil then
					local win = g_WinMgr:getWindow(recommend_slot..i.."/money")
					if win == nil then
						win = g_WinMgr:createWindow("EQHomeLook/StaticImage", recommend_slot..i.."/money")
						win:setProperty("MousePassThroughEnabled", "true")
					end
					g_WinMgr:getWindow(recommend_slot..i):addChildWindow(win)
					if row.money_type == 1 then
						win:setProperty("Image", "set:Button5 image:NewImage444")
					else
						win:setProperty("Image", "set:Button5 image:NewImage443")
					end
				end
			end	
		end
	end
	
	ShopCenter_UpdatePage()
end

-----------------------------------------
--刷新商城物品
-----------------------------------------
function ShopCenter_UpdateGoods()
	g_WinMgr:getWindow("Marketroot/Market/Selling_price1"):setText(0)

	if page == 1 then
		return
	end
	
	Shop_RemoveSelectBox()
	
	for i=1,goods_slot_count do
		local slotWin = g_WinMgr:getWindow(goods_slot[page]..i)
		if slotWin:getChildCount() > 0 then
			slotWin:removeChildWindow(slotWin:getChildAtIdx(0))
			slotWin:removeChildWindow(goods_slot[page]..i.."/money")
		end
	end
		
	for i=1,goods_slot_count do
		local itemid 	= g_mainplayer:getItemID(i-1,EQGlobaClass.ItemPack.PT_SHOPCENTER)
		if itemid > 0 then
			local itemrow = g_itemtableMgr:getItemRow(itemid)	
			if itemrow ~= nil then
				StorageUI_DynamicLoadItem(goods_slot[page]..i, "item", i, itemrow.imageset, itemrow.icon, "False")
				
				local row = g_mainplayer:getShopCenterRow(i-1, EQGlobaClass.ItemPack.PT_SHOPCENTER)
				if row ~= nil then
					local win = g_WinMgr:getWindow(goods_slot[page]..i.."/money")
					if win == nil then
						win = g_WinMgr:createWindow("EQHomeLook/StaticImage", goods_slot[page]..i.."/money")
						win:setProperty("MousePassThroughEnabled", "true")
					end
					g_WinMgr:getWindow(goods_slot[page]..i):addChildWindow(win)
					if row.money_type == 1 then
						win:setProperty("Image", "set:Button5 image:NewImage444")
					else
						win:setProperty("Image", "set:Button5 image:NewImage443")
					end
				end
			end	
		end
	end
	
	ShopCenter_UpdatePage()
end

-----------------------------------------
--点击推荐
-----------------------------------------
function ShopCenter_ClickRecommend(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(recommend_slot)+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
	if row ~= nil then
		--	超链接(hezhr)
		if g_CS_HYPERLINK == g_CursorMgr:getState() then
			ClickHyperLinkItem(row.item_id)
			return
		end	
		
		goods_row = row
		g_WinMgr:getWindow("Marketroot/Market/Selling_price1"):setText(goods_row.price)
		if goods_row.money_type == 1 then
			g_WinMgr:getWindow("Marketroot/Market/Selling_img"):setProperty("Image", "set:Button5 image:NewImage428")
		else
			g_WinMgr:getWindow("Marketroot/Market/Selling_img"):setProperty("Image", "set:Button5 image:NewImage427")
		end
		
		if preview_checkbox:isSelected() == true then
			Preview_Open(goods_row, preview_pos)
		end
	end
end

-----------------------------------------
--点击物品
-----------------------------------------
function ShopCenter_ClickGoods(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(goods_slot[page])+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_SHOPCENTER)
	if row ~= nil then
		--	超链接(hezhr)
		if g_CS_HYPERLINK == g_CursorMgr:getState() then
			ClickHyperLinkItem(row.item_id)
			return
		end	
		
		goods_row = row
		g_WinMgr:getWindow("Marketroot/Market/Selling_price1"):setText(goods_row.price)
		if goods_row.money_type == 1 then
			g_WinMgr:getWindow("Marketroot/Market/Selling_img"):setProperty("Image", "set:Button5 image:NewImage428")
		else
			g_WinMgr:getWindow("Marketroot/Market/Selling_img"):setProperty("Image", "set:Button5 image:NewImage427")
		end
		
		if preview_checkbox:isSelected() == true then
			Preview_Open(goods_row, preview_pos)
		end
	end
end

-----------------------------------------
--上页
-----------------------------------------
function ShopCenter_LastPage()
	if page == 1 then
		g_mainplayer:setPackLastPage(EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
		ShopCenter_UpdateRecommend()
	else
		g_mainplayer:setPackLastPage(EQGlobaClass.ItemPack.PT_SHOPCENTER)
		ShopCenter_UpdateGoods()
	end
	
	goods_row = nil
end

-----------------------------------------
--下页
-----------------------------------------
function ShopCenter_NextPage()
	if page == 1 then
		g_mainplayer:setPackNextPage(EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
		ShopCenter_UpdateRecommend()
	else
		g_mainplayer:setPackNextPage(EQGlobaClass.ItemPack.PT_SHOPCENTER)
		ShopCenter_UpdateGoods()
	end
	
	goods_row = nil
end

-----------------------------------------
--页数
-----------------------------------------
function ShopCenter_UpdatePage()
	local curpage = 0
	local pagecount = 0
	if page == 1 then
		curpage = g_mainplayer:getPackCurPage(EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
		pagecount = g_mainplayer:getPackPageCount(EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
	else
		curpage = g_mainplayer:getPackCurPage(EQGlobaClass.ItemPack.PT_SHOPCENTER)
		pagecount = g_mainplayer:getPackPageCount(EQGlobaClass.ItemPack.PT_SHOPCENTER)
	end
		
	g_WinMgr:getWindow("Marketroot/Market/Page"):setText(curpage)
	g_WinMgr:getWindow("Marketroot/Market/Page1"):setText("/"..pagecount)
end

-----------------------------------------
--购买
----------------------------------------
function ShopCenter_Buy()
	if goods_row == nil then
		MsgBox_createMsgBox(g_MainUIWindows,0,"","请选择一样商品！", "MsgBox_cancelFunc", "")
	else
		local str
		if goods_row.money_type == 1 then
			str = "金"
		else
			str = "EQ"
		end

		local itemrow = g_itemtableMgr:getItemRow(goods_row.item_id)
		MsgBox_ShowBuyOneConfirm(itemrow.name, goods_row.price, str, "ShopCenter_ConfirmBuy")
	end
end

-----------------------------------------
--确认购买
-----------------------------------------
function ShopCenter_ConfirmBuy()
	--发购买包
	g_req_buy_sys_shop_goods.goods_id = goods_row.good_id
	g_msgmgr:send(g_req_buy_sys_shop_goods)

--	g_Loger:logDebug("req_buy_sys_shop_goods "..goods_row.good_id)
end

-----------------------------------------
--设置选中框
-----------------------------------------
local select_slot
function Shop_SetSelectBox(args)
	select_slot = CEGUI.toWindowEventArgs(args).window

	if select_slot:getChildCount() ~= 0 then
			citem:setProperty("Image", "set:windows1 image:BackGround14")
			citem:setProperty("MousePassThroughEnabled", "true")
			citem:setProperty("WantsMultiClickEvents", "false")
			citem:setProperty("FrameEnabled", "false")
			citem:setProperty("BackgroundEnabled", "false")
			citem:setProperty("AlwaysOnTop", "true")
			if select_slot:getChildAtIdx(0):getChildCount() > 0 then
				iocnWnd = select_slot:getChildAtIdx(0):getChildAtIdx(0)
				iocnWnd:addChildWindow(citem)
				
--				g_Loger:logMessage("Shop_SetSelectBox")
			end
	end
end

-----------------------------------------
--取消选中框
-----------------------------------------
function Shop_RemoveSelectBox()
	if select_slot == nil then
		return
	end
	
	if select_slot:getChildCount() > 0 then
		if select_slot:getChildAtIdx(0):getChildCount() > 0 then
			if select_slot:getChildAtIdx(0):getChildAtIdx(0):getChildCount() > 0 then
				local child = select_slot:getChildAtIdx(0):getChildAtIdx(0)
				child:removeChildWindow(child:getChildAtIdx(0))
				
--				g_Loger:logMessage("Shop_RemoveSelectBox")
			end
		end
	end
end

--使能其他按钮
function Shop_otherBtnEnbled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_PlayerProperty"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_House"):setEnabled(bl)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Storage"):setEnabled(bl)
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

--关闭其他界面
function Shop_closeOtherUI()
	if true == g_PlayerPropertyFlag then
		PlayerPropertyUI_OnClosePlayerProperty()
	end
	if true == g_houseInfoFlag then
		CloseHouseInfoUI()
	end
	if true == g_StorageUIFlag then
		StorageUI_OnCloseStorage()
	end
	if true == g_NpcFlag then
		--NpcShop_Close()
	end
	if true == g_ActionUIFlag then
		Action_closeActionUI()
	end
	if true == g_openFriendlist then
		MainUI_closeFriendlist()
	end
	if true == ExitWindowFlag then
		ExitWnd_closeUI()
	end
	if true == g_AddressBook then
		CloseAddressBook()
	end
	if true == g_funnyFlag then
		CloseFunnyUI()
	end
end

-----------------------------------------
--显示itemhint
-----------------------------------------
function ShopCenter_RecommendMouseEnter(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(recommend_slot)+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_SHOPCENTER_RECOMMEND)
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

function ShopCenter_GoodsMouseEnter(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotindex = tonumber(string.sub(winname, string.len(goods_slot[page])+1, string.len(winname)))
	
	local row = g_mainplayer:getShopCenterRow(slotindex-1, EQGlobaClass.ItemPack.PT_SHOPCENTER)
	if nil~= row then
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
function ShopCenter_RecommendMouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end

function ShopCenter_GoodsMouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end