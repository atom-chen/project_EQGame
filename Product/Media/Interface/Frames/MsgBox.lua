---------------------------------
--消息框和
--消息各种处理函数
--
----------------------------------
g_GameEventMgr:subscribeEvent("CREATEMSGBOX_SUCCESS", "MsgBox_setEachPos")
g_GameEventMgr:subscribeEvent("CREATEMSGBOX_SUCCESS", "MsgBox_subscribeKeyUpEvent")

g_MsgboxWinTable = {}
local lastPosX,lastPosY
local dx ,dy 
local counter = 0
--创建消息框
---------------
--parentWin			父窗口
--winIdx 			窗口的唯一编号
--title 			窗口标题名称
--content 			消息内容
--okFunc			确定函数
--cancelFunc 		取消函数

function MsgBox_createMsgBox(parentWin,winIdx,title,content,okFunc,cancelFunc,okBtnTxt)
	local baseMsgBoxName = "MsgBox_"
	--消息窗口
	local msgBoxName = baseMsgBoxName ..tostring(counter)--(winIdx)
	counter = counter + 1
	g_WinMgr:destroyWindow(msgBoxName)
	
	local msgBoxWin = g_WinMgr:createWindow("NewEQHomeLook/NewFrameWindow",msgBoxName)
	table.insert(g_MsgboxWinTable,msgBoxWin:getName())
	msgBoxWin:setSize(CEGUI.UVector2(CEGUI.UDim(0,200), CEGUI.UDim(0,150)))
	msgBoxWin:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-115), CEGUI.UDim(0.5,-60)))
	EQGlobaFunc.SetText(msgBoxName,title)
	MainUI_ShieldClick(msgBoxWin)
	
	--设置消息窗口为模态对话框
	Common_setModalState(msgBoxWin:getName(), true)
	msgBoxWin:setProperty("AlwaysOnTop","True")
	Common_injectEmptyEvt(msgBoxName)
	
	--注册窗口XX按钮事件
	g_WinMgr:getWindow(msgBoxName .."__auto_closebutton__"):subscribeEvent("Clicked", "MsgBox_cancelFunc")
	
	--消息框的背景图片
	--local winBg = g_WinMgr:createWindow("EQHomeLook/StaticImage",msgBoxName .."/win_Bg")
	--winBg:setProperty("Image","set:common1 image:Window7_Mid")
	--winBg:setSize(CEGUI.UVector2(CEGUI.UDim(0,220), CEGUI.UDim(0,85)))
	--winBg:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-111), CEGUI.UDim(0.5,-34)))
	--msgBoxWin:addChildWindow(winBg)
	
	--文本显示框
	local msgTxtBox = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", msgBoxName .."/msg_txt")
	msgTxtBox:setSize(CEGUI.UVector2(CEGUI.UDim(0,172), CEGUI.UDim(0,50)))	-- 设置控件13个汉字宽度
	msgTxtBox:setPosition(CEGUI.UVector2(CEGUI.UDim(0,15), CEGUI.UDim(0,50)))
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setAlign("AT_CENTER_HOR")	-- 设置水平居中
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):clearUp()
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setCols("#00FFFC")
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):addElementText(content, false)	-- 添加文本
	local height = EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setHeight(height)
	msgBoxWin:addChildWindow(msgTxtBox)
	
	--确定按钮
	if okFunc ~= "" then
		local msgConfirmBtn = g_WinMgr:createWindow("EQHomeLook/ImageButton", msgBoxName .."/msg_confirm")
		msgConfirmBtn:setProperty("NormalImage", "set:Button5 image:NewImage335")
		msgConfirmBtn:setProperty("HoverImage", "set:Button5 image:NewImage337")
		msgConfirmBtn:setProperty("PushedImage", "set:Button5 image:NewImage339")
		msgConfirmBtn:setProperty("Font", "simsun-10")
		msgConfirmBtn:setProperty("NormalTextColour", "FF00FFFC")
		msgConfirmBtn:setProperty("HoverTextColour", "FF00FFFC")
		msgConfirmBtn:setProperty("PushedTextColour", "FF1BADAC")
		
		msgConfirmBtn:setSize(CEGUI.UVector2(CEGUI.UDim(0,46), CEGUI.UDim(0,26)))
		if okBtnTxt == nil then
			EQGlobaFunc.SetText(msgConfirmBtn:getName(),"确定")
		else
			EQGlobaFunc.SetText(msgConfirmBtn:getName(),okBtnTxt)
		end
		msgBoxWin:addChildWindow(msgConfirmBtn)
		
		if okFunc ~= "" and cancelFunc ~= "" then
			msgConfirmBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0,44), CEGUI.UDim(0,100)))
		else
			msgConfirmBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-24), CEGUI.UDim(0.5,25)))
		end
		--注册确定事件
		msgConfirmBtn:subscribeEvent("Clicked", okFunc)
		if okFunc ~= "MsgBox_cancelFunc" then
			msgConfirmBtn:subscribeEvent("Clicked", "MsgBox_cancelFunc")
		end
	end
	
	--取消按钮
	if cancelFunc ~= "" then
		local msgCancelBtn = g_WinMgr:createWindow("EQHomeLook/ImageButton", msgBoxName .."/msg_cancel")
		msgCancelBtn:setProperty("NormalImage", "set:Button5 image:NewImage335")
		msgCancelBtn:setProperty("HoverImage", "set:Button5 image:NewImage337")
		msgCancelBtn:setProperty("PushedImage", "set:Button5 image:NewImage339")
		msgCancelBtn:setProperty("Font", "simsun-10")
		msgCancelBtn:setProperty("NormalTextColour", "FF00FFFC")
		msgCancelBtn:setProperty("HoverTextColour", "FF00FFFC")
		msgCancelBtn:setProperty("PushedTextColour", "FF1BADAC")
		
		msgCancelBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(1,-80), CEGUI.UDim(0,100)))
		msgCancelBtn:setSize(CEGUI.UVector2(CEGUI.UDim(0,46), CEGUI.UDim(0,26)))
		EQGlobaFunc.SetText(msgCancelBtn:getName(),"取消")
		msgBoxWin:addChildWindow(msgCancelBtn)
		--注册取消事件
		msgCancelBtn:subscribeEvent("Clicked", cancelFunc)
		if cancelFunc ~= "MsgBox_cancelFunc" then
			msgCancelBtn:subscribeEvent("Clicked", "MsgBox_cancelFunc")
		end
	end
	
	parentWin:addChildWindow(msgBoxWin)
	
	g_GameEventMgr:fireEvent("CREATEMSGBOX_SUCCESS")
	
	return msgBoxName
end
-----------------------------------------
--扩展消息框(add by hezhr)
--width: 消息窗口宽度; title: 标题; content: 内容
--contentTable: 用来传递复选框文字内容及复选框状态的表,表的构造{{text="",state=true}, {}...}
--okFunc: 点击确定按钮响应的函数名; cancelFunc: 点击取消按钮响应的函数名
--返回: 保存复选框名的表{"","",...},消息框名
-----------------------------------------
function MsgBox_createMsgBoxEx(width, title, content, contentTable, okFunc, cancelFunc)
	local msgBoxName = "MsgBoxEx"
	local count = 0
	-- 获取可能消息框名字
	while(nil~=g_WinMgr:getWindow(msgBoxName)) do
		msgBoxName = msgBoxName..tostring(count)
		count = count + 1
	end
	-- 创建消息框
	local msgBox = g_WinMgr:createWindow("NewEQHomeLook/NewFrameWindow", msgBoxName)
	msgBox:setWidth(CEGUI.UDim(0, width))
	EQGlobaFunc.SetText(msgBoxName, title)
	g_WinMgr:getWindow(msgBoxName.."__auto_closebutton__"):subscribeEvent("Clicked", "MsgBoxEx_close")
	local xPos, yPos = 15, 45
	-- 创建文本显示框
	local msgBoxContent = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", msgBoxName.."/content")
	EQGlobaFunc.toMultiLineBox(msgBoxContent:getName()):setAlign("AT_CENTER_VER")	-- 设置垂直居中
	EQGlobaFunc.toMultiLineBox(msgBoxContent:getName()):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(msgBoxContent:getName()):setCols("#00FFFC")
	g_mlbParse:getData(content, msgBoxContent:getName())
	local contentWidth = EQGlobaFunc.toMultiLineBox(msgBoxContent:getName()):getFitWidth("simsun-10", content)
	local contentMaxWidth = width - 30	-- 文本显示框的最大宽度
	if contentWidth >= contentMaxWidth then
		msgBoxContent:setWidth(CEGUI.UDim(0, contentMaxWidth))
		contentWidth = contentMaxWidth
	else
		msgBoxContent:setWidth(CEGUI.UDim(0, contentWidth))
	end
	local contentHeight = EQGlobaFunc.toMultiLineBox(msgBoxContent:getName()):getFormatHeight()
	msgBoxContent:setHeight(CEGUI.UDim(0, contentHeight))
	msgBoxContent:setPosition(CEGUI.UVector2(CEGUI.UDim(0, (width-contentWidth)/2), CEGUI.UDim(0, yPos)))
	msgBox:addChildWindow(msgBoxContent)
	yPos = yPos + contentHeight + 10
	local checkBoxW, checkBoxH = 20, 20	-- 复选框宽、高
	-- 创建复选框
	local checkBoxNameTable = {}
	for i, v in pairs(contentTable) do
		local checkBox = g_WinMgr:createWindow("NewEQHomeLook/NewCheckbox", msgBoxName.."/checkBox"..tostring(i))
		table.insert(checkBoxNameTable, msgBoxName.."/checkBox"..tostring(i))
		checkBox:setSize(CEGUI.UVector2(CEGUI.UDim(0, checkBoxW), CEGUI.UDim(0, checkBoxH)))
		checkBox:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos+5), CEGUI.UDim(0, yPos)))
		checkBox:subscribeEvent("CheckStateChanged", "MsgBoxEx_changeCheckBoxState")
		CEGUI.toCheckbox(checkBox):setSelected(v.state)
		msgBox:addChildWindow(checkBox)
		local checkBoxContent = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", msgBoxName.."/checkBoxContent"..tostring(i))
		checkBoxContent:setWidth(CEGUI.UDim(0, width-30-checkBoxW-15))
		checkBoxContent:setPosition(CEGUI.UVector2(CEGUI.UDim(0, xPos+5+checkBoxW+5), CEGUI.UDim(0, yPos)))
		checkBoxContent:subscribeEvent("MouseClick", "MsgBoxEx_changeCheckBoxState")
		EQGlobaFunc.toMultiLineBox(checkBoxContent:getName()):setAlign("AT_CENTER_VER")	-- 设置垂直居中
		EQGlobaFunc.toMultiLineBox(checkBoxContent:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(checkBoxContent:getName()):setCols("#00FFFC")
		g_mlbParse:getData(v.text, checkBoxContent:getName())
		local checkBoxContentHeight = EQGlobaFunc.toMultiLineBox(checkBoxContent:getName()):getFormatHeight()
		if checkBoxContentHeight < checkBoxH then
			checkBoxContent:setHeight(CEGUI.UDim(0, checkBoxH))
			yPos = yPos + checkBoxH + 5
		else
			checkBoxContent:setHeight(CEGUI.UDim(0, checkBoxContentHeight))
			yPos = yPos + checkBoxContentHeight + 5				
		end
		msgBox:addChildWindow(checkBoxContent)
	end	
	local btnW, btnH = 45, 25	-- 按钮宽、高
	-- 创建确定按钮
	if "" ~= okFunc then
		local okBtn = g_WinMgr:createWindow("EQHomeLook/ImageButton", msgBoxName.."/ok")
		okBtn:setProperty("NormalImage", "set:Button5 image:NewImage335")
		okBtn:setProperty("HoverImage", "set:Button5 image:NewImage337")
		okBtn:setProperty("PushedImage", "set:Button5 image:NewImage339")
		okBtn:setProperty("Font", "simsun-10")
		okBtn:setProperty("NormalTextColour", "FF00FFFC")
		okBtn:setProperty("HoverTextColour", "FF00FFFC")
		okBtn:setProperty("PushedTextColour", "FF1BADAC")
		okBtn:setSize(CEGUI.UVector2(CEGUI.UDim(0, btnW), CEGUI.UDim(0, btnH)))
		EQGlobaFunc.SetText(okBtn:getName(), "确定")
		msgBox:addChildWindow(okBtn)
		if "" ~= cancelFunc then
			okBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0, (width-2*btnW)/3), CEGUI.UDim(0, yPos)))
		else
			okBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0, (width-btnW)/2), CEGUI.UDim(0, yPos)))
		end
		okBtn:subscribeEvent("Clicked", okFunc)
		okBtn:subscribeEvent("Clicked", "MsgBoxEx_close")
	end	
	-- 创建取消按钮
	if "" ~= cancelFunc then
		local cancelBtn = g_WinMgr:createWindow("EQHomeLook/ImageButton", msgBoxName.."/cancel")
		cancelBtn:setProperty("NormalImage", "set:Button5 image:NewImage335")
		cancelBtn:setProperty("HoverImage", "set:Button5 image:NewImage337")
		cancelBtn:setProperty("PushedImage", "set:Button5 image:NewImage339")
		cancelBtn:setProperty("Font", "simsun-10")
		cancelBtn:setProperty("NormalTextColour", "FF00FFFC")
		cancelBtn:setProperty("HoverTextColour", "FF00FFFC")
		cancelBtn:setProperty("PushedTextColour", "FF1BADAC")
		cancelBtn:setSize(CEGUI.UVector2(CEGUI.UDim(0, btnW), CEGUI.UDim(0, btnH)))
		EQGlobaFunc.SetText(cancelBtn:getName(), "取消")
		msgBox:addChildWindow(cancelBtn)
		if "" ~= okFunc then
			cancelBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0, btnW+2*((width-2*btnW)/3)), CEGUI.UDim(0, yPos)))
		else
			cancelBtn:setPosition(CEGUI.UVector2(CEGUI.UDim(0, (width-btnW)/2), CEGUI.UDim(0, yPos)))
		end
		cancelBtn:subscribeEvent("Clicked", cancelFunc)
		cancelBtn:subscribeEvent("Clicked", "MsgBoxEx_close")
	end
	if ""~=okFunc or ""~=cancelFunc then
		yPos = yPos + btnH + 20
	elseif ""==okFunc and ""==cancelFunc then
		yPos = yPos + 20
	end
	-- 限定消息框的最小高度,若<137的话,则消息框皮肤会重叠(这与NewFrameWindow的皮肤有关)
	if yPos < 137 then
		yPos = 137
	end
	msgBox:setHeight(CEGUI.UDim(0, yPos))
	return checkBoxNameTable, msgBoxName
end
-----------------------------------------
--关闭扩展消息框响应函数(add by hezhr)
-----------------------------------------
function MsgBoxEx_close(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local msgBox = win:getParent()
	if nil == msgBox then
		return
	end
	Common_setModalState(msgBox:getName(), false)	-- 取消设置为模态对话框
	g_WinMgr:destroyWindow(msgBox)
end
-----------------------------------------
--改变复选框状态响应函数(add by hezhr)
-----------------------------------------
function MsgBoxEx_changeCheckBoxState(args)
	-- 屏蔽鼠标右键
	if 1 == CEGUI.toMouseEventArgs(args).button then
		return
	end
	local winname = CEGUI.toWindowEventArgs(args).window:getName()
	local sPos, ePos = string.find(winname, "checkBoxContent")
	-- 点击的是复选框
	if nil == sPos then
		return
	end
	-- 点击的是复选框文字
	local checkBoxName = string.sub(winname, 1, sPos-1).."checkBox"..string.sub(winname, ePos+1, -1)
	-- 改变复选框状态
	if CEGUI.toCheckbox(g_WinMgr:getWindow(checkBoxName)):isSelected() then
		CEGUI.toCheckbox(g_WinMgr:getWindow(checkBoxName)):setSelected(false)
	else
		CEGUI.toCheckbox(g_WinMgr:getWindow(checkBoxName)):setSelected(true)
	end
end
-----------------------------------------
--对扩展消息框的进一步封装(add by hezhr)
--parentWin: 消息框的父窗口; width: 消息窗口宽度; title: 标题; content: 内容
--contentTable: 用来传递复选框文字内容及复选框状态的表,表的构造{{text="",state=true}, {}...}
--okFunc: 点击确定按钮响应的函数名; cancelFunc: 点击取消按钮响应的函数名
--返回: 保存复选框名的表{"","",...},消息框名
-----------------------------------------
function MsgBox_createMsgBoxExA(parentWin, width, title, content, contentTable, okFunc, cancelFunc)
	if width < 200 then	-- 限制最小宽度(确定和取消按钮可能会重叠)
		width = 200
	end
	local checkBoxNameTable, msgBoxName = MsgBox_createMsgBoxEx(width, title, content, contentTable, okFunc, cancelFunc)
	-- 设置窗口属性
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local width = g_WinMgr:getWindow(msgBoxName):getWidth():asAbsolute(screenW)
	local height = g_WinMgr:getWindow(msgBoxName):getHeight():asAbsolute(screenH)
	local x = (screenW-width)/2
	local y = (screenH-height)/2
	g_WinMgr:getWindow(msgBoxName):setPosition(CEGUI.UVector2(CEGUI.UDim(0, x), CEGUI.UDim(0, y)))
	Common_setModalState(msgBoxName, true)	-- 设置为模态对话框
	MainUI_ShieldClick(g_WinMgr:getWindow(msgBoxName))
	Common_injectEmptyEvt(msgBoxName)
	parentWin:addChildWindow(g_WinMgr:getWindow(msgBoxName))
	return checkBoxNameTable, msgBoxName
end
-----------------------------------------
--创建淡入浅出消息框(2011/7/7 add by hezhr)
--parentWin: 父窗口; width: 宽度; x: x坐标; y: y坐标; content: 内容
--delayTime: 开始淡入浅出前的延迟时间; fadeTime: 淡入浅出的时间(单位/ms)
-----------------------------------------
function MsgBox_createFadeMsgBox(parentWin, width, x, y, content, delayTime, fadeTime)
	local msgBoxName = "FadeMsgBox"
	local count = 0
	-- 获取可能消息框名字
	while(nil~=g_WinMgr:getWindow(msgBoxName)) do
		msgBoxName = msgBoxName..tostring(count)
		count = count + 1
	end
	-- 创建消息框
	local hint = g_WinMgr:createWindow("EQHomeLook/ItemhintFrame", msgBoxName)
	local tip = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", msgBoxName.."/tip")
	EQGlobaFunc.toMultiLineBox(tip:getName()):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(tip:getName()):setCols("#05E5FF")
	EQGlobaFunc.toMultiLineBox(tip:getName()):setSide("#043644", 1)
	EQGlobaFunc.toMultiLineBox(tip:getName()):setWidth(width-10)
	g_mlbParse:getData(content, tip:getName())
	local height = EQGlobaFunc.toMultiLineBox(tip:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(tip:getName()):setHeight(height)
	EQGlobaFunc.toMultiLineBox(tip:getName()):setPosition(5, 5)
	hint:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height+10)))
	hint:setPosition(CEGUI.UVector2(CEGUI.UDim(0, x), CEGUI.UDim(0, y)))
	hint:addChildWindow(tip)
	parentWin:addChildWindow(hint)
	-- 播放淡入浅出效果,true:播放结束自动销毁消息框
	EQGlobaFunc.setWindowFade(msgBoxName, delayTime, fadeTime, true)
end
-----------------------------------------
local msgBoxNameC = "__MsgBox_CreateMsgBoxC__Name__"
local msgBoxTextNameC = msgBoxNameC .."/msg_txt"
local msgBoxOkNameC = msgBoxNameC .."/msg_confirm"
local msgBoxCancelNameC = msgBoxNameC .."/msg_cancel"
local msgBoxCounter = 0

function MsgBox_CreateMsgBoxC(parentWin, title, okFunc, cancelFunc)
	msgBoxNameC = "__MsgBox_CreateMsgBoxC__Name__" .. tostring(msgBoxCounter)
	msgBoxTextNameC = msgBoxNameC .."/msg_txt"
	msgBoxOkNameC = msgBoxNameC .."/msg_confirm"
	msgBoxCancelNameC = msgBoxNameC .."/msg_cancel"
	msgBoxCounter = msgBoxCounter + 1
	g_WinMgr:destroyWindow(msgBoxNameC)
	
	local fsize, fxy, oksize, okxy, okxy1, cancelsize, cancelxy, txtsize, txtxy
	
	local msgBoxWin
	if title ~= "" and title ~= nil then
		msgBoxWin = g_WinMgr:createWindow("NewEQHomeLook/NewFrameWindow",msgBoxNameC)
		EQGlobaFunc.SetText(msgBoxNameC,title)
		fsize = CEGUI.UVector2(CEGUI.UDim(0,200), CEGUI.UDim(0,150))
		fxy = CEGUI.UVector2(CEGUI.UDim(0.5,-115), CEGUI.UDim(0.5,-60))
		oksize = CEGUI.UVector2(CEGUI.UDim(0,46), CEGUI.UDim(0,26))
		okxy = CEGUI.UVector2(CEGUI.UDim(0,44), CEGUI.UDim(0,100))
		okxy1 = CEGUI.UVector2(CEGUI.UDim(0.5,-24), CEGUI.UDim(0.5,25))
		cancelsize = CEGUI.UVector2(CEGUI.UDim(0,46), CEGUI.UDim(0,26))
		cancelxy = CEGUI.UVector2(CEGUI.UDim(1,-80), CEGUI.UDim(0,100))
		txtsize = CEGUI.UVector2(CEGUI.UDim(0,172), CEGUI.UDim(0,50))
		txtxy = CEGUI.UVector2(CEGUI.UDim(0,15), CEGUI.UDim(0,50))
		g_WinMgr:getWindow(msgBoxNameC .."__auto_closebutton__"):subscribeEvent("Clicked", "MsgBox_cancelFunc")
	else
		msgBoxWin = g_WinMgr:createWindow("NewEQHomeLook/NewStaticImage2",msgBoxNameC)
		fsize = CEGUI.UVector2(CEGUI.UDim(0,230), CEGUI.UDim(0,120))
		fxy = CEGUI.UVector2(CEGUI.UDim(0.5,-107), CEGUI.UDim(0.5,-64))
		oksize = CEGUI.UVector2(CEGUI.UDim(0,46), CEGUI.UDim(0,26))
		okxy = CEGUI.UVector2(CEGUI.UDim(0.5,-54), CEGUI.UDim(1,-34))
		okxy1 = CEGUI.UVector2(CEGUI.UDim(0.5,-24), CEGUI.UDim(1,-34))
		cancelsize = CEGUI.UVector2(CEGUI.UDim(0,46), CEGUI.UDim(0,26))
		cancelxy = CEGUI.UVector2(CEGUI.UDim(0.5,8), CEGUI.UDim(1,-34))
		txtsize = CEGUI.UVector2(CEGUI.UDim(0,200), CEGUI.UDim(0,67))
		txtxy = CEGUI.UVector2(CEGUI.UDim(0,14), CEGUI.UDim(0,22))
	end
	msgBoxWin:setSize(fsize)
	msgBoxWin:setPosition(fxy)
	
	MainUI_ShieldClick(msgBoxWin)
	
	--设置消息窗口为模态对话框
	Common_setModalState(msgBoxWin:getName(), true)
	msgBoxWin:setProperty("AlwaysOnTop","True")
	Common_injectEmptyEvt(msgBoxNameC)
	
	--文本显示框
	local msgTxtBox = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", msgBoxTextNameC)
	msgTxtBox:setSize(txtsize)
	msgTxtBox:setPosition(txtxy)
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setAlign("AT_CENTER_HOR")	-- 设置水平居中
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setAlign("AT_CENTER_VER")  -- 设置垂直居中
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setFont("simsun-10")
	EQGlobaFunc.toMultiLineBox(msgTxtBox:getName()):setCols("#00FFFC")
	msgBoxWin:addChildWindow(msgTxtBox)
	
	--确定按钮
	if okFunc ~= "" then
		local msgConfirmBtn = g_WinMgr:createWindow("EQHomeLook/ImageButton", msgBoxOkNameC)
		msgConfirmBtn:setProperty("NormalImage", "set:Button5 image:NewImage335")
		msgConfirmBtn:setProperty("HoverImage", "set:Button5 image:NewImage337")
		msgConfirmBtn:setProperty("PushedImage", "set:Button5 image:NewImage339")
		msgConfirmBtn:setProperty("Font", "simsun-10")
		msgConfirmBtn:setProperty("NormalTextColour", "FF00FFFC")
		msgConfirmBtn:setProperty("HoverTextColour", "FF00FFFC")
		msgConfirmBtn:setProperty("PushedTextColour", "FF1BADAC")
		
		msgConfirmBtn:setSize(oksize)
		EQGlobaFunc.SetText(msgConfirmBtn:getName(),"确定")
		msgBoxWin:addChildWindow(msgConfirmBtn)
		
		if okFunc ~= "" and cancelFunc ~= "" then
			msgConfirmBtn:setPosition(okxy)
		else
			msgConfirmBtn:setPosition(okxy1)
		end
		--注册确定事件
		msgConfirmBtn:subscribeEvent("Clicked", okFunc)
		if okFunc ~= "MsgBox_cancelFunc" then
			msgConfirmBtn:subscribeEvent("Clicked", "MsgBox_cancelFunc")
		end
	end
	
	--取消按钮
	if cancelFunc ~= "" then
		local msgCancelBtn = g_WinMgr:createWindow("EQHomeLook/ImageButton", msgBoxCancelNameC)
		msgCancelBtn:setProperty("NormalImage", "set:Button5 image:NewImage335")
		msgCancelBtn:setProperty("HoverImage", "set:Button5 image:NewImage337")
		msgCancelBtn:setProperty("PushedImage", "set:Button5 image:NewImage339")
		msgCancelBtn:setProperty("Font", "simsun-10")
		msgCancelBtn:setProperty("NormalTextColour", "FF00FFFC")
		msgCancelBtn:setProperty("HoverTextColour", "FF00FFFC")
		msgCancelBtn:setProperty("PushedTextColour", "FF1BADAC")
		
		msgCancelBtn:setPosition(cancelxy)
		msgCancelBtn:setSize(cancelsize)
		EQGlobaFunc.SetText(msgCancelBtn:getName(),"取消")
		msgBoxWin:addChildWindow(msgCancelBtn)
		--注册取消事件
		msgCancelBtn:subscribeEvent("Clicked", cancelFunc)
		if cancelFunc ~= "MsgBox_cancelFunc" then
			msgCancelBtn:subscribeEvent("Clicked", "MsgBox_cancelFunc")
		end
	end
	
	parentWin:addChildWindow(msgBoxWin)
	
	return msgBoxWin:getName()
end

function MsgBox_GetMsgBoxCMultiLineBox()
	return EQGlobaFunc.toMultiLineBox(msgBoxTextNameC)
end

function MsgBox_AddTextToMsgBoxC(text, color, newLine)
	EQGlobaFunc.toMultiLineBox(msgBoxTextNameC):addElementText(text, color, newLine)
end

function MsgBox_ShowBuyOneConfirm(name, tprice, unit, okFun)
	MsgBox_CreateMsgBoxC(g_MainUIWindows, "", okFun, "MsgBox_cancelFunc")
	MsgBox_AddTextToMsgBoxC(name, "#FFFF00", false)
	MsgBox_AddTextToMsgBoxC("共需", "#FFFFFF", true)
	MsgBox_AddTextToMsgBoxC(tostring(tprice), "#FBCD04", false)
	MsgBox_AddTextToMsgBoxC(unit.."币，是否购买？", "#FFFFFF", false)
end

function MsgBox_ShowBuyConfirm(tprice, unit, okFun)
	MsgBox_CreateMsgBoxC(g_MainUIWindows, "", okFun, "MsgBox_cancelFunc")
	MsgBox_AddTextToMsgBoxC("共需", "#FFF600", false)
	MsgBox_AddTextToMsgBoxC(tostring(tprice), "#FBCD04", false)
	MsgBox_AddTextToMsgBoxC(unit.."币，是否购买？", "#FFF600", false)
end

function MsgBox_ShowBuyMultiConfirm(eq, gold, okFun)
	MsgBox_CreateMsgBoxC(g_MainUIWindows, "", okFun, "MsgBox_cancelFunc")
	MsgBox_AddTextToMsgBoxC("共需", "#FFF600", false)
	if eq > 0 then
		MsgBox_AddTextToMsgBoxC(tostring(eq).."EQ币，", "#FBCD04", false)
	end
	if gold > 0 then
		MsgBox_AddTextToMsgBoxC(tostring(gold).."金币，", "#FBCD04", false)
	end
	MsgBox_AddTextToMsgBoxC("是否购买？", "#FFF600", false)
end


--取消函数
function MsgBox_cancelFunc(args)
	local argsWin = CEGUI.toWindowEventArgs(args).window
	local parentWin = argsWin:getParent()
	local MainWin = parentWin:getParent()
	--取消消息窗口为模态对话框
	Common_setModalState(parentWin:getName(), false)
	MainWin:removeChildWindow(parentWin)
	for i,v in ipairs(g_MsgboxWinTable) do
		if v == parentWin:getName() then
			table.remove(g_MsgboxWinTable,i)
		end
	end
	local tab_n = table.getn(g_MsgboxWinTable)
	if tab_n ~= 0 then
		Common_setModalState(g_MsgboxWinTable[tab_n], true)
	end
	g_GameEventMgr:fireEvent("NOTIFY_CLOSE_SYSMSG_BOX")	-- 关闭消息框(2011/4/29 add by hezhr)
end
---------------------------------------------------
--
function MsgBox_setEachPos()
	for i,v in ipairs(g_MsgboxWinTable) do
		local win_half_W = (g_WinMgr:getWindow(v):getWidth():asAbsolute(1024))/2
		local win_half_H = (g_WinMgr:getWindow(v):getHeight():asAbsolute(1024))/2
		g_WinMgr:getWindow(v):setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,(-win_half_W + (i-1)*10)), CEGUI.UDim(0.5,-win_half_H + (i-1) * win_half_H)))
	end
	
	local tab_n = table.getn(g_MsgboxWinTable)
	if tab_n ~= 0 then
		Common_setModalState(g_MsgboxWinTable[tab_n], true)
	end
	for i = tab_n, 1, -1 do

	end
end
----------------------------------------------------
--
function MsgBox_subscribeKeyUpEvent()
	for i,v in ipairs(g_MsgboxWinTable) do
		g_WinMgr:getWindow(v):subscribeEvent("KeyUp", "MsgBox_KeyUpEvent")
	end
end
----------------------------------------------------
--
function MsgBox_KeyUpEvent(args)
	local keyvalue = CEGUI.toKeyEventArgs(args).scancode 
	local key_win = CEGUI.toWindowEventArgs(args).window
	if keyvalue == 28 or keyvalue == 156 then   --回车
		if key_win ~= nil then
			local parent_win = key_win:getParent()
			if parent_win ~= nil then
				Common_setModalState(key_win:getName(), false)
				parent_win:removeChildWindow(key_win)
			end
		end
		for i,v in ipairs(g_MsgboxWinTable) do
			if key_win:getName() == v then
				table.remove(g_MsgboxWinTable,i)
			end
		end
		
		local tab_n = table.getn(g_MsgboxWinTable)
		if tab_n ~= 0 then
			Common_setModalState(g_MsgboxWinTable[tab_n], true)
		end
	end	
end