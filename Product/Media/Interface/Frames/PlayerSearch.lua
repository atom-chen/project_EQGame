--玩家搜索相关
----------------------------------
MainUI_ShieldClick(g_PsInputUI)
MainUI_ShieldClick(g_PsOutputUI)
---------------------------------
---------------------------------
--
local mSelectedPlayerAcc = ""
local resultList = {}
local Ps_showList= {}
local Maxsize = 15
local Ps_curpage = 1
local Ps_totalPage = 1
---------------------------------
--
local friendlist_searchBtn = g_WinMgr:getWindow("freindlist/shuaixn1")
local player_search_closebtn = g_WinMgr:getWindow("find_friend__auto_closebutton__")
local player_search_nickInputbox = g_WinMgr:getWindow("find_friend/tab/Tab 1/enter_words")
local player_search_mailInputbox = g_WinMgr:getWindow("find_friend/tab/Tab 2/enter_words2")
local Player_search_cfmbtn = g_WinMgr:getWindow("find_friend/tab/Tab 1/done1")
local Player_search_cancelbtn = g_WinMgr:getWindow("find_friend/tab/Tab 1/cancle1")
local Player_search_cfmbtn1 = g_WinMgr:getWindow("find_friend/tab/Tab 2/done2")
local Player_search_cancelbtn1 = g_WinMgr:getWindow("find_friend/tab/Tab 2/cancle2")

local Ps_addAsFriendBtn = g_WinMgr:getWindow("find_friend_result/add_player")
local Ps_reSearchBtn = g_WinMgr:getWindow("find_friend_result/refind")
local psList_cancelListBtn = g_WinMgr:getWindow("find_friend_result/add_player1")
local PsList_closeBtn = g_WinMgr:getWindow("find_friend_result__auto_closebutton__")
local PsList_preBtn = g_WinMgr:getWindow("find_friend_result/pageup")
local PsList_nextBtn = g_WinMgr:getWindow("find_friend_result/pageup1")
local PsList_PageNum = g_WinMgr:getWindow("find_friend_result/yeshu")
local PsList_VertiScrollbar = g_WinMgr:getWindow("find_friend_result/scrollbar")
local PsList_toVScrollbar = CEGUI.toScrollbar(PsList_VertiScrollbar)
local PsList_HorizScrollbar = g_WinMgr:getWindow("find_friend_result/h_srcrollbar")
local PsList_toHScrollbar = CEGUI.toScrollbar(PsList_HorizScrollbar)
---
--

local playerNickListbox = CEGUI.toListbox(g_WinMgr:getWindow("find_friend_result/result_window/list_name1"))
	  playerNickListbox:setMultiselectEnabled(false)
local playerAccListbox = CEGUI.toListbox(g_WinMgr:getWindow("find_friend_result/result_window/list_mail1"))
	  playerAccListbox:setMultiselectEnabled(false)
local playerLocatListbox = CEGUI.toListbox(g_WinMgr:getWindow("find_friend_result/result_window/list_from1"))
	  playerLocatListbox:setMultiselectEnabled(false)

-------------
friendlist_searchBtn:subscribeEvent("Clicked", "Ps_openSui")
player_search_closebtn:subscribeEvent("Clicked", "Ps_closeSui")
Player_search_cfmbtn:subscribeEvent("Clicked", "Ps_clickInputNick")
Player_search_cfmbtn1:subscribeEvent("Clicked", "Ps_clickInputMail")
Player_search_cancelbtn:subscribeEvent("Clicked", "Ps_cancelInput")
Player_search_cancelbtn1:subscribeEvent("Clicked", "Ps_cancelInput")

Ps_addAsFriendBtn:subscribeEvent("Clicked", "Ps_addAsFriend")
Ps_reSearchBtn:subscribeEvent("Clicked", "Ps_reSearchPlayer")
psList_cancelListBtn:subscribeEvent("Clicked", "PsList_cancelList")
PsList_closeBtn:subscribeEvent("Clicked", "Ps_closePsListUI")
PsList_preBtn:subscribeEvent("Clicked", "PsList_prePage")
PsList_nextBtn:subscribeEvent("Clicked", "PsList_nextPage")
PsList_VertiScrollbar:subscribeEvent("ScrollPosChanged", "PsList_VscrollPosChange")
PsList_HorizScrollbar:subscribeEvent("ScrollPosChanged", "PsList_HscrollPosChange")

playerNickListbox:subscribeEvent("ItemSelectionChanged", "PsPlayerListbox_Selected")
playerAccListbox:subscribeEvent("ItemSelectionChanged", "PsPlayerListbox_Selected")
playerLocatListbox:subscribeEvent("ItemSelectionChanged", "PsPlayerListbox_Selected")

--g_GameEventMgr:subscribeEvent("UPDATE_SEARCHKEY","FriendList_search_update")
--
------------------------------------------------------
------------------------------------------------------
--打开搜索窗口
function Ps_openSui()
	Ps_clearSearchInput()
	g_MainUIWindows:addChildWindow(g_PsInputUI)
	SetWindowCenter(g_PsInputUI)
	MainUI_hideMFFrame("find_friend")
	Common_injectEmptyEvt("find_friend")
end
------------------------------------------------------
--关闭搜索窗口
function Ps_closeSui()
	g_MainUIWindows:removeChildWindow(g_PsInputUI)
end
-------------------------------------------------------
--输入的是昵称
function Ps_clickInputNick(args)
	local inputTxt = player_search_nickInputbox:getText()
	Ps_submitReqInfo(inputTxt,0)
end
--输入的是邮件地址
function Ps_clickInputMail(args)
	local inputTxt = player_search_mailInputbox:getText()
	Ps_submitReqInfo(inputTxt,1)
end
--------------------------------------------
--提交请求信息
function Ps_submitReqInfo(inputTxt,mode)
	if inputTxt == "" then
		return
	else
		g_PlayerSearch:req_searchPlayer(inputTxt, mode)
	end
end
--------------------------------------------
--取消清空输入
function Ps_cancelInput(args)
	local cancelBtn = CEGUI.toWindowEventArgs(args).window
	local inputbox = cancelBtn:getParent():getChildAtIdx(0)
	inputbox:setText("")
end
--------------------------------------------
function Ps_clearSearchInput()
	player_search_nickInputbox:setText("")
	player_search_mailInputbox:setText("")
end
--------------------------------------------
--
--------------------------------------------
--
function Ps_getResultplayerList(nick,acc,province,city)
	table.insert(resultList,
				 {playerNick = nick,
				  playerAcc = acc,
				  playerProvince = province,
				  playerCity = city
				 })
	
end
--------------------------------------------
--
function Ps_clearResultList()
	resultList = {}
end
--------------------------------------------
--
function Ps_isResultListEmpty()
	local listNum = table.getn(resultList)
	if listNum == 0 then
		return true
	else
		return false
	end
end
--------------------------------------------
--
function Ps_openPsListboxUI()
	if Ps_isResultListEmpty() then
--		g_Loger:logError("outputSearchPlayerError")
		--return
	else
		Ps_closeSui()
		g_MainUIWindows:addChildWindow(g_PsOutputUI)
		SetWindowCenter(g_PsOutputUI)
		MainUI_hideMFFrame("find_friend_result")
		Common_injectEmptyEvt("find_friend_result")
		Ps_updateShowList(resultList,Ps_curpage)
		Ps_setListboxData()
		PsList_setcurPage()
	end
end
--------------------------------------------
--
function Ps_closePsListUI()
	PsList_clearPlayerList()
	Ps_resetAllListbox()
	PsList_resetVscrollbar()
	g_MainUIWindows:removeChildWindow(g_PsOutputUI)
end
--------------------------------------------
--
function Ps_setListboxData()
	Ps_resetAllListbox()
	Ps_totalPage = getTotalPage(resultList,Maxsize)
	for i,v in ipairs(Ps_showList) do
		--g_Loger:logError(tostring(v.playerNick))
		EQGlobaFunc.createListboxTextItem(playerNickListbox:getName(), v.playerNick, i)
		EQGlobaFunc.createListboxTextItem(playerAccListbox:getName(), v.playerAcc, i)
		EQGlobaFunc.createListboxTextItem(playerLocatListbox:getName(), 
										  v.playerProvince .." " ..v.playerCity, i)
	end
	PsList_setScrollProperty()
	PsList_setListboxScrollbar_unVisible()
end
---------------------------------------------
--
function Ps_resetAllListbox()
	playerNickListbox:resetList()
	playerAccListbox:resetList()
	playerLocatListbox:resetList()
	
end
---------------------------------------------
--
function PsPlayerListbox_Selected(args)
	local listbox = CEGUI.toWindowEventArgs(args).window
	local selectItem = CEGUI.toListbox(listbox):getFirstSelectedItem()
	if selectItem ~= nil then
		local itemIdx = selectItem:getID()
		PsList_setListItemState(listbox,itemIdx-1)
		PsList_saveSelectPlayer(itemIdx-1)
	else
		PsList_clearAllSelections()
		Ps_clearSelectPlayerAcc()
	end
	
end
----------------------------------------------
--
function PsList_setListItemState(listbox,itemIdx)
	local listboxName = listbox:getName()
	if listboxName == playerNickListbox:getName() then
		playerAccListbox:setItemSelectState(itemIdx,true)
		playerLocatListbox:setItemSelectState(itemIdx,true)
		
	elseif listboxName == playerAccListbox:getName() then
		playerNickListbox:setItemSelectState(itemIdx,true)
		playerLocatListbox:setItemSelectState(itemIdx,true)
		
	elseif listboxName == playerLocatListbox:getName() then
		playerNickListbox:setItemSelectState(itemIdx,true)
		playerAccListbox:setItemSelectState(itemIdx,true)
	end
end
----------------------------------------------
--
function PsList_clearAllSelections()
	playerNickListbox:clearAllSelections()
	playerAccListbox:clearAllSelections()
	playerLocatListbox:clearAllSelections()
end
----------------------------------------------
--
function PsList_saveSelectPlayer(itemIdx)
	local item = playerAccListbox:getListboxItemFromIndex(itemIdx)
	local selectPlayerAcc = item:getText()
	--g_Loger:logError(tostring(selectPlayerAcc))
	Ps_saveSelectPlayerAcc(selectPlayerAcc)
end
-----------------------------------------------
--
function Ps_saveSelectPlayerAcc(acc)
	mSelectedPlayerAcc = acc
end
-----------------------------------------------
--
function Ps_getSelectPlayerAcc()
	return mSelectedPlayerAcc
end
------------------------------------------------
--
function Ps_clearSelectPlayerAcc()
	mSelectedPlayerAcc = ""
end
------------------------------------------------
--
function Ps_addAsFriend(args)
	if Ps_getSelectPlayerAcc() ~= "" then
		local playerAcc = Ps_getSelectPlayerAcc()
		g_FriendSys:reqAsFriend(playerAcc)
	end
end
------------------------------------------------
--
function Ps_reSearchPlayer(args)
	Ps_closePsListUI()
	Ps_openSui()
end
------------------------------------------------
--
function PsList_cancelList()
	Ps_closePsListUI()
end
------------------------------------------------
--
function PsList_clearPlayerList()
	g_PlayerSearch:clearPlayerList()
end
------------------------------------------------
--翻页相关
------------------------------------------------
--
function Ps_updateShowList(curshowTable,page)
	local temp_outList = {}
	Ps_clearShowList()
	temp_outList = getData(curshowTable, page, Maxsize)
	Ps_setShowListData(temp_outList)
end
------------------------------------------------
--
function Ps_clearShowList()
	Ps_showList = {}
end
-------------------------------------------------
--
function Ps_setShowListData(const_list)
	Ps_showList = const_list
end
-------------------------------------------------
--
function PsList_prePage()
	if Ps_curpage <= 1 then
		Ps_curpage = 1
	else
		Ps_curpage = Ps_curpage - 1
	end
	Ps_updateShowList(resultList,Ps_curpage)
	Ps_setListboxData()
	PsList_setcurPage()
	PsList_resetVscrollbar()
end
-------------------------------------------------
--
function PsList_nextPage()
	if Ps_curpage >= Ps_totalPage then
		Ps_curpage = Ps_curpage
	else
		Ps_curpage = Ps_curpage + 1
	end
	Ps_updateShowList(resultList,Ps_curpage)
	Ps_setListboxData()
	PsList_setcurPage()
	PsList_resetVscrollbar()
end
-------------------------------------------------
--
function PsList_setcurPage()
	PsList_PageNum:setText(tostring(Ps_curpage) .."/" ..tostring(Ps_totalPage))
end
-------------------------------------------------
--
function PsList_VscrollPosChange()
	local NickVscrollbar = playerNickListbox:getVertScrollbar()
	local AccVscrollbar = playerAccListbox:getVertScrollbar()
	local LocatVscrollbar = playerLocatListbox:getVertScrollbar()
	--g_Loger:logError(NickVscrollbar:getName())
	local scrollbarPos = PsList_toVScrollbar:getScrollPosition()
	--g_Loger:logError(tostring(scrollbarPos))
	NickVscrollbar:setScrollPosition(scrollbarPos)
	AccVscrollbar:setScrollPosition(scrollbarPos)
	LocatVscrollbar:setScrollPosition(scrollbarPos)
end
-------------------------------------------------
--
function PsList_HscrollPosChange()
	local NickHscrollbar = playerNickListbox:getHorzScrollbar ()
	local AccHscrollbar = playerAccListbox:getHorzScrollbar ()
	local LocatHscrollbar = playerLocatListbox:getHorzScrollbar ()
	local scrollbarPos = PsList_toHScrollbar:getScrollPosition()
	NickHscrollbar:setScrollPosition(scrollbarPos)
	AccHscrollbar:setScrollPosition(scrollbarPos)
	LocatHscrollbar:setScrollPosition(scrollbarPos)
end
-------------------------------------------------
--
function PsList_setScrollProperty()
	local NickVscrollbar = playerNickListbox:getVertScrollbar()
	local doc_size = NickVscrollbar:getDocumentSize()
	local page_size = NickVscrollbar:getPageSize()
	local step_size = NickVscrollbar:getStepSize()
	PsList_setVscrollProperty(doc_size,page_size,step_size)
	--
	local NickHscrollbar = playerNickListbox:getHorzScrollbar ()
	local hnick_doc_size = NickHscrollbar:getDocumentSize()
	--g_Loger:logError("hnick_doc_size**" ..tostring(hnick_doc_size))
	local hnick_page_size = NickHscrollbar:getPageSize()
	--g_Loger:logError("hnick_page_size**" ..tostring(hnick_page_size))
	local hnick_step_size = NickHscrollbar:getStepSize()
	--g_Loger:logError("hnick_step_size**" ..tostring(hnick_step_size))
	--
	local AccHscrollbar = playerAccListbox:getHorzScrollbar ()
	local hacc_doc_size = AccHscrollbar:getDocumentSize()
	--g_Loger:logError("hacc_doc_size**" ..tostring(hacc_doc_size))
	local hacc_page_size = AccHscrollbar:getPageSize()
	--g_Loger:logError("hacc_page_size**" ..tostring(hacc_page_size))
	local hacc_step_size = AccHscrollbar:getStepSize()
	--g_Loger:logError("hacc_step_size**" ..tostring(hacc_step_size))

	--
	local LocatHscrollbar = playerLocatListbox:getHorzScrollbar ()
	local hlocat_doc_size = LocatHscrollbar:getDocumentSize()
	--g_Loger:logError("hlocat_doc_size**" ..tostring(hlocat_doc_size))
	local hlocat_page_size = LocatHscrollbar:getPageSize()
	--g_Loger:logError("hlocat_page_size**" ..tostring(hlocat_page_size))
	local hlocat_step_size = LocatHscrollbar:getStepSize()
	--g_Loger:logError("hlocat_step_size**" ..tostring(hlocat_step_size))
	--
	local h_doc_size = math.max(hnick_doc_size,math.max(hacc_doc_size,hlocat_doc_size))
		--g_Loger:logError("h_doc_size**" ..tostring(h_doc_size))
	local h_page_size = math.max(hnick_page_size,math.max(hacc_page_size,hlocat_page_size))
		--g_Loger:logError("h_page_size**" ..tostring(h_page_size))
	local h_step_size = math.max(hnick_step_size,math.max(hacc_step_size,hlocat_step_size))
		--g_Loger:logError("h_step_size**" ..tostring(h_step_size))
	if h_doc_size < h_page_size then
		--local temp_swap = h_doc_size
		--h_doc_size = 2*h_doc_size
		--h_page_size = temp_swap
	end
	--暂先写死数字
	PsList_setHscrollProperty(500,10,5)
end
--------------------------------------------------
--
function PsList_setVscrollProperty( docsize,pagesize,stepsize)
	PsList_toVScrollbar:setDocumentSize(docsize)
	PsList_toVScrollbar:setPageSize(pagesize)
	PsList_toVScrollbar:setStepSize(stepsize)
end
--------------------------------------------------
--
function PsList_setHscrollProperty( docsize,pagesize,stepsize)
	PsList_toHScrollbar:setDocumentSize(docsize)
	PsList_toHScrollbar:setPageSize(pagesize)
	PsList_toHScrollbar:setStepSize(stepsize)
end
--------------------------------------------------
--
function PsList_setListboxScrollbar_unVisible()
	local NickVscrollbar = playerNickListbox:getVertScrollbar()
	local AccVscrollbar = playerAccListbox:getVertScrollbar()
	local LocatVscrollbar = playerLocatListbox:getVertScrollbar()
	
	local NickHscrollbar = playerNickListbox:getHorzScrollbar()
	local AccHscrollbar = playerAccListbox:getHorzScrollbar()
	local LocatHscrollbar = playerLocatListbox:getHorzScrollbar()
	NickVscrollbar:setVisible(false)
	AccVscrollbar:setVisible(false)
	LocatVscrollbar:setVisible(false)
	
	NickHscrollbar:setVisible(false)
	AccHscrollbar:setVisible(false)
	LocatHscrollbar:setVisible(false)
end
--------------------------------------------------
--
function PsList_resetVscrollbar()
	PsList_toVScrollbar:setScrollPosition(0)
	PsList_toHScrollbar:setScrollPosition(0)
end