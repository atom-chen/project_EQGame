
function debugDoFile(file)
	g_Loger:logMessage("StartDoFile:" .. file)
	dofile(file)
	g_Loger:logMessage("EndDoFile.")
end

function debugGetWindow(winName)
	local win = g_WinMgr:getWindow(winName)
	
	if win == nil then
		g_Loger:logError("Win '" .. winName .. "' not valid!")
	end
		
	return win
end

function clientGameLoads()

	--EQLog管理


	--设置LOG模式
	--"setLogMode",
	--("LOG_MODE_NONE", 	没使用LOG
	--("LOG_MODE_FILE", 	记录到文件, ../log目录下
	--("LOG_MODE_CONSOLE", 	记录到控制台窗口
	--("LOG_MODE_BOTH",		上面两种方式同时开启

	--设置LOG级别
	--"setLogLevel",
	--("LOG_TYPE_ALL", 		全部开启
	--("LOG_TYPE_DEBUG", 	调试		getLogger():logDebug
	--("LOG_TYPE_MESSAGE", 	消息		getLogger():logMessage
	--("LOG_TYPE_WARNING", 	警报		getLogger():logWarning
	--("LOG_TYPE_ERROR", 	错误		getLogger():logError
	--("LOG_TYPE_TRACE", 	跟踪		getLogger():logMessage

	g_Loger			=	EQGlobaClass.getLogger()
	--测试版本
	g_Loger:setLogMode(EQGlobaClass.EQLog.LOG_MODE_BOTH)
	--g_Loger:setLogLevel(EQGlobaClass.EQLog.LOG_TYPE_ERROR)
	--发行版本
	--g_Loger:setLogMode(EQGlobaClass.EQLog.LOG_MODE_FILE)
	--g_Loger:logDebug("乐园客户端准备OK。client ready.")
	g_Loger:logMessage("clientGameLoads script loading...")


	-- Interface 根目录
	--g_ifpath = "../../Product/Media/Interface/"
	g_ifpath = "../Media/Interface/"

	-- Globals 全局化的东西全部放在这里
	-- require("./Glues/GlobaLs.lua")
	--dofile(g_ifpath.."Glues/Const.lua")
	--dofile(g_ifpath.."Glues/GlobalsClient.lua")
	--dofile(g_ifpath.."Frames/reloadui.lua")


	-- 加载界面方案,加载Layout文件
	--dofile(g_ifpath.."Glues/InitialClient.lua")
	--dofile("../Media/Interface/Glues/Initial.lua")

	-- UI窗口名称变量统一声明
	--dofile(g_ifpath.."Frames/UIWindowName.lua")

	-- 登录脚本
	--dofile(g_ifpath.."Glues/LoginClient.lua")

	-- 加载CreatePlayer.lua文件
	--dofile(g_ifpath.."Glues/CreatePlayer.lua")

	-- 加载LoadMesh.lua文件
	--dofile(g_ifpath.."Glues/LoadMesh.lua")
	-----------------------------------------------
	--通用函数脚本文件
	--dofile(g_ifpath.."Frames/CommonFun.lua")
	--
	-------------------------------------------------
	--dofile(g_ifpath.."Frames/MainUI.lua");
	--
	---- 其它游戏中的脚本处理
	dofile(g_ifpath.."Frames/ExitWnd.lua");
	dofile(g_ifpath.."Frames/ChatSet.lua");
	--dofile(g_ifpath.."Frames/PlayerPropertyUI.lua")
	OnLoadPlayerInfo()
	--
	----仓库界面------------------------------
	dofile(g_ifpath.."Frames/StorageUI.lua")
	StorageUI_Onload()
	--
	----装备栏界面----------------------------------
	dofile(g_ifpath.."Frames/EquipmentHandle.lua")
	EquipmentHandle_Onload()
	--
	----家装界面------------------------------
	dofile(g_ifpath.."Frames/Fitmode.lua")
	FitMod_Onload()
	--
	----
	dofile(g_ifpath.."Frames/Action.lua")
	--
	----环形菜单界面
	dofile(g_ifpath.."Frames/circleMenu.lua")
	circleMenu_Onload()
	--
	----好友界面
	dofile(g_ifpath.."Frames/FriendList.lua")
	--
	----玩家搜索
	dofile(g_ifpath.."Frames/PlayerSearch.lua")
	----好友列表弹出菜单界面
	--dofile(g_ifpath.."Frames/FriendListpopUpWin.lua")
	----friendListpopUpWin_onLoad()
	--
	----NPC界面
	dofile(g_ifpath.."Frames/NPCDialog.lua")
	NPCDialog_Onload()
	--
	----疾病系统
	dofile(g_ifpath.."Frames/Disease.lua")
	DiseaseRegEvent()
	----农场界面
	dofile(g_ifpath.."Frames/Farm_new.lua")
	--
	----路标界面
	--dofile(g_ifpath.."Frames/Guide.lua")
	--
	dofile(g_ifpath.."Frames/dtime.lua")
	--
	----UI事件注册
	dofile(g_ifpath.."Glues/UIEventReg.lua")
	--
	-------------------------------------------------
	----交易模块
	dofile(g_ifpath.."Frames/Deal.lua")
	DealPanleOnLoad()
	--
	--
	-------------------------------------------------
	----邀请模块
	dofile(g_ifpath.."Frames/Invition.lua")
	InviteDialogOnload()		
	-----------------------------------------------
	--商城
	dofile(g_ifpath.."Frames/ShopCenter.lua")
	ShopCenterUI_Onload()
	--
	dofile(g_ifpath.."Frames/FitModeShopCenter.lua")
	FitModeShopCenterUI_Onload()
	-----------------------------------------------
	--任务
	dofile(g_ifpath.."Frames/Task.lua")
	Task_Onload()
	
	-----------------------------------------------
	--npc商店
	--dofile(g_ifpath.."Frames/NpcShop.lua")
	--NpcShop_Onload()
	
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
	dofile(g_ifpath.."Frames/HouseInfo.lua")
	HouseInfoOnload()
	
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
	-----------------------------------------------
	--邮件
	dofile(g_ifpath.."Frames/Mail.lua")
	OnloadMailUI()
	
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
	--处理聊天表情相关的逻辑(hezhr)
	dofile(g_ifpath.."Frames/ChatExpression.lua")
	-----------------------------------------------
	--处理公告系统相关的逻辑(hezhr)
	dofile(g_ifpath.."Frames/BroadCast.lua")
	-----------------------------------------------
	--处理美容院的逻辑(hezhr)
	dofile(g_ifpath.."Frames/beauty.lua")
	-----------------------------------------------
	dofile(g_ifpath.."Frames/DynamicMsg.lua")
	-----------------------------------------------
	--仓库
	dofile(g_ifpath.."Frames/Storage.lua")
	StorageOnload()
	--礼物
	dofile(g_ifpath.."Frames/GiftSystem.lua")
	OnLoadGiftSystem()
	-----------------------------------------------
	--新手帮助
	dofile(g_ifpath.."Frames/PlayerGuide.lua")
	dofile(g_ifpath.."Frames/PlayerGuideScripts.lua")
	-----------------------------------------------
	--微博
	dofile(g_ifpath.."Frames/MicroBlog.lua")
	-----------------------------------------------
	--小地图
	dofile(g_ifpath.."Frames/SmallMap.lua")
	
	g_Loger:logMessage("clientGameLoads load script done.")
	--g_Loger:logDebug("ok, game init all done.")
	-----------------------------------------------
	dofile(g_ifpath.."Frames/Systemconfig.lua")
	ConfigOption_Onload()
	--物品交互
	dofile(g_ifpath.."Frames/ItemInteract.lua")
	IIT_onLoadInit()
	------
	--
	dofile(g_ifpath.."Frames/DriftBottleSystem.lua")
	
	----预览
	dofile(g_ifpath.."Frames/preview.lua")
	Preview_Onload()
	
	----物品使用
	dofile(g_ifpath.."Frames/UseItem.lua")
	--
	dofile(g_ifpath.."Frames/FriendList_offlineMsg_Mgr.lua")
	FLofflineMsgMgr_onLoadInit()
	
	dofile(g_ifpath.."Frames/UIVoice.lua")
	--防沉迷
	dofile(g_ifpath.."Frames/WallowTime.lua")
	
	--垃圾回收
	dofile(g_ifpath.."Frames/garbage.lua")
	
	----界面管理
	dofile(g_ifpath.."Frames/UIManager.lua")

	--鲜花鸡蛋
	dofile(g_ifpath.."Frames/fne.lua")
	Fne_Onload()
	dofile(g_ifpath.."Frames/FlowerEggLog.lua")
	FlowerEggLog_Onload()
	--大表情
	dofile(g_ifpath.."Frames/BigExpression.lua")
	--帮助按钮功能
	dofile(g_ifpath.."Frames/UIHelpBtnShowFunc.lua")
end