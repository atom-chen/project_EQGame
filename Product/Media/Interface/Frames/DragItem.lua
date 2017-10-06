-----------------------------------------
-- 文件说明	：物品粘贴相关逻辑处理
-- 作者		：麦_mike
-- 修改		：
-- 日期		：10/07/25
-----------------------------------------

local splitMax = 999999999
local textbox = nil

--脚本加载后执行------------------------------------------------------------------------
function DragItem_Onload()
	g_WinMgr:getWindow("Root/chaifen/background/done"):subscribeEvent("MouseClick","ClickOK")
	--g_WinMgr:getWindow("Root/chaifen/cancel"):subscribeEvent("MouseClick","ClickCancel")
	g_GameEventMgr:subscribeEvent("DRAG_ITEM_UNDOED","Event_undo")
	textbox = g_WinMgr:getWindow("Root/chaifen/background/editbg/edit")
	textbox:subscribeEvent("TextChanged", "OnSplitTBTextChanged")
	CEGUI.toEditbox(textbox):setMaxTextLength(9)
	CEGUI.toEditbox(textbox):setValidationString("^[0-9]*$")
	
	g_WinMgr:getWindow("Root/chaifen/background/all"):subscribeEvent("MouseClick","OnAllClick")
	g_WinMgr:getWindow("Root/chaifen/background/clear"):subscribeEvent("MouseClick","OnClearClick")
	g_WinMgr:getWindow("Root/chaifen/background/backspace"):subscribeEvent("MouseClick","OnBackspaceClick")
	
	for i=0,9 do 
		g_WinMgr:getWindow("Root/chaifen/background/num"..i):subscribeEvent("MouseClick","OnNumClick")
	end
	
	g_WinMgr:getWindow("Root/chaifen__auto_closebutton__"):subscribeEvent("MouseClick","OnCloseClick")
end

--点击数字键
function OnNumClick(args)
	local tarwin = CEGUI.toWindowEventArgs(args).window
	local len = string.len(tarwin:getName())
	local num = string.sub(tarwin:getName(),len,len)
	local strnum = textbox:getText()..tostring(tonumber(num))
	
	SetSplitValue(tonumber(strnum))
end

--点击后退键
function OnBackspaceClick(args)
	local strnum = textbox:getText()
	local n = string.len(strnum)
	
	if 1 == n then
		OnClearClick(nil)
	else
		textbox:setText(string.sub(strnum, 1, -2))
	end
end

--点击清理键
function OnClearClick(args)
	textbox:setText("0")
	textbox:setCaratIndex(2)
end

--点击所有键
function OnAllClick(args)
	SetSplitValue(splitMax)
end

function SetSplitValue(value)
	if value > splitMax then
		value = splitMax
		EQGlobaFunc.SetSysMsgText("输入超过了上限！")
	end
	
	textbox:setText(tostring(value))
end

--输入正确性检查
function OnSplitTBTextChanged(args)
	if nil == textbox then
		return
	end
	
	local up = false
	local strv = textbox:getText()
	local v = 0
	local l = string.len(strv)
	
	if l < 2 then
		return
	end
	
	if "0" == string.sub(strv, 1, 1) then
		up = true
	end
	
	v = tonumber(strv)
	
	if v > splitMax then
		v = splitMax
		up = true
		EQGlobaFunc.SetSysMsgText("输入超过了上限！")
	end
	
	if up then
		textbox:setText(tostring(v))
	end
end

--
function OnCloseClick(args)
	g_DragItem:undo()
	Event_undo()
end

--响应撤消事件
function Event_undo()
	NormalCancel()
	
	if(g_CursorMgr:getState() == g_CS_SPLIT_ITEM) then
		g_CursorMgr:backtoLastState()
		g_CursorMgr:setSpliting(false)
	end
	
	if(g_CursorMgr:getState() == g_CS_HYPERLINK) then
		g_CursorMgr:setState(g_CS_NORMAL)
	end
	
	RemoveSplitWnd()
end

--点击确定
function ClickOK()
	local strnum = g_WinMgr:getWindow("Root/chaifen/background/editbg/edit"):getText()
	local num = tonumber(strnum)
	
	if num == nil or num <=0 or num > splitMax then
		ClickCancel()
	else
		g_DragItem:setSplitNum(num)
		g_DragItem:setVisible(true)
		RemoveSplitWnd()
	end
	
	g_CursorMgr:setSpliting(false)
end

--点击撤消
function ClickCancel()
	g_DragItem:undo()
	RemoveSplitWnd()
	g_CursorMgr:setSpliting(false)
end

--准备拆分
function SplitItem(maxitem)
	if maxitem == nil then
		--g_Loger:logError("maxitem == nil on function SplitItem.")
		return
	end
	
	if maxitem == nul or maxitem < 2 then
		MsgBox_createMsgBox(g_MainUIWindows, 9999, "", "不能拆分","MsgBox_cancelFunc","")
		g_DragItem:undo()
		
		if(g_CursorMgr:getState() == g_CS_SPLIT_ITEM) then
			g_CursorMgr:backtoLastState()
		end
		return
	end
	splitMax = maxitem
	ShowSplitWnd(maxitem / 2)
	g_DragItem:setVisible(false)
	
	EQGlobaFunc.SetText(g_SplitWnd:getName(), "上限:"..tostring(maxitem))
	
	--CEGUI.toEditbox(textbox):setValidationString(GetValidationString(maxitem))
end

--显示拆分界面
function ShowSplitWnd(num)
	CEGUI.toEditbox(textbox):activate()
	
	g_MainUIWindows:addChildWindow(g_SplitWnd)
	g_SplitWnd:activate()
	Common_setModalState(g_SplitWnd:getName(), true)
	g_SplitWnd:setAlwaysOnTop(true)
	OnClearClick(nil)
	
	if(g_CursorMgr:getState() == g_CS_SPLIT_ITEM) then
		g_CursorMgr:backtoLastState()
	end
	g_CursorMgr:setSpliting(true)
end

--取消拆分界面
function RemoveSplitWnd()
	g_MainUIWindows:removeChildWindow(g_SplitWnd)
	Common_setModalState(g_SplitWnd:getName(), false)
	g_CursorMgr:setSpliting(false)
end

--@ 获取文本输入框最大能输入的数字的ValidationString
--@ maxValue必须为正整数
--@ 例"123": "^[1-9]?$|^[1-9][0-9]$|^1[0-2][0-3]$"
function GetValidationString(maxValue)
	local s
	
	if maxValue < 10 then
		s = "^[1-" .. tostring(maxValue) .."]?$"
	else
		local sv,st,i,j,n
		
		sv = tostring(maxValue)
		n = string.len(sv)
		
		s = "^[1-9]?$"
		
		for i = 2, n - 1 do
			s = s .. "|^[1-9]"
			for j = 2, i do
				s = s .. "[0-9]"
			end
			s = s .. "$"
		end
		
		s = s .. "|^"
		
		st = string.sub(sv, 1, 1)
		
		if st == "1" then
			s = s .. "1"
		else
			s = s .. "[1-" .. st .. "]"
		end
			
		for i = 2, n do
			st = string.sub(sv, i, i)
			
			if st == "0" then
				s = s .. st
			else
				s = s .. "[0-" .. st .. "]"
			end
		end
		
		s = s .. "$"
	end
	
	return s
end
