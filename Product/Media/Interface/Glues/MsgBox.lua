-----------------------------------------
-- 文件说明	：Lua文件初始化相关操作
-- 作者		：
-- 修改		：麦
-- 日期		：10/03/25
-----------------------------------------
function MessageBox(title,okEvent,cancelEvent)

	if okEvent == "" then
		okEvent = "NormalCancel"
	end
	
	if cancelEvent == "" then
		cancelEvent = "NormalCancel"
	end
	
	g_MainUIWindows:addChildWindow(g_ExitWindow)
	g_ExitWindow:setSize(CEGUI.UVector2(CEGUI.UDim(0,229), CEGUI.UDim(0,117)))
	EQGlobaFunc.SetText("Wnd_Confirm/STxt_Exit",title)
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):removeEvent("Clicked")
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):removeEvent("Clicked")
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):setVisible(true)
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Cancel"):subscribeEvent("Clicked", cancelEvent)
	g_WinMgr:getWindow("Wnd_Confirm/Btn_Confirm"):subscribeEvent("Clicked", okEvent)
	g_ExitWindow:activate()
	g_ExitWindow:setModalState(true)
end

function NormalCancel()
	if false == g_MainUIWindows:isChild(g_ExitWindow) then
		return
	end
	g_ExitWindow:setModalState(false)
	g_MainUIWindows:removeChildWindow(g_ExitWindow)
end