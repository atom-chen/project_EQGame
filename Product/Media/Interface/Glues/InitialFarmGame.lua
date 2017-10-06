-----------------------------------------
-- 文件说明	：Lua文件初始化相关操作
-- 作者		：
-- 修改		：麦
-- 日期		：10/03/25
-----------------------------------------

function initialFarmGame()

	g_FitmodeStorage 	= g_WinMgr:loadWindowLayout("FitmodeStorage.layout")
	--仓库界面
	g_Storage 			= g_WinMgr:loadWindowLayout("StorageUI.layout")
	--人物属性界面
	g_PlayerProUI 		= g_WinMgr:loadWindowLayout("PlayerPropertyUI.layout")
	g_OtherPlayerProUI  = g_WinMgr:loadWindowLayout("OtherPlayerInfoUI.layout")
	--动作按钮界面
	g_ActionUI			= g_WinMgr:loadWindowLayout("ActionWnd.layout")
	--环形菜单界面
	g_circleMenuUI		= g_WinMgr:loadWindowLayout("circleMenu.layout")

	g_DealUI					= g_WinMgr:loadWindowLayout("deal.layout")

	g_FriendListUI		= g_WinMgr:loadWindowLayout("FriendList.layout")
	--好友列表弹出菜单界面
	g_FriendLstpopUpUI	= g_WinMgr:loadWindowLayout("popupWin.layout")
	--
	g_GMFriendpopUpUI = g_WinMgr:loadWindowLayout("GMFriend_manage.layout")

	--房屋售卖
	g_houseSalesUI =  g_WinMgr:loadWindowLayout("houseshop.layout")

	--商城
	g_ShopCenterUI			= g_WinMgr:loadWindowLayout("Market.layout")
	g_FitModeShopCenter = g_WinMgr:loadWindowLayout("FitmodeMkt.layout")
	g_FitModeShoppingCart = g_WinMgr:loadWindowLayout("Shoplist.layout")
	--农场主界面
	--g_FarmUI = g_WinMgr:loadWindowLayout("Farm.layout")
	--农场好友列表
	FarmFriendList = g_WinMgr:loadWindowLayout("FarmFriendList.layout")
	--农场记录界面
	g_FarmRecordUI		= g_WinMgr:loadWindowLayout("FarmRecord.layout")
	--农场商店界面
	g_FarmMarketUI		= g_WinMgr:loadWindowLayout("shangdian.layout")
	--农场商店售卖界面
	--种子
	g_FarmShopSeedUI = g_WinMgr:loadWindowLayout("FarmSeedSaleUI.layout")
	--道具
	g_FarmShopPropUI = g_WinMgr:loadWindowLayout("FarmPropSaleUI.layout")
	--装饰 
	g_FarmShopOrnaUI = g_WinMgr:loadWindowLayout("FarmOrnamentSaleUI.layout")
	--农场公告界面
	--g_FarmBoardUI		= g_WinMgr:loadWindowLayout("BulletinBoard.layout")
	--农场仓库界面
	g_FarmStorage		= g_WinMgr:loadWindowLayout("cangku.layout")
	--农场系统设置界面
	g_FarmSystemSetUI	= g_WinMgr:loadWindowLayout("ManorSystemSetting.layout")
	--农场仓库售卖界面
	g_FStorageSaleUI      =  g_WinMgr:loadWindowLayout("maichu.layout")
	--农场个人信息界面
	g_FPlayerInfoUI       = g_WinMgr:loadWindowLayout("FarmPlayerInfo.layout")
	--农场装饰界面
	g_FarmDecUI			= g_WinMgr:loadWindowLayout("farmdecoration.layout")
	--农场作物状态显示界面
	g_FarmCropSta		= g_WinMgr:loadWindowLayout("FarmPacebar.layout")
	--农场种子包界面
	g_FarmSeedBagUI = g_WinMgr:loadWindowLayout("FarmSeedbag.layout")

	--家政背景
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

	--红土地升级界面
	g_upgradeLandUI = g_WinMgr:loadWindowLayout("farmland_upgrade.layout")
	--
	g_redLandRewardUI = g_WinMgr:loadWindowLayout("red_farmland_harvest.layout")
	--
	g_redLandStealRewardUI = g_WinMgr:loadWindowLayout("red_farmland_steal.layout")
	--
	g_TUTSRewardUI = g_WinMgr:loadWindowLayout("turn_soil_attain.layout")
	--
	g_BuyDogFoodUI = g_WinMgr:loadWindowLayout("buy_dog_food.layout")
	--
	g_BuyDogUI = g_WinMgr:loadWindowLayout("buy_dog.layout")
	--
	g_UseDogFoodUI = g_WinMgr:loadWindowLayout("use_dog_food.layout")
	--
	g_RemainDogFoodUI = g_WinMgr:loadWindowLayout("dog_food_remain.layout")
	--
	g_BuyDogFood_ConfirmUI = g_WinMgr:loadWindowLayout("buy_dog_food_confirm.layout")
	--公告界面
	g_BroadCastUI = g_WinMgr:loadWindowLayout("broadcast.layout")
	
	--宝箱
	g_magicBoxPickUI = g_WinMgr:loadWindowLayout("baoxiang_wupin.layout")
	--农场新手引导界面
	g_newBieTaskUI = g_WinMgr:loadWindowLayout("newbie_task.layout")
	--领取农场任务界面
	g_getFarmTaskUI = g_WinMgr:loadWindowLayout("farm_get_task.layout")
	--完成农场任务界面
	g_completeFarmTaskUI = g_WinMgr:loadWindowLayout("farm_complete_task.layout")
	--垃圾
	g_garbagePickUI = g_WinMgr:loadWindowLayout("laji_wupin.layout")
	g_DiseaseSpecialEventUI  = g_WinMgr:loadWindowLayout("DiseaseSpecialEvent.layout")
	
	g_Loger:logMessage("initialFarmGame done.")
end