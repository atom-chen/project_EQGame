-----------------------------------------------------------------
--房屋买卖
--xiaoMing
--2010-07-26
-----------------------------------------------------------------
--HS 代表HouseSales
--1, 2, 3, 4 分别代表 套房 别墅 特殊 特价 5代表推荐
local mData = {{}, {}, {}, {}, {}}
local mSearchData = {}
local mLevelData = {{}, {}, {}, {}, {}} --存放每种类型的搜索
local buy_house_result = { bhr_replace = 1, bhr_add = 2 }
local buy_house_replace_type = { yes = 1, no = 2 }
local mIsOpenUI = false
local mIsLoad = false
local mCurSearchLevel = 0
local mCurPage = 1
local mPerPageDataCount = 6
local mChooseHouseId
local mReplaceHouseType
local m_checkBoxNameTable = {}	-- 消息框中复选框表(add by hezhr)
local mWindow
--选中框
g_HSSelectFrame = g_WinMgr:createWindow("TaharezLook/StaticImage","HSSelectFrame")
g_HSSelectFrame:setProperty("Image", "set:windows1 image:SelectBlock7")
g_HSSelectFrame:setProperty("MousePassThroughEnabled", "true")
g_HSSelectFrame:setProperty("WantsMultiClickEvents", "false")
g_HSSelectFrame:setProperty("FrameEnabled", "false")
g_HSSelectFrame:setProperty("BackgroundEnabled", "false")
g_HSSelectFrame:setProperty("AlwaysOnTop", "true")

local isSaling = false
g_GameEventMgr:subscribeEvent("START_BUY_HOUSE","OnStartHouseSale")
g_GameEventMgr:subscribeEvent("END_BUY_HOUSE","OnEndHouseSale")

-----------------------------------------------------------------
--设置选中框
local function _setSelectFrame(window )
	window:addChildWindow(g_HSSelectFrame)
	mWindow = window
end
---
local function _removeSelectFrame()
	if mWindow then
		mWindow:removeChildWindow(g_HSSelectFrame)
		mChooseHouseId = -1
	end
end
-----------------------------------------------------------------
local function _SetSearchLevel(level)
	mCurSearchLevel = level
	FSetText(g_houseSales.levelInfo, "房屋等级"..level)
end
-----------------------------------------------------------------
local function _Init()
	mCurPage = 1
	--_SetSearchLevel(1)
	mChooseHouseId = -1
end
-----------------------------------------------------------------
local function _ClearPage()
	for i = 1, mPerPageDataCount do
		g_WinMgr:getWindow(g_houseSales.slotName..i.."/Txt1"):setText("")
		g_WinMgr:getWindow(g_houseSales.slotName..i.."/Txt2"):setText("")
		g_WinMgr:getWindow(g_houseSales.slotName..i.."/Txt3"):setText("")
		g_WinMgr:getWindow(g_houseSales.slotName..i.."/Txt4"):setText("")
		g_WinMgr:getWindow(g_houseSales.slotName..i.."/Txt5"):setText("")
	end
	--g_WinMgr:getWindow(g_houseSales.info):setText("")
end
-----------------------------------------------------------------
--用于对当前选中对象的描述
local function _SetDescript(itemRow)
	--FSetText(g_houseSales.info, itemRow.descriptInfo)
end
-----------------------------------------------------------------
local function _Updata(page, srcTable)
	data = getData(srcTable, page, mPerPageDataCount)
	local itemRow, priceStr
	--_ClearPage()

	if 0 >= table.getn(data) then
		mChooseHouseId = -1
		if nil ~= mWindow then
			mWindow:removeChildWindow(g_HSSelectFrame)
		end
	end

	for i, v in pairs(data) do
		itemRow = g_logictableMgr:getHouseSalesRow(v.id)
		if 1 == i then
			_SetDescript(itemRow)
			mChooseHouseId = v.id
			_setSelectFrame(g_WinMgr:getWindow(g_houseSales.slotName..i))
		end

		FSetText(g_houseSales.slotName..i.."/Txt1", itemRow.name)
		FSetText(g_houseSales.slotName..i.."/Txt2", "等级"..itemRow.level)
		if (0 == itemRow.payType) then
			priceStr = "免费"
		elseif(1 == itemRow.payType) then
			priceStr = "金币："..itemRow.price
		else
			priceStr = "E Q："..itemRow.price
		end
		FSetText(g_houseSales.slotName..i.."/Txt3", priceStr)
		FSetText(g_houseSales.slotName..i.."/Txt4", "需要鲜花" ..itemRow.cost_flower)
		FSetText(g_houseSales.slotName..i.."/Txt5", "需要活跃值" ..itemRow.need_decoration)
	end
	FUpdatePageInfo(g_houseSales.pageInfo, page, getTotalPage(srcTable, mPerPageDataCount))
end
-----------------------------------------------------------------
--由于界面上有一个根据等级检索的功能。
--目前的做法是：遍历总表，将符合要求的
--插入另一个新的表中。
local function _GetSearchData(srcTable, level)
	destTable = {}
	
	for i,v in pairs(srcTable) do
		if v.level == level then
			table.insert(destTable, {id = v.id, level = v.level})
		end
	end

	return destTable
end
-----------------------------------------------------------------
local function _GetCurType()
	local index = GetCurTabIndex(g_houseSales.tabName)

	if 0 == index then
		index = 5
	end

	return index
end
-----------------------------------------------------------------
local function _SetUIVisible(flag)
	if g_WinMgr:getWindow("Task/TipAnimate") then
		g_WinMgr:getWindow("Task/TipAnimate"):setVisible(flag)
	end
	if g_WinMgr:getWindow("MailAnimation") then
		g_WinMgr:getWindow("MailAnimation"):setVisible(flag)
	end
	g_WinMgr:getWindow("DFwnd_MainRoot/RunBtn"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/Btn_ShowUpstair"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/Btn_SwitchWallnRoof"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_PutFurniture"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/telephone"):setVisible(flag)
	g_WinMgr:getWindow("DFwnd_MainRoot/hide"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/SImg_VMenubar"):setVisible(flag)
	--g_WinMgr:getWindow("DFwnd_MainRoot/DayNnight"):setVisible(flag)
	--g_WinMgr:getWindow("MainRoot/PointToPlayer"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/exit"):setVisible(not flag)
	g_WinMgr:getWindow("MainRoot/ZoomButton"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/liwu"):setVisible(flag)
	g_WinMgr:getWindow("DFwnd_MainRoot/broadcast"):setVisible(flag)
	g_WinMgr:getWindow("DFwnd_MainRoot/TaskBtn"):setVisible(flag)
	g_WinMgr:getWindow("DFwnd_MainRoot/tip"):setVisible(flag)
	
	SetUnViewMailGiftUIVisible(flag)	--礼物图标
	
	if mLiWuAnJianVisible == true then
		g_WinMgr:getWindow("DFwnd_MainRoot/liwuanjian"):setVisible(flag)
	end
	
	if true == flag then
		g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("BroadCast/BE/PareName"))
		g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("MainRoot/MTxt_ShowChatbox"))	
		g_MainUIWindows:addChildWindow(g_WinMgr:getWindow("BroadCast/BE/SpeakerName"))
	else
		g_MainUIWindows:removeChildWindow(g_WinMgr:getWindow("BroadCast/BE/PareName"))
		g_MainUIWindows:removeChildWindow(g_WinMgr:getWindow("MainRoot/MTxt_ShowChatbox"))
		g_MainUIWindows:removeChildWindow(g_WinMgr:getWindow("BroadCast/BE/SpeakerName"))
	end
end
-----------------------------------------------------------------
local function _SortTable(a, b)
	return a.id < b.id
end
-----------------------------------------------------------------
function HSOnloadUI()
	--g_WinMgr:getWindow(g_houseSales.levelBox):subscribeEvent("KeyUp", "HSOnEnterClick")
	--g_WinMgr:getWindow(g_houseSales.preLevel):subscribeEvent("MouseClick", "PreLevelHouse")
	--g_WinMgr:getWindow(g_houseSales.nextLevel):subscribeEvent("MouseClick", "NextLevelHouse")

	g_WinMgr:getWindow("houseshop/SlotBg/Btn_1"):subscribeEvent("Clicked", "HouseSales_sortByLv")
	g_WinMgr:getWindow("houseshop/SlotBg/Btn_2"):subscribeEvent("Clicked", "HouseSales_sortByPrice")
	g_WinMgr:getWindow("houseshop/SlotBg/Btn_3"):subscribeEvent("Clicked", "HouseSales_sortByFlower")
--
	g_WinMgr:getWindow(g_houseSales.prePage):subscribeEvent("MouseClick", "PrePageHouse")
	g_WinMgr:getWindow(g_houseSales.nextPage):subscribeEvent("MouseClick", "NextPageHouse")

	g_WinMgr:getWindow(g_houseSales.buyHouseBtn):subscribeEvent("Clicked", "HSShowConfirmDialog")
	g_WinMgr:getWindow(g_houseSales.closeBtn):subscribeEvent("Clicked", "OnCloseHouseSalesUI")
	g_WinMgr:getWindow(g_houseSales.viewHosueBtn):subscribeEvent("Clicked", "ReqPreviweHouse")
	g_WinMgr:getWindow("MainRoot/exit"):subscribeEvent("Clicked", "ReqEndPreviewHouse")

	for i = 1, 5 do
		g_WinMgr:getWindow(g_houseSales.tab..i):subscribeEvent("Clicked", "HSOnTabClicked")
	end

	for i = 1, mPerPageDataCount do
		g_WinMgr:getWindow(g_houseSales.slotName..i):subscribeEvent("MouseClick", "GetClickHouse")
	end

	MainUI_ShieldClick(g_houseSalesUI)
	Common_injectEmptyEvt(g_HouseSalesUINames.root)
end
-----------------------------------------------------------------
function HSShowConfirmDialog()
	if -1 ~= mChooseHouseId then
		--MsgBox_createMsgBoxEx(g_MainUIWindows,10030,"","替换房屋会清除你在房屋内的所有东西，是否继续？", "ReqBuyHouse", "MsgBox_cancelFunc")
		ReqBuyHouse()
	end
end
-----------------------------------------------------------------
function ReqBuyHouse()
	g_smallModules:reqBuyHouse(mChooseHouseId)
end
function ReqBuyHouseAdd()
	g_smallModules:reqBuyHouseAdd(mChooseHouseId)
end
function ReqBuyHouseReplace()
	-- (add by hezhr)
	if CEGUI.toCheckbox(g_WinMgr:getWindow(m_checkBoxNameTable[1])):isSelected() then
		mReplaceHouseType = buy_house_replace_type.yes
	else
		mReplaceHouseType = buy_house_replace_type.no
	end
	g_smallModules:reqBuyHouseReplace(mChooseHouseId, mReplaceHouseType)
end
-----------------------------------------------------------------
function OnStartHouseSale()
	--每个客户端启动时只读一次表
	--即第一次打开面板的时候读取
	_Init()
	if false == mIsLoad then
		g_smallModules:saveHouseShopData()

		for i = 1, 5 do
			table.sort(mData[i], _SortTable)
		end
	end
	
	CEGUI.toTabControl(g_WinMgr:getWindow(g_houseSales.tabName)):setSelectedTab(0)
	HSOnTabClicked()
	SetUIEnable(false)
	
	OpenHouseSalesUI()
	
	isSaling = true
end

function OnEndHouseSale()
	if mIsOpenUI then
		CloseHouseSalesUI()
		SetUIEnable(true)
	end
	
	isSaling = false
end

function refreshHouseSaleUI()
	if isSaling == true then
		OpenHouseSalesUI()
	else
		CloseHouseSalesUI()
	end
end

function SetUIEnable(enable)
	g_WinMgr:getWindow("MainRoot/VMenubar/Btn_Navigate"):setEnabled(enable)
end

function PopHouseSalesUI()
	if false == mIsOpenUI then
		ReqOpenHouseSalesUI()
		
		UIM_DoOpenAndCloseUI(g_HouseSalesUINames.root)
	else
		ReqCloseHouseSalesUI()
	end
end

function ReqOpenHouseSalesUI()
	EQGlobaFunc.reqStartBuyHouse()
end

function OpenHouseSalesUI()
	UIM_OpenWindow(g_HouseSalesUINames.root)
	SetWindowCenter(g_WinMgr:getWindow(g_HouseSalesUINames.root))
	mIsOpenUI = true
end

function ReqCloseHouseSalesUI()
	EQGlobaFunc.reqEndBuyHouse()
end

function CloseHouseSalesUI()
	UIM_CloseWindow(g_HouseSalesUINames.root)
	mIsOpenUI = false
end
-------------------------------------------------------------
--
function HouseSales_getUIOpenedFlag()
	return mIsOpenUI
end
-----------------------------------------------------------------
function OnCloseHouseSalesUI()
	if mIsOpenUI then
		ReqCloseHouseSalesUI()
	end
end
-----------------------------------------------------------------
function PrePageHouse()
	if 1 == mCurPage then
		return
	end

	local type = _GetCurType()

	mCurPage = mCurPage - 1

	if 0 == mCurSearchLevel then
		_Updata(mCurPage, mData[type])
	else
		_Updata(mCurPage, mSearchData)
	end
end
-----------------------------------------------------------------
function NextPageHouse()
	local totalPage
	local type = _GetCurType()
	if 0 == mCurSearchLevel then
		totalPage = getTotalPage(mData[type], mPerPageDataCount)
	else
		totalPage = getTotalPage(mSearchData, mPerPageDataCount)
	end

	if mCurPage >= totalPage then
		return
	end
	mCurPage = mCurPage + 1

	if 0 == mCurSearchLevel then
		_Updata(mCurPage, mData[type])
	else
		_Updata(mCurPage, mSearchData)
	end
end
-----------------------------------------------------------------
function GetClickHouse(args)
	local type = _GetCurType()
	local clickName = CEGUI.toWindowEventArgs(args).window:getName()

	local index = GetClickWindowIndex(clickName, g_houseSales.slotName, 1, mPerPageDataCount)

	if 0 ~= index then
		index = FShopGetTableIndex(index, mCurPage, mPerPageDataCount)
	else
		return
	end

	if nil == index then
		return
	end

	local itemRow
	if 0 == mCurSearchLevel then
		if index <= table.getn(mData[type]) then
			itemRow = g_logictableMgr:getHouseSalesRow(mData[type][index].id)
		end
	else
		if nil ~= mSearchData[index] then
			itemRow = g_logictableMgr:getHouseSalesRow(mSearchData[index].id)
		end
	end
	if nil ~= itemRow then
		mChooseHouseId = itemRow.indexId
		_SetDescript(itemRow)
		_setSelectFrame(CEGUI.toWindowEventArgs(args).window)
	end
end
-----------------------------------------------------------------
function lua_addHouseSales(indexId, type, levelV, recommend, price, cost_flower,index, num)
	if true == recommend then
		table.insert(mData[5], {id = indexId, level = levelV,price = price,cost_flower = cost_flower})
		mLevelData[5][levelV] = levelV
	end

	table.insert(mData[type], {id = indexId, level = levelV,price = price,cost_flower = cost_flower})
	mLevelData[type][levelV] = levelV

	if index == num then
		mIsLoad = true
	end
end
-----------------------------------------------------------------
--function PreLevelHouse()
--	local type = _GetCurType()
--	local preLevel
--	local sl = {}
--	
--	for i, v in pairs(mLevelData[type]) do
--		if v < mCurSearchLevel then
--			table.insert(sl, v)
--		end
--	end
--	
--	if 0 == table.getn(sl) then
--		return
--	end
--	
--	table.sort(sl, function(a, b) return a > b end)
--	
--	UpdateSaleHouseList(type, sl[1], 1)
--end
-----------------------------------------------------------------
--function NextLevelHouse()
--	local type = _GetCurType()
--	local sl = {}
--	
--	for i, v in pairs(mLevelData[type]) do
--		if v > mCurSearchLevel then
--			table.insert(sl, v)
--		end
--	end
--	
--	if 0 == table.getn(sl) then
--		return
--	end
--	
--	table.sort(sl)
--	
--	UpdateSaleHouseList(type, sl[1], 1)
--end
-----------------------------------------------------------------
function HSOnTabClicked()
	local type = _GetCurType()
	local sl = {}
	_ClearPage()
	_removeSelectFrame()
	for i, v in pairs(mLevelData[type]) do
		table.insert(sl, v)
	end
	
	if 0 == table.getn(sl) then
		return
	end
	
	table.sort(sl)
	
	UpdateSaleHouseList(type, sl[1], 1)
end
-----------------------------------------------------------------
function UpdateSaleHouseList(type, level, page)
	--_SetSearchLevel(level)
	mCurPage = page
	--mSearchData = _GetSearchData(mData[type], level)
	--_Updata(mCurPage, mSearchData)
	_Updata(mCurPage, mData[type])
end
-----------------------------------------------------------------
function ReqPreviweHouse()
	if -1 ~= mChooseHouseId then
		g_smallModules:reqPreviewHouse(mChooseHouseId)
		OnCloseHouseSalesUI()
	end
end
-----------------------------------------------------------------
function ReqEndPreviewHouse()
	g_smallModules:reqEndPreviewHouse()
	--_SetUIVisible(true)
end
function OnEndPreviewHouse()
	_SetUIVisible(true)
end
-----------------------------------------------------------------
function PreviewHouseUI()
	_SetUIVisible(false)
end

function OnHandleBuyHouse(args)
	if buy_house_result.bhr_replace == args then
		-- (modify by hezhr)
		local ckblist = {{text = "沿用房屋权限设置（包括欢迎语）", state = true}}
		--默认为yes
		mReplaceHouseType = buy_house_replace_type.yes
		m_checkBoxNameTable = MsgBox_createMsgBoxExA(g_MainUIWindows,300,"","该操作会替换你当前使用的房屋，并且清除所有当前使用房屋内的数据，是否继续？", ckblist, "ReqBuyHouseReplace", "MsgBoxEx_close")
	elseif buy_house_result.bhr_add == args then
		MsgBox_createMsgBox(g_MainUIWindows,0,"","确定购买此房屋吗？", "ReqBuyHouseAdd", "MsgBox_cancelFunc")
	else
		--g_Loger:logError("OnHandleBuyHouse:undefined buy_house_result:"..tostring(args))
	end
end
-----------------------------------------------------------------
--按等级排序
--
local function _sortByLv(a,b)
	if a.level == b.level then
		if a.price == b.price then
			return a.id < b.id
		else
			return a.price < b.price
		end
	else
		return a.level < b.level
	end
end
--
function HouseSales_sortByLv()
	local type = _GetCurType()
	if type ~= nil and type ~= 0 then
		table.sort(mData[type], _sortByLv)
		_Updata(mCurPage,mData[type])
	end
end
-------------------------------------------------------------------
--按价格排序
local function _sortByPrice(a,b)
	if a.price == b.price then
		if a.level == b.level then
			return a.id < b.id
		else
			return a.level < b.level
		end
	else
		return a.price < b.price
	end
end
--
function HouseSales_sortByPrice()
	local type = _GetCurType()
	if type ~= nil and type ~= 0 then
		table.sort(mData[type], _sortByPrice)
		_Updata(mCurPage,mData[type])
	end
end
-------------------------------------------------------------------
--按鲜花排序
local function _sortByFlower(a,b)
	if a.cost_flower == b.cost_flower then
		if a.level == b.level then
			return a.id < b.id
		else
			return a.level < b.level
		end
	else
		return a.cost_flower < b.cost_flower
	end
end
--
function HouseSales_sortByFlower()
	local type = _GetCurType()
	if type ~= nil and type ~= 0 then
		table.sort(mData[type], _sortByFlower)
		_Updata(mCurPage,mData[type])
	end
end