--好友列表界面相关
MainUI_ShieldClick(g_FriendListUI)
MainUI_ShieldClick(g_FriendLstpopUpUI)
MainUI_ShieldClick(g_GMFriendpopUpUI)

local friend_search_win = g_WinMgr:getWindow("freindlist/searchbox")

friend_search_win:subscribeEvent("TextChanged", "FriendList_sTxt_change")
g_GameEventMgr:subscribeEvent("UPDATE_SEARCHKEY","FriendList_search_update")
g_WinMgr:getWindow("freindlist/sys_msg"):subscribeEvent("Clicked", "FLofflineMsgMgr_openOfflineMsgUI")

function FriendList_sTxt_change(args)
	local search_win = CEGUI.toWindowEventArgs(args).window
	local sTxt = search_win:getText()
	
	g_FriendSys:search_matchKey(sTxt, 1)
end
----------------------------------------------------------------
--
function FriendList_search_update()

end
-----------------------------------------------------------------
--
function FriendList_setGMpopWinDisable(flag)

end
--
function FriendList_setBtnEnabledInGmHouse(flag)
	g_WinMgr:getWindow("MainRoot/telephone"):setVisible(flag)
	g_WinMgr:getWindow("MainRoot/liwu"):setVisible(flag)
	
	if g_WinMgr:getWindow("DFwnd_MainRoot/liwuanjian"):isVisible() then
		g_WinMgr:getWindow("DFwnd_MainRoot/liwuanjian"):setEnabled(flag)
	end
end