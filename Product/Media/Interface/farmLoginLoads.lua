function farmLoginLoads()
	--EQLog����


	--����LOGģʽ
	--"setLogMode",
	--("LOG_MODE_NONE", 	ûʹ��LOG
	--("LOG_MODE_FILE", 	��¼���ļ�, ../logĿ¼��
	--("LOG_MODE_CONSOLE", 	��¼������̨����
	--("LOG_MODE_BOTH",		�������ַ�ʽͬʱ����

	--����LOG����
	--"setLogLevel",
	--("LOG_TYPE_ALL", 		ȫ������
	--("LOG_TYPE_DEBUG", 	����		getLogger():logDebug
	--("LOG_TYPE_MESSAGE", 	��Ϣ		getLogger():logMessage
	--("LOG_TYPE_WARNING", 	����		getLogger():logWarning
	--("LOG_TYPE_ERROR", 	����		getLogger():logError
	--("LOG_TYPE_TRACE", 	����		getLogger():logMessage

	g_Loger			=	EQGlobaClass.getLogger()
	--���԰汾
	g_Loger:setLogMode(EQGlobaClass.EQLog.LOG_MODE_BOTH)
	--g_Loger:setLogLevel(EQGlobaClass.EQLog.LOG_TYPE_ERROR)
	--���а汾
	--g_Loger:setLogMode(EQGlobaClass.EQLog.LOG_MODE_FILE)
	g_Loger:logDebug("ũ���ͻ���׼��OK��client ready.")
	g_Loger:logMessage("script loading...")


	-- Interface ��Ŀ¼
	--g_ifpath = "../../Product/Media/Interface/"
	g_ifpath = "../Media/Interface/"


	-- Globals ȫ�ֻ��Ķ���ȫ����������
	-- require("./Glues/Globals.lua")
	dofile(g_ifpath.."Frames/timer.lua")
	dofile(g_ifpath.."Glues/Const.lua")
	--dofile(g_ifpath.."Glues/Globals.lua")
	dofile(g_ifpath.."Glues/GlobalsFarm.lua")
	dofile(g_ifpath.."Frames/reloadui.lua")


	-- ���ؽ��淽��,����Layout�ļ�
	dofile(g_ifpath.."Glues/InitialFarm.lua")
	--dofile("../Media/Interface/Glues/Initial.lua")

	-- UI�������Ʊ���ͳһ����
	dofile(g_ifpath.."Frames/UIWindowName.lua")

	-- ��¼�ű�
	dofile(g_ifpath.."Glues/LoginFarm.lua")
	-- ����CreatePlayer.lua�ļ�
	dofile(g_ifpath.."Glues/CreatePlayer.lua")

	-- ����LoadMesh.lua�ļ�
	--dofile(g_ifpath.."Glues/LoadMesh.lua")
	-----------------------------------------------
	--ͨ�ú����ű��ļ�
	dofile(g_ifpath.."Frames/CommonFun.lua")
	-----------------------------------------------
	dofile(g_ifpath.."Frames/MainUI.lua")
	dofile(g_ifpath.."Frames/Guide.lua")
	
	dofile(g_ifpath.."Frames/ExitWnd.lua")
	
	g_GameEventMgr:subscribeEvent("REQUEST_SCENE_OK","Farm_OnLoadScene")

	g_Loger:logMessage("load script done.")
	g_Loger:logDebug("ok, game init all done.")
end