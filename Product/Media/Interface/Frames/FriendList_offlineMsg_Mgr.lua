-----------------------------------------
-- 文件说明	：离线消息和设置
-- 作者		：Mr_DjHuang
-- 修改		：
-- 日期		：2011-04-29
-----------------------------------------
-----------------------------------------
--
g_offlineUIOpened = false
local offline_inviteRecords = {}
local offline_inviteRecords_showList = {}
local PageMaxSize = 2
local curPage = 1
local totalPage = 1

local isClicked = 0

-----------------------------------------

-----------------------------------------
--
function FLofflineMsgMgr_setisClicked(value)
	isClicked = value
end
-----------------------------------------
--
function FLofflineMsgMgr_onLoadInit()
	Common_injectEmptyEvt(g_OfflineMsgUIName.root)
	MainUI_ShieldClick(g_OfflineMsgMgrUI)
	FLofflineMsgMgr_subcribeEvent()
	--
	--暂时不做的界面禁用掉
	FLofflineMsgMgr_disableSomeWnd()
end
-----------------------------------------
--
function FLofflineMsgMgr_disableSomeWnd()
	--for i = 1,PageMaxSize do
	--	g_WinMgr:getWindow(g_OfflineMsgUIName["checkBox" ..tostring(i)]):
			--setEnabled(false)
	--end
	--g_WinMgr:getWindow(g_OfflineMsgUIName.tabBtn2):setEnabled(false)
end
-----------------------------------------
--
function FLofflineMsgMgr_subcribeEvent()
	g_GameEventMgr:subscribeEvent("NOTIFY_OFFLINE_INVITE",
								  "FLofflineMsgMgr_notifyOfflineInviteRecords")
	g_GameEventMgr:subscribeEvent("OFFLINE_INVITERECORDS_SHOWlIST_UPDATE",
								  "FLofflineMsgMgr_inviteRecords_showListUpdate")
	
	for i = 1,PageMaxSize do
		g_WinMgr:getWindow(g_OfflineMsgUIName["confirmBtn" ..tostring(i)]):
			subscribeEvent("Clicked", "FLofflineMsgMgr_agreeInvite" ..tostring(i))
		g_WinMgr:getWindow(g_OfflineMsgUIName["cancelBtn" ..tostring(i)]):
			subscribeEvent("Clicked", "FLofflineMsgMgr_disagreeInvite" ..tostring(i))		
	end
	--for i = 1,PageMaxSize do
		--g_WinMgr:getWindow(g_OfflineMsgUIName["checkBox" ..tostring(i)]):
		--	subscribeEvent("CheckStateChanged", "FLofflineMsgMgr_CheckStateChanged" ..tostring(i))
	--end
	g_WinMgr:getWindow(g_OfflineMsgUIName.prePageBtn):
		subscribeEvent("Clicked", "FLofflineMsgMgr_prePage")
	g_WinMgr:getWindow(g_OfflineMsgUIName.nextPageBtn):
		subscribeEvent("Clicked", "FLofflineMsgMgr_nextPage")
	
	g_WinMgr:getWindow(g_OfflineMsgUIName.closeBtn):
		subscribeEvent("Clicked", "FLofflineMsgMgr_closeOfflineMsgUI")
end
-----------------------------------------
--
function FLofflineMsgMgr_setUIPos()
	local win = g_WinMgr:getWindow(g_OfflineMsgUIName.root)
	local h, w
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	h  = win:getHeight():asAbsolute(screenH)
	w  = win:getWidth():asAbsolute(screenW)
	win:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-w/2),	CEGUI.UDim(0.5, -h/2)))
end
-----------------------------------------
--
function FLofflineMsgMgr_clearInviteRecords()
	offline_inviteRecords = {}
end
-----------------------------------------
--
function FLofflineMsgMgr_insertInviteRecord(targetTable,record)
	table.insert(targetTable,record)
end
-----------------------------------------
--
function FLofflineMsgMgr_addInviteData_fromCpp(	invitor,
												invitor_name,
												invitor_type,
												date)
	local temp_Record = {
						 ["invitor"] 		= invitor,
						 ["invitor_name"] 	= invitor_name,
						 ["invitor_type"] 	= invitor_type,
						 ["date"] 			= date
						}
	FLofflineMsgMgr_insertInviteRecord(offline_inviteRecords,temp_Record)
end
-----------------------------------------
--
function FLofflineMsgMgr_notifyOfflineInviteRecords()
--	g_Loger:logMessage("/*NOTIFY_OFFLINE_INVITE*/")
	if isClicked == 0 then
		g_OfflineMsgMgr:addAnimation()
	else
		g_OfflineMsgMgr:removeAnimation()
	end
	--g_Loger:logError("---------------------------------------------")
	for i,v in ipairs(offline_inviteRecords) do
		--g_Loger:logMessage("invitor***" ..tostring(i) .."***" ..tostring(v.invitor))
		--g_Loger:logMessage("invitor_name***" ..tostring(i) .."***" ..tostring(v.invitor_name))
		--g_Loger:logMessage("invitor_type***" ..tostring(i) .."***" ..tostring(v.invitor_type))
		--g_Loger:logMessage("date***" ..tostring(i) .."***" ..tostring(v.date))
	end
	--g_Loger:logError("---------------------------------------------")
	local cur_page = FLofflineMsgMgr_getCurPage()
	FLofflineMsgMgr_setTotalPage()
	if curPage > totalPage then
		curPage = totalPage
	end
	FLofflineMsgMgr_setPageBox()
	FLofflineMsgMgr_updateShowList(offline_inviteRecords,curPage)
end
-----------------------------------------
--
function FLofflineMsgMgr_inviteRecords_showListUpdate()
--	g_Loger:logError("fuck3*******")
	local date = ""
	local text = ""
	local name = ""
	--
	local txtWinName  = ""
	local txtWin	  = nil
	for i = 1, PageMaxSize do
		txtWinName = g_OfflineMsgUIName.tabBtn1 .."_Text" ..tostring(i)
		txtWin = g_WinMgr:getWindow(txtWinName)
		--g_Loger:logError("fuck4*******")
		if txtWin ~= nil then
			--g_Loger:logError("fuck4*******" ..txtWin:getName())
			EQGlobaFunc.toMultiLineBox(txtWin:getName()):clearUp()
		end
	end
--	g_Loger:logError("----------------------------")
	for i,v in ipairs(offline_inviteRecords_showList) do
		date = tostring(v.date) .. " "
		name = tostring(v.invitor_name)
		text = "]请求你通过好友申请，同意吗？"
--		g_Loger:logMessage(date)
--		g_Loger:logMessage(tostring(name))
		FLofflineMsgMgr_setInviteContext(i, date, name, text)
	end
--	g_Loger:logError("----------------------------")
end
-----------------------------------------
--
function FLofflineMsgMgr_setInviteContext(idx, date,name,str)
	local parentWin = g_WinMgr:getWindow(g_OfflineMsgUIName["msgPanel" ..tostring(idx)])
	local txtWinName = g_OfflineMsgUIName.tabBtn1 .."_Text" ..tostring(idx)
	local txtWin = g_WinMgr:getWindow(txtWinName)
	if txtWin == nil then
		txtWin = CEGUI.toMultiLineEditbox(g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", txtWinName))
		parentWin:addChildWindow(txtWin)
		txtWin:setSize(CEGUI.UVector2(CEGUI.UDim(0,250), CEGUI.UDim(0,33)))
		txtWin:setPosition(CEGUI.UVector2(CEGUI.UDim(0,0), CEGUI.UDim(0,0)))
		EQGlobaFunc.toMultiLineBox(txtWin:getName()):setAlign("AT_CENTER_HOR")
		EQGlobaFunc.toMultiLineBox(txtWin:getName()):setAlign("AT_CENTER_VER")		
		EQGlobaFunc.toMultiLineBox(txtWin:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(txtWin:getName()):setCols("#FFFFFFFF")
		txtWin:subscribeEvent("HyperLink_MouseClick", "FLofflineMsgMgr_showPlayerInfo" ..tostring(idx))
	else
		
	end
	EQGlobaFunc.toMultiLineBox(txtWin:getName()):clearUp()
	--g_Loger:logMessage(tostring(date))
	EQGlobaFunc.toMultiLineBox(txtWin:getName()):addElementText(date, false)
	EQGlobaFunc.toMultiLineBox(txtWin:getName()):addElementText("[", false)
	EQGlobaFunc.toMultiLineBox(txtWin:getName()):addElementHyperLink(name, "", "#FF9C00", false)
	EQGlobaFunc.toMultiLineBox(txtWin:getName()):addElementText(str, false)
	local width = EQGlobaFunc.toMultiLineBox(txtWin:getName()):getFitWidth("simsun-10", date .."["..name..str)
	local height = EQGlobaFunc.toMultiLineBox(txtWin:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(txtWin:getName()):setHeight(height)

end
-----------------------------------------
--
function FLofflineMsgMgr_updateShowList(sourceTable,page)
	FLofflineMsgMgr_clearShowList()
	FLofflineMsgMgr_fillDataInShowList(sourceTable, page)
	_FLofflineMsgMgr_notityUpdateShowList()
end
-----------------------------------------
--
function FLofflineMsgMgr_clearShowList()
	offline_inviteRecords_showList = {}
end
-----------------------------------------
--
function FLofflineMsgMgr_fillDataInShowList(sourceTable, page)
	offline_inviteRecords_showList = getData(sourceTable, page, PageMaxSize)
end
-----------------------------------------
--
function FLofflineMsgMgr_showDataOnUI()
	for i,v in ipairs(offline_inviteRecords_showList) do
--		g_Loger:logMessage("invitor***" ..tostring(i) .."***" ..tostring(v.invitor))
--		g_Loger:logMessage("invitor_name***" ..tostring(i) .."***" ..tostring(v.invitor_name))
--		g_Loger:logMessage("invitor_type***" ..tostring(i) .."***" ..tostring(v.invitor_type))
--		g_Loger:logMessage("date***" ..tostring(i) .."***" ..tostring(v.date))
	end
end
-----------------------------------------
--
function FLofflineMsgMgr_setTotalPage()
	totalPage = getTotalPage(offline_inviteRecords, PageMaxSize)
	if totalPage == 0 then
		totalPage = 1
	end
end
-----------------------------------------
--
function FLofflineMsgMgr_getTotalPage()
	return totalPage
end
-----------------------------------------
--
function FLofflineMsgMgr_setCurPage(page)
	curPage = page
end
-----------------------------------------
--
function FLofflineMsgMgr_getCurPage()
	return curPage
end
-----------------------------------------
--
function FLofflineMsgMgr_openOfflineMsgUI()
	if isClicked == 0 then
		FLofflineMsgMgr_setisClicked(1)
		g_OfflineMsgMgr:removeAnimation()
	else
		
	end
	FLofflineMsgMgr_DoopenUI()
	FLofflineMsgMgr_setPageBox()
	FLofflineMsgMgr_setUIPos()
end
-----------------------------------------
--
function FLofflineMsgMgr_DoopenUI()
	UIM_OpenWindow(g_OfflineMsgUIName.root)
	g_offlineUIOpened = true
end
-----------------------------------------
--
function FLofflineMsgMgr_closeOfflineMsgUI()
	FLofflineMsgMgr_DocloseUI()
end
-----------------------------------------
--
function FLofflineMsgMgr_DocloseUI()
	UIM_CloseWindow(g_OfflineMsgUIName.root)
	g_offlineUIOpened = false
end
-----------------------------------------
--
function FLofflineMsgMgr_showPlayerInfo1(args)
--	g_Loger:logError("--***showPlayerInfoLink1***--")
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum == 0 then
		return
	end
	local invitor = offline_inviteRecords_showList[1].invitor
	g_req_other_player_info.account = invitor
	g_msgmgr:send(g_req_other_player_info)
	PopOtherPlayerInfo()
end
-----------------------------------------
--
function FLofflineMsgMgr_showPlayerInfo2(args)
--	g_Loger:logError("--***showPlayerInfoLink2***--")
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum < 2 then
		return
	end
	local invitor = offline_inviteRecords_showList[2].invitor
	g_req_other_player_info.account = invitor
	g_msgmgr:send(g_req_other_player_info)
	PopOtherPlayerInfo()
end
-----------------------------------------
--
function FLofflineMsgMgr_agreeInvite1(args)
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum == 0 then
		return
	end
	local invitor = offline_inviteRecords_showList[1].invitor
	--
	FLofflineMsgMgr_sendAgreeInviteMsg(invitor)
	FLofflineMsgMgr_deleteRecord(invitor)
end
-----------------------------------------
--
function FLofflineMsgMgr_agreeInvite2(args)
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum < 2 then
		return
	end
	local invitor = offline_inviteRecords_showList[2].invitor
	--
	FLofflineMsgMgr_sendAgreeInviteMsg(invitor)
	FLofflineMsgMgr_deleteRecord(invitor)
end
-----------------------------------------
--
function FLofflineMsgMgr_sendAgreeInviteMsg(invitor)
	g_req_agree_invitation.type = 3
	g_req_agree_invitation.invitor = invitor
	g_msgmgr:send(g_req_agree_invitation)
end
-----------------------------------------
--表示删除一个邀请记录，包括存储表和显示表
function FLofflineMsgMgr_deleteRecord(invitor)
	FLofflineMsgMgr_deleteInviteShowRecord(invitor)
	FLofflineMsgMgr_deleteInviteRecord(invitor)
	
end
-----------------------------------------
--只删除存储表中的一条邀请记录
function FLofflineMsgMgr_deleteInviteRecord(invitor)
	local findIdx = 0
	findIdx = _FLofflineMsgMgr_findMatchRecordIdx(offline_inviteRecords,invitor)
	if findIdx ~= 0 then
		table.remove(offline_inviteRecords,findIdx)
	end
	_FLofflineMsgMgr_notifyInviteRecordsDataUpdate()
end
------------------------------------------
--
function _FLofflineMsgMgr_findMatchRecordIdx(table,invitor)
	local find_idx = 0
--	g_Loger:logError(tostring(invitor))
	if table ~= nil then
		for i,v in ipairs(table) do
--			g_Loger:logMessage("invitor***" ..tostring(i) .."***" ..tostring(v.invitor))
			if v.invitor == invitor then
				find_idx = i
				break
			else
				g_Loger:logError("no find!")
				find_idx = 0
			end
		end
	else
		g_Loger:logError("param (table) is nil!")
	end
	return find_idx
end
-----------------------------------------
--只删除显示表中的一条邀请记录
function FLofflineMsgMgr_deleteInviteShowRecord(invitor)
	local findIdx = 0
--	g_Loger:logError(tostring(invitor))
	findIdx = _FLofflineMsgMgr_findMatchRecordIdx(offline_inviteRecords_showList,invitor)
--	g_Loger:logError(tostring(findIdx))
	if findIdx ~= 0 then
		table.remove(offline_inviteRecords_showList,findIdx)
	end
	_FLofflineMsgMgr_notityUpdateShowList()
end
-----------------------------------------
--
function FLofflineMsgMgr_disagreeInvite1(args)
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum == 0 then
		return
	end
	local invitor = offline_inviteRecords_showList[1].invitor
	--
	FLofflineMsgMgr_sendDisagreeInviteMsg(invitor)
	FLofflineMsgMgr_deleteRecord(invitor)
end
-----------------------------------------
--
function FLofflineMsgMgr_disagreeInvite2(args)
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum < 2 then
		return
	end
	local invitor = offline_inviteRecords_showList[2].invitor
	--
	FLofflineMsgMgr_sendDisagreeInviteMsg(invitor)
	FLofflineMsgMgr_deleteRecord(invitor)
end
-----------------------------------------
--
function FLofflineMsgMgr_sendDisagreeInviteMsg(invitor)
	g_req_disagree_invitation.type = 3
	g_req_disagree_invitation.invitor = invitor
	g_msgmgr:send(g_req_disagree_invitation)
end
-----------------------------------------
--
function FLofflineMsgMgr_sendIgnoreSomeoneMsg(who)
	if who ~= nil and who ~= "" then
		g_req_friend_setting_black_someone.who = who
		g_msgmgr:send(g_req_friend_setting_black_someone)
	end
end
-----------------------------------------
--
function FLofflineMsgMgr_CheckStateChanged1(args)
	local checkbox = CEGUI.toCheckbox(g_WinMgr:getWindow(g_OfflineMsgUIName.checkBox1))
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum == 0 then
		return
	end
	local invitor = offline_inviteRecords_showList[1].invitor
	if checkbox:isSelected() == true then
		FLofflineMsgMgr_sendIgnoreSomeoneMsg(invitor)
	else
	
	end
end
-----------------------------------------
--
function FLofflineMsgMgr_CheckStateChanged2(args)
	local checkbox = CEGUI.toCheckbox(g_WinMgr:getWindow(g_OfflineMsgUIName.checkBox2))
	local listNum = table.getn(offline_inviteRecords_showList)
	if listNum < 2 then
		return
	end
	local invitor = offline_inviteRecords_showList[2].invitor
	if checkbox:isSelected() == true then
		FLofflineMsgMgr_sendIgnoreSomeoneMsg(invitor)
	else
	
	end
end
-----------------------------------------
--
function FLofflineMsgMgr_prePage()
	--local curpage = FLofflineMsgMgr_getCurPage()
	if curPage <= 1 then
		curPage = 1
		
	else
		curPage = curPage - 1
		
	end
	FLofflineMsgMgr_updateShowList(offline_inviteRecords,curPage)
	FLofflineMsgMgr_setPageBox()
end
-----------------------------------------
--
function FLofflineMsgMgr_nextPage()
	--local curpage = FLofflineMsgMgr_getCurPage()
	local totalpage = FLofflineMsgMgr_getTotalPage()
	if curPage >= totalpage then
		curPage = totalpage
		
	else
		curPage = curPage + 1
		
	end
--	g_Loger:logError("fuck0*******")
	FLofflineMsgMgr_updateShowList(offline_inviteRecords,curPage)
	FLofflineMsgMgr_setPageBox()
end
-----------------------------------------
--
function FLofflineMsgMgr_setPageBox()
	local txtBox = g_WinMgr:getWindow(g_OfflineMsgUIName.pageBox)
	local curPage = FLofflineMsgMgr_getCurPage()
	local totalPage = FLofflineMsgMgr_getTotalPage()
	txtBox:setText(tostring(curPage) .."/" ..tostring(totalPage))
end
-----------------------------------------
--
function _FLofflineMsgMgr_notityUpdateShowList()
	g_GameEventMgr:fireEvent("OFFLINE_INVITERECORDS_SHOWlIST_UPDATE")
end
------------------------------------------
--
function _FLofflineMsgMgr_notifyInviteRecordsDataUpdate()
	g_GameEventMgr:fireEvent("NOTIFY_OFFLINE_INVITE")
end