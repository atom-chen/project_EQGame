-----------------------------------------------------------------
--农场个人信息
--xiaoMing
--重构
--2010-07-03
-----------------------------------------------------------------
--服务器端将 消息 成果 消费 作为LOG type = 1, 2, 3 而留言单独成为
--一类type = 4
-----------------------------------------------------------------
--全局变量 F 代表农场 PI代表PlayerInfo
g_FPlayerInfoFlag = false
local mData = {{}, {}, {}, {}}
local mCurPage = {1, 1, 1, 1}
--每种类型每页显示的数量
local mPageLogCount = {5, 10, 5, 3}
local mReqFlag = {false, false, false, false}

local mAccount = ""

local mTypeToName = {[4] = "种子", [6] = "道具", [7] = "装饰"}

--农场成果表(hzhr)
local farmGainPack = {}

--图文混排窗口
local mConsWnd
local mMsgWnd
local mLeaveWordWnd
local mPlayerType -- 1 代表主玩家 2代表其他玩家

local mMsgType = {
				["help"] = 1,
				["demage"] = 2,
				["extract"] = 3,
				["bite"] = 4
					}
					
local mLogType = {
					["msg"] = 1,  --消息
					["gain"] = 2, --成果
					["cons"] = 3, --消费
					["comment"] = 4, -- 留言
					}
-- 打开的是否为主玩家信息面板(add by hezhr)
g_farmMainPlayerInfoUI = true
-----------------------------------------------------------------
--打开面板
local function _PopPlayerInfoUI()
	FPInfo_closeOtherUI()
	g_FarmUI:addChildWindow(g_FPlayerInfoUI)
	Common_injectEmptyEvt("FarmPlayerInfo")
	g_FPlayerInfoFlag = true
	SetWindowCenter(g_FPlayerInfoUI)
end
-----------------------------------------------------------------
local function _GetType()
	local index = GetCurTabIndex(g_farmPlayerInfo.tabName)

	if 0 == index then
		return mLogType.msg
	elseif 1 == index then
		return mLogType.comment
	elseif 2 == index then
		return mLogType.gain
	else
		return mLogType.cons
	end
end
-----------------------------------------------------------------
--加载图片
local function _SetSlotImage(slotName, itemWinName, index, imagesetName, imgName)
	StorageUI_DynamicLoadItem(slotName, itemWinName, index, imagesetName, imgName, "False")
end
-----------------------------------------------------------------
local function _ClearMsg()
	EQGlobaFunc.toMultiLineBox(mMsgWnd:getName()):clearUp()
end
-----------------------------------------------------------------
local function _UpdateMsg(curPage, tableData, MaxCount)
	_ClearMsg()
	--获得该页显示的数据
	local data = getData(tableData, curPage, MaxCount)

	for i, v in pairs(data) do
		local time = tostring(v.time)
		local name = tostring(v.userName)
		EQGlobaFunc.toMultiLineBox(mMsgWnd:getName()):addElementText("simsun-10", time, "#000000", true)
		EQGlobaFunc.toMultiLineBox(mMsgWnd:getName()):addElementText("simsun-10", name, "#0381C2", false)
		EQGlobaFunc.toMultiLineBox(mMsgWnd:getName()):addElementText("simsun-10", v.str, "#000000", false)
	end

	if 0 == GetCurTabIndex(g_farmPlayerInfo.tabName) then
		FUpdatePageInfo(g_farmPlayerInfo.pageText,curPage, getTotalPage(mData[mLogType.msg], mPageLogCount[mLogType.msg]))
	end
end
-----------------------------------------------------------------
local function _ClearGain()
	local str = g_farmPlayerInfo.harvestSlot
	local slotWin
	for i = 1, mPageLogCount[mLogType.gain] do
		g_WinMgr:getWindow(str..i):setVisible(false)
		slotWin = g_WinMgr:getWindow(str..i.."/Image1" )
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
		end
		FSetText(str..i.."/Text", "")
	end
end
-----------------------------------------------------------------
local function _UpdateGain(curPage, tableData, MaxCount)
	_ClearGain()

	local gainTable = getData(tableData, curPage, MaxCount)
	local size = table.getn(gainTable)

	local itemNum, itemrow, imagesetName, imgName, slotName

	for i = 1, size do
		itemrow = g_itemtableMgr:getItemRow(gainTable[i]["id"])
		slotWin = g_farmPlayerInfo.harvestSlot..i.."/Image1"
		g_WinMgr:getWindow(g_farmPlayerInfo.harvestSlot..i):setVisible(true)
		g_WinMgr:getWindow(slotWin):setID((curPage-1)*10 + i)
		g_WinMgr:getWindow(slotWin):removeEvent("MouseEnter")
		g_WinMgr:getWindow(slotWin):removeEvent("MouseLeave")
		g_WinMgr:getWindow(slotWin):subscribeEvent("MouseEnter", "FarmGain_MouseEnter")
		g_WinMgr:getWindow(slotWin):subscribeEvent("MouseLeave", "FarmGain_MouseLeave")
		if nil ~= itemrow then
			_SetSlotImage(slotWin, "newImg", i, itemrow.imageset, itemrow.icon)
			FSetText(g_farmPlayerInfo.harvestSlot..i.."/Text", tostring(gainTable[i]["count"]))
		end
	end

	if 2 == GetCurTabIndex(g_farmPlayerInfo.tabName) then
		FUpdatePageInfo(g_farmPlayerInfo.pageText,curPage, getTotalPage(mData[mLogType.gain], mPageLogCount[mLogType.gain]))
	end
end
-----------------------------------------------------------------
local function _UpdateCons(curPage, tableData, MaxCount)
	--清空
	EQGlobaFunc.toMultiLineBox(mConsWnd:getName()):clearUp()

	--是否有必要和 _UpdateMsg 合成为一个函数
	local data = getData(tableData, curPage, MaxCount)

	for i, v in pairs(data) do
		local time = v.time.." 在"
		EQGlobaFunc.toMultiLineBox(mConsWnd:getName()):addElementText("simsun-10", time, "#000000", true)
		EQGlobaFunc.toMultiLineBox(mConsWnd:getName()):addElementText("simsun-10", "商店", "#0099FF", false)
		EQGlobaFunc.toMultiLineBox(mConsWnd:getName()):addElementText("simsun-10", v.str, "#000000", false)
	end

	if 3 == GetCurTabIndex(g_farmPlayerInfo.tabName) then
		FUpdatePageInfo(g_farmPlayerInfo.pageText,curPage, getTotalPage(mData[mLogType.cons], mPageLogCount[mLogType.cons]))
	end
end
-----------------------------------------------------------------
local function _UpdateComment(curPage, tableData, MaxCount)
	--清空
	EQGlobaFunc.toMultiLineBox(mLeaveWordWnd:getName()):clearUp()

	local commentTable = getData(tableData, curPage, MaxCount)

	for i, v in pairs(commentTable) do
		local time = tostring(v.time)
		local name = tostring(v.name)
		
		if v.acc == g_fAccount then
			name = "主人 "
		end	
		
		EQGlobaFunc.toMultiLineBox(mLeaveWordWnd:getName()):addElementText("simsun-10", time, "#000000", true)
		EQGlobaFunc.toMultiLineBox(mLeaveWordWnd:getName()):addElementText("simsun-10", name, "#0381C2", false)
		EQGlobaFunc.toMultiLineBox(mLeaveWordWnd:getName()):addElementText("simsun-10", v.content, "#000000", false)
	end

	--for i = 1, size do
		--FSetText("FarmPlayerInfo/bg/farmMsg/leaveWord/lw"..i, commentTable[i])
		--if true == isMaster then
			--if 2 == typeTable[i] then
				--g_WinMgr:getWindow("FarmPlayerInfo/bg/farmMsg/leaveWord/bt"..i):setVisible(true)
			--else
				--g_WinMgr:getWindow("FarmPlayerInfo/bg/farmMsg/leaveWord/bt"..i):setVisible(false)
			--end
		--end
	--end

	if 1 == GetCurTabIndex(g_farmPlayerInfo.tabName) then
		FUpdatePageInfo(g_farmPlayerInfo.pageText,curPage, getTotalPage(mData[mLogType.comment], mPageLogCount[mLogType.comment]))
	end
end
-----------------------------------------------------------------
local function _Update(type, curPage, tableData, MaxCount)
	if mLogType.msg == type  then
		_UpdateMsg(curPage, tableData, MaxCount)
	elseif mLogType.gain == type then
		_UpdateGain(curPage, tableData, MaxCount)
	elseif mLogType.cons == type then
		_UpdateCons(curPage, tableData, MaxCount)
	else
		_UpdateComment(curPage, tableData, MaxCount)
	end
end
-----------------------------------------------------------------
--type == 0, 插入表尾 == 1 插入表头
local function _addComment(msg, time, insertType)
	local type = msg.type
	local userName = msg.username
	local content = EQGlobaFunc.stringFilter(msg.content,0)
	local account = msg.account
	
	if 0 == insertType then
		table.insert(mData[mLogType.comment], {time = time, acc = account, name = userName, type = type, content = content})
	else
		table.insert(mData[mLogType.comment], 1 , {time = time, acc = account, name = userName, type = type, content = content})
	end
end
-----------------------------------------------------------------
--判断是否是自己的农场
local function _IsMaster(acc)
	if acc == g_mainplayer:getPlayerAccount() then
		return true
	else
		return false
	end
end
-----------------------------------------------------------------
function FPopOtherPlayerInfo()
	g_farmMainPlayerInfoUI = false	-- (add by hezhr)
	if 2 == mPlayerType and true == g_FPlayerInfoFlag then
		FClosePlayerInfoUI()
		return
	elseif false == g_FPlayerInfoFlag then
		_PopPlayerInfoUI()
	end
	
	InitFarmPlayerInfo(g_FarmMgr.mAccount)
	ReqFarmPlayerInfo()
	mPlayerType = 2
end
-----------------------------------------------------------------
function SetFarmPlayerType(type)
	mPlayerType = type
end
-----------------------------------------------------------------
function FPopMainPlayerInfo()
	g_farmMainPlayerInfoUI = true	-- (add by hezhr)
	if 1 == mPlayerType and true == g_FPlayerInfoFlag then
		FClosePlayerInfoUI()
		return
	elseif false == g_FPlayerInfoFlag then
		_PopPlayerInfoUI()
	end
	
	InitFarmPlayerInfo(g_mainplayer:getPlayerAccount())
	ReqFarmPlayerInfo()
	mPlayerType = 1
end
-----------------------------------------------------------------
--关闭面板
function FClosePlayerInfoUI()
	g_FarmUI:removeChildWindow(g_FPlayerInfoUI)
	g_FPlayerInfoFlag = false
	mPlayerType = 0
end
-----------------------------------------------------------------
--面板初始化
local mInfoAccount = ""
local mInfo = nil
local mOtherAccount = ""
local mOtherInfo = nil
function InitFarmPlayerInfo(acc)
	if nil == acc then
	  acc = g_mainplayer:getPlayerAccount()
	end
	mCurPage = {1, 1, 1, 1}
	CEGUI.toTabControl(g_WinMgr:getWindow(g_farmPlayerInfo.tabName)):setSelectedTabAtIndex(0)
	mReqFlag = {false, false, false, false}
	--暂时用帐号
	local name, level, gold, exp   
	--local tabWin
	if true == _IsMaster(acc) then
		selfInfo = g_FarmMgr:getSelfFarmInfo()
		if mInfo ~= nil then
			if "" ~= mInfo.nick_name then
				name  = mInfo.nick_name
			elseif "" ~= mInfo.full_name then
				name = mInfo.full_name
			else
				name = mInfoAccount
			end
		else
			name = acc
		end
		level = selfInfo.mSlev
		gold  = g_FarmMgr:getFarmMoney()--g_FarmMgr.mGameGold
		exp   = selfInfo.mSexp
		g_WinMgr:getWindow(g_farmPlayerInfo.clearMsg):setVisible(true)
		g_WinMgr:getWindow(g_farmPlayerInfo.clearLeaveWord):setVisible(true)	
	else
		if nil ~= mOtherInfo then
			if "" ~= mOtherInfo.nick_name then
				name  = mOtherInfo.nick_name
			elseif "" ~= mOtherInfo.full_name then
				name = mOtherInfo.full_name
			else
				name = mOtherAccount
			end
		else
			name = g_FarmMgr.mAccount
		end
		level	 = g_FarmMgr.mLev
		gold     = ""
		exp      = g_FarmMgr.mExp
		g_WinMgr:getWindow(g_farmPlayerInfo.clearMsg):setVisible(false)
		g_WinMgr:getWindow(g_farmPlayerInfo.clearLeaveWord):setVisible(false)
	end

	mAccount = acc 
	EQGlobaFunc.SetText(g_farmPlayerInfo.name,"")
	EQGlobaFunc.SetText(g_farmPlayerInfo.name,name)
	EQGlobaFunc.SetText(g_farmPlayerInfo.level, tostring(level))
	EQGlobaFunc.SetText(g_farmPlayerInfo.gameGold,tostring(gold))
	EQGlobaFunc.SetText(g_farmPlayerInfo.exp,tostring(exp))
	Farm_setFriendPhoto("FarmPlayerInfo/bg/photo", acc)	-- 设置面板头像(add by hezhr)
end
----------------------------------------------------------------
--
function setDataForPlayerInfoUI(account, info)
	mInfoAccount = account
	mInfo = info
end
-----------------------------------------------------------------
--
function clearInfo()
	mInfoAccount = ""
	mInfo = nil
end
-----------------------------------------------------------------
--
function setDataForOtherPlayerInfoUI(account, info)
	mOtherAccount = account
	mOtherInfo = info
end
-----------------------------------------------------------------
--
function clearOtherInfo()
	mOtherAccount = ""
	mOtherInfo = nil
end
-----------------------------------------------------------------
function ReqFarmPlayerInfo()
	local type = _GetType()

	if true == mReqFlag[type] then
		_Update(type, mCurPage[type], mData[type], mPageLogCount[type])
		return
	else
		--本应该在通知的时候清空 鉴于收发消息的机制 在请求的时候清空
		FClearPlayerInfo(type)
	end

	if mLogType.comment == type then
		g_FarmMgr:reqCommentList(mAccount)
	else
		g_FarmMgr:reqLogList(type, mAccount)
	end
end
-----------------------------------------------------------------
function FReqComment()
	local comment = g_WinMgr:getWindow(g_farmPlayerInfo.leaveWord):getText()
	if "" == comment or "\n" == comment then
		return
	end
	g_FarmMgr:reqComment(mAccount, comment)
end
-----------------------------------------------------------------
function FGetCurPage(type, tableData)
	return tableData[type]
end
-----------------------------------------------------------------
function FSetCurPage(value, type, tableData)
	tableData[type] = value
end
-----------------------------------------------------------------
function lua_addLogMsg(msg, time, curIndex, num)
	--通知更新数据 代表请求成功
		if 1 == curIndex then
			mReqFlag[1] = true
		end
		
		local logType = msg.log_type
		local userName = msg.user_name
		local cropId = msg.crop_id
		local cropQuantity = msg.crop_quantity
		local str
		
		if mMsgType.extract == logType then
			local cropName = g_itemtableMgr:getItemRow(cropId).name
			str = "来农场偷走"..cropQuantity.."个"..cropName
		elseif mMsgType.help == logType then
			str = "来农场帮忙"
		elseif mMsgType.demage == logType then
			str = "来农场破坏"
		else
			--被狗咬
			local dogName = g_itemtableMgr:getItemRow(msg.pet_id).name
			str = "来农场摘取被"..dogName.."发现了，它为主人抓获"..msg.bite_coin.."个金币。"
		end

		table.insert(mData[mLogType.msg], {time = time, userName = userName, str = str})

		if curIndex == num then
		--数据更新完毕 更新页面显示
			_UpdateMsg(mCurPage[mLogType.msg], mData[mLogType.msg], mPageLogCount[mLogType.msg])
		end
end
-----------------------------------------------------------------
function lua_addLogGain(msg, curIndex, num)
	--通知更新数据 代表请求成功
	if 1 == curIndex then
		mReqFlag[2] = true
	end

	local cropId = msg.crop_id
	local cropQuantity = msg.crop_quantity
	--获取等级 用于排序
	local cropLevel = g_FarmTableMgr:getFarmItemRow(cropId).itemLevel

	table.insert(mData[mLogType.gain], {id = cropId, count = cropQuantity, level = cropLevel})
	table.insert(farmGainPack, {id = msg.crop_id, steal = msg.steal_quantity, gain = msg.gain_quantity,level = cropLevel})

	if curIndex == num then
		table.sort(mData[mLogType.gain], FarmSortFunc)
		table.sort(farmGainPack, FarmSortFunc)
		_UpdateGain(mCurPage[mLogType.gain], mData[mLogType.gain], mPageLogCount[mLogType.gain])
	end
end
-----------------------------------------------------------------
function lua_addLogCons(msg, time, curIndex, num)
	if 1 == curIndex then
		mReqFlag[3] = true
	end
	
	local str
	local cropId = msg.crop_id
	local cropQuantity = msg.crop_quantity
	local cropType = g_itemtableMgr:getItemRow(cropId).type
	local useCoin = msg.use_coin
	local leaveCoin =  msg.leave_coin
	local type = msg.type

	local name = g_itemtableMgr:getItemRow(cropId).name

	if 1 == type then
		str = "购买"..cropQuantity.."个"..name..mTypeToName[cropType]..",花费"..useCoin.."金币，还剩"..leaveCoin.."金币。"
	else
		str = "卖出"..cropQuantity.."个"..name..mTypeToName[cropType]..",获得"..useCoin.."金币，还剩"..leaveCoin.."金币。"
	end
	
	table.insert(mData[mLogType.cons], { time = time, type = type, str = str})

	if curIndex == num then
		_UpdateCons(mCurPage[mLogType.cons], mData[mLogType.cons], mPageLogCount[mLogType.cons])
	end
end
-----------------------------------------------------------------
function lua_addComment(msg, time, curIndex, num)
	if 1 == curIndex then
		mReqFlag[4] = true
	end

	_addComment(msg, time, 0)
	
	if curIndex == num then
		_UpdateComment(mCurPage[mLogType.comment], mData[mLogType.comment], mPageLogCount[mLogType.comment])
	end
end
-----------------------------------------------------------------
--留言成功
function lua_notiComment(msg, time)
	_addComment(msg, time, 1)
	_UpdateComment(mCurPage[mLogType.comment], mData[mLogType.comment], mPageLogCount[mLogType.comment])
	g_WinMgr:getWindow(g_farmPlayerInfo.leaveWord):setText("")
end
-----------------------------------------------------------------
function lua_cleanLogMsg()
	FClearPlayerInfo(mLogType.msg)
end
-----------------------------------------------------------------
function lua_cleanLogGain()
	FClearPlayerInfo(mLogType.gain)
end
-----------------------------------------------------------------
function lua_cleanLogCons()
	FClearPlayerInfo(mLogType.cons)
end
-----------------------------------------------------------------
function lua_cleanComment()
	FClearPlayerInfo(mLogType.comment)
end
-----------------------------------------------------------------
function FClearPlayerInfo(type)
	mData[type] = {}
	if 2 == type then
		farmGainPack = {}
	end
	mCurPage[type] = 1
	_Update(type, mCurPage[type], mData[type], mPageLogCount[type])
end
-----------------------------------------------------------------
function FPlayerInfoOnload()
	g_WinMgr:getWindow(g_farmPlayerInfo.tab1):subscribeEvent("Clicked", "ReqFarmPlayerInfo")
	g_WinMgr:getWindow(g_farmPlayerInfo.tab2):subscribeEvent("Clicked", "ReqFarmPlayerInfo")
	g_WinMgr:getWindow(g_farmPlayerInfo.tab3):subscribeEvent("Clicked", "ReqFarmPlayerInfo")
	g_WinMgr:getWindow(g_farmPlayerInfo.tab4):subscribeEvent("Clicked", "ReqFarmPlayerInfo")
	g_WinMgr:getWindow(g_farmPlayerInfo.prePage):subscribeEvent("MouseClick", "FPInfoOnLeftBtn")
	g_WinMgr:getWindow(g_farmPlayerInfo.nextPage):subscribeEvent("MouseClick", "FPInfoOnRightBtn")
	g_WinMgr:getWindow(g_farmPlayerInfo.closeBtn):subscribeEvent("Clicked", "FClosePlayerInfoUI")
	g_WinMgr:getWindow(g_farmPlayerInfo.leaveWordBtn):subscribeEvent("Clicked", "FReqComment")
	g_WinMgr:getWindow(g_farmPlayerInfo.clearLeaveWord):subscribeEvent("Clicked", "FReqClearComment")
	g_WinMgr:getWindow(g_farmPlayerInfo.clearLeaveWord):setProperty("NormalTextColour","FF0099FF")
	g_WinMgr:getWindow(g_farmPlayerInfo.clearMsg):subscribeEvent("Clicked", "FReqClearMsg")
	g_WinMgr:getWindow(g_farmPlayerInfo.clearMsg):setProperty("NormalTextColour","FF0099FF")
	
	mConsWnd = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mConsWnd")
	mConsWnd:setSize(CEGUI.UVector2(CEGUI.UDim(0, 380), CEGUI.UDim(0,110)))
	g_WinMgr:getWindow(g_farmPlayerInfo.consumeText):addChildWindow(mConsWnd)
	EQGlobaFunc.toMultiLineBox(mConsWnd:getName()):setScrollbarVisible("SP_OLD", true, 0)	

	mMsgWnd = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mMsgWnd")
	mMsgWnd:setSize(CEGUI.UVector2(CEGUI.UDim(0, 380), CEGUI.UDim(0,110)))
	g_WinMgr:getWindow(g_farmPlayerInfo.msgText):addChildWindow(mMsgWnd)
	EQGlobaFunc.toMultiLineBox(mMsgWnd:getName()):setScrollbarVisible("SP_OLD", true, 0)	

	mLeaveWordWnd = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "mLeaveWordWnd")
	mLeaveWordWnd:setSize(CEGUI.UVector2(CEGUI.UDim(0, 380), CEGUI.UDim(0,80)))
	g_WinMgr:getWindow(g_farmPlayerInfo.leaveWordText):addChildWindow(mLeaveWordWnd)
	EQGlobaFunc.toMultiLineBox(mLeaveWordWnd:getName()):setScrollbarVisible("SP_OLD", true, 0)
	
	local window = g_WinMgr:getWindow(g_farmPlayerInfo.leaveWord)
	CEGUI.toMultiLineEditbox(window):setMaxTextLength(50)
	
	MainUI_ShieldClick(g_FPlayerInfoUI)
	
	for i = 1, mPageLogCount[2] do
		g_WinMgr:getWindow(g_farmPlayerInfo.harvestSlot..i):setVisible(false)
	end
end
-----------------------------------------------------------------
--FPInfoOnLeftBtn
function FPInfoOnLeftBtn()
	local type = _GetType()
	local curPage = FGetCurPage(type, mCurPage)
	if 1 >= curPage then
		return
	else
		FSetCurPage(curPage - 1, type, mCurPage)
		_Update(type, curPage - 1, mData[type], mPageLogCount[type])
	end
end
-----------------------------------------------------------------
function FPInfoOnRightBtn()
	local type = _GetType()
	local curPage = FGetCurPage(type, mCurPage)
	local totalPage = getTotalPage(mData[type], mPageLogCount[type])

	if curPage >= totalPage then
		return
	else
		FSetCurPage(curPage + 1, type, mCurPage )
		_Update(type, curPage + 1, mData[type], mPageLogCount[type])
	end
end
-----------------------------------------------------------------
function FPInfo_closeOtherUI()
	if true== g_FStorageFlag then
		FRemoveStoragePanel()
	end
	if true == g_FShopFlag then
		CloseFarmShopUI()
	end
	if true == g_FSettingFlag then
		FSettingClose()
	end
	if true == g_FarmDecoFlag then
		Dec_closeDecUI()
	end
	if true == mOpenFlag then
		CloseUpgradeLandUI()
	end
end
-----------------------------------------------------------------
function FReqClearComment()
	g_FarmMgr:reqClearComment()
end
-----------------------------------------------------------------
function FReqClearMsg()
	--暂时请求时清除
	FClearPlayerInfo(1)
	g_FarmMgr:reqclearLog() 
end
-----------------------------------------------------------------
--显示成果itemhint
function FarmGain_MouseEnter(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local id = win:getID()
	local itemrow = g_itemtableMgr:getItemRow(farmGainPack[id]["id"])
	local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
	createItemhint_farmgain(itemrow.name, farmGainPack[id]["gain"], farmGainPack[id]["steal"], rect, g_FarmUI)
end
-----------------------------------------------------------------
--隐藏itemhint
function FarmGain_MouseLeave(args)
	destroyItemhint(g_FarmUI)
end