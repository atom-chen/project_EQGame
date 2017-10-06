--农场装饰
g_GameEventMgr:subscribeEvent("EXIST_PURCHASE_ITEM","Dec_popTip")

g_FarmDecoFlag = false

--
local preClickBtn = nil
local lastClick_win = nil
--装饰表
g_FarmDecorationTab = {
						[1] = {}, --背景
						[2] = {}, --房屋
						[3] = {}, --狗窝
						[4] = {}, --狗
						[5] = {}, --路标
						[6] = {}, --篱笆
						[7] = {}, --稻草人
						[8] = {}, --告示牌
						[9] = {}, --告示牌内容
					  }

--存储当前农场装饰数据
local curDecTable = {
					 [1] = {}, --背景
					 [2] = {}, --房屋
					 [3] = {}, --狗窝
					 [4] = {}, --狗
					 [5] = {}, --路标
					 [6] = {}, --篱笆
					 [7] = {}, --稻草人
					 [8] = {}, --告示牌
					 [9] = {}, --告示牌内容
					}
--
local showDecDataBuffer = {}
local MAXCOUNT = 10
local curType = 1
local curPage = 1
local totalPage = 1

--
function Dec_test()
	--g_Loger:logDebug("EnterDecTest...")
	local s = table.getn(showDecDataBuffer)
	--g_Loger:logDebug("s..." ..s)
	for i = 1,s do
		--g_Loger:logDebug("insId..." ..showDecDataBuffer[i].insId)
		--g_Loger:logDebug("tpltId..." ..showDecDataBuffer[i].tpltId)
		--g_Loger:logDebug("count..." ..showDecDataBuffer[i].count)
		--g_Loger:logDebug("imageset..." ..showDecDataBuffer[i].imageset)
		--g_Loger:logDebug("icon..." ..showDecDataBuffer[i].icon)
	end
end
--
function Dec_openDecUI()
	if g_FarmDecoFlag == false then
		if g_FarmDecUI == nil then
			g_FarmDecUI	= g_WinMgr:loadWindowLayout("farmdecoration.layout")
		end
		--
		Dec_resetDataTab()	--请求前清空装饰表数据
		g_req_open_ui.type = 3
		g_msgmgr:send(g_req_open_ui)
		--
		Dec_closeOtherUI()
		g_FarmUI:addChildWindow(g_FarmDecUI)
		Dec_initBtnSta(curType)
		initEvent()
		MainUI_ShieldClick(g_FarmDecUI)
		Common_injectEmptyEvt("decoration/zhuchuangkou")
		g_FarmDecoFlag = true
		SetWindowCenter(g_FarmDecUI)
	else
		Dec_closeDecUI()
	end
end

--
function Dec_closeDecUI()
	g_FarmUI:removeChildWindow(g_FarmDecUI)
	g_FarmDecoFlag = false
end

--
function Dec_showData(type)
	local baseStr = "decoration/zhuchuangkou/biaoqian/Tab 1/icon"
	totalPage = getTotalPage(g_FarmDecorationTab[curType],MAXCOUNT)
	Dec_hideSlot()
	Dec_setNumTxt()
	for i, v in pairs(showDecDataBuffer) do
		local baseStr1 = baseStr ..i
		g_WinMgr:getWindow(baseStr1):setVisible(true)

		g_WinMgr:getWindow(baseStr1):removeEvent("MouseClick")
		g_WinMgr:getWindow(baseStr1):removeEvent("MouseEnter")
		g_WinMgr:getWindow(baseStr1):removeEvent("MouseLeave")
		g_WinMgr:getWindow(baseStr1):subscribeEvent("MouseClick", "Dec_changeDec")
		g_WinMgr:getWindow(baseStr1):subscribeEvent("MouseEnter", "FarmDecoration_MouseEnter")
		g_WinMgr:getWindow(baseStr1):subscribeEvent("MouseLeave", "FarmDecoration_MouseLeave")			
		g_WinMgr:getWindow(baseStr1):setID(i)
		local itemName = "decIcon"
		if g_FarmDecorationTab[type][i] ~= nil then
			StorageUI_DynamicLoadItem(baseStr1.."/1", itemName, i, v.imageset,v.icon,"False")
		end
		--
		if v.tpltId == curDecTable[type].num_tplt_id then
			g_WinMgr:getWindow(baseStr1.."/1"):getChildAtIdx(0):getChildAtIdx(0):
				setProperty("ImageColours","tl:FF7F7F7F tr:FF7F7F7F bl:FF7F7F7F br:FF7F7F7F")
			lastClick_win = g_WinMgr:getWindow(baseStr1):getName()
		else
			g_WinMgr:getWindow(baseStr1.."/1"):getChildAtIdx(0):getChildAtIdx(0):
				setProperty("ImageColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
		end
	end
end

--
function Dec_getDecData(instanceId,templateId, count)
	local itemRow 		= g_itemtableMgr:getItemRow(templateId)
	local ornamentRow 	= g_FarmTableMgr:getOrnamentRow(templateId)
	local imageset 		= itemRow.imageset
	local icon 			= itemRow.icon
	local type 			= ornamentRow.type
	local data = {}
	
	data["insId"] 	= instanceId
	data["tpltId"] 	= templateId
	data["count"] 	= count
	data["imageset"] 	= imageset
	data["icon"] 		= icon
	Dec_saveData(type,data)
	--g_Loger:logDebug("g_FarmDecorationTab..." .."size..." ..table.getn(g_FarmDecorationTab[type]) .."...Type..." ..type)
	--Dec_test()
	updateDecData(curType,1)
	Dec_showData(curType)
end
--
function Dec_saveData(type,data)
	table.insert(g_FarmDecorationTab[type],data)
end
--
function updateDecData(type,page)
	showDecDataBuffer ={}
	showDecDataBuffer = getData(g_FarmDecorationTab[type], page, MAXCOUNT)
end
----------------------------------------
--
function Dec_clearShowBufferData()
	Dec_resetDataTab()
	showDecDataBuffer ={}
end
--
function Dec_nextPage()
	--g_Loger:logDebug("totalPage..." ..totalPage)
	--g_Loger:logDebug("curType..." ..curType)

	if curPage >= totalPage then
		curPage = totalPage
	else
		curPage = curPage + 1
	end
	--g_Loger:logDebug("curType..." ..curType)
	--g_Loger:logDebug("curPage..." ..curPage)

	updateDecData(curType,curPage)
	Dec_showData(curType)
	Dec_setNumTxt()
end
--
function Dec_prePage()
	if curPage <= 1 then
		curPage = 1
	else
		curPage = curPage - 1
	end
	updateDecData(curType,curPage)
	Dec_showData(curType)
	Dec_setNumTxt()
end
--
function onClickTypeBtn(args)
	if preClickBtn ~= nil then
		resetBtnState()
	end
	--g_Loger:logDebug("g_FarmDecorationTab..." .."size..." ..table.getn(g_FarmDecorationTab[1]) .."...Type..." ..1)

	local clickBtn =  CEGUI.toWindowEventArgs(args).window
	local btnName = clickBtn:getName()
	local btnID = clickBtn:getID()
	local pushbtn = CEGUI.toButtonBase(clickBtn)
	local pushflag = pushbtn:isPushed()
	if pushflag then
		clickBtn:setProperty("NormalImage", "set:window2 image:farm_Background1")
		clickBtn:setProperty("HoverImage", "set:window2 image:farm_Background1")
		clickBtn:setProperty("PushedImage", "set:window2 image:farm_Background1")
		clickBtn:setProperty("NormalTextColour","FF000000")
		clickBtn:setProperty("HoverTextColour","FF000000")
	end
	--g_Loger:logDebug("btnName..." ..btnName)
	--g_Loger:logDebug("btnID..." ..btnID)
	curType = btnID
	updateDecData(curType,1)
	Dec_showData(curType)
	Dec_setNumTxt()
	preClickBtn = clickBtn:getName()
	
end
--重置类型按钮的图片状态
function resetBtnState()
	g_WinMgr:getWindow(preClickBtn):setProperty("NormalImage", "set:window2 image:")
	g_WinMgr:getWindow(preClickBtn):setProperty("HoverImage", "set:window2 image:farm_Background1")
	g_WinMgr:getWindow(preClickBtn):setProperty("PushedImage", "set:window2 image:farm_Background1")
	g_WinMgr:getWindow(preClickBtn):setProperty("NormalTextColour","FFFFFFFF")
    g_WinMgr:getWindow(preClickBtn):setProperty("HoverTextColour","FFFCFF00")
end 

--
function Dec_initBtnSta(type)
	if type == 6 then
		type = 5
	elseif type == 7 then
		type = 8
	elseif type == 8 then
		type = 6
	elseif type == 9 then
		type = 7
	end
	for i = 1,8 do
		if i == type then
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("NormalImage", "set:window2 image:farm_Background1")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("HoverImage", "set:window2 image:farm_Background1")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("PushedImage", "set:window2 image:farm_Background1")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("NormalTextColour","FF000000")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("HoverTextColour","FF000000")
		else
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("NormalImage", "set:window2 image:")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("HoverImage", "set:window2 image:farm_Background1")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("PushedImage", "set:window2 image:farm_Background1")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("NormalTextColour","FFFFFFFF")
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setProperty("HoverTextColour","FFFCFF00")
		end
	end
	if type == 5 then
		type = 6
	elseif type == 8 then
		type = 7
	elseif type == 6 then
		type = 8
	elseif type == 7 then
		type = 9
	end
	if type == 6 then
		preClickBtn = 
			"decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..5
	elseif type == 7 then
		preClickBtn = 
			"decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..8
	elseif type == 8 then
		preClickBtn = 
			"decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..6
	elseif type == 9 then
		preClickBtn = 
			"decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..7
	else
		preClickBtn = 
			"decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..type
	end
end

--
function initEvent()
	for i = 1,8 do
		if i < 5 then
			g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
				setID(i)
		end
		g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
			removeEvent("Clicked")
		g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..i):
			subscribeEvent("Clicked","onClickTypeBtn")
	end
	g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..5):setID(6)
	g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..6):setID(8)
	g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..7):setID(9)
	g_WinMgr:getWindow("decoration/zhuchuangkou/biaoqian/Tab 1/beijing2/anjian" ..8):setID(7)

	--
	g_WinMgr:getWindow("decoration/zhuchuangkou/fanye1"):removeEvent("Clicked")
	g_WinMgr:getWindow("decoration/zhuchuangkou/fanye2"):removeEvent("Clicked")
	g_WinMgr:getWindow("decoration/zhuchuangkou/fanye1"):
		subscribeEvent("Clicked","Dec_prePage")
	g_WinMgr:getWindow("decoration/zhuchuangkou/fanye2"):
		subscribeEvent("Clicked","Dec_nextPage")
	g_WinMgr:getWindow("decoration/zhuchuangkou__auto_closebutton__"):
		subscribeEvent("Clicked","Dec_closeDecUI")
	
	for i = 1, MAXCOUNT do
		local baseStr = "decoration/zhuchuangkou/biaoqian/Tab 1/icon"
		for j = 1, 4 do
			g_WinMgr:getWindow(baseStr..i.."/"..j):
				setProperty("MousePassThroughEnabled", "true")
		end
	end
end

--
function Dec_hideSlot()
	local slotBaseName = "decoration/zhuchuangkou/biaoqian/Tab 1/icon"
	for i = 1,10 do
		g_WinMgr:getWindow(slotBaseName ..i):setVisible(false)
	end
end
--重置装饰保存表
function Dec_resetDataTab()
		g_FarmDecorationTab = {
								[1] = {}, 
								[2] = {}, 
								[3] = {}, 
								[4] = {}, 
								[5] = {}, 
								[6] = {}, 
								[7] = {}, 
								[8] = {}, 
								[9] = {}, 
							  }
end
--
function Dec_popTip()
	print("item already exist!")
	g_FarmMarketUI:addChildWindow(g_ExitWindow)
	g_ExitWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,150), CEGUI.UDim(0,80)))
	g_ExitWindow:setPosition(CEGUI.UVector2(CEGUI.UDim(0,100), CEGUI.UDim(0,80)))
	EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit","已有该类装饰，无法购买")
	g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):
		setSize(CEGUI.UVector2(CEGUI.UDim(0,100), CEGUI.UDim(0,30)))
	g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):
		setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-50), CEGUI.UDim(0.5,-30)))
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):
		setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-30), CEGUI.UDim(0.5,10)))
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):
		setSize(CEGUI.UVector2(CEGUI.UDim(0,48), CEGUI.UDim(0,17)))
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):removeEvent("Clicked")
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):
		subscribeEvent("Clicked", "Dec_cancelPopTop")
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):setVisible(false)
end
-------------------------------------------------------------------
function Dec_cancelPopTop()
	g_FarmMarketUI:removeChildWindow(g_ExitWindow)
end
-------------------------------------------------------------------
function Dec_changeDec(args)
	local selectSlot = CEGUI.toWindowEventArgs(args).window
	if lastClick_win ~= nil and lastClick_win ~= selectSlot:getName() then
		Dec_resetSlotSta(lastClick_win)
	end
	local Idx = selectSlot:getID()
	local insID = showDecDataBuffer[Idx].insId
	local tpltID = showDecDataBuffer[Idx].tpltId
	
	local childWin1 = selectSlot:getChildAtIdx(0)
	local childchildWin = childWin1:getChildAtIdx(0):getChildAtIdx(0)
	if insID ~= curDecTable[curType].str_ins_id then
		g_FarmMgr:reqChangeDec(insID,tpltID)
		childchildWin:
			setProperty("ImageColours","tl:FF7F7F7F tr:FF7F7F7F bl:FF7F7F7F br:FF7F7F7F")
	else
		g_FarmMgr:reqRemoveDec(tpltID)
		childchildWin:
			setProperty("ImageColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
	end
	lastClick_win = selectSlot:getName()
	
	--
	Dec_resetDataTab()	--请求前清空装饰表数据
	g_req_open_ui.type = 3
	g_msgmgr:send(g_req_open_ui)
	--

end
--------------------------------------------------------------------------
--
function Dec_resetSlotSta(winName)
	g_WinMgr:getWindow(winName):getChildAtIdx(0):getChildAtIdx(0):getChildAtIdx(0):
		setProperty("ImageColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
end
--
function Dec_setNumTxt()
	g_WinMgr:getWindow("decoration/zhuchuangkou/fanye3"):setText(curPage .."/" ..totalPage)
end

--
function Dec_closeOtherUI()
	if true== g_FStorageFlag then
		FRemoveStoragePanel()
	end
	if true == g_FShopFlag then
		CloseFarmShopUI()
	end
	if true == g_FSettingFlag then
		FSettingClose()
	end
	if true == g_FPlayerInfoFlag then
		FClosePlayerInfoUI()
	end
	if true == mOpenFlag then
		CloseUpgradeLandUI()
	end
end
-----------------------------------------------------------------
--显示装饰itemhint
function FarmDecoration_MouseEnter(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local index = win:getID()
	if 0 == table.getn(showDecDataBuffer) then
		return
	end
	local id = showDecDataBuffer[index].tpltId
	if nil ~= id then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		local effectTime = ""
		local itemrow = g_itemtableMgr:getItemRow(id)
		if 1 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
		elseif 2 == itemrow.effectTimeID then
			effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
		end
		createItemhint_farm(id, rect, g_FarmUI, 3, effectTime)
	end
end
-----------------------------------------------------------------
--隐藏itemhint
function FarmDecoration_MouseLeave(args)
	destroyItemhint(g_FarmUI)
end
-------------------------------------------------------------------
--
function Dec_saveCurDec(idx,ins_id,tplt_id)
	curDecTable[idx] = {str_ins_id = ins_id, num_tplt_id = tplt_id}
end