-----------------------------------------
-- 文件说明	邀请流程
-- 作者		：xiaoMing
-- 日期		：10/04/
-----------------------------------------
local mInviteMsgBoxName = ""

function InviteDialogOnload()
	g_GameEventMgr:subscribeEvent("SHOW_INVITE_DIALOG", "PopInviteDialog")
	g_GameEventMgr:subscribeEvent("UNSHOW_INVITE_DIALOG", "DeleteInviteDialog")
end


-- 弹出邀请对话框
function PopInviteDialog()
	local name = g_smallModules:getInvitee()
	local type = g_smallModules:getInviteType()
	local str = ""
	
	if 1 == type then
		str = ""
	--邀请交易
		str = "]邀请您进行交易"
	elseif 2 == type then
	--邀请来我家
		str = "]邀请您去他(她)家"
	elseif 3 == type then
		str = "]请求加您为好友"
	end

	mInviteMsgBoxName = MsgBox_CreateMsgBoxC(g_MainUIWindows, "", "AgreeInvite", "DisagreeInvite")
	local txtWin = MsgBox_GetMsgBoxCMultiLineBox()
	txtWin:setCols("#FFFFFF")
	txtWin:clearUp()
	txtWin:addElementText("[", false)
	txtWin:addElementHyperLink(name, "", "#FF9C00", false)
	txtWin:addElementText(str, false)
	g_WinMgr:getWindow(txtWin:getName()):subscribeEvent("HyperLink_MouseClick", "HL_showPlayerInfo")
end

--邀请人取消邀请请求后删除邀请对话框
function DeleteInviteDialog()
	if false == g_WinMgr:isWindowPresent(mInviteMsgBoxName) then
		return
	end
	
	local iwin = g_WinMgr:getWindow(mInviteMsgBoxName)
	if nil == iwin then
		return
	end
	
	local pwin = iwin:getParent()
	if nil == pwin then
		return
	end
	
	Common_setModalState(iwin:getName(), false)
	pwin:removeChildWindow(iwin)
end

--同意邀请
function AgreeInvite()
	g_smallModules:agreeInvite()
end

--拒绝邀请
function DisagreeInvite()
	g_smallModules:disagreeInvite()
end

function HL_showPlayerInfo()
	local invitor = g_smallModules:getInvitorAcc()
	if invitor ~= "" then
		g_req_other_player_info.account = invitor
		g_msgmgr:send(g_req_other_player_info)
		PopOtherPlayerInfo()
	end
end