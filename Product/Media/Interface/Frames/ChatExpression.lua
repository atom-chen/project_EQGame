-----------------------------------------
-- 文件说明	：聊天表情
-- 作者		：何展然
-- 修改		：
-- 日期		：10/10/27
-----------------------------------------

g_WinMgr:getWindow("MainUI/Bottomhud/Editbox/Btn_ChatExpression"):subscribeEvent("Clicked", "OpenChatExpression")
g_WinMgr:getWindow("ChatExpressionUI/pageup"):subscribeEvent("Clicked", "PageUp")
g_WinMgr:getWindow("ChatExpressionUI/pagedown"):subscribeEvent("Clicked", "PageDown")
MainUI_ShieldClick(g_ChatExpressionUI)	-- 屏蔽鼠标单击穿透
--g_MainUIWindows:addChildWindow(g_ChatExpressionUI)
g_ChatExpressionUI:setAlwaysOnTop(true)
--CloseChatExpressionUI()
local baseName = "ChatExpressionUI/expr"	-- 表情控件的前缀名字
local initFlag = false	-- 聊天表情栏初始标识
local curPage = 1	-- 当前页
local maxPage = EQGlobaClass.getChatExpressionTableManager():getExpressionCount()/23	-- 最大页数

-- 聊天表情表
local chatExpressionTable = 
{
	[1] = 11, [2] = 21, [3] = 31, [4] = 41, [5] = 51,
	[6] = 12, [7] = 22, [8] = 32, [9] = 42, [10] = 52,
	[11] = 13, [12] = 23, [13] = 33, [14] = 43, [15] = 53,
	[16] = 14, [17] = 24, [18] = 34, [19] = 44, [20] = 54,
	[21] = 15, [22] = 25, [23] = 35
}

function OpenChatExpressionUI()
	if false == initFlag then
		initChatExpression()
	end
	
	updateChatExpression()
	
	UIM_OpenWindow(g_ChatExpUINames.root)
end

function CloseChatExpressionUI()
	UIM_CloseWindow(g_ChatExpUINames.root)
end

-- 打开聊天表情栏
function OpenChatExpression(args)
	if false == g_MainUIWindows:isChild(g_ChatExpressionUI) then
		OpenChatExpressionUI()
		
		UIM_DoOpenAndCloseUI(g_ChatExpUINames.root)
	else
		CloseChatExpressionUI()
	end
end

-- 向前翻页
function PageUp(args)
	clearChatExpression()
	if 1 < curPage then
		curPage = curPage - 1
	end
	updateChatExpression()
end

-- 向后翻页
function PageDown(args)
	clearChatExpression()
	if maxPage > curPage then
		curPage = curPage + 1
	end
	updateChatExpression()
end

-- 创建聊天表情栏中的动画控件
function createChatExpressionCtrl(ctrlname)
	local ctrl = g_WinMgr:createWindow("EQHomeLook/AnimateBox", ctrlname)
	EQGlobaFunc.toAnimateBox(ctrlname):setSize(20, 20)
	g_ChatExpressionUI:addChildWindow(ctrl)
	return ctrl
end

-- 初始聊天表情栏
function initChatExpression()
	local exprName = "ChatExpressionUI/expression"
	for i = 1, 23 do
		local name = nil
		if i >= 10 then
			name = baseName..i
		else
			name = baseName.."0"..i
		end
		local expression = createChatExpressionCtrl(name)
		g_WinMgr:getWindow(exprName..i):addChildWindow(expression)
	end
	initFlag = true
end

-- 单击选中表情
function chooseExpre(args)
	if 1 == CEGUI.toMouseEventArgs(args).button then	--	鼠标右键
		return
	end
	local win = CEGUI.toWindowEventArgs(args).window
	local len = string.len(baseName)
	local id = tonumber(string.sub(win:getName(), len+1, -1))
	local index = tonumber(curPage..chatExpressionTable[id])
	local transfer = EQGlobaClass.getChatExpressionTableManager():getTransfer(index)
	if "" ~= transfer then	-- 有表情
		local ChatBox = g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat")
		limitCtrlTextLen(ChatBox, CEGUI.toEditbox(ChatBox):getMaxTextLength(), transfer)
	else	-- 没表情
		-- g_Loger:logMessage("---id: "..index.." chatExpression is not exist!---")
	end
	CloseChatExpressionUI()
	--激活聊天输入栏，并设置光标位置
	g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):activate()
	local len = string.len(g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):getText())
	g_WinMgr:getWindow("MainRoot/HMenubar/ETxt_Chat"):setCaratIndex(len)
end

-- 控制控件字符长度
function limitCtrlTextLen(win, len, str)
	local tips = "输入字符长度已超过限定"
	local baseText = win:getText()
	if EQGlobaFunc.getCharacterCount(baseText) >= len then
		parseChatShowBox(tips, 1)
		return
	end
	local text = baseText..str
	if EQGlobaFunc.getCharacterCount(text) > len then
		-- for i = 1, EQGlobaFunc.getCharacterCount(str) do
			-- text = baseText..EQGlobaFunc.subCharacter(str, 0, i)
			-- if EQGlobaFunc.getCharacterCount(text) > len then
				-- text = baseText..EQGlobaFunc.subCharacter(str, 0, i-1)
				-- EQGlobaFunc.SetText(win:getName(), text)
				-- return
			-- end
		-- end
		parseChatShowBox(tips, 1)
		return
	else
		local index = win:getCaratIndex()	--获取当前光标的位置
		local pre = EQGlobaFunc.subCharacter(baseText, 0, index)	--光标前部
		local suf = EQGlobaFunc.subCharacter(baseText, index, EQGlobaFunc.getCharacterCount(baseText))	--光标后部
		text = pre..str..suf
		EQGlobaFunc.SetText(win:getName(), text)
	end
end

-- 翻页时清除表情内容
function clearChatExpression()
	for i = 1, 23 do
		local name = nil
		if i >= 10 then
			name = baseName..i
		else
			name = baseName.."0"..i
		end
		EQGlobaFunc.toAnimateBox(name):clearUp()
	end
end

-- 添加表情内容
function updateChatExpression()
	for i = 1, 23 do
		local name = nil
		if i >= 10 then
			name = baseName..i
		else
			name = baseName.."0"..i
		end
		local id = tonumber(curPage..chatExpressionTable[i])
		local ani = EQGlobaClass.createAnimate()
		if -1 ~= EQGlobaClass.getChatExpressionTableManager():getChatExpression(ani, id) then	-- 有表情
			EQGlobaFunc.toAnimateBox(name):addAnimate(ani)
			EQGlobaFunc.toAnimateBox(name):play(true)	-- 循环播放
			g_WinMgr:getWindow(name):removeEvent("MouseClick")
			g_WinMgr:getWindow(name):subscribeEvent("MouseClick", "chooseExpre")	-- 注册单击事件
		else	-- 没表情
			-- g_Loger:logMessage("---id: "..id.." chatExpression is not exist!---")
		end
	end
end

-- 解析聊天表情转义符
function parseChatExprssion(mlbox, text)
	local key = "~b"
	if nil ~= string.find(text, key) then	-- 有关键字"~b"
		local s, e = string.find(text, key)
		local id = tonumber(string.sub(text, e+1, e+3))	-- 获取聊天表情id
		if nil ~= id then	-- 有id
			local ani = EQGlobaClass.createAnimate()
			if -1 ~= EQGlobaClass.getChatExpressionTableManager():getChatExpression(ani, id) then	-- 有表情
				local pre = string.sub(text, 1, s-1)
				local suf = string.sub(text, e+4, -1)
				local aniBox = g_WinMgr:createWindow("EQHomeLook/AnimateBox")
				EQGlobaFunc.toAnimateBox(aniBox:getName()):addAnimate(ani)
				EQGlobaFunc.toAnimateBox(aniBox:getName()):play(true)	-- 循环播放
				if "" ~= pre then
					EQGlobaFunc.toMultiLineBox(mlbox:getName()):addElementText(pre, false)
				end
				EQGlobaFunc.toMultiLineBox(mlbox:getName()):addElementWin(aniBox:getName(), false)
				parseChatExprssion(mlbox, suf)
			else	-- 无表情
				local pre = string.sub(text, 1, e+3)
				local suf = string.sub(text, e+4, -1)
				if "" ~= pre then
					EQGlobaFunc.toMultiLineBox(mlbox:getName()):addElementText(pre, false)
				end
				parseChatExprssion(mlbox, suf)
			end
		else	-- 无id
			local pre = string.sub(text, 1, e)
			local suf = string.sub(text, e+1, -1)
			if "" ~= pre then
				EQGlobaFunc.toMultiLineBox(mlbox:getName()):addElementText(pre, false)
			end
			parseChatExprssion(mlbox, suf)
		end
	else	-- 无关键字"~b"
		EQGlobaFunc.toMultiLineBox(mlbox:getName()):addElementText(text, false)	
	end
end

