-----------------------------------------
-- 文件说明	：Lua文件初始化相关操作
-- 作者		：
-- 修改		：麦
-- 日期		：10/03/25
-----------------------------------------

function initialClientGame()
	-------------------------------------------
	---- 加载界面方案
	-------------------------------------------
	--g_SchemeMgr:loadScheme("LoadIcon.scheme")
	--g_SchemeMgr:loadScheme("TaharezLook.scheme")
	--g_SchemeMgr:loadScheme("EQHomeLookSkin.scheme")
	--
	-------------------------------------------
	---- 设置默认样式
	-------------------------------------------
	----
	----创建默认乐园界面根窗口
	--g_UIRoot = g_WinMgr:createWindow("DefaultWindow", "UIRoot")
	--
	---- 设置默认鼠标指针样式
	--g_GuiSystem:setDefaultMouseCursor("MouseArrow", "MouseArrow")
	--g_GuiSystem:setDefaultFont("simsun-10")
	--
	---- 设置默认工具拦提示样式
	--g_GuiSystem:setDefaultTooltip("TaharezLook/Tooltip")
	-------------------------------------------
	---- 加载Layout文件
	-------------------------------------------
	--g_LoginWindows 		= g_WinMgr:loadWindowLayout("LoginWindows.layout")
	--g_MessageBox 		= g_WinMgr:loadWindowLayout("MessageBox.layout")
	--g_CreatePlayer 		= g_WinMgr:loadWindowLayout("CreatePlayer.layout")
	----
	--g_MainUIWindows 	= g_WinMgr:loadWindowLayout("MainUI.layout")
	g_ExitWindow 		= g_WinMgr:loadWindowLayout("ExitWnd.layout")
	g_FitmodeStorage 	= g_WinMgr:loadWindowLayout("FitmodeStorage.layout")
	--仓库界面
	g_Storage 			= g_WinMgr:loadWindowLayout("StorageUI.layout")
	--人物属性界面
	--g_PlayerProUI 		= g_WinMgr:loadWindowLayout("PlayerPropertyUI.layout")
	--g_OtherPlayerProUI  = g_WinMgr:loadWindowLayout("OtherPlayerInfoUI.layout")
	--动作按钮界面
	g_ActionUI			= g_WinMgr:loadWindowLayout("ActionWnd.layout")
	--环形菜单界面
	g_circleMenuUI		= g_WinMgr:loadWindowLayout("circleMenu.layout")

	g_DealUI					= g_WinMgr:loadWindowLayout("deal.layout")

	g_FriendListUI		= g_WinMgr:loadWindowLayout("FriendList.layout")
	--好友列表弹出菜单界面
	g_FriendLstpopUpUI	= g_WinMgr:loadWindowLayout("FriendList_manage.layout")
	--
	g_GMFriendpopUpUI = g_WinMgr:loadWindowLayout("GMFriend_manage.layout")
	--
	g_goToFriendHouseUI = g_WinMgr:loadWindowLayout("Gotohouse.layout")

	--房屋售卖
	g_houseSalesUI =  g_WinMgr:loadWindowLayout("houseshop.layout")

	--商城
	g_ShopCenterUI			= g_WinMgr:loadWindowLayout("Market.layout")
	g_FitModeShopCenter = g_WinMgr:loadWindowLayout("FitmodeMkt.layout")
	g_FitModeShoppingCart = g_WinMgr:loadWindowLayout("Shoplist.layout")

	----家政背景
	g_BackGroundUI = g_WinMgr:loadWindowLayout("housesweep.layout")
	g_AddressBookUI = g_WinMgr:loadWindowLayout("addressbook.layout")
	--任务界面
	g_Task          = g_WinMgr:loadWindowLayout("Taskboard.layout")

	--NPC商店界面
	g_NpcShop       = g_WinMgr:loadWindowLayout("NpcShop.layout")

	--物品拆分
	g_SplitWnd = g_WinMgr:loadWindowLayout("chaifen.layout")

	--房屋信息
	g_houseInfoUI = g_WinMgr:loadWindowLayout("housedata.layout")

	--娱乐界面
	g_funnyUI = g_WinMgr:loadWindowLayout("game.layout")

	--打工选择界面
	g_chooseWorksUI = g_WinMgr:loadWindowLayout("eqwork.layout")
	g_workUI = g_WinMgr:loadWindowLayout("work.layout")
	g_workConfirmUI = g_WinMgr:loadWindowLayout("workhint.layout")
	--
	--邮件面板
	g_MailBoxLayout = g_WinMgr:loadWindowLayout("mailbox.layout")
	--邮件警告面板
	g_mailWarmingUI = g_WinMgr:loadWindowLayout("BulletinBoard.layout")

	--家政确认框
	g_houseServiceTip = g_WinMgr:loadWindowLayout("housesweepcheck.layout")

	--房屋欢迎语
	g_WelcomeUI = g_WinMgr:loadWindowLayout("housewelcome.layout")

	--新手引导界面
	g_PlayerguideUI = g_WinMgr:loadWindowLayout("PlayerGuideUI.layout")

	--仓库界面
	g_storageUI = g_WinMgr:loadWindowLayout("StoreUI.layout")

	--仓库金钱输入界面
	g_storageInputMoneyUI = g_WinMgr:loadWindowLayout("storemoney.layout")

	--聊天表情界面(hezhr)
	g_ChatExpressionUI = g_WinMgr:loadWindowLayout("ChatExpressionUI.layout")
	--美容院界面(hezhr)
	g_BeautyUI = g_WinMgr:loadWindowLayout("beauty.layout")
	--美容院选择界面(hezhr)
	g_BeautyChooseUI = g_WinMgr:loadWindowLayout("beauty_parts_list.layout")
	
	--选择制作礼物界面
	g_chooseMakeGiftUI = g_WinMgr:loadWindowLayout("liwu1.layout")

	--手工制作礼物界面
	g_DIYGiftUI = g_WinMgr:loadWindowLayout("liwu2.layout")

	--快递礼物界面
	g_MailGiftUI = g_WinMgr:loadWindowLayout("liwu3.layout")

	--礼盒选择界面
	g_GiftBoxUI = g_WinMgr:loadWindowLayout("liwuhe.layout")

	--放置回收礼物界面
	g_PutGiftUI = g_WinMgr:loadWindowLayout("shouliwu.layout")

	--查看礼物界面
	g_OpenGiftUI = g_WinMgr:loadWindowLayout("shouliwu1.layout")

	--选择礼物物品
	g_GiftItemUI = g_WinMgr:loadWindowLayout("ChooseGiftItem.layout")

	--礼物卡选择界面
	g_GiftCardUI = g_WinMgr:loadWindowLayout("liwuka.layout")
	
	--礼物卡选择界面
	g_LogGiftUI = g_WinMgr:loadWindowLayout("shouliwu2.layout")
	
	--新手帮助界面
	g_NewBieHelpUI = g_WinMgr:loadWindowLayout("newbiehelp.layout")
	
	--公告界面
	g_BroadCastUI = g_WinMgr:loadWindowLayout("broadcast.layout")
	
	--小喇叭公告界面
	g_BroadCastSpeakerUI = g_WinMgr:loadWindowLayout("broadcast2.layout")
	
	g_ConfigOption = g_WinMgr:loadWindowLayout("Systemconfig.layout")
	
	g_Preview = g_WinMgr:loadWindowLayout("preview.layout")
	--玩家搜索界面
	g_PsInputUI = g_WinMgr:loadWindowLayout("find_friend.layout")
	g_PsOutputUI = g_WinMgr:loadWindowLayout("find_friend_result.layout")
	
	--刺猬教学界面(add by hezhr)
	g_CiWeiTeachUI = g_WinMgr:loadWindowLayout("ciweijiaoxue.layout")
	
	--宝箱
	g_magicBoxPickUI = g_WinMgr:loadWindowLayout("baoxiang_wupin.layout")
	--垃圾
	g_garbagePickUI = g_WinMgr:loadWindowLayout("laji_wupin.layout")
	--截图到博客界面
	g_jietuBlogUI = g_WinMgr:loadWindowLayout("jietu_blog.layout")
	--微博验证界面
	g_blogVerifierUI = g_WinMgr:loadWindowLayout("yanzheng-qq-blog.layout")
	--物品交互界面
	g_ItemInteractUI = g_WinMgr:loadWindowLayout("jiaohu.layout")
	--离线消息和设置界面
	g_OfflineMsgMgrUI = g_WinMgr:loadWindowLayout("Frienlist_sys.layout")
	-- 迷你地图
	g_miniMap = g_WinMgr:loadWindowLayout("minimap.layout")
	-- 照相机
	g_cameraUI = g_WinMgr:loadWindowLayout("zhaoxiangji.layout")
	-- 照相机上传界面
	g_cameraUploadUI = g_WinMgr:loadWindowLayout("updatebtn.layout")
	-- 照相机留言界面
	g_cameraLiuyanUI = g_WinMgr:loadWindowLayout("zxliuyan.layout")
	---------------------
	--漂流瓶
	g_DriftStartUI = g_WinMgr:loadWindowLayout("drifting1.layout")
	--
	g_selectBottleTypeUI = g_WinMgr:loadWindowLayout("Type.layout")
	--
	g_ContactBottleEditUI = g_WinMgr:loadWindowLayout("contact.layout")
	--普通瓶子信息填写界面
	g_NormalDriftEditUI = g_WinMgr:loadWindowLayout("ordinary.layout")
	--我的瓶子记录界面
	g_MyBottleRecordsUI = g_WinMgr:loadWindowLayout("Bottle.layout")
	g_MyBottleItemMoveInUI =  g_WinMgr:loadWindowLayout("DriftBottleItemMoveInUI.layout")
	g_MyBottleDelBtnUI = g_WinMgr:loadWindowLayout("DriftBottleDelBtn.layout")
	g_BottleDetailInfoUI = g_WinMgr:loadWindowLayout("countkan.layout")
	--
	g_RengAnimatUI = g_WinMgr:loadWindowLayout("reng.layout")
	--
	g_PickAnimatUI = g_WinMgr:loadWindowLayout("lao.layout")
	g_DiseaseSpecialEventUI  = g_WinMgr:loadWindowLayout("DiseaseSpecialEvent.layout")
	
	--鲜花鸡蛋
	g_FlowerLog = g_WinMgr:loadWindowLayout("flower_history.layout")
	g_EggLog = g_WinMgr:loadWindowLayout("egg_history.layout")
	g_Fne = g_WinMgr:loadWindowLayout("fne.layout")
	g_FnePreview = g_WinMgr:loadWindowLayout("fnepreview.layout")
	--大表情界面
	g_BigExpressionUI = g_WinMgr:loadWindowLayout("BigExpressionUI.layout")
end