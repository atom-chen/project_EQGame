-----------------------------------------
-- 文件说明：退出游戏处理
-----------------------------------------
g_ExitWindow:setProperty("AlwaysOnTop", "true") --始终显示在最上方
MainUI_ShieldClick(g_ExitWindow) --屏蔽界面鼠标事件

--g_WinMgr:getWindow("MainRoot/VMenubar/Btn_System"):subscribeEvent("Clicked", "OnExit")

ExitWindowFlag = false

function OnExit()
	if ExitWindowFlag == false then	
		g_MainUIWindows:addChildWindow(g_ExitWindow)
		Common_setModalState(g_ExitWindow:getName(), true)
		g_ExitWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,229), CEGUI.UDim(0,117)))
		EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit","您确定退出么？")
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):removeEvent("Clicked")
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):removeEvent("Clicked")
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):setVisible(true)
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):subscribeEvent("Clicked", "CancelExit")    --取消退出事件
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):subscribeEvent("Clicked", "ConfirmExit")
		ExitWindowFlag = true
		g_ExitWindow:activate()
		return
	end
	
	if ExitWindowFlag == true then
		ExitWnd_closeUI()
	end
	
end

--
function ExitWnd_closeUI()
	g_MainUIWindows:removeChildWindow(g_ExitWindow)
	ExitWindowFlag = false
end

function ConfirmExit()
	EQGlobaFunc.ExitClient()	--完全退出客户端(hezhr)
	-- EQGlobaFunc.toMultiLineBox("MainRoot/MTxt_ShowChatbox"):clearUp()	-- 登录前清空聊天输出框内容(hezhr)
	-- g_mainplayer:getTask():clearTaskList()	-- 清空任务面板上的任务(hezhr)
	-- g_MainUIWindows:removeChildWindow(g_ExitWindow)
	-- g_GuiSystem:setGUISheet(g_LoginWindows)
	-- EQGlobaFunc.ExitApp()
	-- ExitWindowFlag = false
end

function CancelExit()
	Common_setModalState(g_ExitWindow:getName(), false)
	g_MainUIWindows:removeChildWindow(g_ExitWindow)
	ExitWindowFlag = false
end

--临时Npc对话框
function OnNpcDlg()
	if ExitWindowFlag == false then	
		g_MainUIWindows:addChildWindow(g_ExitWindow)
		EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit","您好，欢迎来到EQ家园！")
		g_ExitWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,230), CEGUI.UDim(0,117)))
		g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setPosition(CEGUI.UVector2(CEGUI.UDim(0.5,-100), CEGUI.UDim(0.5,-30)))
		
		g_WinMgr:getWindow("Wnd_Confirm/STxt_Exit"):setSize(CEGUI.UVector2(CEGUI.UDim(0,200), CEGUI.UDim(0,30)))
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):removeEvent("Clicked")
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):subscribeEvent("Clicked", "CancelExit")    --取消退出事件
		g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):setVisible(false)
		ExitWindowFlag = true
		g_ExitWindow:activate()
		return
	end
	
	if ExitWindowFlag == true then
		g_MainUIWindows:removeChildWindow(g_ExitWindow)
		ExitWindowFlag = false
	end
	
end