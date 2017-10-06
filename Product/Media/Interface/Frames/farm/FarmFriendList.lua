--农场好友列表界面逻辑
--
g_FarmFriendListFlag = false
local prePushWin = nil

--农场好友列表
farmFriendInfo_farmStatus_Map = {}
--好友显示列表
showFriendListTab = {}
--是否在搜索状态
local is_search = 0
--搜索好友结果列表
local searchResult_List = {}

MAXCOUNT = 10
local curPage = 1
local totalPage = 1
PageAmount = nil
g_mSelectName = nil
local mSelectAcc = nil
--
g_WinMgr:getWindow("farm_friendlist/open_button"):subscribeEvent("Clicked", "FarmFL_onclickSetPos")
g_WinMgr:getWindow("farm_friendlist/better"):subscribeEvent("Clicked", "FarmFL_reflash")
g_WinMgr:getWindow("farm_friendlist/first_page"):subscribeEvent("Clicked", "FarmFL_firstPage")
g_GameEventMgr:subscribeEvent("CHANGE_RESOLUTION","FarmFL_resolutionChanged")
local ffriend_search_win = g_WinMgr:getWindow("farm_friendlist/search")
ffriend_search_win:subscribeEvent("TextChanged", "FfriendList_sTxt_change")

--
local insertFlag = 0
-----------------------------------------
--农场好友头像表(add by hezhr)
g_farmFriendPhoto = {}
--农场好友列表当前选中的账号(add by hezhr)
g_farmSelectAccount = ""
--
function lua_clearFriendFarmStateTable()
	farmFriendInfo_farmStatus_Map = {
				--{
				-- friendAcc  = selfInfo.mSaccount,
				-- friendName = selfInfo.mSusername,
				-- friendImg  = "",
				-- friendExp  = selfInfo.mSexp + g_nextLv - 200,
				-- farmState  = selfInfo.mfarmState,
				--}
			}
end
--
--
--function lua_addFriendFarmState(friendBasicInfo, farmStatus)	
--	table.insert(farmFriendInfo_farmStatus_Map,
--				 {
--				  friendAcc  = friendBasicInfo.account,
--				  friendName = friendBasicInfo.username,
--				  friendImg  = "",
--				  friendExp  = farmStatus.exp,
--				  farmState  = farmStatus.status,
--				 }
--				)
--end
function lua_addFriendFarmState(playerAcc, playerName, farmStatus)
	table.insert(farmFriendInfo_farmStatus_Map,
				 {
				  friendAcc  = playerAcc,--friendBasicInfo.account,
				  friendName = playerName,--friendBasicInfo.username,
				  friendImg  = "",
				  friendExp  = farmStatus.exp,
				  farmState  = farmStatus.status,
				 }
				)
	Farm_insertFriendPhoto(playerAcc, "")	-- 初始好友头像(add by hezhr)
	if g_objmgr:isMainPlayer(playerAcc) then	-- (add by hezhr)
		Farm_setNeeddUpdateFriendPhoto(playerAcc, true)
	end
end

--请求农场好友列表
function FarmFL_reqFL()
	hideListBox()
	g_FarmMgr:reqFriendFarmState()
end

--打开农场好友列表
function FarmFL_openFL()
	if is_search == 0 then
		updateData(farmFriendInfo_farmStatus_Map,curPage)
	elseif is_search == 1 then
		updateData(searchResult_List,curPage)
	end
	showFarmFriendList()
	g_FarmUI:addChildWindow(FarmFriendList)
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local h
	h  = FarmFriendList:getHeight():asAbsolute(screenH)
	if g_FarmFriendListFlag == false then
		FarmFriendList:setPosition(CEGUI.UVector2(CEGUI.UDim(0,screenW-20),	CEGUI.UDim(0.5, -h/2)))
	end
	Common_injectEmptyEvt("farm_friendlist")
end
-----------------------------------------------
--
function FarmFL_resolutionChanged()
	local h, w
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	h  = FarmFriendList:getHeight():asAbsolute(screenH)
	w  = FarmFriendList:getWidth():asAbsolute(screenW)
	if g_FarmFriendListFlag == false then
		FarmFriendList:setPosition(CEGUI.UVector2(CEGUI.UDim(0,screenW-20),	CEGUI.UDim(0.5, -h/2)))
	else
		FarmFriendList:setPosition(CEGUI.UVector2(CEGUI.UDim(0,screenW-205),	CEGUI.UDim(0.5, -h/2)))
	end
end
--
function showFarmFriendList()
	local strBaseName = "farm_friendlist/F"
	local strBaseCovName = "farm_friendlist/F"
	local k = 1
	if is_search == 0 then
		totalPage = getTotalPage(farmFriendInfo_farmStatus_Map, MAXCOUNT)
	elseif is_search == 1 then
		totalPage = getTotalPage(searchResult_List, MAXCOUNT)
	end
	hideListBox()
	for i, v in pairs(showFriendListTab) do
		g_WinMgr:getWindow(strBaseName ..i):setVisible(true)
		g_WinMgr:getWindow(strBaseName ..i .."_cover" ..i):setVisible(true)
		g_WinMgr:getWindow(strBaseCovName ..i .."_cover" ..i):setID(i)
		g_WinMgr:getWindow(strBaseName ..k .."/name" ..k):setID(i)
		g_WinMgr:getWindow(strBaseCovName ..i .."_cover" ..i):removeEvent("MouseEnter")
		g_WinMgr:getWindow(strBaseCovName ..i .."_cover" ..i):removeEvent("MouseButtonDown")
		g_WinMgr:getWindow(strBaseCovName ..i .."_cover" ..i):subscribeEvent("MouseEnter", "isHovering")
		g_WinMgr:getWindow(strBaseCovName ..i .."_cover" ..i):subscribeEvent("MouseButtonDown", "handleMouseClick")
		local nameLen = EQGlobaFunc.getUTF8Len(v.friendName)
		local sub4Str
		--if nameLen > 4 then
		--	sub4Str = string.sub(v.friendName,1,4)
		--	EQGlobaFunc.SetText(strBaseName ..k .."/name" ..k, sub4Str .."...")
		--else
		if v.friendName ~= "" then
			EQGlobaFunc.SetText(strBaseName ..k .."/name" ..k, v.friendName)
		else
			EQGlobaFunc.SetText(strBaseName ..k .."/name" ..k, v.friendAcc)
		end
		--end
		g_WinMgr:getWindow(strBaseName ..k .."/number" ..k):setText(i+MAXCOUNT*(curPage-1))
		if 	v.farmState == 1 then
			g_WinMgr:getWindow(strBaseName ..k .."/state" ..k):setProperty("Image", "set:window2 image:farm_harvest")
		elseif v.farmState == 2 then
			g_WinMgr:getWindow(strBaseName ..k .."/state" ..k):setProperty("Image", "set:window2 image:farm_bug")
		elseif v.farmState == 3 then
			g_WinMgr:getWindow(strBaseName ..k .."/state" ..k):setProperty("Image", "set:window2 image:farm_grass")
		elseif v.farmState == 4 then
			g_WinMgr:getWindow(strBaseName ..k .."/state" ..k):setProperty("Image", "set:window2 image:")
		elseif v.farmState == 5 then
			g_WinMgr:getWindow(strBaseName ..k .."/state" ..k):setProperty("Image", "set:Button1 image:")
		end
		k = k + 1
		-- 插入账号列表(add by hezhr)
		if Farm_needUpdateFriendPhoto(v.friendAcc) then
			Farm_setFriendPhoto(strBaseName..i.."/icon"..i, v.friendAcc)
			g_FarmMgr:insertAccountList(v.friendAcc)
		end
	end
	g_FarmMgr:reqEqUsersInfo()	-- 请求好友信息(add by hezhr)
	FarmFL_setDefPushState()
	FarmFL_setNumTxt()
end

--触发排序函数
function triggerSort()
	table.sort(farmFriendInfo_farmStatus_Map, tableSort)
	updateData(farmFriendInfo_farmStatus_Map,1)
	--showFarmFriendList()
end

--排序函数
function tableSort(a,b)
	if (a.friendExp == b.friendExp)	then
		return a.friendName < b.friendName
	else
		return a.friendExp > b.friendExp
	end
end

--
function nextPage()
	if curPage >= totalPage then
		curPage = curPage
	else
		curPage = curPage + 1
	end
	
	if is_search == 0 then
		updateData(farmFriendInfo_farmStatus_Map,curPage)
	elseif is_search == 1 then
		updateData(searchResult_List,curPage)
	end
	for i, v in pairs(g_farmFriendPhoto) do
		Farm_setNeeddUpdateFriendPhoto(v.account, true)	
	end

	showFarmFriendList()
	FarmFL_setNumTxt()
	FarmFL_aaa()
end

--
function prePage()
	if curPage <= 1 then
		curPage = 1
	else
		curPage = curPage - 1
	end
	
	if is_search == 0 then
		updateData(farmFriendInfo_farmStatus_Map,curPage)
	elseif is_search == 1 then
		updateData(searchResult_List,curPage)
	end
	for i, v in pairs(g_farmFriendPhoto) do
		Farm_setNeeddUpdateFriendPhoto(v.account, true)	
	end

	showFarmFriendList()
	FarmFL_setNumTxt()
	FarmFL_aaa()
end

--
function updateData(curshowTable,page)
	showFriendListTab = {}
	showFriendListTab = getData(curshowTable, page, MAXCOUNT)
end

--
function hideListBox()
	local strBaseName1 = "farm_friendlist/F"
	for i = 1, MAXCOUNT do
		g_WinMgr:getWindow(strBaseName1 ..i):setVisible(false)
		g_WinMgr:getWindow(strBaseName1 ..i .."_cover" ..i):setVisible(false)
	end
end

--
function isHovering(args)
	local hoverBtn = CEGUI.toWindowEventArgs(args).window
	local serchID = hoverBtn:getID()
	local hoverWin =  g_WinMgr:getWindow("farm_friendlist/F" ..serchID .."/name" ..serchID)
	local WinID = hoverWin:getID()
	--g_Loger:logDebug("HoverID.." ..WinID)
	mSelectAcc = showFriendListTab[WinID].friendAcc
	g_mSelectName = showFriendListTab[WinID].friendName
	--g_Loger:logDebug("mSelectAcc.." ..mSelectAcc)
end

--
function handleMouseClick(args)
	local mouseEvtArgs = CEGUI.toMouseEventArgs(args)
	local keyval = CEGUI.toMouseEventArgs(args).button
	if keyval == 1 then
		FarmFL_popWin()
	elseif keyval == 0 then
		g_FarmMgr:setPushFlag(0)
		CloseAllFarmUI()
		FarmFL_setPushStaUI(args)
		clearOtherInfo()
		Farm_clearOtherName()
		g_FarmMgr:reqEnterFarm(mSelectAcc)
		g_farmSelectAccount = mSelectAcc	-- (add by hezhr)
	end
end

--
function FarmFL_popWin()
	
end

--
local initFL = false
function FarmFL_onclickSetPos()
	if false == g_FarmFriendListFlag then
		FarmFL_setUIPos("farm_friendlist",100,0,0,-183.5,0)
		g_WinMgr:getWindow("farm_friendlist/friends_button"):setProperty("Image", "set:Button1 image:imagebutton95_normal")
		g_WinMgr:getWindow("farm_friendlist"):setAlwaysOnTop(true)
		if initFL == false then
			FarmFL_reqFL()
			initFL = true
		end
		g_FarmFriendListFlag = true
	else
		FarmFL_setUIPos("farm_friendlist",100,0,0,183.5,0)
		g_WinMgr:getWindow("farm_friendlist/friends_button"):setProperty("Image", "set:Button1 image:imagebutton94_normal")
		g_WinMgr:getWindow("farm_friendlist"):setAlwaysOnTop(false)
		g_FarmFriendListFlag = false
	end
end
--
function FarmFL_setUIPos(winName,playTime,startX,startY,endX,endY)
	g_uiAnimationInfo.type = 1
	g_uiAnimationInfo.playTime = playTime
	g_uiAnimationInfo.startOffsetX = startX
	g_uiAnimationInfo.startOffsetY = startY
	g_uiAnimationInfo.endOffsetX = endX
	g_uiAnimationInfo.endOffsetY = endY

	--g_uiAnimationInfo.delayTime = delayTime
	EQGlobaFunc.playUIAnimation(winName, g_uiAnimationInfo)
end
--
function FarmFL_setNumTxt()
	g_WinMgr:getWindow("farm_friendlist/pages"):setText(curPage .."/" ..totalPage)
end
--
function FarmFL_setPushStaUI(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local pushWin = CEGUI.toPushButton(win)
	--if prePushWin ~= nil then
		--g_Loger:logDebug("******************************prePushWin*************************" ..prePushWin:getName())
		resetPushState()
	--end
	prePushWin = nil
	pushWin:setProperty("NormalImage", "set:window2 image:farm_freindlist2")
	pushWin:setProperty("HoverImage", "set:window2 image:farm_freindlist2")
	pushWin:setProperty("PushedImage", "set:window2 image:farm_freindlist2")
	prePushWin = pushWin
end
--
function resetPushState()
	if prePushWin ~= nil then
		prePushWin:setProperty("NormalImage", "set:window2 image:")
		prePushWin:setProperty("HoverImage", "set:window2 image:farm_friendbg")
		prePushWin:setProperty("PushedImage", "set:window2 image:farm_freindlist2")
	end
end
--
function FarmFL_setDefPushState()
	for i, v in pairs(showFriendListTab) do
		if g_objmgr:isMainPlayer(g_FarmMgr.mAccount) then
			if g_objmgr:isMainPlayer(v.friendAcc) then
				g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("NormalImage", "set:window2 image:farm_freindlist2")
				g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("HoverImage", "set:window2 image:farm_freindlist2")
				g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("PushedImage", "set:window2 image:farm_freindlist2")
				prePushWin = CEGUI.toPushButton(g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i))
			else
				g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("NormalImage", "set:window2 image:")
				g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("HoverImage", "set:window2 image:farm_friendbg")
				g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("PushedImage", "set:window2 image:farm_freindlist2")
			end
		end
	end
end
--
function FarmFL_reflash(args)
	is_search = 0
	ffriend_search_win:setText("")
	FarmFL_reqFL()
	FarmFL_aaa()
	-- 点击刷新按钮，重置农场好友头像需要更新(add by hezhr)
	for i, v in pairs(g_farmFriendPhoto) do
		Farm_setNeeddUpdateFriendPhoto(v.account, true)	
	end
end
--
function FarmFL_aaa()
	for i, v in pairs(showFriendListTab) do
		if g_FarmMgr.mAccount == v.friendAcc then
			g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("NormalImage", "set:window2 image:farm_freindlist2")
			g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("HoverImage", "set:window2 image:farm_freindlist2")
			g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("PushedImage", "set:window2 image:farm_freindlist2")
			prePushWin = CEGUI.toPushButton(g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i))
		else
			g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("NormalImage", "set:window2 image:")
			g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("HoverImage", "set:window2 image:farm_friendbg")
			g_WinMgr:getWindow("farm_friendlist/F" ..i .."_cover" ..i):setProperty("PushedImage", "set:window2 image:farm_freindlist2")
		end
	end
end
--
function FarmFL_firstPage()
	ffriend_search_win:setText("")
	updateData(farmFriendInfo_farmStatus_Map,1)
	curPage = 1
	showFarmFriendList()
	FarmFL_setNumTxt()
	FarmFL_aaa()
end
--
function FarmFL_resetFLpos()
	if true == g_FarmFriendListFlag then
		FarmFL_setUIPos("farm_friendlist",100,0,0,183.5,0)
		g_WinMgr:getWindow("farm_friendlist/friends_button"):setProperty("Image", "set:Button1 image:imagebutton94_normal")
		g_WinMgr:getWindow("farm_friendlist"):setAlwaysOnTop(false)
		g_FarmFriendListFlag = false
	end
end
-----------------------------------------
--
function FarmFL_search_match_friend(search_key)
	searchResult_List = {}
	if search_key == "" then
		is_search = 0
		updateData(farmFriendInfo_farmStatus_Map,1)
		for i, v in pairs(g_farmFriendPhoto) do
			Farm_setNeeddUpdateFriendPhoto(v.account, true)	
		end
		showFarmFriendList()
		FarmFL_setNumTxt()
		FarmFL_aaa()
		return
	end
	
	is_search = 1
	for i,v in ipairs(farmFriendInfo_farmStatus_Map) do
		if string.find(v.friendName, search_key) then
			table.insert(searchResult_List, v)
		end
	end
	
	for i,v in ipairs(searchResult_List) do
		Farm_setNeeddUpdateFriendPhoto(v.friendAcc, true)	
	end
	updateData(searchResult_List,1)
	showFarmFriendList()
	FarmFL_setNumTxt()
	FarmFL_aaa()
end
-----------------------------------------
--
function FfriendList_sTxt_change(args)
	local search_win = CEGUI.toWindowEventArgs(args).window
	local sTxt = search_win:getText()
	FarmFL_search_match_friend(sTxt)
end
-----------------------------------------
--找出账号在当前好友列表的索引位置(add by hezhr)
function Farm_findAccountIndex(account)
	for i, v in pairs(showFriendListTab) do
		if account == v.friendAcc then
			return i
		end
	end
	return 0
end
-----------------------------------------
--插入农场好友头像数组(add by hezhr)
local function _insertFriendPhotoVec(friendPhoto, photoData)
	local photoName = EQGlobaClass.getDynamicCreateImagesetMgr():getImageName(photoData)
	if "" ~= photoName then	-- 存在相同数据的图片
		friendPhoto.curName = photoName
		return friendPhoto
	end
	photoName = friendPhoto.account.."_"..tostring(table.getn(friendPhoto.photoVec))
	local ret = EQGlobaClass.getDynamicCreateImagesetMgr():saveImage(photoData, photoName) -- 保存图片
	if 0 == ret then	-- 保存成功
		table.insert(friendPhoto.photoVec, photoName)
		friendPhoto.curName = photoName
	elseif -1 == ret then	-- 存在相同数据的图片
		friendPhoto.curName = EQGlobaClass.getDynamicCreateImagesetMgr():getImageName(photoData)
	elseif -2 == ret then	-- 存在同名图片
		friendPhoto.curName = photoName
	else	-- 保存失败
		-- 使用原来的图片
	end
	return friendPhoto
end
-----------------------------------------
--插入农场好友头像表(add by hezhr)
function Farm_insertFriendPhoto(account, photoData)
	for i, v in pairs(g_farmFriendPhoto) do
		if account == v.account then	-- 存在该账号
			v = _insertFriendPhotoVec(v, photoData)
			return
		end
	end
	local photoVec = {}
	table.insert(photoVec, "")
	local friendPhoto = {}
	friendPhoto.account = account	-- 账号
	friendPhoto.curName = ""		-- 当前头像名
	friendPhoto.photoVec = photoVec	-- 头像名数组
	friendPhoto.needUpdate = true	-- 是否需要更新头像
	table.insert(g_farmFriendPhoto, friendPhoto)
	local index = table.getn(g_farmFriendPhoto)
	g_farmFriendPhoto[index] = _insertFriendPhotoVec(g_farmFriendPhoto[index], photoData)
end
-----------------------------------------
--移出农场好友头像表(add by hezhr)
function Farm_removeFriendPhoto(account)
	for i, v in pairs(g_farmFriendPhoto) do
		if account == v.account then	-- 存在该账号
			table.remove(g_farmFriendPhoto, i)
			return
		end
	end
end
-----------------------------------------
--获取农场好友头像名(add by hezhr)
function Farm_getFriendPhotoName(account)
	for i, v in pairs(g_farmFriendPhoto) do
		if account == v.account then	-- 存在该账号
			return v.curName
		end
	end
	return "NULL"
end
-----------------------------------------
--是否需要更新农场好友头像(add by hezhr)
function Farm_needUpdateFriendPhoto(account)
	for i, v in pairs(g_farmFriendPhoto) do
		if account == v.account then	-- 存在该账号
			return v.needUpdate
		end
	end
	return false
end
-----------------------------------------
--设置是否需要更新农场好友头像(add by hezhr)
function Farm_setNeeddUpdateFriendPhoto(account, needUpdate)
	for i, v in pairs(g_farmFriendPhoto) do
		if account == v.account then	-- 存在该账号
			v.needUpdate = needUpdate
		end
	end	
end
-----------------------------------------
--设置窗口图片为农场好友头像(add by hezhr)
function Farm_setFriendPhoto(winname, account)
	local photoName = Farm_getFriendPhotoName(account)
	if "NULL"~=photoName and ""~=photoName then
		EQGlobaClass.getDynamicCreateImagesetMgr():setWindowImage(winname, photoName)
	else	-- 设置默认好友头像
		g_WinMgr:getWindow(winname):setProperty("Image", "set:Button1 image:ImageButton41")
	end
end
