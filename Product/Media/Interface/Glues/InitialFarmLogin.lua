-----------------------------------------
-- 文件说明	：Lua文件初始化相关操作
-- 作者		：
-- 修改		：麦
-- 日期		：10/03/25
-----------------------------------------
function initialFarmLogin()
	-----------------------------------------
	-- 加载界面方案
	-----------------------------------------
	g_SchemeMgr:loadScheme("LoadIcon.scheme")
	g_SchemeMgr:loadScheme("TaharezLook.scheme")
	g_SchemeMgr:loadScheme("EQHomeLookSkin.scheme")

	-----------------------------------------
	-- 设置默认样式
	-----------------------------------------
	--创建默认农场界面根窗口
	g_UIRoot = g_WinMgr:createWindow("DefaultWindow", "UIRoot")
	-- 设置默认鼠标指针样式
	g_GuiSystem:setDefaultMouseCursor("MouseArrow", "MouseArrow")
	g_GuiSystem:setDefaultFont("simsun-10")

	-- 设置默认工具拦提示样式
	g_GuiSystem:setDefaultTooltip("NewEQHomeLook/NewTooltip")
	CEGUI.toTooltip(g_GuiSystem:getDefaultTooltip()):setHoverTime(0)
	CEGUI.toTooltip(g_GuiSystem:getDefaultTooltip()):setHoverTime(0)
	-----------------------------------------
	-- 加载Layout文件
	-----------------------------------------
	g_LoginWindows 		= g_WinMgr:loadWindowLayout("LoginWindows.layout")
	g_MessageBox 		= g_WinMgr:loadWindowLayout("MessageBox.layout")
	g_CreatePlayer 		= g_WinMgr:loadWindowLayout("CreatePlayer.layout")
	g_MainUIWindows 	= g_WinMgr:loadWindowLayout("MainUI.layout")
	g_ExitWindow 		= g_WinMgr:loadWindowLayout("ExitWnd.layout")
	
	--loading
	g_LoadingUI = g_WinMgr:loadWindowLayout("Loading.layout")
	
	--农场主界面
	g_FarmUI = g_WinMgr:loadWindowLayout("Farm.layout")
	g_UIRoot:addChildWindow(g_FarmUI)
	
	-------- XJ ---------------------------------------------------------------------------------------
	g_PlayerAttrUI      = g_WinMgr:loadWindowLayout("Player-attribute.layout")--主界面人物小属性框
	g_smallMapUI		= g_WinMgr:loadWindowLayout("map.layout")--小地图
	g_MainUIWindows:addChildWindow(g_smallMapUI)
	g_gotoUI			= g_WinMgr:loadWindowLayout("goto.layout")--路标
	---------------------------------------------------------------------------------------------------
end