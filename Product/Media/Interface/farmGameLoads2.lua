--------------------------------------------------------------
-- 登录场景处理
--------------------------------------------------------------
--function Farm_OnLoadScene()
--	--print("succ")
--	--print("加载场景！")
--	if g_clientGameInited == false then
--		initialFarmGame()
--		farmGameLoads()
--		g_clientGameInited = true
--	end
--	
--	--EQGlobaFunc.LoadScene()
--	--local avatar 		= g_rtt:getAvatar()
--	--avatar:getAdjuster():setForCreateRole(false)
--------
--	----已不用这种做法了
--	----LoadMesh_LoadMesh()
--	--g_GuiSystem:setGUISheet(g_UIRoot)
--	--g_UIRoot:addChildWindow(g_MainUIWindows)
--	g_FriendSys:reqFriendList()
--end

function farmGameLoads()
	-- 其它游戏中的脚本处理
	--dofile(g_ifpath.."Frames/ExitWnd.lua")
	dofile(g_ifpath.."Frames/ChatSet.lua")
	dofile(g_ifpath.."Frames/PlayerPropertyUI.lua")

	--仓库界面------------------------------
	dofile(g_ifpath.."Frames/StorageUI.lua")
	StorageUI_Onload()

	--装备栏界面----------------------------------
	dofile(g_ifpath.."Frames/EquipmentHandle.lua")
	EquipmentHandle_Onload()

	--家装界面------------------------------
	dofile(g_ifpath.."Frames/Fitmode.lua")
	FitMod_Onload()

	--
	dofile(g_ifpath.."Frames/Action.lua")

	--环形菜单界面
	dofile(g_ifpath.."Frames/circleMenu.lua")
	circleMenu_Onload()

	--好友界面
	dofile(g_ifpath.."Frames/FriendList.lua")

	--好友列表弹出菜单界面
	--dofile(g_ifpath.."Frames/FriendListpopUpWin.lua")
	--friendListpopUpWin_onLoad()

	--NPC界面
	dofile(g_ifpath.."Frames/NPCDialog.lua")
	NPCDialog_Onload()

	--疾病系统
	dofile(g_ifpath.."Frames/Disease.lua")
	DiseaseRegEvent()



	--UI事件注册
	dofile(g_ifpath.."Glues/UIEventReg.lua")

	-----------------------------------------------
	--交易模块
	dofile(g_ifpath.."Frames/Deal.lua")
	DealPanleOnLoad()


	-----------------------------------------------
	--邀请模块
	dofile(g_ifpath.."Frames/Invition.lua")
	InviteDialogOnload()
	dofile(g_ifpath.."Frames/page.lua")

	-----------------------------------------------
	--农场界面
	dofile(g_ifpath.."Frames/farm/Farm.lua")
	--
	dofile(g_ifpath.."Frames/farm/FarmFriendList.lua")
	--
	dofile(g_ifpath.."Frames/farm/FarmStore.lua")
	FShopUIOnLoad()
	--Farm_onLoad()
	--庄园小系统模块
	--dofile(g_ifpath.."Frames/FarmSmallSystem.lua")

	dofile(g_ifpath.."Frames/farm/FarmPlayerInfo.lua")
	FPlayerInfoOnload()

	dofile(g_ifpath.."Frames/farm/FarmSystemSetting.lua")
	FSettingUIOnLoad()

	-----------------------------------------------
	--庄园仓库
	dofile(g_ifpath.."Frames/farm/FarmStorage.lua")
	FStorageOnLoad()

	--农场装饰
	dofile(g_ifpath.."Frames/farm/FarmDecoration.lua")
	-----------------------------------------------
	--农场种子包
	dofile(g_ifpath.."Frames/farm/FarmSeedBag.lua")
	OnLoadSeedBagUI()
	--
	dofile(g_ifpath.."Frames/farm/FarmTip.lua")
	-----------------------------------------------
	--农场任务(2011/4/14 add by hezhr)
	dofile(g_ifpath.."Frames/farm/FarmTask.lua")
	FarmTask_subscribeScripts()
	-----------------------------------------------
	--商城
	dofile(g_ifpath.."Frames/ShopCenter.lua")
	ShopCenterUI_Onload()
	--
	dofile(g_ifpath.."Frames/FitModeShopCenter.lua")
	FitModeShopCenterUI_Onload()

	--通用模块
	dofile(g_ifpath.."Frames/page.lua")

	-----------------------------------------------
	--任务
	dofile(g_ifpath.."Frames/Task.lua")
	Task_Onload()

	-----------------------------------------------
	--npc商店
	dofile(g_ifpath.."Frames/NpcShop.lua")
	NpcShop_Onload()

	-----------------------------------------------
	--动态UI
	dofile(g_ifpath.."Frames/DynamicUI.lua")
	-----------------------------------------------
	--房屋买卖
	dofile(g_ifpath.."Frames/HouseSales.lua")
	HSOnloadUI()

	-----------------------------------------------
	--物品拖动
	dofile(g_ifpath.."Frames/DragItem.lua")
	DragItem_Onload()
	-----------------------------------------------
	--家政
	dofile(g_ifpath.."Frames/DomesticService.lua")
	DomesticServiceOnLoad()

	-----------------------------------------------
	--物品拆分
	dofile(g_ifpath.."Frames/SplitItem.lua")
	-----------------------------------------------
	--房屋信息
	--dofile(g_ifpath.."Frames/HouseInfo.lua")
	--HouseInfoOnload()

	-----------------------------------------------
	--消息提示
	dofile(g_ifpath.."Glues/MsgBox.lua")
	-----------------------------------------------
	--娱乐界面
	dofile(g_ifpath.."Frames/FunnyPanel.lua")
	OnLoadFunnyUI()
	-----------------------------------------------
	--打工
	dofile(g_ifpath.."Frames/PlayerWorks.lua")
	OnLoadWorksUI()
	-----------------------------------------------
	--邮件
	dofile(g_ifpath.."Frames/Mail.lua")
	OnloadMailUI()
	--
	dofile(g_ifpath.."Frames/MsgBox.lua")
	-----------------------------------------------
	----路标界面
	--dofile(g_ifpath.."Frames/Guide.lua")
	--
	
	dofile(g_ifpath.."Frames/dtime.lua")

	-- OnReceiveHandle 消息接收全部放这里
	dofile(g_ifpath.."Frames/OnReceiveHandle.lua")

	-----------------------------------------------
	--camera脚本文件
	dofile(g_ifpath.."Glues/EQCamera.lua")
	EQCamera_Onload()

	-----------------------------------------------
	--itemhint(hezhr)
	dofile(g_ifpath.."Frames/Itemhint.lua")
	-----------------------------------------------
	--图文混排控件配置文件(hezhr)
	dofile(g_ifpath.."Glues/MultiLineBox_ConfigFile.lua")
	-----------------------------------------------
	--处理聊天表情逻辑(hezhr)
	dofile(g_ifpath.."Frames/ChatExpression.lua")
	-----------------------------------------------
	--处理公告系统相关的逻辑(hezhr)
	dofile(g_ifpath.."Frames/farm/FarmBroadCast.lua")
	-----------------------------------------------
	dofile(g_ifpath.."Frames/DynamicMsg.lua")

	-----------------------------------------------
	--仓库
	dofile(g_ifpath.."Frames/Storage.lua")
	StorageOnload()
	-----------------------------------------------
	--红土地
	dofile(g_ifpath.."Frames/farm/RedLand.lua")
	OnLoadRedLand()
	--狗和狗粮
	dofile(g_ifpath.."Frames/farm/DogNFood.lua")
	DogNFood_onLoad()
	--防沉迷
	dofile(g_ifpath.."Frames/farm/FarmWallowTime.lua")
	
	--垃圾回收
	dofile(g_ifpath.."Frames/garbage.lua")

	g_Loger:logMessage("farmGameLoads done.")
end

