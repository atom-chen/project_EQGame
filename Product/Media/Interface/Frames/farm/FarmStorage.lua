-----------------------------------------------------------------
--庄园仓库
--xiaoMing
-----------------------------------------------------------------
--require('luaunit')

g_FStorageFlag = false
g_FStorageSaleFlag = false
g_UseDogFoodFlag = false

local mLastflag = false
local mLastType = 0

local mType = {
				fruit = 1,
				prop  = 2,
				seed  = 3,
				}

--存放数据
--直接用数字下标更容易操作
--1 代表 flower 2代表seed
local mData = {{}
				 ,{},
				  {}}

local mFStoragePrice = {0, 0, 0}

local mCurPage = {1, 1, 1}
--表示每页显示的数量
local mFStoragePageSize = 15

local mPickItemInfo = {curCount = 1, maxCount = 0, instanceId = "", lock = -1, prePrice = 0}

local mTipText = {"     你的仓库空空的，连老鼠都不来光顾，一分".."\n".."耕耘一分收获，赶紧去种植吧！",                   
			"     没有可以使用的道具",
			"     仓库里空空，什么种子都没有"
			}
			
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
-----------------------------------------------------------------
---------------------------- 私有函数 ---------------------------
-----------------------------------------------------------------
-----------------------------------------------------------------
local function _InitData()
	mData = {{}
			,{},
			 {}}
	mPickItemInfo = {curCount = 1, maxCount = 0, instanceId = "", lock = -1, prePrice = 0}

	mFStoragePrice = {0, 0, 0}
	mCurPage = {1, 1, 1}
	
	if true == g_FStorageSaleFlag then
		FCloseStorageSaleUI()
	end
end
-----------------------------------------------------------------
--获取当前type
local function _GetType()
	local index = GetCurTabIndex(g_farmStorage["tabName"])
	return index + 1
end
-----------------------------------------------------------------
--清空一个Grid
--0 代表全部， 1 大icon 2 活动icon 3 锁icon 4 为等级
local function _ClearGrid(gridName)
	--隐藏格子
	g_WinMgr:getWindow(gridName):setVisible(false)
	
	for i =1, 2 do
		local slotName = gridName.."/"..i
		local slotWin = g_WinMgr:getWindow(slotName)
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
		end
	end
	--锁
	g_WinMgr:getWindow(gridName.."/3"):setProperty("Image", "")
	--等级
	g_WinMgr:getWindow(gridName.."/4"):setText("")
end
-----------------------------------------------------------------
--清空格子Icon
local function _ClearAllGird(type)
	local str = "Root/zhuchuangkou/biaoqian/Tab "..type.."/icon"
	
	for i = 1, mFStoragePageSize do
		local gridName = str..i
		_ClearGrid(gridName)
	end
end
-----------------------------------------------------------------
--动态加载图片
--1 种子等icon
local function _SetSlotImage(slotName, index, imagesetName, imgName)
	StorageUI_DynamicLoadItem(slotName, "newImg", index, imagesetName, imgName, "False")
	local container = g_WinMgr:getWindow(slotName):getChildAtIdx(0)
	container:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}")
end
-----------------------------------------------------------------
--清空item数量的显示
local function _ClearItemCountWin(type)
	local str = "Root/zhuchuangkou/biaoqian/Tab "..type.."/txt"
	
	for i = 1, mFStoragePageSize do
		local winName = str..i
		g_WinMgr:getWindow(winName):setText("")
	end
end
-----------------------------------------------------------------
--加载一个Grid
--0 代表全部， 1 大icon 2 活动icon 3 锁icon 4 为等级
--此处返回一个price
local function _SetGrid(gridName, gridIndex, itemRow, lock ,level)
	local imagesetName, iconName, slotName
	g_WinMgr:getWindow(gridName):setVisible(true)
	--加载果实icon
	if nil ~= itemRow then
		slotName = gridName.."/1"
		imagesetName = itemRow.imageset
		iconName = itemRow.icon
		_SetSlotImage(slotName, gridIndex, imagesetName, iconName)
	end
	--加载锁icon
	if nil ~= lock then
		if 1 == lock then
			g_WinMgr:getWindow(gridName.."/3"):setProperty("Image", "set:window2 image:farm_Storehouse2")
		end
	end

	FSetText(gridName.."/4", level.."级")
end
-----------------------------------------------------------------
--更新 根据Tab当前页面 获取type
local function _Update(curPage, type)
	local itemTable = getData(mData[type],curPage,mFStoragePageSize)
	local curType = _GetType()
	
	_ClearAllGird(type)
	_ClearItemCountWin(type)

	if 0 == table.getn(itemTable) then	
		return
	end

	local str  = "Root/zhuchuangkou/biaoqian/Tab "..type.."/icon"
	local str1 = "Root/zhuchuangkou/biaoqian/Tab "..type.."/txt"

	local size = table.getn(itemTable)
	
	--若道具中没有狗粮，则关闭使用狗粮界面
	if type == 2 then
		local hasDogFood = false
		for i = 1, size do
			local itemRow = g_itemtableMgr:getItemRow(itemTable[i]["id"])
			local propRow = g_FarmTableMgr:getFarmPropRow(itemRow.subID)
			if propRow.type == 7 then
				hasDogFood = true
			end
		end
		if hasDogFood == false then
			FarmStorage_closeUseDogFoodUI()
		end
	end
	--
	for i = 1, size do
		--icon
		--lock
		local winName = str..i
		local itemRow = g_itemtableMgr:getItemRow(itemTable[i]["id"])
		
		--lock
		_SetGrid(winName, i, itemRow, itemTable[i]["lock"], itemTable[i]["level"])
		--numText
		winName = str1..i
		g_WinMgr:getWindow(winName):setText(itemTable[i]["count"])
	end
	
	if curType == type then
		g_WinMgr:getWindow(g_farmStorage["totalPrice"]):setText(mFStoragePrice[type])
		FUpdatePageInfo(g_farmStorage["pageText"], curPage, getTotalPage(mData[type], mFStoragePageSize))
	end
end
-----------------------------------------------------------------
--获取Item数据
local function _GetItemInfo(packData)
	local itemRow = g_itemtableMgr:getItemRow(packData["id"])
	local result = {}
	--名字 图片集名称 icon名称 单价 总价 数量
	result["name"] = itemRow.name
	result["imagesetName"] = itemRow.imageset
	result["icon"] = itemRow.icon
	result["perPrice"] = itemRow.sell_price
	result["count"] = packData["count"]
	result["totalPrice"] = packData["count"]*result["perPrice"]
	result["instanceId"] = packData["instanceId"]
	result["lock"] = packData["lock"]
	result["active"] = 0--暂时无法获得此值
	result["level"] = packData["level"]
	result["subID"] = itemRow.subID
	return result
end
-----------------------------------------------------------------
local function _ChangeNum(side, count)
	if 1 == count then
		return
	end

	if 0 == side then
		if 1 == mPickItemInfo["curCount"] then
			return
		else
			mPickItemInfo["curCount"] = mPickItemInfo["curCount"] - 1
		end
	else
		if count == mPickItemInfo["curCount"] then
			return
		else
			mPickItemInfo["curCount"] = mPickItemInfo["curCount"] + 1
		end
	end

	g_WinMgr:getWindow(g_farmStorage["saleNum"]):setText(mPickItemInfo["curCount"])
	g_WinMgr:getWindow("Root/maichu/zhuyemian/txt6"):setText(mPickItemInfo["prePrice"] * mPickItemInfo["curCount"])

end
-----------------------------------------------------------------
--获得选中的item的表索引
local function _GetItemIndex(type, slotName)
	--凡此类字符串都可以封装 暂时直接显示
	local str1 = "Root/zhuchuangkou/biaoqian/Tab "..type.."/icon", cmpName, index
	local str2 = "/DragContainer"
	for i = 1, mFStoragePageSize do
		cmpName = str1..i
		if slotName == cmpName then
			index = i
			break
		end
	end

	if nil == index then
		return
	end

	index = (FGetStorageCurPage(type)-1) * mFStoragePageSize + index
	if index <= table.getn(mData[type]) then
		return index
	else
		return nil
	end
end
----------------------------------------------------------------
--设置使用狗粮界面信息
function _SetUseDogFoodData(result)
	local imgWinName = "use_dog_food/background/icon"
	local nameWin = "use_dog_food/background/name"
	local eqGoldWin = "use_dog_food/background/count"
	
	EQGlobaFunc.SetText(imgWinName,tostring(result.instanceId))
	EQGlobaFunc.SetText("use_dog_food/background",tostring(result.count))
	StorageUI_DynamicLoadItem(imgWinName, "dogfood", 0, result.imagesetName, result.icon, "False")
	EQGlobaFunc.SetText(nameWin,result.name)
	
	EQGlobaFunc.SetText(eqGoldWin,tostring(result.perPrice))
end
-----------------------------------------------------------------
--设置销售面板的数据
local function _SetSaleUIData(itemInfoTable)
	mPickItemInfo["curCount"] = itemInfoTable["count"]
	mPickItemInfo["maxCount"] = mPickItemInfo["curCount"]
	mPickItemInfo["instanceId"] = itemInfoTable["instanceId"]..""
	mPickItemInfo["lock"]	= itemInfoTable["lock"]
	mPickItemInfo["prePrice"] = itemInfoTable["perPrice"]
	--加载ICON
	_SetSlotImage( "Root/maichu/zhuyemian/icon1/1",
					1, itemInfoTable["imagesetName"],
					itemInfoTable["icon"])

	if 0 == mPickItemInfo["lock"] then
		g_WinMgr:getWindow(g_farmStorage["lockIcon"]):setVisible(false)
		g_WinMgr:getWindow(g_farmStorage["saleBtn"]):setEnabled(true)
		EQGlobaFunc.SetText(g_farmStorage["lockText"],"未上锁")
		EQGlobaFunc.SetText(g_farmStorage["lockBtn"],"锁定")
	else
		g_WinMgr:getWindow(g_farmStorage["lockIcon"]):setVisible(true)
		g_WinMgr:getWindow(g_farmStorage["saleBtn"]):setEnabled(false)
		EQGlobaFunc.SetText(g_farmStorage["lockText"],"已锁定")
		EQGlobaFunc.SetText(g_farmStorage["lockBtn"],"解锁")
	end
	--设置活动标签
	if 1 == itemInfoTable["active"] then
		g_WinMgr:getWindow("Root/maichu/zhuyemian/icon1/2"):setVisible(true)
	else
		g_WinMgr:getWindow("Root/maichu/zhuyemian/icon1/2"):setVisible(false)
	end

	g_WinMgr:getWindow(g_farmStorage["saleNum"]):setText(mPickItemInfo["curCount"])
	EQGlobaFunc.SetText("Root/maichu/zhuyemian/txt7","请输入数量(1~"..itemInfoTable["count"]..")")
	g_WinMgr:getWindow("Root/maichu/zhuyemian/txt5"):setText(itemInfoTable["perPrice"])
	g_WinMgr:getWindow("Root/maichu/zhuyemian/txt6"):setText(itemInfoTable["totalPrice"])
	EQGlobaFunc.SetText("Root/maichu/zhuyemian/txt1",itemInfoTable["name"])

end
-----------------------------------------------------------------
local function _SortData()
	for i = 1, 3 do
		table.sort(mData[i], FarmSortFunc)
	end
end
-----------------------------------------------------------------
function FStorageOnLoad()
	--注册事件
	--请求卖出所有果实
	local win
	win = g_WinMgr:getWindow(g_farmStorage["sellAllBtn"])
	win:subscribeEvent("Clicked", "FIsAllSale")
	win:setProperty("NormalTextColour","FF000000")
	--请求卖出果实
	win = g_WinMgr:getWindow(g_farmStorage["saleBtn"])
	win:subscribeEvent("Clicked", "OnSaleFarmStorage")
	win:setProperty("NormalTextColour","FF000000")
	--请求放入背包
	--win = g_WinMgr:getWindow(g_farmStorage["inputBagBtn"])
	--win:subscribeEvent("Clicked", "FReqPutInBag")
	--win:setProperty("NormalTextColour","FF000000")
	--win:setEnabled(false)
	----请求锁定或解锁
	win = g_WinMgr:getWindow(g_farmStorage["lockBtn"])
	win:subscribeEvent("Clicked", "FReqLock")
	win:setProperty("NormalTextColour","FF000000")
	--Tab点击事件
	g_WinMgr:getWindow(g_farmStorage["tab1Btn"]):subscribeEvent("Clicked", "FTabBtnClicked")
	g_WinMgr:getWindow(g_farmStorage["tab2Btn"]):subscribeEvent("Clicked", "FTabBtnClicked")
	g_WinMgr:getWindow(g_farmStorage["tab3Btn"]):subscribeEvent("Clicked", "FTabBtnClicked")

	win = g_WinMgr:getWindow(g_farmStorage["saleCancle"])
	win:subscribeEvent("Clicked", "FCloseStorageSaleUI")
	win:setProperty("NormalTextColour","FF000000")
	g_WinMgr:getWindow(g_farmStorage["saleCloseBtn"]):subscribeEvent("Clicked", "FCloseStorageSaleUI")

	--翻页按钮注册事件
	g_WinMgr:getWindow(g_farmStorage["perPageBtn"]):subscribeEvent("Clicked", "FOnClickedLeftBtn")
	g_WinMgr:getWindow(g_farmStorage["nextPageBtn"]):subscribeEvent("Clicked", "FOnClickedRightBtn")

	--销售页面数量按钮
	g_WinMgr:getWindow(g_farmStorage["salePerNum"]):subscribeEvent("Clicked", "FOnClickedLeftNum")
	g_WinMgr:getWindow(g_farmStorage["saleNextNum"]):subscribeEvent("Clicked", "FOnClickedRightNum")

	g_WinMgr:getWindow(g_farmStorage["closeBtn"]):subscribeEvent("Clicked", "FRemoveStoragePanel")
	g_WinMgr:getWindow(g_farmStorage["saleNum"]):subscribeEvent("TextChanged", "FOnChangeNumInput")
	
	g_WinMgr:getWindow(g_farmStorage["toShopBtn"]):subscribeEvent("Clicked", "PopFarmShopUI")
	
	g_WinMgr:getWindow(g_farmStorage["UseDogFoodConfirm"]):subscribeEvent("Clicked", "FarmStorage_UseDogFood")
	g_WinMgr:getWindow(g_farmStorage["UseDogFoodCancel"]):subscribeEvent("Clicked", "FarmStorage_closeUseDogFoodUI")
	g_WinMgr:getWindow(g_farmStorage["UseDogFoodCloseBtn"]):subscribeEvent("Clicked", "FarmStorage_closeUseDogFoodUI")
	
	g_WinMgr:getWindow(g_farmStorage.emptyTipWnd):setVisible(false)

	local window = g_WinMgr:getWindow(g_farmStorage["saleNum"])
	CEGUI.toEditbox(window):setValidationString("^$|^[0-9]*[1-9][0-9]*$")
	
	for i = 1, 3 do
		for j = 1, mFStoragePageSize do
			local slotName = "Root/zhuchuangkou/biaoqian/Tab "..i.."/icon"..j
			g_WinMgr:getWindow(slotName):subscribeEvent("MouseClick", "FarmStorage_switchPopPropUI")
			g_WinMgr:getWindow(slotName):subscribeEvent("MouseEnter", "FarmStorage_MouseEnter")
			g_WinMgr:getWindow(slotName):subscribeEvent("MouseLeave", "FarmStorage_MouseLeave")
			
			for z = 1, 4 do
				g_WinMgr:getWindow(slotName.."/"..z):setProperty("MousePassThroughEnabled", "true")
			end
			
		end
	end
	
	for i = 1, 3 do
		_ClearAllGird(i)
	end
		
end
-----------------------------------------------------------------
function FReqStorageData()
		Dec_resetDataTab()	--请求前清空装饰表数据
		g_req_open_ui.type = 3
		g_msgmgr:send(g_req_open_ui)
end
-----------------------------------------------------------------
--打开
function FPopStoragePanel()
	if false == g_FStorageFlag then
		FarmStorage_closeOtherUI()
		g_FarmUI:addChildWindow(g_FarmStorage)
		g_FStorageFlag = true
		FReqStorageData()
		CEGUI.toTabControl(g_WinMgr:getWindow(g_farmStorage.tabName)):setSelectedTabAtIndex(0)
		SetWindowCenter(g_FarmStorage)
	else
		FRemoveStoragePanel()
	end
end
-----------------------------------------------------------------
--关闭
function FRemoveStoragePanel()
	g_FarmUI:removeChildWindow(g_FarmStorage)
	g_FStorageFlag = false

	if true == g_FStorageSaleFlag then
		FCloseStorageSaleUI()
	end
end
-----------------------------------------------------------------
--翻页按钮 左
function FOnClickedLeftBtn()
	local type = _GetType()
	local totalPage = getTotalPage(mData[type], mFStoragePageSize)

	if 1 >= totalPage then
		return
	end

	if 1 == mCurPage[type] then
		return
	end

	mCurPage[type] = mCurPage[type] - 1
	_Update(mCurPage[type], type)
end
-----------------------------------------------------------------
--翻页按钮 右
function FOnClickedRightBtn()
	local type = _GetType()
	local totalPage = getTotalPage(mData[type], mFStoragePageSize)

	if 1 >= totalPage then
		return
	end

	if totalPage == mCurPage[type] then
		return
	end

	mCurPage[type] = mCurPage[type] + 1
	_Update(mCurPage[type], type)
end
-----------------------------------------------------------------
--get set 函数
function FGetStorageCurPage(type)
	return mCurPage[type]
end
-----------------------------------------------------------------
--庄园仓库售卖模块
-----------------------------------------------------------------
-----------------------------------------------------------------
function FPopStorageSaleUI()
	if false == g_FStorageSaleFlag then
		g_FarmUI:addChildWindow(g_FStorageSaleUI)
		Common_injectEmptyEvt("Root/maichu")
		g_FStorageSaleFlag = true
	else
		return
	end
end
-----------------------------------------------------------------
function FCloseStorageSaleUI()
	g_FarmUI:removeChildWindow(g_FStorageSaleUI)
	g_FStorageSaleFlag = false
end
-----------------------------------------------------------------
--
function FarmStorage_switchPopPropUI(args)
	local SlotWin 		= CEGUI.toWindowEventArgs(args).window
	local SlotName 		= SlotWin:getName()
	local type  = _GetType()
	local index = _GetItemIndex(type, SlotName)
	if nil ~= index then
		local result = _GetItemInfo(mData[type][index])
		local propRow = g_FarmTableMgr:getFarmPropRow(result.subID)
		if propRow ~= nil then
			if propRow.type == ENUM_PROPTYPE.DOG_FOOD then
				FarmStorage_UseDogFoodUI(args,result)
			else
				FarmStorageSaleUI(args,result)
			end
		else
			FarmStorageSaleUI(args,result)
		end
	end
end
-----------------------------------------------------------------
--
function FarmStorage_UseDogFoodUI(args,result)
	FarmStorage_popUseDogFoodUI()
	_SetUseDogFoodData(result)
end
-----------------------------------------------------------------
--
function FarmStorage_popUseDogFoodUI()
	if false == g_UseDogFoodFlag then
		g_FarmUI:addChildWindow(g_UseDogFoodUI)
		Common_injectEmptyEvt("use_dog_food")
		g_UseDogFoodFlag = true
	else
		return
	end
end
-----------------------------------------------------------------
--
function FarmStorage_closeUseDogFoodUI()
	g_FarmUI:removeChildWindow(g_UseDogFoodUI)
	g_UseDogFoodFlag = false
end
----------------------------------------------------------------
--
function FarmStorage_UseDogFood()
	local imgWinName = "use_dog_food/background/icon"
	local imgWinName1 = "use_dog_food/background"
	
	local itemInstStr = g_WinMgr:getWindow(imgWinName):getText()
	local countStr = g_WinMgr:getWindow(imgWinName1):getText()
	local foodCount = tonumber(countStr)
	if foodCount ~= 0 then
		g_FarmMgr:reqUsePropStr(0,itemInstStr)
	else
		return
	end
	foodCount = foodCount - 1
	g_WinMgr:getWindow(imgWinName1):setText(tostring(foodCount))
end
-----------------------------------------------------------------
--售卖界面
function FarmStorageSaleUI(args,result)
	FPopStorageSaleUI()
	_SetSaleUIData(result)
end
-----------------------------------------------------------------
function FOnClickedLeftNum()
	_ChangeNum(0, mPickItemInfo["maxCount"])
end
-----------------------------------------------------------------
function FOnClickedRightNum()
	_ChangeNum(1, mPickItemInfo["maxCount"])
end
-----------------------------------------------------------------
function FOnChangeNumInput()
	local inputMoney = g_WinMgr:getWindow(g_farmStorage["saleNum"]):getText()
	local value = tonumber(inputMoney)

	if nil == value then
		return
	end

	if value > mPickItemInfo["maxCount"] then
		mPickItemInfo["curCount"] = mPickItemInfo["maxCount"]
		g_WinMgr:getWindow(g_farmStorage["saleNum"]):setText(mPickItemInfo["curCount"])
	else
		mPickItemInfo["curCount"] = value
	end
	
	g_WinMgr:getWindow("Root/maichu/zhuyemian/txt6"):setText(mPickItemInfo.prePrice * mPickItemInfo.curCount)
end
-----------------------------------------------------------------
function FTabBtnClicked()
--每次点击只更新页面信息 不更新格子中的内容
	local type = _GetType()
	local totalPage = getTotalPage(mData[type], mFStoragePageSize)
	local curPage = FGetStorageCurPage(type)
	
	g_WinMgr:getWindow(g_farmStorage["totalPrice"]):setText(mFStoragePrice[type])
	FUpdatePageInfo(g_farmStorage["pageText"], curPage, getTotalPage(mData[type], mFStoragePageSize))
	
	--仓库为空提示
	if 0 == table.getn(mData[type]) then
		HandleFarmStorageEmpty(type, true)
	else
		HandleFarmStorageEmpty(type, false)
	end
end

-----------------------------------------------------------------
function lua_addFarmStorage(instanceId, effTime, templateId, count, lock, index, num)
	if 1 == index then
		_InitData()
	end
	--根据模板ID 获得type 然后分类
	local itemRow = g_itemtableMgr:getItemRow(templateId)
	if nil ~= itemRow then
		type = itemRow.type
		--type 转换
		if 5 == type then
			--果实
			type = mType.fruit
		elseif 6 == type then
			--道具
			type = mType.prop
		elseif 4 == type then
			--种子
			type = mType.seed
		elseif 7 == type then --装饰
			Dec_getDecData(instanceId,templateId, count)
		else
			type = 0
		end

		if 0 ~= type and 7 ~= type then
			local data = {}
			data["id"] = templateId
			data["count"] = count
			data["instanceId"] = instanceId
			data["lock"] = lock
			data["level"] = g_FarmTableMgr:getFarmItemRow(templateId).itemLevel
			data["effTime"] = effTime
			
			if 0 == lock then
				mFStoragePrice[type] = mFStoragePrice[type] + itemRow.sell_price * count
			end
			--保存数据
			table.insert(mData[type],data)
		end
	end

	if index == num then
		--排序
		_SortData()
		--更新
		for i = 1, 3 do
			local curPage = FGetStorageCurPage(i)
			_Update(curPage, i)		
		end	
		--种子包 + 道具包		
		SetSeedAndPropPack(mData[mType.seed], mData[mType.prop])
		UpdateSeedBag()
		--检测默认初始页是否为空 
		FTabBtnClicked()
	end
end
-----------------------------------------------------------------
--请求卖出
function OnSaleFarmStorage()
	type = _GetType()
	
	if 2 == type then
		MsgBox_createMsgBox(g_FarmUI,10010,"","道具来之不易，确定卖出？", "FramStorageReqSell", "MsgBox_cancelFunc")
	elseif 3 == type then
		MsgBox_createMsgBox(g_FarmUI,10011,"","种子来之不易，确定卖出？", "FramStorageReqSell", "MsgBox_cancelFunc")
	else
		FramStorageReqSell()
	end
end
-----------------------------------------------------------------
function FramStorageReqSell()
	local instanceId = mPickItemInfo["instanceId"]
	local count = mPickItemInfo["curCount"]
	g_FarmMgr:reqOneSell(tostring(instanceId), count)
	g_farmTaskFinishSell = true	-- (add by hezhr)
	g_GameEventMgr:subscribeEvent("NOTIFY_CLOSE_SYSMSG_BOX", "FarmTask_script7")	-- (add by hezhr)
end
-----------------------------------------------------------------
--全部卖出确认框
function FIsAllSale()
	MsgBox_createMsgBox(g_FarmUI,10012,"","确定全部卖出吗？", "FReqStorageSaleAll", "MsgBox_cancelFunc")
end
-----------------------------------------------------------------
--请求全部卖出
function FReqStorageSaleAll()
	local type = _GetType()

	if 1 == type then
	--果实
		type = 5
	elseif 2 == type then
	--道具
		type = 6
	elseif 3 == type then
	--种子
		type = 4
	else
		return
	end

	g_FarmMgr:reqAllSell(type)
	g_GameEventMgr:subscribeEvent("NOTIFY_CLOSE_SYSMSG_BOX", "FarmTask_script7")	-- (add by hezhr)
end
-----------------------------------------------------------------
--请求锁定或解锁物品
function FReqLock()
	local lock = mPickItemInfo["lock"]
	local instanceId = mPickItemInfo["instanceId"]
	if 0 == lock then
		g_FarmMgr:reqLockStorageItem(instanceId)
	else
		g_FarmMgr:reqUnLockStorageItem(instanceId)
	end
end
-----------------------------------------------------------------
--
function FarmStorage_closeOtherUI()
	if true == g_FShopFlag then
		CloseFarmShopUI()
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
	Common_injectEmptyEvt("Root/zhuchuangkou")
end
-----------------------------------------------------------------
function HandleFarmStorageEmpty(type, flag)
	if flag == mLastflag and mLastType == type then
		return
	else
		if true == flag then
			g_WinMgr:getWindow(g_farmStorage.emptyTipWnd):setProperty("AlwaysOnTop", "true")
			FSetText(g_farmStorage.emptyTxt, mTipText[type])
		end
	
		g_WinMgr:getWindow(g_farmStorage.emptyTipWnd):setVisible( flag)
		g_WinMgr:getWindow(g_farmStorage.sellAllBtn):setEnabled(not flag)
		mLastflag = flag
		mLastType = type
	end	
end
-----------------------------------------------------------------
--显示itemhint
function FarmStorage_MouseEnter(args)
	local slotname = CEGUI.toWindowEventArgs(args).window:getName()
	local type = _GetType()
	local flag = 1
	if 1 == type then	-- 蔬果
		flag = 0
	end
	local index = _GetItemIndex(type, slotname)
	if nil ~= index then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		data = mData[type][index]
		createItemhint_farm(data.id, rect, g_FarmUI, flag, data.effTime)
	end
end
-----------------------------------------------------------------
--隐藏itemhint
function FarmStorage_MouseLeave(args)
	destroyItemhint(g_FarmUI)
end