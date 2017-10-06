-----------------------------------------
-- 文件说明	：Lua文件初始化相关操作
-- 作者		：
-- 修改		：麦
-- 日期		：10/03/25
-----------------------------------------

function initialClientLogin()
	EQGlobaFunc.EQSrtBreak("0")
	-----------------------------------------
	-- 加载界面方案
	-----------------------------------------
	g_SchemeMgr:loadScheme("LoadIcon.scheme")
	g_SchemeMgr:loadScheme("TaharezLook.scheme")
	g_SchemeMgr:loadScheme("EQHomeLookSkin.scheme")
	
	-----------------------------------------
	-- 设置默认样式
	-----------------------------------------
	--e
	--创建默认乐园界面根窗口
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
	--
	g_MainUIWindows 	= g_WinMgr:loadWindowLayout("MainUI.layout")
	g_UIRoot:addChildWindow(g_MainUIWindows)
	--g_ExitWindow 		= g_WinMgr:loadWindowLayout("ExitWnd.layout")
	--g_FitmodeStorage 	= g_WinMgr:loadWindowLayout("FitmodeStorage.layout")
	----仓库界面
	--g_Storage 			= g_WinMgr:loadWindowLayout("StorageUI.layout")
	----人物属性界面
	g_PlayerProUI 		= g_WinMgr:loadWindowLayout("PlayerPropertyUI.layout")
	g_OtherPlayerProUI  = g_WinMgr:loadWindowLayout("OtherPlayerInfoUI.layout")
	--loading
	g_LoadingUI = g_WinMgr:loadWindowLayout("Loading.layout")
	
	-------- XJ ---------------------------------------------------------------------------------------
	g_PlayerAttrUI      = g_WinMgr:loadWindowLayout("Player-attribute.layout")--主界面人物小属性框
	g_smallMapUI		= g_WinMgr:loadWindowLayout("map.layout")--小地图
	g_MainUIWindows:addChildWindow(g_smallMapUI)
	g_gotoUI			= g_WinMgr:loadWindowLayout("goto.layout")--路标
	---------------------------------------------------------------------------------------------------
	
	----动作按钮界面
	--g_ActionUI			= g_WinMgr:loadWindowLayout("ActionWnd.layout")
	----环形菜单界面
	--g_circleMenuUI		= g_WinMgr:loadWindowLayout("circleMenu.layout")
	--
	--g_DealUI					= g_WinMgr:loadWindowLayout("deal.layout")
	--
	--g_FriendListUI		= g_WinMgr:loadWindowLayout("FriendList.layout")
	----好友列表弹出菜单界面
	--g_FriendLstpopUpUI	= g_WinMgr:loadWindowLayout("FriendList_manage.layout")
	----
	--
	----房屋售卖
	--g_houseSalesUI =  g_WinMgr:loadWindowLayout("houseshop.layout")
	--
	----商城
	--g_ShopCenterUI			= g_WinMgr:loadWindowLayout("Market.layout")
	--g_FitModeShopCenter = g_WinMgr:loadWindowLayout("FitmodeMkt.layout")
	--g_FitModeShoppingCart = g_WinMgr:loadWindowLayout("Shoplist.layout")
	--
	------家政背景
	--g_BackGroundUI = g_WinMgr:loadWindowLayout("housesweep.layout")
	--g_AddressBookUI = g_WinMgr:loadWindowLayout("addressbook.layout")
	----任务界面
	--g_Task          = g_WinMgr:loadWindowLayout("Taskboard.layout")
	--
	----NPC商店界面
	--g_NpcShop       = g_WinMgr:loadWindowLayout("NpcShop.layout")
	--
	----物品拆分
	--g_SplitWnd = g_WinMgr:loadWindowLayout("chaifen.layout")
	--
	----房屋信息
	--g_houseInfoUI = g_WinMgr:loadWindowLayout("housedata.layout")
	--
	----娱乐界面
	--g_funnyUI = g_WinMgr:loadWindowLayout("game.layout")
	--
	----打工选择界面
	--g_chooseWorksUI = g_WinMgr:loadWindowLayout("eqwork.layout")
	--g_workUI = g_WinMgr:loadWindowLayout("work.layout")
	----
	----邮件面板
	--g_MailBoxLayout = g_WinMgr:loadWindowLayout("mailbox.layout")
	----邮件警告面板
	--g_mailWarmingUI = g_WinMgr:loadWindowLayout("BulletinBoard.layout")
	--
	----loading
	--g_LoadingUI = g_WinMgr:loadWindowLayout("Loading.layout")
	--
	----家政确认框
	--g_houseServiceTip = g_WinMgr:loadWindowLayout("housesweepcheck.layout")
	--
	----房屋欢迎语
	--g_WelcomeUI = g_WinMgr:loadWindowLayout("housewelcome.layout")
	--
	----新手引导界面
	--g_PlayerguideUI = g_WinMgr:loadWindowLayout("PlayerGuideUI.layout")
	--
	----仓库界面
	--g_storageUI = g_WinMgr:loadWindowLayout("StoreUI.layout")
	--
	----仓库金钱输入界面
	--g_storageInputMoneyUI = g_WinMgr:loadWindowLayout("storemoney.layout")
	--
	----聊天表情界面(hezhr)
	--g_ChatExpressionUI = g_WinMgr:loadWindowLayout("ChatExpressionUI.layout")
	----美容院界面(hezhr)
	--g_BeautyUI = g_WinMgr:loadWindowLayout("beauty.layout")
	--
end