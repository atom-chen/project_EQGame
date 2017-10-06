-----------------------------------------------------------------
--邮件
--xiaoMing
--2010-08-31
-----------------------------------------------------------------
local mCurPage = 1
local g_mailBoxUI, g_mailUI
local mMailBoxFlag = false
local mMailFlag = false
local mWaringFlag = false
local mPageSize = 6
local mItemCount = 6
local mMaxPageSize = 20
local mCurMailIndex
local mFListName = "showRecverSearchResult"
local mFListWindow = nil
local mRecvManEditName = "mail/open_mailbox/Tab 2/receivemanedit"
local mRecvManEditActive = false
local mFListWindowActive = false
local mTxtChangedBySelFriend = false
local mShowFListName = "showFListButton"
local mShowFListBtn = nil
g_DoSelectMailRecver = false

--1 2 3 4 个人 GM 系统 退信
g_mailImageType = {new = {"mail_normal", "mail_gm",
							"mail_sys", "mail_return"},
				   readed = {"mail_normal_readed", "mail_gm_readed",
							"mail_sys_readed", "mail_return_readed"}
				}
-----------------------------------------------------------------
function OnloadMailUI()
	g_mailBoxUI = g_WinMgr:getWindow(g_mail.mailBox)
	g_mailUI = g_WinMgr:getWindow(g_mail.mail)

	g_WinMgr:getWindow(g_mail.closeBtn):subscribeEvent ("Clicked", "CloseMailBox")
	g_WinMgr:getWindow(g_mail.closeBtn2):subscribeEvent("Clicked", "CloseMail")
	g_WinMgr:getWindow(g_mail.sendBtn):subscribeEvent  ("Clicked", "IsSendMail")

	g_WinMgr:getWindow(g_mail.nextPage):subscribeEvent("Clicked", "NextPageMail")
	g_WinMgr:getWindow(g_mail.prePage):subscribeEvent ("Clicked", "PrePageMail")

	g_WinMgr:getWindow(g_mail.replyBtn):subscribeEvent ("Clicked", "ReplayMail")
	g_WinMgr:getWindow(g_mail.deleteBtn):subscribeEvent("Clicked", "IsDeleteMail")
	g_WinMgr:getWindow(g_mail.returnBtn):subscribeEvent("Clicked", "IsRetureMail")

	g_WinMgr:getWindow(g_mail.tab1Btn):subscribeEvent("MouseClick", "ShowInBox")

	for i = 1, mPageSize do
		g_WinMgr:getWindow(g_mail.mailSlot..i):subscribeEvent("MouseClick", "ReqMailContent")
		g_WinMgr:getWindow(g_mail.titleSlot..i):subscribeEvent("MouseClick", "ReqMailContent2")
	end

	for i = 1, mItemCount do
		win = g_WinMgr:getWindow(g_mail.sendSlot..i)
		win:subscribeEvent("MouseClick", "OnClickSendItemGrid")
		win = g_WinMgr:getWindow(g_mail.sendSlot..i)
		win:subscribeEvent("MouseEnter", "SendItemGrid_MouseEnter")
		win = g_WinMgr:getWindow(g_mail.sendSlot..i)
		win:subscribeEvent("MouseLeave", "SendItemGrid_MouseLeave")

		win = g_WinMgr:getWindow(g_mail.recSlot..i)
		win:subscribeEvent("MouseClick", "OnClickRecItemGrid")
		win = g_WinMgr:getWindow(g_mail.recSlot..i)
		win:subscribeEvent("MouseEnter", "RecvItemGrid_MouseEnter")
		win = g_WinMgr:getWindow(g_mail.recSlot..i)
		win:subscribeEvent("MouseLeave", "RecvItemGrid_MouseLeave")
	end

	g_WinMgr:getWindow(g_mail.waringCloseBtn):subscribeEvent("Clicked", "CloseWaringUI")
	g_WinMgr:getWindow(g_mail.waringSureBtn):subscribeEvent("Clicked", "CloseWaringUI")

	local window = g_WinMgr:getWindow(g_mail.sendSubject)
	CEGUI.toEditbox(window):setMaxTextLength(12)

	window = g_WinMgr:getWindow(g_mail.addressee)
	CEGUI.toEditbox(window):setMaxTextLength(32)

	window = g_WinMgr:getWindow(g_mail.sendContent)
	CEGUI.toMultiLineEditbox(window):setMaxTextLength(256)

	MainUI_ShieldClick(g_mailBoxUI)
	MainUI_ShieldClick(g_mailUI)
	
	--创建收件人列表框
	mFListWindow = g_WinMgr:createWindow("NewEQHomeLook/NewComboDropList",mFListName)
	mFListWindow:setVisible(false)
	mFListWindow:setPosition(CEGUI.UVector2(CEGUI.UDim(0,60), CEGUI.UDim(0,25)))
	mFListWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,183), CEGUI.UDim(0,140)))
	mFListWindow:setProperty("AlwaysOnTop", "true")
	g_WinMgr:getWindow("mail/open_mailbox/Tab 2"):addChildWindow(mFListWindow)
	g_WinMgr:getWindow(mRecvManEditName):subscribeEvent("TextChanged", "OnInputReceiveManChar")
	g_WinMgr:getWindow(mRecvManEditName):subscribeEvent("Activated", "OnInputReceiveManActivated")
	g_WinMgr:getWindow(mRecvManEditName):subscribeEvent("Deactivated", "OnInputReceiveManDeactivated")
	mFListWindow:subscribeEvent("Activated", "OnFLWindowActivated")
	mFListWindow:subscribeEvent("Deactivated", "OnFLWindowDeactivated")
	mFListWindow:subscribeEvent("MouseClick", "OnFLWindowSelected")
	CEGUI.toComboDropList(mFListWindow):setAutoArmEnabled(true)
	--创建临时弹出好友列表按钮
	--mShowFListBtn = g_WinMgr:createWindow("NewEQHomeLook/NewStaticImage3",mShowFListName)
	--mShowFListBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0,220), CEGUI.UDim(0,5)))
	--mShowFListBtn:setSize(CEGUI.UVector2(CEGUI.UDim(0,20), CEGUI.UDim(0,22)))
	--mShowFListBtn:setProperty("AlwaysOnTop", "true")
	--g_WinMgr:getWindow("mail/open_mailbox/Tab 2"):addChildWindow(mShowFListBtn)
	mShowFListBtn = g_WinMgr:getWindow("mail/open_mailbox/Tab 2/button")
	mShowFListBtn:subscribeEvent("MouseClick", "OnSelectFriendFromList")
end

function OnSelectFriendFromList(args)
	if false == g_DoSelectMailRecver then
		g_DoSelectMailRecver = true
		DoOpenFriendListUI()
	else
		DoCloseFriendListUI()
		g_DoSelectMailRecver = false
	end
end

--在好友列表中选择一个好友
function Mail_OnSelectFriendChanged(name)
	if false == g_DoSelectMailRecver then
		return
	end
	mTxtChangedBySelFriend = true
	EQGlobaFunc.SetText(mRecvManEditName, name)
end

function OnInputReceiveManActivated(args)
	mRecvManEditActive = true
end

function OnInputReceiveManDeactivated(args)
	mRecvManEditActive = false
	HideFLWindow()
end

function OnFLWindowActivated(args)
	mFListWindowActive = true
end

function OnFLWindowDeactivated(args)
	mFListWindowActive = false
	HideFLWindow()
end

--收件人字符串更改
function OnInputReceiveManChar(args)
	--跳过非手动更改
	if mTxtChangedBySelFriend then
		mTxtChangedBySelFriend = false
		return
	end
	
	local txt = g_WinMgr:getWindow(mRecvManEditName):getText()
	
	CEGUI.toListbox(mFListWindow):resetList()
	
	if "" == txt then
		mFListWindow:setVisible(false)
		return
	end
	
	mFListWindow:setVisible(true)
	
	FillFList(txt)
end

--在收件人搜索列表中选择一个收件人
function OnFLWindowSelected(args)
	local lbox = CEGUI.toListbox(mFListWindow)
	local item = lbox:getFirstSelectedItem()
	if item then
		EQGlobaFunc.SetText(mRecvManEditName, item:getText())
	end
	mFListWindow:setVisible(false)
end

function HideFLWindow()
	if mRecvManEditActive or mFListWindowActive then
		return
	end
	
	mFListWindow:setVisible(false)
end

--根据关键字填充收件人列表
function FillFList(key)
	g_FriendSys:searchEx(key)
	
	local i
	local cnt = g_FriendSys:getSearchExCnt()
	
	if 0 == cnt then
		mFListWindow:setVisible(false)
		return
	end
	
	for i = 1, cnt do
		EQGlobaFunc.createListboxTextItem(mFListWindow:getName(), g_FriendSys:getSearchExResult(i - 1), i)
	end
end
-----------------------------------------------------------------
--打开邮箱
function OpenMailBox()	
	AddMailBox()
	Initial()	
	ShowInBox()
end
-----------------------------------------------------------------
function AddMailBox()
	OpenMailBoxUI()
end

function OpenMailBoxUI()
	UIM_OpenWindow(g_mail.mailBox)
	SetWindowCenter(g_mailBoxUI)
	mMailBoxFlag = true
	PlayerGuide_do_NewerTeachTaskScript(15007) --新手教学任务
end

function CloseMailBoxUI()
	UIM_CloseWindow(g_mail.mailBox)
	mMailBoxFlag = false
	g_DoSelectMailRecver = false
end
---------------------------------------
--
function Mail_clickNewMailPaoPao()
	SwitchMailBox()
end
-----------------------------------------------------------------
function SwitchMailBox()
	if false == mMailBoxFlag then
		OpenMailBox()
		UIM_DoOpenAndCloseUI(g_mail.mailBox)
		do_task_script("script13")	-- (打开邮箱界面)执行新手任务脚本13(2011/6/15 add by hezhr)
	else
		CloseMailBox()
	end
end
-----------------------------------------------------------------
function ShowInBox()

	if true == g_mailManager.mReqMailListFlag then
		UpdateMailPage()
	else
		g_mailManager:reqMailList()
	end
	
	if true == g_mailManager:isFull() then
		MsgBox_createMsgBox(g_MainUIWindows, 10004, "", "你有新的邮件，但是收件箱已满，请清理.", "MsgBox_cancelFunc", "")
	end
	
	CEGUI.toTabControl(g_WinMgr:getWindow(g_mail.tabName)):setSelectedTabAtIndex(0)
end
-----------------------------------------------------------------
--打开发件箱
function OpenOutBox()
	if false == mMailBoxFlag then
		OpenMailBox()
	else
		Initial()
	end
	
	ShowOutBox()
end
-----------------------------------------------------------------
function ShowOutBox()
	CEGUI.toTabControl(g_WinMgr:getWindow(g_mail.tabName)):setSelectedTabAtIndex(1)
end
-----------------------------------------------------------------
function Initial()
	ClearOutBox()
	mCurPage = 1
end
-----------------------------------------------------------------
function ClearOutBox()
	g_mailManager:clearSendMail()
	ClearMailSlot(g_mail.sendSlot)
	
	g_WinMgr:getWindow(g_mail.addressee):setText("")
	g_WinMgr:getWindow(g_mail.sendContent):setText("")
	g_WinMgr:getWindow(g_mail.sendSubject):setText("")
end
-----------------------------------------------------------------
function ReqMailContent(args)
	local windowName = CEGUI.toWindowEventArgs(args).window:getName()
	local index = GetClickWindowIndex(windowName, g_mail.mailSlot, 1, mPageSize)
	mCurMailIndex = index
	g_mailManager:reqMailContent(mCurPage, index)
end
-----------------------------------------------------------------
function ReqMailContent2(args)
	local windowName = CEGUI.toWindowEventArgs(args).window:getName()
	local index = GetClickWindowIndex(windowName, g_mail.titleSlot, 1, mPageSize)
	mCurMailIndex = index
	g_mailManager:reqMailContent(mCurPage, index)
end
-----------------------------------------------------------------
function OpenMail()
	if false == mMailFlag then
		g_MainUIWindows:addChildWindow(g_mailUI)
		SetWindowCenter(g_mailUI)
		mMailFlag = true
	end
	--g_mailUI:setProperty("AlwaysOnTop", "true")
end
-----------------------------------------------------------------
function SetMail(mail)
	OpenMail()

	FSetText(g_mail.sender, mail.account)

	if "" == mail.title then
		FSetText(g_mail.recSubject, "无标题")
	else
		FSetText(g_mail.recSubject, mail.title)
	end

	FSetText(g_mail.recContent, mail.content)

	UpdateMailItem()
end
-----------------------------------------------------------------
--邮件图标变已读
function ChangeMailIcon(mailType)
	--改变该邮件在邮件列表的显示
	local slotWin = g_WinMgr:getWindow(g_mail.mailSlot..mCurMailIndex)
	local TempSource = slotWin:getChildAtIdx(0)
	slotWin:removeChildWindow(TempSource)

	local imageName = g_mailImageType.readed[mailType]
	StorageUI_DynamicLoadItem(g_mail.mailSlot..mCurMailIndex,
							"newImg",
							mCurMailIndex,
							"common1",
							imageName, "False")
end
-----------------------------------------------------------------
function UpdateMailItem()
	ClearMailSlot(g_mail.recSlot)

	local item
	local count = g_mailManager:getCurMailItemCount()

	for i = 0, count - 1 do
		item = g_mailManager:getCurMailItem(i)
		if nil ~= item then
			SetMailItem(g_mail.recSlot..(i + 1), i + 1, item.templateId, item.count)
		end
	end
end
-----------------------------------------------------------------
function CloseMailBox()
	--解锁
	g_mailManager:clearSendMail()
	RemoveMailBoxUI()
end
-----------------------------------------------------------------
function RemoveMailBoxUI()
	CloseMailBoxUI()

	if true == mMailFlag then
		CloseMail()
	end
end
-----------------------------------------------------------------
function SuccessSendMail()
	MsgBox_createMsgBox(g_MainUIWindows,10003,"","邮件发送成功！", "MsgBox_cancelFunc", "")
	RemoveMailBoxUI()
end

-----------------------------------------------------------------
function CloseMail()
	g_MainUIWindows:removeChildWindow(g_mailUI)
	mMailFlag = false
	--g_mailUI:setProperty("AlwaysOnTop", "false")
end
-----------------------------------------------------------------
--从混合字符串中获取帐号
function GetAccFromCStr(acc)
	--帐号查找
	local s = string.find(acc, "%(")
	local e = string.find(acc, "%)")
	if s and e then
		acc = string.sub(acc, s + 1, e - 1)
	end
	
	g_Loger:logError("SendMail: "..acc)
	return acc
end

function SendMail()
	local acc = g_WinMgr:getWindow(g_mail.addressee):getText()
	local contents = g_WinMgr:getWindow(g_mail.sendContent):getText()
	local title = g_WinMgr:getWindow(g_mail.sendSubject):getText()
	
	acc = GetAccFromCStr(acc)
	
	--g_Loger:logError("acc: "..acc)
	g_mailManager:sendMail(acc, title, contents)
	PlaySpecialVoice(g_special_voice_map.send_mail)
end
-----------------------------------------------------------------
function IsSendMail()
	local name = g_WinMgr:getWindow(g_mail.addressee):getText()

	if "" == name or nil == name then
		--报错
		MsgBox_createMsgBox(g_MainUIWindows,10005,"","请输入名字.", "MsgBox_cancelFunc", "")
		return
	end

	local contents = g_WinMgr:getWindow(g_mail.sendContent):getText()
	local title = g_WinMgr:getWindow(g_mail.sendSubject):getText()

	--内容为空自带一回车
	if("\n" == contents and "" == title and false == g_mailManager:sendMailHaveItem()) then
		MsgBox_createMsgBox(g_MainUIWindows,10008,"","邮件为空，无法发送。", "MsgBox_cancelFunc", "")
		return
	end

	if true == g_mailManager:sendMailHaveItem() then
		MsgBox_createMsgBox(g_MainUIWindows,10000,"","此邮件中包含物品，是否继续发送。", "SendMail", "MsgBox_cancelFunc")
	else
		SendMail()
	end
end
-----------------------------------------------------------------
function IsDeleteMail()
	if true == g_mailManager:curMailHaveItem() then
		MsgBox_createMsgBox(g_MainUIWindows,10001,"","此邮件中包含物品，是否真要删除。", "DeleteMail", "MsgBox_cancelFunc")
	else
		DeleteMail()
	end
end
-----------------------------------------------------------------
function DeleteMail()
	g_mailManager:reqDeleteMail()
end
-----------------------------------------------------------------
function ReplayMail()
	type = g_mailManager:getCurMailType()

	if 2 == type or 3 == type then
		MsgBox_createMsgBox(g_MainUIWindows,10006,"","此邮件无法回复.", "MsgBox_cancelFunc", "")
	else
		CEGUI.toTabControl(g_WinMgr:getWindow(g_mail.tabName)):setSelectedTabAtIndex(1)

		local mail = g_mailManager:getCurMail()
		FSetText(g_mail.addressee , mail.account)

		if("" == mail.title) then
			FSetText(g_mail.sendSubject , "re:无标题")
		else
			FSetText(g_mail.sendSubject , "re:"..mail.title)
		end
		--设置帐号
		--g_mailManager:setReturnAcc()
	end
	CloseMail()
end
-----------------------------------------------------------------
function IsRetureMail()
	g_DragItem:undo()
	if true == g_mailManager:curMailHaveItem() then
		MsgBox_createMsgBox(g_MainUIWindows,10002,"","此邮件中包含物品，是否真要退信？", "ReturnMail", "MsgBox_cancelFunc")
	else
		ReturnMail()
	end
end
-----------------------------------------------------------------
function ReturnMail()
	type = g_mailManager:getCurMailType()
	if 1 == type then
		g_mailManager:reqReturnMail()
	else
		MsgBox_createMsgBox(g_MainUIWindows,10007,"","此邮件无法退回.", "MsgBox_cancelFunc", "")
	end
end
-----------------------------------------------------------------
function OnClickSendItemGrid(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_mail.sendSlot, 1, mItemCount)
	Storage_ClickCloseEquipTips()	-- 隐藏装备提示(hezhr)
	index = index - 1
	local indexItemID = g_mailManager:getSendMailItemId(index)
	--区分左键点击和右键点击 0 为左键 1为右键
	local mbtn = CEGUI.toMouseEventArgs(args).button

	if(g_DragItem:isDraging()) then
		--仓库拖到发件箱
		if g_PT_WAREHOUSE == g_DragItem:getPacktype() then
				
			--若为拆分状态提示无法拆分
			if g_DragItem:getSplitNum() > 0 then
				MsgBox_createMsgBox(g_MainUIWindows,10601,"","无法这样拆分物品.", "MsgBox_cancelFunc", "")
				return
			end
			
			local dragIndex = g_DragItem:getIndex()
			g_mailManager:setSendMailItem(index, dragIndex)
			g_DragItem:undo()
			UpdateSendItem()
		--发件箱拖到发件箱
		elseif g_PT_OUTBOX == g_DragItem:getPacktype() then
			if index == g_DragItem:getIndex() then
				g_DragItem:undo()
				return
			else
				g_mailManager:swapItem(index, g_DragItem:getIndex())
				g_DragItem:undo()
				UpdateSendItem()
			end
		elseif g_PT_STORAGE == g_DragItem:getPacktype() then
			--若为拆分状态提示无法拆分
			if g_DragItem:getSplitNum() > 0 then
				MsgBox_createMsgBox(g_MainUIWindows,10601,"","无法这样拆分物品.", "MsgBox_cancelFunc", "")	
			else
				MsgBox_createMsgBox(g_MainUIWindows,10602,"","无法直接将仓库中的物品进行交易.", "MsgBox_cancelFunc", "")	
			end
				
			return
		end
	elseif -1 ~= indexItemID then--点击的格子有物品
		--	超链接(hezhr)
		if g_CS_HYPERLINK == g_CursorMgr:getState() then
			ClickHyperLinkItem(indexItemID)
			return
		end
		
		if 0 == mbtn then
			g_DragItem:drag(winName,index,g_PT_OUTBOX)
			g_dratItemID = indexItemID	--保存吸起来的物品模板id(hezhr)
		elseif 1 == mbtn then
			g_mailManager:backSendItem(index, 1)
			UpdateSendItem()
		end
	end
end
-----------------------------------------------------------------
function SetMailItem(winName, index, templateId, count)
		local itemrow = g_itemtableMgr:getItemRow(templateId)
		if nil ~= itemrow then
			StorageUI_DynamicLoadItem(winName,
									"newImg",
									index,
									itemrow.imageset,
									itemrow.icon, "False",count)
		end
end
-----------------------------------------------------------------
--
function UpdateSendItem()
	ClearMailSlot(g_mail.sendSlot)
	local id, count
	for index = 0, 5 do
		id = g_mailManager:getSendMailItemId(index)
		count = g_mailManager:getSendMailItemCount(index)
		SetMailItem(g_mail.sendSlot..(index + 1), index + 1, id, count)
	end
end
-----------------------------------------------------------------
function ClearMailSlot(winName)
	local slotWin
	for index = 1, mPageSize do
		slotWin = g_WinMgr:getWindow(winName..index)
		if slotWin ~= nil then
			if slotWin:getChildCount() > 0 then
				local TempSource = slotWin:getChildAtIdx(0)
				slotWin:removeChildWindow(TempSource)
			end
		end
	end
end
-----------------------------------------------------------------
function ClearInBoxPage()
	ClearMailSlot(g_mail.mailSlot)

	for index = 1, mPageSize do
		FSetText(g_mail.titleSlot..index.."/name"..index, "")
		FSetText(g_mail.titleSlot..index.."/title"..index, "")
		FSetText(g_mail.titleSlot..index.."/date"..index, "")
	end
end
-----------------------------------------------------------------
function ClearMailBox()
	ClearInBoxPage()
	mCurPage = 1
	--是否要清空当前邮件 待定
end
-----------------------------------------------------------------
--更新发件箱页面
function UpdateMailPage()
	ClearInBoxPage()
	local startIndex = ( mCurPage - 1 )* mPageSize
	local endIndex = g_mailManager:getPageLastIndex(mCurPage)
	g_mailManager:reqKnowMailList(startIndex, endIndex)
	FUpdatePageInfo(g_mail.pageText, mCurPage, g_mailManager:getTotalPage())

	if true == g_mailManager:isAnimating() and true == g_mailManager:isAllReaded() then
		g_mailManager:removeAnimation()
	end
end
-----------------------------------------------------------------
function SetMailGrid(gridIndex, mail)
	local imageName
	if 3 == mail.read_status then
		imageName = g_mailImageType.readed[mail.type]
	else
		imageName =	g_mailImageType.new[mail.type]
	end

	StorageUI_DynamicLoadItem(g_mail.mailSlot..gridIndex,"newImg",gridIndex,"common1",imageName, "False")

	slotWin = g_WinMgr:getWindow(g_mail.mailSlot..gridIndex)

	FSetText(g_mail.titleSlot..gridIndex.."/name"..gridIndex, mail.name)

	if "" == mail.title then
		FSetText(g_mail.titleSlot..gridIndex.."/title"..gridIndex, "无标题")
	elseif(30 < string.len(mail.title)) then
		local str = string.sub(mail.title, 1, 30)
		FSetText(g_mail.titleSlot..gridIndex.."/title"..gridIndex, str.."...")
	else
		FSetText(g_mail.titleSlot..gridIndex.."/title"..gridIndex, mail.title)
	end

	if 0 ~= mail.remain_time then
		FSetText(g_mail.titleSlot..gridIndex.."/date"..gridIndex, tostring(math.floor(mail.remain_time/(24 * 60 * 60))).."天")
	end
end
-----------------------------------------------------------------
function NextPageMail()
	local totalPage = g_mailManager:getTotalPage()
	if mCurPage >= totalPage or mCurPage >= mMaxPageSize then
		return
	end

	mCurPage = mCurPage + 1
	UpdateMailPage()
end
-----------------------------------------------------------------
function PrePageMail()
	if 1 >= mCurPage then
		return
	end

	mCurPage = mCurPage - 1
	UpdateMailPage()
end
-----------------------------------------------------------------
function OnClickRecItemGrid(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_mail.recSlot, 1, mItemCount)

	index = index - 1
	local item = g_mailManager:getCurMailItem(index)

	if nil == item then
		return
	end
	--	超链接(hezhr)
	if g_CS_HYPERLINK == g_CursorMgr:getState() then
		ClickHyperLinkItem(item.templateId)
		return
	end
	g_mailManager:reqGetMailItem(index)
end
-----------------------------------------------------------------
function OpenInBox()
	if true == g_mailManager.mReqMailListFlag then
		UpdateMailPage()
	else
		g_mailManager:reqMailList()
	end
end
-----------------------------------------------------------------
function SubCurPage()
	if 1 < mCurPage then
		mCurPage = mCurPage - 1
	end
end
-----------------------------------------------------------------
-- 打开发送附件栏itemhint(hezhr)
function SendItemGrid_MouseEnter(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_mail.sendSlot, 1, mItemCount)
	index = index - 1
	local indexItemID = g_mailManager:getSendMailItemId(index)

	if -1 ~= indexItemID then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_mailManager:getSendMailItemEffectTime(index)
		createItemhint_shop(indexItemID, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
-----------------------------------------------------------------
-- 关闭发送附件栏itemhint(hezhr)
function SendItemGrid_MouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
-----------------------------------------------------------------
-- 打开接收附件栏itemhint(hezhr)
function RecvItemGrid_MouseEnter(args)
	local winName = CEGUI.toWindowEventArgs(args).window:getName()
	local index =	GetClickWindowIndex(winName, g_mail.recSlot, 1, mItemCount)
	index = index - 1
	local item = g_mailManager:getCurMailItem(index)

	if nil ~= item then
		local rect = CEGUI.toWindowEventArgs(args).window:getPixelRect()
		--时效相关
		local effectTime = g_mailManager:getCurMailItemEffectTime(index)
		createItemhint_shop(item.templateId, rect, g_MainUIWindows, effectTime, 1, nil)
	end
end
-----------------------------------------------------------------
-- 关闭接收附件栏itemhint(hezhr)
function RecvItemGrid_MouseLeave(args)
	destroyItemhint(g_MainUIWindows)
end
-----------------------------------------------------------------
-- 清除指定物品窗口上的子窗口(hezhr)
function MailClearItem(index)
	slotWinname = g_mail.sendSlot..index
	slotWin = g_WinMgr:getWindow(slotWinname)
	if nil ~= slotWin then
		if slotWin:getChildCount() > 0 then
			local TempSource = slotWin:getChildAtIdx(0)
			slotWin:removeChildWindow(TempSource)
			destroyItemhint(g_MainUIWindows)
		end
	end
end
-----------------------------------------------------------------
-- 处理邮件中物品的时效(hezhr)
function handleEffTimeInMail()
	if true == mMailBoxFlag then
		g_mailManager:updateItems()
	end
end