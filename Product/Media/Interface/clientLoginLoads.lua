--
function clientLoginLoads()
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
	--g_Loger:logDebug("��԰�ͻ���׼��OK��client ready.")
	g_Loger:logMessage("clientLoginLoads script loading...")


	-- Interface ��Ŀ¼
	--g_ifpath = "../../Product/Media/Interface/"
	g_ifpath = "../Media/Interface/"

	-- Globals ȫ�ֻ��Ķ���ȫ����������
	-- require("./Glues/GlobaLs.lua")
	dofile(g_ifpath.."Frames/timer.lua")
	dofile(g_ifpath.."Glues/Const.lua")
	dofile(g_ifpath.."Glues/GlobalsClient.lua")
	dofile(g_ifpath.."Frames/reloadui.lua")

	-----------------------------------------------
	--ͨ�ú����ű��ļ�
	dofile(g_ifpath.."Frames/CommonFun.lua")
	--ͨ��ģ��
	dofile(g_ifpath.."Frames/page.lua")
	-- ���ؽ��淽��,����Layout�ļ�
	dofile(g_ifpath.."Glues/InitialClient.lua")
	--dofile("../Media/Interface/Glues/Initial.lua")

	-- UI�������Ʊ���ͳһ����
	dofile(g_ifpath.."Frames/UIWindowName.lua")

	-- ��¼�ű�
	dofile(g_ifpath.."Glues/LoginClient.lua")

	-- ����CreatePlayer.lua�ļ�
	dofile(g_ifpath.."Glues/CreatePlayer.lua")

	-- ����LoadMesh.lua�ļ�
	dofile(g_ifpath.."Glues/LoadMesh.lua")
	--
	-------------------------------------------------
	dofile(g_ifpath.."Frames/MainUI.lua")
	dofile(g_ifpath.."Frames/Guide.lua")
	--
	---- ������Ϸ�еĽű�����
	--dofile(g_ifpath.."Frames/ExitWnd.lua")
	--dofile(g_ifpath.."Frames/ChatSet.lua")
	dofile(g_ifpath.."Frames/PlayerPropertyUI.lua")
	PlayerPropertyUIOnLoad()
	
	g_Loger:logMessage("clientLoginLoads load script done.")
	--g_Loger:logDebug("ok, game init all done.")
	
	-- OnReceiveHandle ��Ϣ����ȫ��������
	dofile(g_ifpath.."Frames/OnReceiveHandle.lua")
	
	dofile(g_ifpath.."Frames/MsgBox.lua")
end