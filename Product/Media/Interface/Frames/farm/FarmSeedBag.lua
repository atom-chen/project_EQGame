-----------------------------------------------------------------
--农场播种界面(后来添加了道具数据)
--xiaoMing
--2010-08-23
-----------------------------------------------------------------
local mBagSize = 6
g_seedBagFlag = false
local mStartIndex = 1
local mClickSeedId = 0
local mLastState
local mPack = {}
-----------------------------------------------------------------
function OnLoadSeedBagUI()
	--设置界面位置
	local h1, h2
	screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	h1 = g_FarmSeedBagUI:getHeight():asAbsolute(screenH)
	h2 = g_WinMgr:getWindow("FarmmainUI/FarmEntertainment"):getHeight():asAbsolute(screenH)
	w  = g_FarmSeedBagUI:getWidth():asAbsolute(screenW)
	g_FarmSeedBagUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-w/2),
													CEGUI.UDim(1, -h1-h2-15)))

	for i = 1, mBagSize do
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i):subscribeEvent("MouseClick", "OnClickSeed")
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i):subscribeEvent("MouseEnter", "FarmSeed_MouseEnter")
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i):subscribeEvent("MouseLeave", "FarmSeed_MouseLeave")
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i.."/icon"..i):setProperty("MousePassThroughEnabled", "true")
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i.."/quantity"..i):setProperty("MousePassThroughEnabled", "true")
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i.."/source"..i):setProperty("MousePassThroughEnabled", "true")
	end
	
	g_WinMgr:getWindow(g_farmSeedBag.toFarmStorage):subscribeEvent("MouseClick", "ToFarmStorage")
	g_WinMgr:getWindow(g_farmSeedBag.toShopBtn):subscribeEvent("Clicked" , "ToFarmShop")
	
	--连续点击
	g_WinMgr:getWindow(g_farmSeedBag.preBtn):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow(g_farmSeedBag.preBtn):setAutoRepeatRate(0.1)

	g_WinMgr:getWindow(g_farmSeedBag.nextBtn):setMouseAutoRepeatEnabled(true)
	g_WinMgr:getWindow(g_farmSeedBag.nextBtn):setAutoRepeatRate(0.1)
	
	--屏蔽双击按键
	g_WinMgr:getWindow(g_farmSeedBag.preBtn):setWantsMultiClickEvents(false)
	g_WinMgr:getWindow(g_farmSeedBag.nextBtn):setWantsMultiClickEvents(false)

	g_WinMgr:getWindow(g_farmSeedBag.preBtn):subscribeEvent("MouseButtonDown", "OnPreSeed")
	g_WinMgr:getWindow(g_farmSeedBag.nextBtn):subscribeEvent("MouseButtonDown", "OnNextSeed")
	
	for i = 1, mBagSize do
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i):setVisible(false)
	end
	
	MainUI_ShieldClick(g_FarmSeedBagUI)
	Common_injectEmptyEvt("farm_seedbag")
end
-----------------------------------------------------------------
function PopSeedBag()
	if false == g_seedBagFlag then
		EQGlobaFunc.setFarmSeedingCurSta(0, "", "")
		mStartIndex = 1
		mClickSeedId = 0
		mLastState = true
		--暂时不判断 仓库数据是否已经存在
		FReqStorageData()
		g_FarmUI:addChildWindow(g_FarmSeedBagUI)
		g_seedBagFlag = true
	else
		CloseSeedBag()
	end
end
-----------------------------------------------------------------
function CloseSeedBag()
	g_FarmUI:removeChildWindow(g_FarmSeedBagUI)
	g_seedBagFlag = false
end
-----------------------------------------------------------------
function SetSeedAndPropPack(seedPack, propPack)
	mPack = {}

	mSeedCount = table.getn(seedPack)
	mPropCount = table.getn(propPack)
	
	--种子数据
	for seedIndex, v in pairs(seedPack) do
		mPack[seedIndex] = v 
	end
	--道具数据
	local index = 1
	for propIndex, v in pairs(propPack) do
		---
		--狗粮类道具不用显示在种子快速通道包里
		local itemRow = g_itemtableMgr:getItemRow(v.id)
		local subID = itemRow.subID
		local propRow = g_FarmTableMgr:getFarmPropRow(subID)
		local propType = propRow.type
		if propType ~= 7 then -- 7——道具是狗粮类
			mPack[mSeedCount + index] = v
			index = index + 1
		end
	end
	
end
-----------------------------------------------------------------
function UpdateSeedBag()
	CheckSeedIsEmpty(mPack)
	ClearSeedBag()

	if 0 ==  table.getn(mPack) then
		g_WinMgr:getWindow(g_farmSeedBag.info):setVisible(true)
		g_WinMgr:getWindow(g_farmSeedBag.toFarmStorage):setVisible(false)
	else
		g_WinMgr:getWindow(g_farmSeedBag.info):setVisible(false)
		g_WinMgr:getWindow(g_farmSeedBag.toFarmStorage):setVisible(true)
	end

	data = GetSeedBagData(mStartIndex, mBagSize, mPack)

	local itemRow, imageSet, image

	for i, v in pairs(data) do
		--设置icon
		itemRow = g_itemtableMgr:getItemRow(v.id)
		imagesetName = itemRow.imageset
		iconName = itemRow.icon
		StorageUI_DynamicLoadItem(g_farmSeedBag.slotName..i.."/icon"..i,
									"newImg",
									i, imagesetName,
									iconName, "False")
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i):setVisible(true)
		--设置活动标志

		--设置数量
		FSetText(g_farmSeedBag.slotName..i.."/quantity"..i, v.count.."")
	end
end
-----------------------------------------------------------------
function ClearSeedBag()
	for i = 1, mBagSize do
		g_WinMgr:getWindow(g_farmSeedBag.slotName..i):setVisible(false)
		local lockSlotName = g_farmSeedBag.slotName..i.."/source"..i
		local iconSlotName = g_farmSeedBag.slotName..i.."/icon"..i
		local slotWin = g_WinMgr:getWindow(lockSlotName)
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
		end

		slotWin = g_WinMgr:getWindow(iconSlotName)
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
		end

		g_WinMgr:getWindow(g_farmSeedBag.slotName..i.."/quantity"..i):setText("")
	end
end
-----------------------------------------------------------------
function GetSeedBagData(startIndex, bagSize, pack)
	t = {}
	endIndex = math.min(startIndex - 1 + bagSize, table.getn(pack))
	j = 1
	for i = startIndex, endIndex do
		t[j] = pack[i]
		j = j + 1
	end
	return t
end
-----------------------------------------------------------------
function OnPreSeed()
	if 1 >= mStartIndex then
		return
	end

	mStartIndex = mStartIndex - 1
	UpdateSeedBag()
end
-----------------------------------------------------------------
function OnNextSeed()
	if table.getn(mPack) <= mStartIndex + mBagSize - 1 then
		return
	end

	mStartIndex = mStartIndex + 1
	UpdateSeedBag()
end
-----------------------------------------------------------------
function ToFarmStorage()
	FPopStoragePanel()
	--设置种子页
	CEGUI.toTabControl(g_WinMgr:getWindow(g_farmStorage.tabName)):setSelectedTabAtIndex(2)
end
-----------------------------------------------------------------
function ToFarmShop()
	if false == g_FShopFlag then
		PopFarmShopUI()
	else
		--设置种子页
		CEGUI.toTabControl(g_WinMgr:getWindow(g_farmShop.tabName)):setSelectedTabAtIndex(0)
	end
end
-----------------------------------------------------------------
function OnClickSeed(args)
	g_FarmMgr:setPushFlag(1)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index = GetClickWindowIndex(winName, g_farmSeedBag.slotName, 1, mBagSize)

	if nil ~= index then
		index = mStartIndex - 1 + index
		if index > table.getn(mPack) then
			mClickSeedId = 0
			return
		else
			mClickSeedId = mPack[index].instanceId
			local itemRow = g_itemtableMgr:getItemRow(mPack[index].id)
			
			if 4 == itemRow.type then
				EQGlobaFunc.setFarmSeedingCurSta(8, itemRow.imageset, itemRow.icon)
			elseif 6 == itemRow.type then
				EQGlobaFunc.setFarmSeedingCurSta(12, itemRow.imageset, itemRow.icon)
			end
			
			CloseSeedBag()
		end
	end
end
-----------------------------------------------------------------
function GetClickSeed()
	return mClickSeedId
end
-----------------------------------------------------------------
function CheckSeedIsEmpty(pack)
	if 0 == mClickSeedId then
		return
	else
		for i, v in pairs(pack) do
			if mClickSeedId == v.instanceId then
				return
			end
		end

		--上次播种的种子已播完，设置鼠标状态
		EQGlobaFunc.setFarmSeedingCurSta(0, "", "")
	end
end
-----------------------------------------------------------------
--显示种子itemhint
function FarmSeed_MouseEnter(args)
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local index = GetClickWindowIndex(winname, g_farmSeedBag.slotName, 1, mBagSize)
	if nil ~= index then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		index = mStartIndex - 1 + index
		data = mPack[index]
		createItemhint_farm(data.id, rect, g_FarmUI, 1, data.effTime)
	end
end
-----------------------------------------------------------------
--隐藏itemhint
function FarmSeed_MouseLeave(args)
	destroyItemhint(g_FarmUI)
end
