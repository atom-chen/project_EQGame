--
function clientLoginLoads()
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
	g_Loger:logMessage("clientLoginLoads script loading...")


	-- Interface 根目录
	--g_ifpath = "../../Product/Media/Interface/"
	g_ifpath = "../Media/Interface/"

	-- Globals 全局化的东西全部放在这里
	-- require("./Glues/GlobaLs.lua")
	dofile(g_ifpath.."Frames/timer.lua")
	dofile(g_ifpath.."Glues/Const.lua")
	dofile(g_ifpath.."Glues/GlobalsClient.lua")
	dofile(g_ifpath.."Frames/reloadui.lua")

	-----------------------------------------------
	--通用函数脚本文件
	dofile(g_ifpath.."Frames/CommonFun.lua")
	--通用模块
	dofile(g_ifpath.."Frames/page.lua")
	-- 加载界面方案,加载Layout文件
	dofile(g_ifpath.."Glues/InitialClient.lua")
	--dofile("../Media/Interface/Glues/Initial.lua")

	-- UI窗口名称变量统一声明
	dofile(g_ifpath.."Frames/UIWindowName.lua")

	-- 登录脚本
	dofile(g_ifpath.."Glues/LoginClient.lua")

	-- 加载CreatePlayer.lua文件
	dofile(g_ifpath.."Glues/CreatePlayer.lua")

	-- 加载LoadMesh.lua文件
	dofile(g_ifpath.."Glues/LoadMesh.lua")
	--
	-------------------------------------------------
	dofile(g_ifpath.."Frames/MainUI.lua")
	dofile(g_ifpath.."Frames/Guide.lua")
	--
	---- 其它游戏中的脚本处理
	--dofile(g_ifpath.."Frames/ExitWnd.lua")
	--dofile(g_ifpath.."Frames/ChatSet.lua")
	dofile(g_ifpath.."Frames/PlayerPropertyUI.lua")
	PlayerPropertyUIOnLoad()
	
	g_Loger:logMessage("clientLoginLoads load script done.")
	--g_Loger:logDebug("ok, game init all done.")
	
	-- OnReceiveHandle 消息接收全部放这里
	dofile(g_ifpath.."Frames/OnReceiveHandle.lua")
	
	dofile(g_ifpath.."Frames/MsgBox.lua")
end