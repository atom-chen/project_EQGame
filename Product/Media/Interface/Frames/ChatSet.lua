-----------------------------------------
--  文件说明：处理聊天相关的逻辑
-----------------------------------------

-----------------------------------------
local m_menuBar = g_WinMgr:getWindow("MainRoot/SImg_HMenubar1")
local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
local zoomFlag = 0	-- 缩放标志，初始为小窗口=0,大窗口=1(hezhr)
local scrollFlag = true -- 滚动条默认标志，处在最下面
local limitLines = 50	-- 做多显示的行数
local limitFlag = false	-- 开始控制行数标志
local m_smallWidth, m_smallHeight = 431, 133	-- 聊天框小窗口宽、高(7行)
local m_bigWidth, m_bigHeight = 435, 190	-- 聊天框大窗口宽、高(10行)
local m_zoomWidth, m_zoomHeight = 16, 16	-- 缩放窗口宽、高
local m_animateWin = nil	-- 消息提示动画窗口
local m_animate = EQGlobaClass.createAnimate()	-- 消息提示动画
-----------------------------------------
-- 获取聊天输出窗口，此函数放在文件最前面(hezhr)
-----------------------------------------
function getChatBox()
	local chatBoxName = "MainRoot/MTxt_ShowChatbox"
	local chatBox = g_WinMgr:getWindow(chatBoxName)
	if nil == chatBox then
		chatBox = g_WinMgr:createWindow("EQHomeLook/MultiLineBox3", chatBoxName)
		EQGlobaFunc.toMultiLineBox(chatBoxName):setFont("simsun-10")	-- 设置字体：新宋10号
		EQGlobaFunc.toMultiLineBox(chatBoxName):setCols("#FFFFFF")	-- 设置颜色：白色
		EQGlobaFunc.toMultiLineBox(chatBoxName):setSize(m_smallWidth, m_smallHeight)	-- 设置初始大小
		EQGlobaFunc.toMultiLineBox(chatBoxName):setScrollbarVisible("SP_OLD", true, 1)	-- 设置滚动条
		EQGlobaFunc.toMultiLineBox(chatBoxName):setEachLineHeight(16)	-- 强制设置每行高度
		EQGlobaFunc.toMultiLineBox(chatBoxName):clearUp()
		chatBox:subscribeEvent("HyperLink_MouseClick", "ShowChatBox_GetHyperLink")
		chatBox:subscribeEvent("HyperLink_MouseEnter", "ShowChatBox_OpenHyperLink")
		chatBox:subscribeEvent("HyperLink_MouseLeave", "ShowChatBox_CloseHyperLink")
		chatBox:subscribeEvent("Text_MouseClick", "SpeedPrivateChat_MouseClick")
		g_MainUIWindows:addChildWindow(chatBox)
	end
	return chatBox
end
local chatShowBox = getChatBox()	-- 聊天输出框
-----------------------------------------
-- 获取聊天输出缩放按钮，此函数放在文件最前面(hezhr)
-----------------------------------------
function getChatBox_ZoomButton()
	local zoomButtonName = "MainRoot/ZoomButton"
	local zoomButton = g_WinMgr:getWindow(zoomButtonName)
	if nil == zoomButton then
		zoomButton = g_WinMgr:createWindow("EQHomeLook/ImageButton", zoomButtonName)
		zoomButton:setSize(CEGUI.UVector2(CEGUI.UDim(0, m_zoomWidth), CEGUI.UDim(0, m_zoomHeight)))
		zoomButton:setProperty("NormalImage", "set:Button6 image:NewImage127")
		zoomButton:setProperty("HoverImage", "set:Button6 image:NewImage128")
		zoomButton:setProperty("PushedImage", "set:Button6 image:NewImage127")
		--chatShowBox:addChildWindow(zoomButton)
		zoomButton:setPosition(CEGUI.UVector2(CEGUI.UDim(1, -m_zoomWidth), CEGUI.UDim(0, 0)))
	end
	return zoomButton
end
local zoomButton = getChatBox_ZoomButton()	-- 缩放按钮
-----------------------------------------
-- 有消息时，提示动画(hezhr)
-----------------------------------------
function ChatSet_MsgAnimate(animate)
	animate:initAnimate(100, 28, 25)
	animate:addAnimateFrame("Button5", "NewImage73")
	animate:addAnimateFrame("Button5", "NewImage74")
	animate:addAnimateFrame("Button5", "NewImage75")

	m_animateWin = g_WinMgr:getWindow("ChatSet/MsgAnimate")
	if nil == m_animateWin then
		m_animateWin = g_WinMgr:createWindow("EQHomeLook/AnimateBox", "ChatSet/MsgAnimate")		
		m_animateWin:setSize(CEGUI.UVector2(CEGUI.UDim(0, 28), CEGUI.UDim(0, 25)))
		EQGlobaFunc.toAnimateBox(m_animateWin:getName()):addAnimate(animate)
		m_animateWin:subscribeEvent("MouseButtonDown", "ChatSet_ClickMsgAnimate")
	end
	m_animateWin:setVisible(false)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):addChildWindow(m_animateWin)	
end
-- 该调用放在函数ChatSet_MsgAnimate()定义后面
ChatSet_MsgAnimate(m_animate)
--坐标相关
local m_chatBox_y = m_menuBar:getHeight():asAbsolute(screenH) + m_smallHeight - 5
chatShowBox:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 1), CEGUI.UDim(1, -m_chatBox_y)))
--屏蔽处理
MainUI_ShieldClick(chatShowBox)
MainUI_ShieldClick(m_menuBar)
--事件注册
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_Enter"):subscribeEvent("Clicked", "ChatSet_addChat")
g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):subscribeEvent("Clicked", "ChatSet_OnHideChatWindow")
g_WinMgr:getWindow("MainRoot/MTxt_ShowChatbox"):subscribeEvent("MouseClick", "ChatBox_Onclicked")	--(hezhr)
zoomButton:subscribeEvent("Clicked", "zoomChatBoxSize")	--(hezhr)
-----------------------------------------
-- 点击聊天框(hezhr)
-----------------------------------------
function ChatBox_Onclicked(args)
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
end
-----------------------------------------
--  显示聊天窗口
-----------------------------------------
function showChatShowBox()
	chatShowBox:setVisible(true)
	local speaker, pare = createBECtrl()
	speaker:setVisible(true)
	g_MainUIWindows:addChildWindow(pare)
	pare:setID(1)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):setProperty("NormalImage","set:Button6 image:NewImage111")
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):setProperty("HoverImage","set:Button6 image:NewImage113")
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):setProperty("PushedImage","set:Button6 image:NewImage113")	
end
-----------------------------------------
--  隐藏聊天窗口
-----------------------------------------
function hideChatShowBox()
	chatShowBox:setVisible(false)
	local speaker, pare = createBECtrl()
	speaker:setVisible(false)
	g_MainUIWindows:removeChildWindow(pare)
	pare:setID(0)
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):setProperty("NormalImage","set:Button6 image:NewImage112")
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):setProperty("HoverImage","set:Button6 image:NewImage114")
	g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):setProperty("PushedImage","set:Button6 image:NewImage114")	
end
-----------------------------------------
--  隐藏显示聊天窗口
-----------------------------------------
function ChatSet_OnHideChatWindow(args)
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	if false == chatShowBox:isVisible() then
		showChatShowBox()
	else
		hideChatShowBox()
	end
end
-----------------------------------------
-- 点击消息提示动画
-----------------------------------------
function ChatSet_ClickMsgAnimate(args)
	if 1 == CEGUI.toMouseEventArgs(args).button then	-- 屏蔽鼠标右键
		return
	end
	m_animateWin:setVisible(false)
	EQGlobaFunc.toAnimateBox(m_animateWin:getName()):stop()
	showChatShowBox()
end
-----------------------------------------
--	大窗口
-----------------------------------------
function bigChatShowBox()
	chatShowBox:setSize(CEGUI.UVector2(CEGUI.UDim(0, m_bigWidth), CEGUI.UDim(0, m_bigHeight)))
	m_chatBox_y = m_menuBar:getHeight():asAbsolute(screenH) + m_bigHeight - 5
	chatShowBox:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 1), CEGUI.UDim(1, -m_chatBox_y)))
	local speaker, pare = createBECtrl()
	BE_setPosition(speaker, pare)
end
-----------------------------------------
--	小窗口
-----------------------------------------
function smallChatShowBox()
	chatShowBox:setSize(CEGUI.UVector2(CEGUI.UDim(0, m_smallWidth), CEGUI.UDim(0, m_smallHeight)))
	m_chatBox_y = m_menuBar:getHeight():asAbsolute(screenH) + m_smallHeight - 5
	chatShowBox:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 1), CEGUI.UDim(1, -m_chatBox_y)))
	local speaker, pare = createBECtrl()
	BE_setPosition(speaker, pare)
end
-----------------------------------------
--改变聊天窗的尺寸(hezhr)
-----------------------------------------
function zoomChatBoxSize(args)
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	local renderHeight = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getRenderHeight()
	local scrollSize = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getScrollbarDocumentSize()
	local scrollPos = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getScrollbarPosition()
	if 0 == zoomFlag then
		bigChatShowBox()
		zoomFlag = 1
	else
		smallChatShowBox()
		zoomFlag = 0
	end
	if scrollPos + renderHeight == scrollSize then
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):setScrollbarPosition(scrollSize)
	end	
	OnZoomView()
end
-----------------------------------------
--控制行数(hezhr)
-----------------------------------------
function ChatShowBox_LimitLines(win, num)
	local lines = EQGlobaFunc.toMultiLineBox(win:getName()):getLineSize()
	local limitH = 0

	if num <= lines then
		local oldHeight = EQGlobaFunc.toMultiLineBox(win:getName()):getFormatHeight()
		local pos = EQGlobaFunc.toMultiLineBox(win:getName()):findElementText("")	
		EQGlobaFunc.toMultiLineBox(win:getName()):eraseElement(0)
		local pos = EQGlobaFunc.toMultiLineBox(win:getName()):findElementText("")
		if -1 ~= pos then
			for i=1, pos do
				EQGlobaFunc.toMultiLineBox(win:getName()):eraseElement(0)
			end
		end
		limitFlag = true
		local newHeight = EQGlobaFunc.toMultiLineBox(win:getName()):getFormatHeight()
		limitH = oldHeight - newHeight
		return limitH
	end
	limitFlag = false
	return limitH
end
-----------------------------------------
--聊天输出口内容解析(hezhr)
-----------------------------------------
function parseChatShowBox(text, flag)
	if false == chatShowBox:isVisible() then
		g_WinMgr:getWindow("MainRoot/HMenubar/Btn_HideChatFrame"):setProperty("NormalImage","set:Button5 image:NewImage73")
		m_animateWin:setVisible(true)
		EQGlobaFunc.toAnimateBox(m_animateWin:getName()):play(true)
	end
	local oldScrollSize = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getScrollbarDocumentSize()
	local scrollPos = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getScrollbarPosition()
	ChatShowBox_LimitLines(chatShowBox, limitLines)	--	控制聊天框的行数(最多支持50行)
	
	EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText("", true)	-- 换行
	if 0 == flag then	-- 普通场景聊天的信息提醒
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):setCols("#FFFFFF")
		local sf1, ef1 = string.find(text, "［")
		local sf2, ef2 = string.find(text, "］")
		local sf3, ef3 = string.find(text, "：")
		if nil==sf1 or nil==sf2 or nil==sf3 then
			parseHyperLink(chatShowBox, text)
		else
			local f1 = string.sub(text, 1, ef1)
			local name = string.sub(text, ef1+1, sf2-1)
			local f2 = string.sub(text, sf2, ef3)
			local txt = string.sub(text, ef3+1, -1)
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(f1, "#FFFFFF", false)
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(name, "#FFFFFF", false)
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(f2, "#FFFFFF", false)
			parseHyperLink(chatShowBox, txt)
		end
	end
	if 1 == flag then	-- 系统消息
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(text, "#FFF600", false)
	end
	if 2 == flag then	-- GM发言(游戏管理员发出的消息)
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(text, "#FF0000", false)
	end
	if 3 == flag then	-- 好友上下线提醒
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(text, "#FF9C00", false)
	end
	if 4 == flag then	-- 人物属性改变的信息提醒
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(text, "#FFF600", false)
	end
	if 5 == flag then	-- 密语聊天
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):setCols("#EC6EFF")
		local sf1, ef1 = string.find(text, "［")
		local sf2, ef2 = string.find(text, "］")
		local sf3, ef3 = string.find(text, "：")
		if nil==sf1 or nil==sf2 or nil==sf3 then
			parseHyperLink(chatShowBox, text)
		else
			local f1 = string.sub(text, 1, ef1)
			local name = string.sub(text, ef1+1, sf2-1)
			local f2 = string.sub(text, sf2, ef3)
			local txt = string.sub(text, ef3+1, -1)
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(f1, "#EC6EFF", false)
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(name, "#EC6EFF", false)
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(f2, "#EC6EFF", false)
			parseHyperLink(chatShowBox, txt)
		end
	end
	if 6 == flag then	-- 集团式聊天显示(家族和公会)的聊天频道
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):addElementText(text, "#19E30F", false)
	end
	
	local limitH = ChatShowBox_LimitLines(chatShowBox, limitLines)
	local newScrollSize = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getScrollbarDocumentSize()
	local renderHeight = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getRenderHeight()
	if scrollPos + renderHeight == oldScrollSize then	-- 滚动条在底部
		EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):setScrollbarPosition(newScrollSize)
	else	-- 用来设置默认滚动条在最下面
		if true == scrollFlag then
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):setScrollbarPosition(newScrollSize)
			local lines = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getLineSize()
			if 0 == zoomFlag then	-- 小窗口
				if lines > 7 then
					scrollFlag = false
				end			
			elseif 1 == zoomFlag then	-- 大窗口
				if lines > 10 then
					scrollFlag = false
				end					
			end
		end
		if true == limitFlag then
			scrollPos = EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):getScrollbarPosition()
			EQGlobaFunc.toMultiLineBox(chatShowBox:getName()):setScrollbarPosition(scrollPos - limitH)
		end
	end
end
-----------------------------------------
--解析超链接
-----------------------------------------
function parseHyperLink(mlbox, text)
	local sPos, ePos = string.find(text, "%b[]")
	if nil == sPos then	-- 没有超链接标识
		parseChatExprssion(mlbox, text)	--	解析聊天表情
	else	--	有超链接标识
		local pre = string.sub(text, 1, sPos)
		parseChatExprssion(mlbox, pre)
		local mid = string.sub(text, sPos+1, ePos-1)
		if nil == string.find(mid, "%b[]") then
			local itemrow = g_itemtableMgr:getItemRowEx(mid)
			if nil == itemrow then
				parseChatExprssion(mlbox, mid)
			else
				EQGlobaFunc.toMultiLineBox(mlbox:getName()):addElementHyperLink(mid, tostring(itemrow.id), "#FF9C00", false)
			end
		else
			parseHyperLink(mlbox, mid)
		end
		local suf = string.sub(text, ePos, string.len(text))
		parseHyperLink(mlbox, suf)
	end
end
-----------------------------------------
--把格式为("0-0-0-0")的字符串转为Rect
-----------------------------------------
function StringToRect(str)
	local rect = {left=0, top=0, right=0, bottom=0}
	rect.left = tonumber(string.sub(str, 1, string.find(str, "-")-1))
	str = string.sub(str, string.find(str, "-")+1, string.len(str))
	rect.top = tonumber(string.sub(str, 1, string.find(str, "-")-1))
	str = string.sub(str, string.find(str, "-")+1, string.len(str))
	rect.right = tonumber(string.sub(str, 1, string.find(str, "-")-1))
	rect.bottom = tonumber(string.sub(str, string.find(str, "-")+1, string.len(str)))
	return rect
end
-----------------------------------------
--获取超链接id(hezhr)
-----------------------------------------
function ShowChatBox_GetHyperLink(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local id = tonumber(EQGlobaFunc.toMultiLineBox(win:getName()):getHyperLinkId())
	if g_CS_HYPERLINK == g_CursorMgr:getState() then
		ClickHyperLinkItem(id)
	end
end
-----------------------------------------
--打开超链接(hezhr)
-----------------------------------------
function ShowChatBox_OpenHyperLink(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local id = tonumber(EQGlobaFunc.toMultiLineBox(win:getName()):getHyperLinkId())
	local rectStr = EQGlobaFunc.toMultiLineBox(win:getName()):getTriggerRect()
	local rect = StringToRect(rectStr)
	--时效相关
	local effectTime = ""
	local itemrow = g_itemtableMgr:getItemRow(id)
	if 1 == itemrow.effectTimeID then
		effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).effectTime
	elseif 2 == itemrow.effectTimeID then
		effectTime = g_itemtableMgr:getItemEffectTimeRow(itemrow.effectTimeID).delTime
	end
	createItemhint_shop(id, rect, g_MainUIWindows, effectTime, 0, nil)
end
-----------------------------------------
--关闭超链接(hezhr)
-----------------------------------------
function ShowChatBox_CloseHyperLink(args)
	destroyItemhint(g_MainUIWindows)
end
-----------------------------------------
--点击名字快速私聊(hezhr)
-----------------------------------------
function SpeedPrivateChat_MouseClick(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local content = EQGlobaFunc.toMultiLineBox(win:getName()):getTextContent()
	local rectStr = EQGlobaFunc.toMultiLineBox(win:getName()):getTriggerRect()
	local rect = StringToRect(rectStr)
	if 30~=rect.left and 95~=rect.left then	-- 过滤掉非玩家名字
		return
	end
	if content == g_mainplayer:getName() then	-- 过滤掉自己
		return
	end
	local acc = g_FriendSys:getFriendAccount(content)
	g_ChatSys:setTalktoAccount(acc)
	g_ChatSys:setTalktoName(content)
	g_ChatSys:talkPickObj()
end
-----------------------------------------
--按下R键快速私聊(hezhr)
-----------------------------------------
function SpeedPrivateChat_RKeyDown()
	if true == g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):isActive() then
		return
	end
	local acc = g_ChatSys:getTalktoAccount()
	if "" == acc then
		return
	end
	g_ChatSys:setTalktoAccount(acc)
	g_ChatSys:setTalktoName(g_ChatSys:getTalktoName())
	g_ChatSys:talkPickObj()	
end

-----------------------------------------
--  聊天处理
-----------------------------------------
local ChatBox = g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat")
CEGUI.toEditbox(ChatBox):setMaxTextLength(50)  --聊天最大字符长度为50

ChatBox:subscribeEvent("KeyDown","ChatSet_OnChat")

function ChatSet_OnChat(args)
	local keyvalue = CEGUI.toKeyEventArgs(args).scancode 
	if keyvalue == 28 or keyvalue == 156 then   -- 回车键
		ChatSet_addChat()
	end
end

--聊天输入输出
--ChatTex = ""
function ChatSet_addChat()
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	CloseChatExpressionUI()	-- 隐藏聊天表情栏(hezhr)
	local bool
	local text =  ChatBox:getText()	
	if text == "" then     
		return
	end
	ChatSet_Push(text)
	bool = ChatSet_LoadScript(text)	
	if bool==1 and g_mainplayer:isGmEnabled() then
		ChatBox:setText("Command Succeed!")
	end
	--输入为空，返回
	if text == "" then     
		return
	end
	
	-- local textlen = string.len(text)
	
	--输入为全部空白，返回
	-- local BlankCount = 0    
	-- for v in string.gmatch(text,"%s") do
		-- if v == " " then
			-- BlankCount = BlankCount + 1
		-- end
	-- end
	
	-- if textlen == BlankCount then
		-- return
	-- end
--	print(text)

	-- ChatTex = ChatTex..text..'\n'
	
	g_ChatSys:requestTalk()
	ChatBox:setText("")
	-- 清除正规表达式
	ChatBox:setValidationString(".*")	
end
-------------------------------------------
----  动态加载文件处理
-------------------------------------------
--g_CommandList = {"reloadui",
--				 "clean",
--				}
--
--function ChatSet_LoadScript(command)
--	local buff = string.lower(command)
--	local num
--	local temp
--	for num = 1,2 do
--		local temp = "/"..g_CommandList[num]
--		local i = string.find(buff,temp)
--		if i == 1 then
--			return ChatSet_FindCommand(g_CommandList[num],buff)
--		end
--	end
--end
--
--function  ChatSet_FindCommand(CommandName,Command)
--	if CommandName == "reloadui" then
--		local temp = string.find(Command," ")
--		if temp == 10 then
--			local len = string.len(Command)
--			local filename = string.sub(Command,11,len)
--			local path = g_ifpath--.."Frames/"
--			path = path..filename
--			g_LuaMgr:loadState(path)
--			return 1
--		else
--			return 0
--		end
--	end
--	if CommandName == "clean" then
--		local temp = string.len(Command)
--		if temp == 6 then
--			print("123")
--			--CEGUI.toMultiLineEditbox(g_ChatWin):initialiseComponents()
--			return 1
--		end
--		return 0
--	end
--
--end
--
-----------------------------------------
--  聊天输入记录记忆处理
-----------------------------------------
g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):subscribeEvent("KeyDown", "ChatSet_ShowRecord")
g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):subscribeEvent("MouseClick", "ETxt_Chat_Onclicked")	

function ETxt_Chat_Onclicked(args)
	closeNPCDlg()	-- 关闭npc对话框(hezhr)
	g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):activate()	
end

local recordlist = {"","","","","","","","","",""}

local pointer = 2
local suffix = 1
local UpFlag

function ChatSet_ShowRecord(args)
	local keyvalue = CEGUI.toKeyEventArgs(args).scancode 
	if keyvalue == 208 then --向下按键
		print(pointer)
		if recordlist[pointer-1] == "" then
			return
		else
			UpFlag = true
		end
		if pointer > 1 then 
			pointer = pointer - 1
			--g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):setText(recordlist[pointer])
			EQGlobaFunc.SetText("MainRoot/HMenubar/ETxt_Chat",recordlist[pointer])
		else
			--g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):setText(recordlist[1])
			EQGlobaFunc.SetText("MainRoot/HMenubar/ETxt_Chat",recordlist[1])
		end
	end
	if keyvalue == 200 then --向上按键
		print(pointer)
		if UpFlag == true then
			if recordlist[pointer + 1] == "" then
				return
			end
			if pointer < 10 then
				pointer = pointer + 1
				--g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):setText(recordlist[pointer])
				EQGlobaFunc.SetText("MainRoot/HMenubar/ETxt_Chat",recordlist[pointer])
			end
			if pointer == 10 then
				--g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):setText(recordlist[10])
				EQGlobaFunc.SetText("MainRoot/HMenubar/ETxt_Chat",recordlist[10])
			end	
		end
	end
end

function  ChatSet_Push(str)
	UpFlag = false
	local i,j
	if suffix <= 10 then
		recordlist[suffix] = str
		pointer = suffix + 1
		suffix = suffix + 1
		return
	end
	if suffix > 10 then
		for i=1,9 do 
			recordlist[i] = recordlist[i+1]
		end
		recordlist[10] = str
		pointer = 11
	end
end


-----------------------------------------
--  其他处理
-----------------------------------------


--CEGUI.toMultiLineEditbox(g_ChatWin):setShowVertScrollbar(true)   --显示滚动条

-- local ChatVSbar = CEGUI.toMultiLineEditbox(g_ChatWin):getVertScrollbar()
-- local bool = g_GuiSystem:injectMouseWheelChange(1.0)   
-- g_ChatWin:subscribeEvent("MouseEnter", "OnVSbar")
-- function OnVSbar(args)
	--print(bool)
	--ChatVSbar:onMouseWheel(CEGUI.toMouseEventArgs(args))
-- end
-- function RemoveWidgetEvent(EventName,WidgetName)
	-- local IsEvent = g_WinMgr:getWindow(WidgetName):isEventPresent(EventName)
	-- if IsEvent == true then
		-- g_WinMgr:getWindow(WidgetName):removeEvent(EventName)
		-- return true
	-- else
		-- return false
	-- end
-- end  

-- function ReRegisterEvent(EventName,WidgetName,FuncName)
	-- local flag = RemoveWidgetEvent(EventName,WidgetName)
	-- if flag == true then
		-- g_WinMgr:getWindow(WidgetName):subscribeEvent(EventName,FuncName)
	-- else
		-- return
	-- end
-- end
