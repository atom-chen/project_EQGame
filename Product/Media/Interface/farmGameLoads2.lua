--------------------------------------------------------------
-- ��¼��������
--------------------------------------------------------------
--function Farm_OnLoadScene()
--	--print("succ")
--	--print("���س�����")
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
--	----�Ѳ�������������
--	----LoadMesh_LoadMesh()
--	--g_GuiSystem:setGUISheet(g_UIRoot)
--	--g_UIRoot:addChildWindow(g_MainUIWindows)
--	g_FriendSys:reqFriendList()
--end

function farmGameLoads()
	-- ������Ϸ�еĽű�����
	--dofile(g_ifpath.."Frames/ExitWnd.lua")
	dofile(g_ifpath.."Frames/ChatSet.lua")
	dofile(g_ifpath.."Frames/PlayerPropertyUI.lua")

	--�ֿ����------------------------------
	dofile(g_ifpath.."Frames/StorageUI.lua")
	StorageUI_Onload()

	--װ��������----------------------------------
	dofile(g_ifpath.."Frames/EquipmentHandle.lua")
	EquipmentHandle_Onload()

	--��װ����------------------------------
	dofile(g_ifpath.."Frames/Fitmode.lua")
	FitMod_Onload()

	--
	dofile(g_ifpath.."Frames/Action.lua")

	--���β˵�����
	dofile(g_ifpath.."Frames/circleMenu.lua")
	circleMenu_Onload()

	--���ѽ���
	dofile(g_ifpath.."Frames/FriendList.lua")

	--�����б����˵�����
	--dofile(g_ifpath.."Frames/FriendListpopUpWin.lua")
	--friendListpopUpWin_onLoad()

	--NPC����
	dofile(g_ifpath.."Frames/NPCDialog.lua")
	NPCDialog_Onload()

	--����ϵͳ
	dofile(g_ifpath.."Frames/Disease.lua")
	DiseaseRegEvent()



	--UI�¼�ע��
	dofile(g_ifpath.."Glues/UIEventReg.lua")

	-----------------------------------------------
	--����ģ��
	dofile(g_ifpath.."Frames/Deal.lua")
	DealPanleOnLoad()


	-----------------------------------------------
	--����ģ��
	dofile(g_ifpath.."Frames/Invition.lua")
	InviteDialogOnload()
	dofile(g_ifpath.."Frames/page.lua")

	-----------------------------------------------
	--ũ������
	dofile(g_ifpath.."Frames/farm/Farm.lua")
	--
	dofile(g_ifpath.."Frames/farm/FarmFriendList.lua")
	--
	dofile(g_ifpath.."Frames/farm/FarmStore.lua")
	FShopUIOnLoad()
	--Farm_onLoad()
	--ׯ԰Сϵͳģ��
	--dofile(g_ifpath.."Frames/FarmSmallSystem.lua")

	dofile(g_ifpath.."Frames/farm/FarmPlayerInfo.lua")
	FPlayerInfoOnload()

	dofile(g_ifpath.."Frames/farm/FarmSystemSetting.lua")
	FSettingUIOnLoad()

	-----------------------------------------------
	--ׯ԰�ֿ�
	dofile(g_ifpath.."Frames/farm/FarmStorage.lua")
	FStorageOnLoad()

	--ũ��װ��
	dofile(g_ifpath.."Frames/farm/FarmDecoration.lua")
	-----------------------------------------------
	--ũ�����Ӱ�
	dofile(g_ifpath.."Frames/farm/FarmSeedBag.lua")
	OnLoadSeedBagUI()
	--
	dofile(g_ifpath.."Frames/farm/FarmTip.lua")
	-----------------------------------------------
	--ũ������(2011/4/14 add by hezhr)
	dofile(g_ifpath.."Frames/farm/FarmTask.lua")
	FarmTask_subscribeScripts()
	-----------------------------------------------
	--�̳�
	dofile(g_ifpath.."Frames/ShopCenter.lua")
	ShopCenterUI_Onload()
	--
	dofile(g_ifpath.."Frames/FitModeShopCenter.lua")
	FitModeShopCenterUI_Onload()

	--ͨ��ģ��
	dofile(g_ifpath.."Frames/page.lua")

	-----------------------------------------------
	--����
	dofile(g_ifpath.."Frames/Task.lua")
	Task_Onload()

	-----------------------------------------------
	--npc�̵�
	dofile(g_ifpath.."Frames/NpcShop.lua")
	NpcShop_Onload()

	-----------------------------------------------
	--��̬UI
	dofile(g_ifpath.."Frames/DynamicUI.lua")
	-----------------------------------------------
	--��������
	dofile(g_ifpath.."Frames/HouseSales.lua")
	HSOnloadUI()

	-----------------------------------------------
	--��Ʒ�϶�
	dofile(g_ifpath.."Frames/DragItem.lua")
	DragItem_Onload()
	-----------------------------------------------
	--����
	dofile(g_ifpath.."Frames/DomesticService.lua")
	DomesticServiceOnLoad()

	-----------------------------------------------
	--��Ʒ���
	dofile(g_ifpath.."Frames/SplitItem.lua")
	-----------------------------------------------
	--������Ϣ
	--dofile(g_ifpath.."Frames/HouseInfo.lua")
	--HouseInfoOnload()

	-----------------------------------------------
	--��Ϣ��ʾ
	dofile(g_ifpath.."Glues/MsgBox.lua")
	-----------------------------------------------
	--���ֽ���
	dofile(g_ifpath.."Frames/FunnyPanel.lua")
	OnLoadFunnyUI()
	-----------------------------------------------
	--��
	dofile(g_ifpath.."Frames/PlayerWorks.lua")
	OnLoadWorksUI()
	-----------------------------------------------
	--�ʼ�
	dofile(g_ifpath.."Frames/Mail.lua")
	OnloadMailUI()
	--
	dofile(g_ifpath.."Frames/MsgBox.lua")
	-----------------------------------------------
	----·�����
	--dofile(g_ifpath.."Frames/Guide.lua")
	--
	
	dofile(g_ifpath.."Frames/dtime.lua")

	-- OnReceiveHandle ��Ϣ����ȫ��������
	dofile(g_ifpath.."Frames/OnReceiveHandle.lua")

	-----------------------------------------------
	--camera�ű��ļ�
	dofile(g_ifpath.."Glues/EQCamera.lua")
	EQCamera_Onload()

	-----------------------------------------------
	--itemhint(hezhr)
	dofile(g_ifpath.."Frames/Itemhint.lua")
	-----------------------------------------------
	--ͼ�Ļ��ſؼ������ļ�(hezhr)
	dofile(g_ifpath.."Glues/MultiLineBox_ConfigFile.lua")
	-----------------------------------------------
	--������������߼�(hezhr)
	dofile(g_ifpath.."Frames/ChatExpression.lua")
	-----------------------------------------------
	--������ϵͳ��ص��߼�(hezhr)
	dofile(g_ifpath.."Frames/farm/FarmBroadCast.lua")
	-----------------------------------------------
	dofile(g_ifpath.."Frames/DynamicMsg.lua")

	-----------------------------------------------
	--�ֿ�
	dofile(g_ifpath.."Frames/Storage.lua")
	StorageOnload()
	-----------------------------------------------
	--������
	dofile(g_ifpath.."Frames/farm/RedLand.lua")
	OnLoadRedLand()
	--���͹���
	dofile(g_ifpath.."Frames/farm/DogNFood.lua")
	DogNFood_onLoad()
	--������
	dofile(g_ifpath.."Frames/farm/FarmWallowTime.lua")
	
	--��������
	dofile(g_ifpath.."Frames/garbage.lua")

	g_Loger:logMessage("farmGameLoads done.")
end

