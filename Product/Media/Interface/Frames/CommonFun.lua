-------------------------------------------
--所有脚本通用函数都写在这里

--每个函数名前都加个前缀：Common_XXX
-------------------------------------------
-----------------------------------------
--  屏蔽界面鼠标穿透函数
-----------------------------------------
function MainUI_ShieldClick(ShieldWin)
	ShieldWin:setWantsMultiClickEvents(false)
	ShieldWin:subscribeEvent("MouseButtonDown","MainUI_OnShieldClickDown")
	ShieldWin:subscribeEvent("MouseButtonUp","MainUI_OnShieldClickUp")
	ShieldWin:subscribeEvent("MouseWheel","MainUI_OnShieldClickWheel")
end

function MainUI_OnShieldClickDown(args)
	local keyvalue = CEGUI.toMouseEventArgs(args).sysKeys
	--print(keyvalue)
	if keyvalue == 0x0001 or keyvalue == 0x0002 then
		--print("Left")
	end
	-- if keyvalue == 0x0002 then
		--print("Right")
	-- end
end
function MainUI_OnShieldClickUp(args)
end
function MainUI_OnShieldClickWheel(args)
end
--空鼠标移动函数
function Common_EmptyMoveEvt()
	--g_Loger:logDebug("EmptyMoveEvt**************")
	if g_WinMgr:isWindowPresent("Root/farmpacebar") then
		g_FarmUI:removeChildWindow("Root/farmpacebar")
	end
end

--给控件注册空函数
function Common_injectEmptyEvt(windowName)
	local childNum = g_WinMgr:getWindow(windowName):getChildCount()
	g_WinMgr:getWindow(windowName):subscribeEvent("MouseMove", "Common_EmptyMoveEvt")
	for i=0,(childNum-1) do
		g_WinMgr:getWindow(windowName):getChildAtIdx(i):subscribeEvent("MouseMove", "Common_EmptyMoveEvt")
	end
end
--
function Common_injectCommonEvt(windowName)
	local childNum = g_WinMgr:getWindow(windowName):getChildCount()
	g_WinMgr:getWindow(windowName):subscribeEvent("MouseClick", "Common_eventFunc")
	for i=0,(childNum-1) do
		g_WinMgr:getWindow(windowName):getChildAtIdx(i):subscribeEvent("MouseClick", "Common_eventFunc")
	end
end
--
function Common_eventFunc()
	--local win = g_WinMgr:getWindow("furniMenu")
	--local win1 = g_WinMgr:getWindow("circleMenu")
	if g_WinMgr:isWindowPresent("furniMenu") then
		g_MainUIWindows:removeChildWindow("furniMenu")
	end
	if g_WinMgr:isWindowPresent("circleMenu") then
		g_MainUIWindows:removeChildWindow("circleMenu")
	end
	if g_WinMgr:isWindowPresent("Wnd_Confirm") then
		g_MainUIWindows:removeChildWindow("Wnd_Confirm")
	end
	if g_offlineUIOpened == true then
		FLofflineMsgMgr_closeOfflineMsgUI()
	end
	DoCloseFriendListUI()
end
----分割字符串
--function Split (s, delim)  
--	--assert (type (delim) == "string" and string.len (delim) > 0,   "bad delimiter")  
--	local start = 1  
--	local t = {}  -- results table
--  -- find each instance of a string followed by the delimiter  
--	while true do   
--		local pos = string.find (s, delim, start, true) -- plain find    
--		if not pos then     
--			break   
--		end  
--		table.insert (t, string.sub (s, start, pos - 1))
--		start = pos + string.len (delim)  
--	end -- while  
--	-- insert final one (after last delimiter)  
--	table.insert (t, string.sub (s, start)) 
--	return t
--end
--分割字符串
function Split(szFullString, szSeparator)
	local nFindStartIndex = 1
	local nSplitIndex = 1
	local nSplitArray = {}
	while true do
	   local nFindLastIndex = string.find(szFullString, szSeparator, nFindStartIndex)
	   if not nFindLastIndex then
			nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, string.len(szFullString))
			break
	   end
	   nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, nFindLastIndex - 1)
	   nFindStartIndex = nFindLastIndex + string.len(szSeparator)
	   nSplitIndex = nSplitIndex + 1
	end
	return nSplitArray
end
--返回文件名和后缀
--file:   c:\d1\d2\file.ext
--return: file, ext
function GetFileName(file)
	local fname = nil
	local fext = nil
	local len = string.len(file)
	local s = 0
	local e = len + 1
	local i
	
	for i = len, 1, -1 do
		if string.byte(file, i) == string.byte(".") then
			e = i
			break
		end
	end
	
	for i = len, 1, -1 do
		if string.byte(file, i) == string.byte("/") or string.byte(file, i) == string.byte("\\") then
			s = i
			break
		end
	end
	
	if e ~= len + 1 then
		fext = string.sub(file, e + 1)
	end
	
	fname = string.sub(file, s + 1, e - 1)
	
	return fname, fext
end
-----------------------------------------
--返回文件坐在路径(2011/5/11 add by hezhr)
--file:   c:\d1\d2\file.ext
--return: c:\d1\d2\
function GetFilePath(file)
	local fname = GetFileName(file)
	local s = string.find(file, fname)
	local path = string.sub(file, 1, s-1)
	return path
end
--将窗口设置到屏幕中间
function SetWindowCenter(window)
	local h, w
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	h  = window:getHeight():asAbsolute(screenH)
	w  = window:getWidth():asAbsolute(screenW)
	window:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-w/2),	CEGUI.UDim(0.5, -h/2)))
end

--检测类型 xiaoMing
function CheckType(checkObject, typeName)
	return type(checkObject) == typeName
end
--
-----------------------------------------------------------------
function FSetText(winName, text)
	EQGlobaFunc.SetText(winName, text)
end
-----------------------------------------------------------------
--此函数可以通用
function FUpdatePageInfo(winName, curPage, totalPage)
	local str
	if nil == totalPage then
		str = tostring(curPage)
	else
		if 0 == totalPage then
			totalPage = 1
		end
		str = curPage.."/"..totalPage
	end
	FSetText(winName, str)
end
-----------------------------------------------------------------
--获得item在表中的Index
function FShopGetTableIndex(curIndex, curpage, perPageSize)
	local index = perPageSize * (curpage - 1) + curIndex
	return index
end
-----------------------------------------------------------------
--获取当前Tab索引
function GetCurTabIndex(TabName)
	local window = g_WinMgr:getWindow(TabName)
	local index = CEGUI.toTabControl(window):getSelectedTabIndex()
	return index
end
-----------------------------------------------------------------
--通过比较窗口的名称，获得点击的格子的索引数，前提格子的名称前缀
--是相同的，并以数字递增
function GetClickWindowIndex(windowName, BaseSlotName, StartIndex, EndIndex)
	for i = StartIndex, EndIndex do
		if windowName == BaseSlotName..i then
			return i
		end
	end
end
--动态加载仓库物品------------------------------------------------------------------------------------
function StorageUI_DynamicLoadItem(slotName, itemWinName, index, imagesetName, imgName, DragEnabled,Overlay,itemEnabled)
	local slotWin = g_WinMgr:getWindow(slotName)
	if nil == slotWin then
		--g_Loger:logError("no this slotwindow : "..slotName)
		return
	end
	--g_count=g_count + 1
	local containerWin = g_WinMgr:getWindow(slotName .."/DragContainer" ..index)
	local itemWin
	if containerWin == nil then
		--g_Loger:logDebug("containerWin == nil...")
		containerWin = g_WinMgr:createWindow("DragContainer", slotName .."/DragContainer" ..index)
		if slotWin:getType() == "NewEQHomeLook/NewStaticImage3" then
			containerWin:setProperty("UnifiedAreaRect", "{{0,4},{0,4},{1,-4},{1,-4}")
		else
			containerWin:setProperty("UnifiedAreaRect", "{{0,2},{0,2},{1,-2},{1,-2}")
		end
		containerWin:setProperty("DraggingEnabled", DragEnabled)
		containerWin:setProperty("MousePassThroughEnabled", "true")
		slotWin:addChildWindow(containerWin)

		containeradditem(containerWin,slotName, itemWinName, index, imagesetName, imgName,Overlay,itemEnabled)
		containerWin:subscribeEvent("DragEnded","ItemExchWithStorage")
		--containerWin:subscribeEvent("DragDropItemEnters", "ConHasChild")
		containerWin:subscribeEvent("DragDropItemLeaves","RetSet_gExchgFlag")
	end
	if containerWin:getChildCount() == 0 then
		--g_Loger:logDebug("getChildCount() == 0 ...")

		containeradditem(containerWin,slotName, itemWinName, index, imagesetName, imgName,Overlay,itemEnabled)
	end
	if nil == slotWin then
		--g_Loger:logError("no this slotwindow : "..slotName)
	else
		if slotWin:getChildCount() == 0 then
			slotWin:addChildWindow(containerWin)
		end
	end

	itemwin = containerWin:getChildAtIdx(0)
	itemwin:setProperty("Image", "set:" ..imagesetName .." image:" ..imgName)
	
	if itemEnabled ~= nil then
		if itemEnabled == "False" then
			itemwin:setProperty("ImageColours","tl:FF7F7F7F tr:FF7F7F7F bl:FF7F7F7F br:FF7F7F7F")
		else
			itemwin:setProperty("ImageColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
		end
	else
		--g_Loger:logError("$**********itemEnabled nil*********&")
	end

	--set dragItem Alpha
	--containerWin:setAlpha(1)
	if g_DragItem:isDraging() and g_DragItem:getPacktype() == g_PT_WAREHOUSE then
		--local dragidx = g_DragItem:getIndex()
		local conidx = getStorageIdxByName(containerWin:getName())
		if nil ~= conidx then
			local curPage 	= g_mainplayer:getPackCurPage(g_PT_WAREHOUSE)
			conidx = conidx + (curPage - 1) * C_STORAGE_GIRD_NUM

		--	if conidx == dragidx then
		--		containerWin:setAlpha(0.4)
		--		g_Loger:logDebug(containerWin:getName().."setAlpha(0.4): "..containerWin:getAlpha())
		--	end
		--end
			if StorageGridOnDrag(conidx) == true then
				containerWin:setAlpha(0.4)
			end
		end
	end

	--set itmeOverlay
	if containerWin:getChildCount() > 1 then
		local textwnd = containerWin:getChildAtIdx(1)
		if Overlay == -1 then
			textwnd:setText("")
		else
			textwnd:setText(""..Overlay)
		end
	end
end
---------容器里加物品显示窗口------------------------------------------------------------------------
function containeradditem(containerWin,slotName, itemWinName, index, imagesetName, imgName,Overlay,itemEnabled)
		local winname = slotName .."/DragContainer" ..index .."/" ..itemWinName
		local itemWin = g_WinMgr:createWindow("EQHomeLook/StaticImage",
												winname)
		itemWin:setProperty("Image", "set:" ..imagesetName .." image:" ..imgName)
		itemWin:setProperty("MousePassThroughEnabled", "true")
		itemWin:setProperty("WantsMultiClickEvents", "false")
		itemWin:setProperty("FrameEnabled", "false")
		itemWin:setProperty("BackgroundEnabled", "false")
		if itemEnabled ~= nil then
			if itemEnabled == "False" then
				--g_Loger:logError("$**********item Unenabled*********&")
				itemWin:setProperty("ImageColours","tl:FF7F7F7F tr:FF7F7F7F bl:FF7F7F7F br:FF7F7F7F")
			else
				--g_Loger:logError("$**********item Enabled*********&")
				itemWin:setProperty("ImageColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
			end
		else
			--g_Loger:logError("$**********itemEnabled nil*********&")
		end
		containerWin:addChildWindow(itemWin)
		if Overlay~= nil then--and Overlay > 0 then
			addItemNumTxt(containerWin,winname,Overlay)
		end
end
--创建容器内物品数量显示文本框
function addItemNumTxt(containerWin,winname,Overlay)
	local itemnumtxtname = winname.."overlaytxt"
	local itemNumTxt = g_WinMgr:createWindow("EQHomeLook/StaticText", itemnumtxtname)
	itemNumTxt:setProperty("MousePassThroughEnabled", "true")
	itemNumTxt:setProperty("AlwaysOnTop", "true")
	itemNumTxt:setProperty("BackgroundEnabled", "false")
	itemNumTxt:setProperty("FrameEnabled", "false")
	itemNumTxt:setProperty("HorzFormatting", "LeftAligned")
	itemNumTxt:setProperty("VertFormatting", "TopAligned")
	itemNumTxt:setProperty("Font", "simsun-10")
	itemNumTxt:setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000")
	itemNumTxt:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{0.5,0},{0.5,0}")

	if Overlay == -1 then
		itemNumTxt:setText("")
	else
		itemNumTxt:setText(""..Overlay)
	end

	containerWin:addChildWindow(itemNumTxt)
end
-----------------------------------------
-- 管理模态对话框模块(2011/6/17 add by hezhr)
-----------------------------------------
local m_modalWin = {}
--窗体是否存在
local function _common_findModalWin(winname)
	for i, v in pairs(m_modalWin) do
		if v == winname then
			return i
		end
	end
	return 0
end
--保存窗体
local function _common_saveModalWin(winname)
	if 0 == _common_findModalWin(winname) then	-- 窗体不存在
		table.insert(m_modalWin, winname)
	end
end
--移除窗体
local function _common_deleModalWin(winname)
	local i = _common_findModalWin(winname)
	if 0 ~= i then	-- 窗体存在
		table.remove(m_modalWin, i)
	end
end
--设置模态
local function _common_setModalWin(winname, flag)
	if nil == g_WinMgr:getWindow(winname) then
		return
	end
	g_WinMgr:getWindow(winname):setModalState(flag)
	if flag then
		g_WinMgr:getWindow(winname):setAlwaysOnTop(true)
		g_WinMgr:getWindow(winname):moveToFront()
	end
end
--开始模态
local function _common_startModalMode(winname)
	-- 保存之前的模态对话框
	local oldname = EQGlobaFunc.getModalWindowName()
	if oldname == winname then
		return
	end
	if nil~=g_WinMgr:getWindow(oldname) then
		_common_saveModalWin(oldname)
		_common_setModalWin(oldname, false)
	end
	_common_saveModalWin(winname)
	_common_setModalWin(winname, true)
end
--结束模态
local function _common_endModalMode(winname)
	_common_setModalWin(winname, false)
	_common_deleModalWin(winname)
	local num = table.getn(m_modalWin)
	if 0 ~= num then
		_common_setModalWin(m_modalWin[num], true)
	end
end
-- 用此函数来替代setModalState(),供外部调用
function Common_setModalState(winname, flag)
	if nil == g_WinMgr:getWindow(winname) then
		--g_Loger:logError("Function Common_setModalState() param error...")
		return
	end
	if flag then
		_common_startModalMode(winname)
	else
		_common_endModalMode(winname)
	end
end
-----------------------------------------
-- 想添加到cpp的update函数的乐园函数可添加在此处(2011/7/22 add by hezhr)
-----------------------------------------
function Common_updateMainLua()
	PlayerGuide_listen()	-- 乐园新手任务里事件监听
	MicroBlog_twinkle()	-- 微博里照相时的闪烁效果
	WallowTime_twinkle()	-- 乐园防沉迷里头像的闪烁效果
	FitModeShopCenter_listenWallFloorMode()	-- 家装模式下更换墙壁地板的状态监听
end
-----------------------------------------
-- 想添加到cpp的update函数的农场函数可添加在此处(2011/7/22 add by hezhr)
-----------------------------------------
function Common_updateFarmLua()
	WallowTime_twinkle()	-- 农场防沉迷里头像的闪烁效果
end
-----------------------------------------
