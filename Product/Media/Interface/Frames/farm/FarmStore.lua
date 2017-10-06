-----------------------------------------------------------------
--农场商店
--xiaoMing
--2010-07-06
-----------------------------------------------------------------
--1 从表中读取数据到程序中 FGetShopItemRow 并分类显示
		--_SaveItemData(ItemRow)
--2 点击不同种类的商品 弹出不同的售卖面板
--3 FGetShopItemInfo(type )获得商品的详细信息

--保存数据 type subtype  type 1 种子 2 道具 3 装饰 subtype 目前只有装饰有子类 其他还没有 但以后有可能有
		--装饰：1-背景 2-房屋 3-狗窝 4-狗 5-路标 6-篱笆 7-稻草人 8-告示牌 9-告示牌内容
--目前所有种类只保存id,其他所有数据由查表获得

local mData = {{{}                             },
			   { {}                                 },
			   { {}, {}, {}, {}, {}, {}, {}, {}, {} }}
local mPageSize = {15, 10, 10}
local mCurPage = {{},{},{}}
g_FShopFlag = false
g_FShopSaleFlag = {false, {false,false}, {false,false}}
local mCurSubType = {}
local mOrnaTypeCount = 8 --装饰类型
local mIsLoadData = false
local mClickItem = {}
local mPreClickBtnName = nil
--
--农场道具类型枚举
local ENUM_PROPTYPE = {
					   SHORTEN_TIME 	= 1,
					   AUTO_HAVEST 		= 2,
					   ADD_OUTPUT 		= 3,
					   IMMUNITY_WEED 	= 4,
					   IMMUNITY_BUG 	= 5,
					   AUTO_WATER 		= 6,
					   DOG_FOOD 		= 7,
					   HIDE_TYPE 		= 8,
					   MUTATE_TYPE 		= 9
					  }
--
--农场装饰枚举
local ENUM_DECTYPE = {
					   BACKGROUND 		= 1,
					   FARMHOUSE 		= 2,
					   DOGHOUSE 		= 3,
					   DOG 				= 4,
					   FENCE 			= 5,
					   NOTICEBOARD 		= 6,
					   NOTICECONTENT 	= 7,
					   OTHERDEC 		= 8,
					 }

-----------------------------------------------------------------
local function _Init()
	mCurPage = {{1},{1},{1, 1, 1, 1, 1, 1, 1, 1, 1}}
	mCurSubType = {1, 1, 1}
	mClickItem = {id = -1 , count = 1, price = 0}
	
	resetFSZSBtnState()
	setFSZSBtnPushed("shangdian/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian1")
end
-----------------------------------------------------------------
--
local function _SaveItemData(shopItemRow, dataPack)
	local itemId = shopItemRow.item_id
	--获得总表的type sell_price
	local itemRow = g_itemtableMgr:getItemRow(itemId)
	local type = itemRow.type
	local subType = 1
	--type 转换
	if 4 == type then
	--种子
		type = 1
	elseif 6 == type then
	--道具
		type = 2
	elseif 7 == type then
	--装饰
		type = 3
		local ornaRow = g_FarmTableMgr:getOrnamentRow(itemId)
		subType = ornaRow.type
	else
		return
	end
	table.insert(dataPack[type][subType], {id = itemId, level = shopItemRow.itemLevel,
				gamePrice = shopItemRow.gameprice, eqPrice = shopItemRow.eqprice})
end
-----------------------------------------------------------------
local function _ClearData()
	mData = {{ {}                           },
				{ {}                                 },
			    { {}, {}, {}, {}, {}, {}, {}, {}, {} }}
end
-----------------------------------------------------------------
--排序表
local function _SortData()
	table.sort(mData[1][1], FarmSortFunc)
	table.sort(mData[2][1], FarmSortFunc)

	for i = 1, mOrnaTypeCount do
		table.sort(mData[3][i], FarmSortFunc)
	end
end
-----------------------------------------------------------------
--从表中读取商店物品信息
local function _GetData()
	if false == mIsLoadData then
		_ClearData()
		local count = g_FarmTableMgr:getFarmShopTableCount()
		for startIndex = 1, count do
			local itemRow = g_FarmTableMgr:getFarmShopRow(startIndex)

			if nil == itemRow then
				return
			end
			
			_SaveItemData(itemRow, mData)
		end
		mIsLoadData = true
		--排序
		_SortData()
	end
end
-----------------------------------------------------------------
local function _GetCurPage(type, subType, pageTable)
	return pageTable[type][subType]
end
-----------------------------------------------------------------
local function _SetCurPage(value, type, subtype, pageTable)
	pageTable[type][subtype] = value
end
-----------------------------------------------------------------
function _SetSlotImage(type,slotName, itemWinName, index, imagesetName, imgName)
	StorageUI_DynamicLoadItem(slotName, itemWinName, index, imagesetName, imgName, "False")
	local container = g_WinMgr:getWindow(slotName):getChildAtIdx(0)
	--
	container:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}")
end
-----------------------------------------------------------------
local function _SetGrid(type ,gridName, gridIndex, itemRow, level)
	local imagesetName, iconName, slotName = ""
	g_WinMgr:getWindow(gridName):setVisible(true)
	--加载果实icon
	if nil ~= itemRow then
		slotName = gridName.."/1"
		imagesetName = itemRow.imageset
		iconName = itemRow.icon
		_SetSlotImage(type, slotName,
									"newImg",
									gridIndex, imagesetName,
									iconName)
	end
	--设置等级
	FSetText(gridName.."/4", level.."级")
end
-----------------------------------------------------------------
--获得点击物品的格子Index
local function _GetSlotIndex(winName, type)
	local baseName = "shangdian/zhuchuangkou/biaoqian/Tab "..type.."/icon"
	for i = 1, mPageSize[type] do
		if baseName..i == winName then
			return i
		end
	end
end
-----------------------------------------------------------------
local function _GetType()
	local type = GetCurTabIndex(g_farmShop["tabName"]) + 1
	local subType = mCurSubType[type]
	return type, subType
end
-----------------------------------------------------------------
--清空页面
local function _CleanPage(type)
	local str = "shangdian/zhuchuangkou/biaoqian/Tab "..type.."/icon"
	local slotName , slotWin
	
	for i = 1, mPageSize[type] do
		slotName = str..i.."/1"
		g_WinMgr:getWindow(str..i):setVisible(false)
		slotWin = g_WinMgr:getWindow(slotName)
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
		end
		FSetText(str..i.."/4", "")
		FSetText("shangdian/zhuchuangkou/biaoqian/Tab "..type.."/txt"..i, "")
		FSetText("shangdian/zhuchuangkou/biaoqian/Tab "..type.."/txta"..i, "")
	end
end
-----------------------------------------------------------------
--更新商店页面
local function _Update(type, subtype, page)
	_CleanPage(type)
	--返回 id, level
	local itemTable = getData(mData[type][subtype], page, mPageSize[type])
	local size = table.getn(itemTable)
	local str = "shangdian/zhuchuangkou/biaoqian/Tab "..type.."/icon"
	local itemRow

	for i = 1, size do
		itemRow = g_itemtableMgr:getItemRow(itemTable[i]["id"])
		_SetGrid(type, str..i, i, itemRow, itemTable[i]["level"])
		if type == 2 then
			FSetText("shangdian/zhuchuangkou/biaoqian/Tab "..type.."/txt"..i, "")
		else
			FSetText("shangdian/zhuchuangkou/biaoqian/Tab "..type.."/txt"..i, "$:"..itemTable[i]["gamePrice"])
		end
		if (type == 3 and subtype ~= 4) or type == 2 then
			FSetText("shangdian/zhuchuangkou/biaoqian/Tab "..type.."/txta"..i, "EQ币:"..itemTable[i]["eqPrice"])
		end
	end

	_SetCurPage(page, type, subtype, mCurPage)

	if type == _GetType() then
		FUpdatePageInfo(g_farmShop["pageText"],page, getTotalPage(mData[type][subtype], mPageSize[type]))
		--设置价格
	end
end
-----------------------------------------------------------------
--获得点击的itemId
local function _GetClickItem(args, type, subType)
	local SlotWin 		= CEGUI.toWindowEventArgs(args).window
	local WinName 		= SlotWin:getName()

	local slotIndex = _GetSlotIndex(WinName, type)

	if nil == slotIndex then
		return
	end
	local tableindex = FShopGetTableIndex(slotIndex, mCurPage[type][subType], mPageSize[type])
	mClickItem["id"] = mData[type][subType][tableindex]["id"]
	mClickItem["eqPrice"] = mData[type][subType][tableindex]["eqPrice"]
	mClickItem["gamePrice"] = mData[type][subType][tableindex]["gamePrice"]
end
-----------------------------------------------------------------
--设置种子面板信息
local function _SetSeedInfo()
	local itemId = mClickItem["id"]
	local itemRow = g_itemtableMgr:getItemRow(itemId)
	local imagesetName = itemRow.imageset
	local iconName = itemRow.icon
	StorageUI_DynamicLoadItem(g_farmShop.seedImage, "newImg", 1, imagesetName, iconName, "False")

	itemRow = g_FarmTableMgr:getFarmItemRow(itemId)
	FSetText(g_farmShop["seedSpecialUse"], itemRow.special)
	FSetText(g_farmShop["seedInfo"], itemRow.intro)

	itemRow = g_FarmTableMgr:getSeedRow(itemId)
	local price = g_itemtableMgr:getItemRow(itemRow.fruit_id).sell_price

	if nil == itemRow then
		--g_Loger:logError("the item is no exist in SeedTable")
		return
	end

	local fruit_time = string.format("%.1f", (itemRow.fruit_time/3600))

	FSetText(g_farmShop["seedEptPick"], tostring(itemRow.expect_output))
	FSetText(g_farmShop["seedTime"], fruit_time)
	FSetText(g_farmShop["seedPerPrice"], tostring(price))
	FSetText(g_farmShop["seedPrice"], tostring(mClickItem["gamePrice"]))
	FSetText(g_farmShop["seedEptMoney"], tostring(price*itemRow.expect_output))
	FSetText(g_farmShop["seedExp"], tostring(itemRow.pick_exp))
	FSetText(g_farmShop["seedLevel"], tostring(itemRow.use_level))
	FSetText(g_farmShop["seedName"], itemRow.seed_name)
end
-----------------------------------------------------------------
--设置道具面板信息
local function _SetPropInfo()
	local itemId = mClickItem["id"]
	local itemRow = g_itemtableMgr:getItemRow(itemId)
	local price = itemRow.sell_price
	StorageUI_DynamicLoadItem(g_farmShop.propImage, "newImg", 1, itemRow.imageset, itemRow.icon, "False")
	FSetText(g_farmShop.propGamePrice, mClickItem["gamePrice"].."")
	FSetText(g_farmShop.propEqPrice, mClickItem["eqPrice"].."")
end
------------------------------------------------------------------
--设置购买狗粮界面信息
local dynamicIntroWin = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "buy_dog_food/background/intro/txt")
--
local function _SetDogFoodInfo()
	local itemId = mClickItem["id"]
	local itemRow = g_itemtableMgr:getItemRow(itemId)
	local FarmShopRow = g_FarmTableMgr:getFarmItemRow(itemRow.subID)
	StorageUI_DynamicLoadItem(g_farmShop.BuyDogFoodImg, "newImg", 1, itemRow.imageset, itemRow.icon, "False")
	FSetText(g_farmShop.BuyDogFoodEqGold, mClickItem["eqPrice"].."")
	FSetText(g_farmShop.DogFoodName, itemRow.name)
	g_WinMgr:getWindow(g_farmShop["DogFoodCount"]):setText("1")
	
	dynamicIntroWin:setSize(CEGUI.UVector2(CEGUI.UDim(1,0), CEGUI.UDim(1,0)))
	EQGlobaFunc.toMultiLineBox(dynamicIntroWin:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(dynamicIntroWin:getName()):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(dynamicIntroWin:getName()):setCols("#000000")
	EQGlobaFunc.toMultiLineBox(dynamicIntroWin:getName()):addElementText(FarmShopRow.intro, false)	-- 添加文本
	g_WinMgr:getWindow(g_farmShop.DogFoodIntro):addChildWindow(dynamicIntroWin)
end
-----------------------------------------------------------------
--设置装饰面板信息
local function _SetOrnamentInfo()
	local itemRow = g_FarmTableMgr:getOrnamentRow(mClickItem["id"])
	--local exp = itemRow.experience..""
	--local validatyTime = itemRow.period..""
	itemRow = g_itemtableMgr:getItemRow(mClickItem["id"])
	StorageUI_DynamicLoadItem(g_farmShop.ornaImage, "newImg", 1, itemRow.imageset, itemRow.icon, "False")
	
	FSetText(g_farmShop["ornaName"], itemRow.name)
	FSetText(g_farmShop["ornaEqPrice"], mClickItem["eqPrice"].."")
	FSetText(g_farmShop["ornaGamePrice"], mClickItem["gamePrice"].."")
	--FSetText(g_farmShop["ornaExp"],  exp)
	--FSetText(g_farmShop["ornaValidatyTime"], validatyTime)
end
-----------------------------------------------------------------
--设置商店装饰购买狗的界面信息
local function _SetBuyDogUIInfo()
	local itemRow = g_itemtableMgr:getItemRow(mClickItem["id"])
	StorageUI_DynamicLoadItem(g_farmShop.BuyDogImg, "newImg", 1, itemRow.imageset, itemRow.icon, "False")
	
	FSetText(g_farmShop["BuyDogName"], itemRow.name)
	FSetText(g_farmShop["BuyDogGameGold"], mClickItem["gamePrice"].."")
end
-----------------------------------------------------------------
local function _AddNum(maxCount, winName)
	local curCount = tonumber(g_WinMgr:getWindow(winName):getText())
	if curCount >= maxCount then
		return
	else
		g_WinMgr:getWindow(winName):setText(curCount + 1)
	end
end
-----------------------------------------------------------------
local function _SubNum(minCount, winName)
	local curCount = tonumber(g_WinMgr:getWindow(winName):getText())
	if curCount <= minCount then
		return
	else
		g_WinMgr:getWindow(winName):setText(curCount - 1)
	end
end
-----------------------------------------------------------------
--注册事件
function FShopUIOnLoad()
	g_WinMgr:getWindow(g_farmShop["perPageBtn"]):subscribeEvent("Clicked", "FarmShopPerPage")
	g_WinMgr:getWindow(g_farmShop["nextPageBtn"]):subscribeEvent("Clicked", "FarmShopNextPage")

	g_WinMgr:getWindow(g_farmShop["closeBtn"]):subscribeEvent("Clicked", "CloseFarmShopUI")

	g_WinMgr:getWindow(g_farmShop["tab1Btn"]):subscribeEvent("Clicked", "FShopClickTabBtn")
	g_WinMgr:getWindow(g_farmShop["tab2Btn"]):subscribeEvent("Clicked", "FShopClickTabBtn")
	g_WinMgr:getWindow(g_farmShop["tab3Btn"]):subscribeEvent("Clicked", "FShopClickTabBtn")

	g_WinMgr:getWindow(g_farmShop["seedCloseBtn"]):subscribeEvent("Clicked", "FCloseSeedSaleUI")
	g_WinMgr:getWindow(g_farmShop["propCloseBtn"]):subscribeEvent("Clicked", "FClosePropSaleUI")
	g_WinMgr:getWindow(g_farmShop["ornaCloseBtn"]):subscribeEvent("Clicked", "FCloseOrnaSaleUI")

	g_WinMgr:getWindow(g_farmShop["seedSureBtn"]):subscribeEvent("Clicked", "FarmStore_BuyFarmItemByGold")
	g_WinMgr:getWindow(g_farmShop["propBuyGold"]):subscribeEvent("Clicked", "FarmStore_BuyFarmItemByGold")
	g_WinMgr:getWindow(g_farmShop["propBuyEq"]):subscribeEvent("Clicked", "FarmStore_BuyFarmItemByEqGold")
	g_WinMgr:getWindow(g_farmShop["ornaGameGoldBtn"]):subscribeEvent("Clicked", "FarmStore_BuyFarmItemByGold")
	g_WinMgr:getWindow(g_farmShop["ornaEqGoldBtn"]):subscribeEvent("Clicked", "FarmStore_BuyFarmItemByEqGold")
	g_WinMgr:getWindow(g_farmShop["seedCancleBtn"]):subscribeEvent("Clicked", "FCloseSeedSaleUI")
	g_WinMgr:getWindow(g_farmShop["propCancleBtn"]):subscribeEvent("Clicked", "FClosePropSaleUI")
	g_WinMgr:getWindow(g_farmShop["ornaCancleBtn"]):subscribeEvent("Clicked", "FCloseOrnaSaleUI")
	
	g_WinMgr:getWindow(g_farmShop["BuyDogFoodCloseBtn"]):subscribeEvent("Clicked", "FCloseDogFoodSaleUI")
	g_WinMgr:getWindow(g_farmShop["BuyDogFoodCancelBtn"]):subscribeEvent("Clicked", "FCloseDogFoodSaleUI")
	g_WinMgr:getWindow(g_farmShop["DogFoodToStorage"]):subscribeEvent("Clicked", "ReqBuyDogFoodToStorage")
	g_WinMgr:getWindow(g_farmShop["DogFoodToDogHouse"]):subscribeEvent("Clicked", "ReqBuyDogFoodToDogHouse")
	
	g_WinMgr:getWindow(g_farmShop["BuyDogCancel"]):subscribeEvent("Clicked", "FarmStore_CloseBuyDogUI")
	g_WinMgr:getWindow(g_farmShop["BuyDogCloseBtn"]):subscribeEvent("Clicked", "FarmStore_CloseBuyDogUI")
	g_WinMgr:getWindow(g_farmShop["BuyDogConfirm"]):subscribeEvent("Clicked", "FarmStore_BuyFarmItemByGold")
	g_WinMgr:getWindow("buy_dog_food_confirm/background/done"):subscribeEvent("Clicked", "FarmStore_confirmBuyDogFood")
	g_WinMgr:getWindow("buy_dog_food_confirm/background/cancle"):subscribeEvent("Clicked", "FarmStore_cancelBuyDogFood")
	g_WinMgr:getWindow("buy_dog_food_confirm__auto_closebutton__"):subscribeEvent("Clicked", "FarmStore_cancelBuyDogFood")

	g_WinMgr:getWindow(g_farmShop["seedCount"]):subscribeEvent("TextChanged", "FShopOnCountChange")
	g_WinMgr:getWindow(g_farmShop["propCount"]):subscribeEvent("TextChanged", "FShopOnCountChange")
	g_WinMgr:getWindow(g_farmShop["DogFoodCount"]):subscribeEvent("TextChanged", "FShopOnCountChange")
	
	--g_WinMgr:getWindow(g_farmShop.ornaPreView):subscribeEvent("Clicked", "FPreviewOrna")

	local window = g_WinMgr:getWindow(g_farmShop["seedCount"])
	CEGUI.toEditbox(window):setMaxTextLength(2)
	CEGUI.toEditbox(window):setValidationString("^$|^[0-9]*[1-9][0-9]*$")

	window = g_WinMgr:getWindow(g_farmShop["propCount"])
	CEGUI.toEditbox(window):setMaxTextLength(2)
	CEGUI.toEditbox(window):setValidationString("^$|^[0-9]*[1-9][0-9]*$")
	
	window = g_WinMgr:getWindow(g_farmShop["DogFoodCount"])
	CEGUI.toEditbox(window):setMaxTextLength(2)
	CEGUI.toEditbox(window):setValidationString("^$|^[0-9]*[1-9][0-9]*$")

	g_WinMgr:getWindow(g_farmShop["seedPerNum"]):subscribeEvent("Clicked", "FOnSubNum")
	g_WinMgr:getWindow(g_farmShop["seedNextNum"]):subscribeEvent("Clicked", "FOnAddNum")
	g_WinMgr:getWindow(g_farmShop["propPerNum"]):subscribeEvent("Clicked", "FOnSubNum")
	g_WinMgr:getWindow(g_farmShop["propNextNum"]):subscribeEvent("Clicked", "FOnAddNum")
	g_WinMgr:getWindow(g_farmShop["dogFoodPerNum"]):subscribeEvent("Clicked", "FOnSubNum")
	g_WinMgr:getWindow(g_farmShop["dogFoodNextNum"]):subscribeEvent("Clicked", "FOnAddNum")

	--注册装饰分类按钮 mOrnaTypeCount
	for i = 1, mOrnaTypeCount do
		local win = g_WinMgr:getWindow(g_farmShop.ornaType..i)
		if i < 5 then
			win:setID(i)	
		end
		win:subscribeEvent("MouseClick", "ClickOrnaType")
		win:setProperty("NormalTextColour","FFFFFFFF")
		win:setProperty("HoverTextColour","FFFCFF00")
	end
	g_WinMgr:getWindow(g_farmShop.ornaType..5):setID(6)
	g_WinMgr:getWindow(g_farmShop.ornaType..6):setID(8)
	g_WinMgr:getWindow(g_farmShop.ornaType..7):setID(9)
	g_WinMgr:getWindow(g_farmShop.ornaType..8):setID(7)
	--注册格子事件 待改
	for i = 1, mPageSize[1] do
		local str = "shangdian/zhuchuangkou/biaoqian/Tab 1/icon"..i
		
		for j = 1, 4 do
			g_WinMgr:getWindow(str.."/"..j):setProperty("MousePassThroughEnabled", "true")
		end
		
		g_WinMgr:getWindow(str):subscribeEvent("MouseClick", "UpdateFarmShopSeed")
		g_WinMgr:getWindow(str):subscribeEvent("MouseEnter", "FarmStore_SeedMouseEnter")
		g_WinMgr:getWindow(str):subscribeEvent("MouseLeave", "FarmStore_MouseLeave")
	end
		
	for i = 1, mPageSize[2] do
		local str = "shangdian/zhuchuangkou/biaoqian/Tab 2/icon"..i
		
		for j = 1, 4 do
			g_WinMgr:getWindow(str.."/"..j):setProperty("MousePassThroughEnabled", "true")
		end
		
		g_WinMgr:getWindow(str):subscribeEvent("MouseClick", "UpdateFarmShopProp")
		g_WinMgr:getWindow(str):subscribeEvent("MouseEnter", "FarmStore_PropMouseEnter")
		g_WinMgr:getWindow(str):subscribeEvent("MouseLeave", "FarmStore_MouseLeave")
	end
	
	for i = 1, mPageSize[3] do
		local str = "shangdian/zhuchuangkou/biaoqian/Tab 3/icon"..i
		
		for j = 1, 4 do
			g_WinMgr:getWindow(str.."/"..j):setProperty("MousePassThroughEnabled", "true")
		end
		
		g_WinMgr:getWindow(str):subscribeEvent("MouseClick", "UpdateFarmShopOrna")
		g_WinMgr:getWindow(str):subscribeEvent("MouseEnter", "FarmStore_OrnamentMouseEnter")
		g_WinMgr:getWindow(str):subscribeEvent("MouseLeave", "FarmStore_MouseLeave")
	end
	
	MainUI_ShieldClick(g_FarmShopSeedUI)
	MainUI_ShieldClick(g_FarmShopPropUI)
	MainUI_ShieldClick(g_FarmShopOrnaUI)
	MainUI_ShieldClick(g_FarmMarketUI)
end
-----------------------------------------------------------------
--种子
function UpdateFarmShopSeed(args)
	FPopSeedSaleUI()
	g_WinMgr:getWindow(g_farmShop["seedCount"]):setText("1")
	_GetClickItem(args, 1, mCurSubType[1])
	_SetSeedInfo()
end
-----------------------------------------------------------------
--道具
function UpdateFarmShopProp(args)
	g_WinMgr:getWindow(g_farmShop["propCount"]):setText("1")
	_GetClickItem(args, 2, mCurSubType[2])
	FPopPropSaleUI()

	--更新面板
end
-----------------------------------------------------------------
--装饰
function UpdateFarmShopOrna(args)
	
	_GetClickItem(args, 3, mCurSubType[3])
	
	FPopOrnaSaleUI()
	--更新面板
end
-----------------------------------------------------------------
--购买狗狗的界面
function FarmStore_popBuyDogUI()

end
-----------------------------------------------------------------
--翻页按钮
function FarmShopPerPage()
	local type, subType = _GetType()
	local curPage = _GetCurPage(type, subType, mCurPage)

	if 1 == curPage then
		return
	end

	_SetCurPage(curPage - 1, type, subType, mCurPage)
	_Update(type, subType, curPage - 1)
end
-----------------------------------------------------------------
function FarmShopNextPage()
	local type , subType = _GetType()
	local curPage = _GetCurPage(type, subType, mCurPage)
	local totalPage = getTotalPage(mData[type][subType], mPageSize[type])

	if totalPage <= curPage then
		return
	end

	_SetCurPage(curPage + 1, type, subType, mCurPage)
	_Update(type, subType, curPage + 1)
end
-----------------------------------------------------------------
--点击Tab按钮
function FShopClickTabBtn()
	--是否需要更新显示
	local type ,subType= _GetType()
	local curPage = _GetCurPage(type, subType, mCurPage)
	FUpdatePageInfo(g_farmShop["pageText"],curPage, getTotalPage(mData[type][subType], mPageSize[type]))
end
-----------------------------------------------------------------
--请求买农场物品
function ReqBuyFarmItem(pay_type)
	--模板ID 和数量
	Dec_resetDataTab()	--请求前清空装饰表数据

	g_FarmMgr:reqBuyGoods(mClickItem["id"], 1, mClickItem["count"],pay_type)

	FCloseSeedSaleUI()
	FClosePropSaleUI()
	FCloseOrnaSaleUI()
	FarmStore_CloseBuyDogUI()
end
------------------------------------------------------------------
--
function FarmStore_BuyFarmItemByGold()
	ReqBuyFarmItem(1)
end
------------------------------------------------------------------
--
function FarmStore_BuyFarmItemByEqGold()
	ReqBuyFarmItem(2)
end
------------------------------------------------------------------
--请求购买狗粮到仓库
function ReqBuyDogFoodToStorage()
	popBuyDogFoodConfirmUI(1)
end
----------------------------------------------------------------
--请求购买并使用狗粮
function ReqBuyDogFoodToDogHouse()
	popBuyDogFoodConfirmUI(2)
end
----------------------------------------------------------------
--弹出确认请求购买狗粮界面
local BuyDogFoodConfirmFlag = false
local DogFoodUseage = 1
--
function popBuyDogFoodConfirmUI(type)
	DogFoodUseage = type
	if BuyDogFoodConfirmFlag == false then
		g_FarmUI:addChildWindow(g_BuyDogFood_ConfirmUI)
		Common_injectEmptyEvt("buy_dog_food_confirm")
		FarmStore_setBuyDogFoodConfirmUIinfo()
		setBuyDogFoodConfirmUIPos()
		BuyDogFoodConfirmFlag = true
	end
end
----------------------------------------------------------------
--设置确认请求购买狗粮界面的位置
function setBuyDogFoodConfirmUIPos()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local winWide = g_BuyDogFood_ConfirmUI:getWidth():asAbsolute(screenW)
	local winH = g_BuyDogFood_ConfirmUI:getHeight():asAbsolute(screenH)
	g_BuyDogFood_ConfirmUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-(winWide/2)),CEGUI.UDim(0.5,-(winH/2))))	
end
----------------------------------------------------------------
--设置确认购买狗粮界面的信息
function FarmStore_setBuyDogFoodConfirmUIinfo()
	local imgWinName = "buy_dog_food_confirm/background/icon"
	local eqGoldWinName = "buy_dog_food_confirm/background/confirm_txt/count"
	local eqGoldSum = mClickItem["eqPrice"] * mClickItem["count"]
	g_WinMgr:getWindow(eqGoldWinName):setText(tostring(eqGoldSum))
	g_WinMgr:getWindow(imgWinName):setProperty("Image", "set:32900001Imageset image:01")
end
----------------------------------------------------------------
--确认购买狗粮函数
function FarmStore_confirmBuyDogFood()
	local pay_type = 2
	g_FarmMgr:reqBuyGoods(mClickItem["id"], DogFoodUseage, mClickItem["count"],pay_type)
	closeBuyDogFoodConfirmUI()
	FCloseDogFoodSaleUI()
end
----------------------------------------------------------------
--取消购买狗粮
function FarmStore_cancelBuyDogFood()
	closeBuyDogFoodConfirmUI()
end
----------------------------------------------------------------
--关闭确认购买狗粮界面
function closeBuyDogFoodConfirmUI()
	if BuyDogFoodConfirmFlag == true then
		g_FarmUI:removeChildWindow(g_BuyDogFood_ConfirmUI)
		BuyDogFoodConfirmFlag = false
	end
end
-----------------------------------------------------------------
--商店售卖模块
-----------------------------------------------------------------
function FPopSeedSaleUI()
	if false == g_FShopSaleFlag[1] then
		g_FarmUI:addChildWindow(g_FarmShopSeedUI)	
		g_FShopSaleFlag[1] = true
		
		Common_injectEmptyEvt("FShopSeedSale")

		if true == g_FShopSaleFlag[2][1] then
			FClosePropSaleUI()
		end

		if true == g_FShopSaleFlag[3][1] then
			FCloseOrnaSaleUI()
		end
	else
		return
	end
end
-----------------------------------------------------------------
function FCloseSeedSaleUI()
	g_FarmUI:removeChildWindow(g_FarmShopSeedUI)
	g_FShopSaleFlag[1]  = false
end
-----------------------------------------------------------------
--
function FCloseDogFoodSaleUI()
	g_FarmUI:removeChildWindow(g_BuyDogFoodUI)
	g_FShopSaleFlag[2][2]  = false
end
-----------------------------------------------------------------
function FPopPropSaleUI()
	local farmPropRow = g_FarmTableMgr:getFarmPropRow(mClickItem["id"])
	local propId = farmPropRow.id
	local propType = farmPropRow.type
	if propType == ENUM_PROPTYPE.DOG_FOOD then
		if false == g_FShopSaleFlag[2][2] then
			_SetDogFoodInfo()
			FpopBugDogFoodUI()
			g_FShopSaleFlag[2][2] = true
			Common_injectEmptyEvt(g_BuyDogFoodUI:getName())
			if true == g_FShopSaleFlag[2][1] then
				FClosePropSaleUI()
			end
			
			if true == g_FShopSaleFlag[1] then
				FCloseSeedSaleUI()
			end

			if true == g_FShopSaleFlag[3][1] then
				FCloseOrnaSaleUI()
			end

		end
	else
		if false == g_FShopSaleFlag[2][1] then
			_SetPropInfo()
			g_FarmUI:addChildWindow(g_FarmShopPropUI)
			g_FShopSaleFlag[2][1] = true
			
			Common_injectEmptyEvt(g_FarmShopPropUI:getName())
			if true == g_FShopSaleFlag[2][2] then
				FCloseDogFoodSaleUI()
			end

			if true == g_FShopSaleFlag[1] then
				FCloseSeedSaleUI()
			end

			if true == g_FShopSaleFlag[3][1] then
				FCloseOrnaSaleUI()
			end
		else
			return
		end
	end
end
-----------------------------------------------------------------
--
function FpopBugDogFoodUI()
	g_FarmUI:addChildWindow(g_BuyDogFoodUI)
	setBugDogFoodUIPos()
end
-----------------------------------------------------------------
--
function setBugDogFoodUIPos()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local winWide = g_BuyDogFoodUI:getWidth():asAbsolute(screenW)
	local winH = g_BuyDogFoodUI:getHeight():asAbsolute(screenH)
	g_BuyDogFoodUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-(winWide/2)),CEGUI.UDim(0.5,-(winH/2))))	
end
-----------------------------------------------------------------
function FClosePropSaleUI()
	g_FarmUI:removeChildWindow(g_FarmShopPropUI)
	g_FShopSaleFlag[2][1]  = false
end
-----------------------------------------------------------------
function FPopOrnaSaleUI()
	if mCurSubType[3] == ENUM_DECTYPE.DOG then
		if false == g_FShopSaleFlag[3][2] then
			_SetBuyDogUIInfo()
			g_FarmUI:addChildWindow(g_BuyDogUI)
			g_FShopSaleFlag[3][2] = true
			
			Common_injectEmptyEvt(g_BuyDogUI:getName())

			if true == g_FShopSaleFlag[1] then
				FCloseSeedSaleUI()
			end

			if true == g_FShopSaleFlag[2][1] then
				FClosePropSaleUI()
			end
			
			if true == g_FShopSaleFlag[3][1] then
				FCloseOrnaSaleUI()
			end
		else
			return
		end		
	else
		if false == g_FShopSaleFlag[3][1] then
			_SetOrnamentInfo()
			g_FarmUI:addChildWindow(g_FarmShopOrnaUI)
			g_FShopSaleFlag[3][1] = true
			
			Common_injectEmptyEvt(g_FarmShopOrnaUI:getName())

			if true == g_FShopSaleFlag[1] then
				FCloseSeedSaleUI()
			end

			if true == g_FShopSaleFlag[2][1] then
				FClosePropSaleUI()
			end
			
			if true == g_FShopSaleFlag[3][2] then
				FarmStore_CloseBuyDogUI()
			end
		else
			return
		end
	end
end
-----------------------------------------------------------------
--
function FarmStore_CloseBuyDogUI()
	g_FarmUI:removeChildWindow(g_BuyDogUI)
	g_FShopSaleFlag[3][2] = false
end
-----------------------------------------------------------------
function FCloseOrnaSaleUI()
	g_FarmUI:removeChildWindow(g_FarmShopOrnaUI)
	g_FShopSaleFlag[3][1]  = false
end
-----------------------------------------------------------------
function FShopOnCountChange(args)
	local count = tonumber(CEGUI.toWindowEventArgs(args).window:getText())
	if count == nil then
		--CEGUI.toWindowEventArgs(args).window:setText("1")
		--count = tonumber(CEGUI.toWindowEventArgs(args).window:getText())
		mClickItem["count"] = 1
	else
		mClickItem["count"] = count
	end
end
-----------------------------------------------------------------
function FOnSubNum(args)
	local SlotWin 		= CEGUI.toWindowEventArgs(args).window
	local WinName 		= SlotWin:getName()
	if g_farmShop["seedPerNum"] == WinName then
		WinName = g_farmShop["seedCount"]
	elseif g_farmShop["propPerNum"] == WinName then
		WinName = g_farmShop["propCount"]
	elseif g_farmShop["dogFoodPerNum"] == WinName then
		WinName = g_farmShop["DogFoodCount"]
	else
		WinName = g_farmShop["ornaCount"]
	end
	_SubNum(1, WinName)
end
-----------------------------------------------------------------
function FOnAddNum(args)
	local SlotWin 		= CEGUI.toWindowEventArgs(args).window
	local WinName 		= SlotWin:getName()
	if g_farmShop["seedNextNum"] == WinName then
		WinName = g_farmShop["seedCount"]
	elseif g_farmShop["propNextNum"] == WinName then
		WinName = g_farmShop["propCount"]
	elseif g_farmShop["dogFoodNextNum"] == WinName then
		WinName = g_farmShop["DogFoodCount"]
	else
		WinName = g_farmShop["ornaCount"]
	end
	_AddNum(99, WinName)
end
-----------------------------------------------------------------
--排序表
function FarmSortFunc(a, b)
	if (a.level == b.level)	then
		return a.id < b.id
	else
		return a.level < b.level
	end
end
-----------------------------------------------------------------
function ClickOrnaType(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local index = win:getID()

	if index == mCurSubType[3] then
		return
	else
		mCurSubType[3] = index
		_Update(3, index, mCurPage[3][index] )
	end
	
	resetFSZSBtnState()
	setFSZSBtnPushed(win:getName())
end

function setFSZSBtnPushed(btnName)
	if nil == btnName then
		return
	end
	
	local win = g_WinMgr:getWindow(btnName)
	win:setProperty("NormalImage", "set:window2 image:farm_Background1")
	win:setProperty("HoverImage", "set:window2 image:farm_Background1")
	win:setProperty("PushedImage", "set:window2 image:farm_Background1")
	win:setProperty("NormalTextColour","FF000000")
	win:setProperty("HoverTextColour","FF000000")
	
	mPreClickBtnName = btnName
end
--重置类型按钮的图片状态
function resetFSZSBtnState()
	if nil == mPreClickBtnName then
		return
	end
	
	local win = g_WinMgr:getWindow(mPreClickBtnName)
	win:setProperty("NormalImage", "set:window2 image:")
	win:setProperty("HoverImage", "set:window2 image:farm_Background1")
	win:setProperty("PushedImage", "set:window2 image:farm_Background1")
	win:setProperty("NormalTextColour","FFFFFFFF")
    win:setProperty("HoverTextColour","FFFCFF00")
end 
-----------------------------------------------------------------
function PopFarmShopUI()
	if false == g_FShopFlag then
		FarmStore_closeOtherUI()
		
		g_FarmUI:addChildWindow(g_FarmMarketUI)
		g_FShopFlag = true
		SetWindowCenter(g_FarmMarketUI)
		CEGUI.toTabControl(g_WinMgr:getWindow(g_farmShop.tabName)):setSelectedTabAtIndex(0)
		
		_Init()
		_GetData()
		--更新每个Tab当前页面
		local curPage
		for i = 1, 3 do
			curPage = _GetCurPage(i, mCurSubType[i] ,mCurPage)
			_Update(i, mCurSubType[i], curPage)
		end
	else
		CloseFarmShopUI()
	end
end
-----------------------------------------------------------------
function CloseFarmShopUI()
	g_FarmUI:removeChildWindow(g_FarmMarketUI)
	g_FShopFlag = false

	for i,v in pairs(g_FShopSaleFlag) do
		if true == v then
			if 1 == i then
				FCloseSeedSaleUI()
			elseif 2 == i then
				FClosePropSaleUI()
			else
				FCloseOrnaSaleUI()
			end
		end
	end
end
-----------------------------------------------------------------
function FarmStore_closeOtherUI()
	if true== g_FStorageFlag then
		FRemoveStoragePanel()
	end
	if true == g_FSettingFlag then
		FSettingClose()
	end
	if true == g_FarmDecoFlag then
		Dec_closeDecUI()
	end
	if true == g_FPlayerInfoFlag then
		FClosePlayerInfoUI()
	end
	if true == mOpenFlag then
		CloseUpgradeLandUI()
	end
	Common_injectEmptyEvt("shangdian/zhuchuangkou")
end
-----------------------------------------------------------------
--显示种子itemhint
function FarmStore_SeedMouseEnter(args)
	local index = FShopGetMouseMoveItem(args, 1, mCurSubType[1])
	if nil ~= index then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = ""
		local itemrow = g_itemtableMgr:getItemRow(index)
		if 1 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
		elseif 2 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
		end
		createItemhint_farm(index, rect, g_FarmUI, 1, effectTime)
	end
end
-----------------------------------------------------------------
--显示道具itemhint
function FarmStore_PropMouseEnter(args)
	local index = FShopGetMouseMoveItem(args, 2, mCurSubType[2])
	if nil ~= index then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = ""
		local itemrow = g_itemtableMgr:getItemRow(index)
		if 1 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
		elseif 2 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
		end
		createItemhint_farm(index, rect, g_FarmUI, 2, effectTime)
	end
end
-----------------------------------------------------------------
--显示装饰itemhint
function FarmStore_OrnamentMouseEnter(args)
	local index = FShopGetMouseMoveItem(args, 3, mCurSubType[3])
	if nil ~= index then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = ""
		local itemrow = g_itemtableMgr:getItemRow(index)
		if 1 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
		elseif 2 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
		end
		createItemhint_farm(index, rect, g_FarmUI, 3, effectTime)
	end
end
-----------------------------------------------------------------
--获得鼠标移进去的itemId
function FShopGetMouseMoveItem(args, type, subType)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local slotIndex = _GetSlotIndex(winname, type)
	if nil == slotIndex then
		return
	end
	local tableindex = FShopGetTableIndex(slotIndex, mCurPage[type][subType], mPageSize[type])
	return mData[type][subType][tableindex]["id"]
end
-----------------------------------------
--隐藏itemhint
-----------------------------------------
function FarmStore_MouseLeave(args)
	destroyItemhint(g_FarmUI)
end