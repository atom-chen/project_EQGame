#include "EQFarmPCH.h"
//EQMain
#include "OgreRootEx.h"
#include "EQMainSingletonManager.h"
#include "GraphicsTable.h"
#include "AvatarTable.h"
#include "SceneManager.h"
#include "ItemTable.h"
#include "HouseTable.h"
#include "LogicTable.h"
#include "CommFunc.h"
#include "Player.h"

#include "GameMgr.h"
#include "EQOgreSys.h"
#include "GameState.h"
#include "Variable.h"
#include "AvatarTable.h"
#include "EQCamera.h"
#include "AppListener.h"
#include "EQWndProc.h"
#include "EQOIS.h"
#include "AvatarRTT.h"
#include "FurnitureRTT.h"
#include "OtherPlayerRTT.h"
#include "LuaManager.h"
#include "GUIManager.h"
#include "MessageManager.h"
#include "GameEventMgr.h"
#include "ChatSystem.h"
#include "FriendSystem.h"
#include "PickMgr.h"
#include "EQGameSceneMgr.h"
#include "SmallModules.h"
#include "NPCTable.h"
#include "EventHandles.h"
#include "FarmTable.h"
#include "DynamicUIMgr.h"
#include "UIAnimationMgr.h"
#include "Hotkey.h"
#include "EQCartoon.h"
#include "TaskTable.h"
#include "Timekeeper.h"
#include "MsgSystem.h"
#include "MailManager.h"
#include "BodyActionMgr.h"
#include "CEGUIEx/CEGUIEx.h"
#include "ChatExpressionTable.h"
#include "BroadCastSystem.h"
#include "DynamicCreateImageset.h"
#include "ExceptionHandler.h"
#include "timer.h"
#include "Login.h"
#include "EQVoiceMgr.h"

bool GameMgr::init(HINSTANCE hInstance)
{
	{//�����嶼������new���������������������û��˳��Ҫ��
	 //�����Ҳ���ȽϺ���ķ�ʽ
		new OgreRootEx("", "", GetGameUserDir() + "EQFarm.Ogre.log");
		new EQMainSingletonManager();

		//EQClient
		new VariableSystem;				//��Ϸ����	
		new EQWndProc;					//window
		new EQOIS;						//OIS
		new EQOgreSys;					//Ogreϵͳ
		new AppListener;				//֡������
		new GameStateMgr;				//��Ϸ״̬����

		new PickMgr();

		mLogin = new Login();
		mMainCamera = new EQCamera;	//�������;
		mMainCamera->listenerEvt();
		mAvatarRTT  = new AvatarRTT;	//Render to texture
		mFurnitureRTT = new FurnitureRTT;
		mOtherPlayerRTT = new OtherPlayerRTT;
		
		//thirdpart lib UI ���� �� Lua����
		CLuaManager::getSingleton();
		CGUIManager::getSingleton();

		new EQGameSceneMgr;
		new GameEventMgr;
		new ChatSystem;
		new FriendSystem;

		new SmallModules;
		new MailManager;
// 		new BodyActionMgr;

		new DynamicUIMgr;
		new UIAnimationMgr;
		new MsgSystem;

		// �����������������(hezhr)
		new ChatExpressionTableManager;
		//	��̬����imageset������(hezhr)
		new DynamicCreateImagesetMgr;
		// ����ϵͳ(hezhr)
		new BroadCastSystem;
		//
		new EQVoiceMgr;
	}

	{/*
	 ��ʼ�����������������ĳ�ʼ��˳��ȡ����������ϵ
	 Ҳ����˵���������������ǰͷ�ȳ�ʼ��������
	 һ����˵ֱ����������
	 */
		sHotkey;
		//LOGMSG("VariableSystem,create window...");
		VariableSystem::getSingleton().Initial("..\\config\\EQFarmSystem.cfg");
		HWND hWnd = EQWndProc::getSingleton().createWindow(hInstance);

		//LOGMSG("EQOgreSys initial...");
		EQOgreSys::getSingleton().initial(&hWnd);

		//LOGMSG("AppListener, GameStateMgr, MainPlayer, EQCamera...");
		AppListener::getSingleton().initial(EQOgreSys::getSingleton().
													getRenderWindow());
		GameStateMgr::getSingleton().init();

		
		//Camera initial
		mMainCamera->initial(EQOgreSys::getSingleton().getCamera());
		mMainCamera->pitch(  45  );
		mMainCamera->lookAt( Ogre::Vector3(0,120,0));
		mMainCamera->zoom(1000);
		mMainCamera->yaw( 180 );

		try
		{
			//LOGMSG("TableMgr.load()...");
			AvatarTableManager::getSingleton().load();
			GraphicsTableManager::getSingleton().load();
			ItemTableManager::getSingleton().load();
			HouseTableManager::getSingleton().load();
			LogicTableManager::getSingleton().load();
			NPCTableMgr::getSingleton().load();
			FarmTableMgr::getSingleton().load();
			TaskTableManager::getSingleton().load();


			//���Ҫ����LUA֮ǰ
			GameEventMgr::getSingleton().regAllEvent();
			//���Ҫ����EQOgreSys֮����CLuaManager֮ǰ
			EQGameSceneMgr::getSingleton().init(EQOgreSys::getSingleton().getSceneManager());

			//LOGMSG("UIManager initial...");
			CGUIManager::getSingleton().Initialize(GetGameUserDir() + "EQFarm.CEGUI.log");

			// ����Ϸ��ʼ��ʱע����չ��cegui�ؼ����೧(hezhr)
			CEGUI::registerAllFactories();
			// ����Ҫ����cegui�ؼ��೧ע��ĺ���(hezhr)
			ChatExpressionTableManager::getSingleton().load();


			//
			//mAvatarRTT->startRTT(0,50000,0);

			//LOGMSG("LuaManager initial...");
			CLuaManager::getSingleton().Initialize();
		}
		catch(...)
		{
			// �������
			EQErrorInfoEx::instance().errorType = "��Դ������";
			// �����κδ�����ԭ�������ٴ��׳�
			throw;
		}

		//LOGMSG("OIS, MessageManager initial...");
		EQOIS::getSingleton().initial((size_t)hWnd);
		CMessageManager::getSingleton().Initialize();

		PickMgr::getSingleton().init();
		SmallModules::getSingleton().init();
		EventHandles::init();

		sEQCartoon;

		EQVoiceMgr::getSingleton().init();

	}

	//
	//LOGMSG("GameMgr init done!");
	GameEventMgr::getSingleton().fireEvent("GAME_INIT_DONE");
	//test
	//TestArgs args;
	//args.i = 555;
	//args.name = "hahaha";
	//GameEventMgr::getSingleton().fireEvent("UPDATE_NPC_UI",&args);

	mAppStarted = true;

	return true;
}
//---------------------------------------------------------------------
bool GameMgr::update(bool active)
{
	//if(!active)
	//	return true;

	CMessageManager::getSingleton().Update();
	GameStateMgr::getSingleton().update();
	//sEventSys.ProcessAllEvent();
	EQOgreSys::getSingleton().renderOneFrame();

	EQGameSceneMgr::getSingleton().update();
	SmallModules::getSingleton().update();

	DynamicUIMgr::getSingleton().update();
	UIAnimationMgr::getSingleton().update();

	BroadCastSystem::getSingleton().update();	// hezhr

	sTimekeeper.update();
	runningTimerMgr();

	return true;
}
//----------------------------------------------------------------------
void GameMgr::cleanup()
{
	/*
	��ɨ�������ɨ��˳��ȡ����������ϵ
	Ҳ����˵��������������ں�ͷ����ɨ��
	һ����˵����ǰ���
	*/
	finishTimerMgr();

	delete EQVoiceMgr::getSingletonPtr();
	delete MsgSystem::getSingletonPtr();
	delete UIAnimationMgr::getSingletonPtr();
	delete DynamicUIMgr::getSingletonPtr();

	delete FriendSystem::getSingletonPtr();

	delete ChatSystem::getSingletonPtr();
	//
	GameEventMgr::getSingletonPtr()->cleanup();
	delete GameEventMgr::getSingletonPtr();
	GameStateMgr::getSingleton().cleanup();
	//ɾ������
	HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	if ( scene )
		EQSceneManager::getSingleton().destroyScene();
	////ɾ����Դ��
	//Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("OgreMax");

	//CMessageManager::getSingleton().Terminate();
	AppListener::getSingleton().cleanup();
	EQOgreSys::getSingleton().cleanup();
	EQOIS::getSingleton().cleanup();
	EQWndProc::getSingleton().cleanup();
	//VariableSystem::getSingleton().cleanup();
	EQGameSceneMgr::getSingleton().cleanup();

	delete mAvatarRTT;
	delete mOtherPlayerRTT;
	delete mFurnitureRTT;
	delete mMainCamera;
	delete mLogin;

	delete PickMgr::getSingletonPtr();
	delete GameStateMgr::getSingletonPtr();
	delete AppListener::getSingletonPtr();
	delete EQOgreSys::getSingletonPtr();
	
	delete EQOIS::getSingletonPtr();
	delete EQWndProc::getSingletonPtr();
	delete VariableSystem::getSingletonPtr();
	delete SmallModules::getSingletonPtr();

	CGUIManager::getSingleton().Terminate();
	CLuaManager::getSingleton().Terminate();

	delete EQMainSingletonManager::getSingletonPtr();
	delete OgreRootEx::getSingletonPtr();

	delete EQGameSceneMgr::getSingletonPtr();

	delete MailManager::getSingletonPtr();
// 	delete BodyActionMgr::getSingletonPtr();

	// ɾ���������������(hezhr)
	delete ChatExpressionTableManager::getSingletonPtr();
	// ����ϵͳ(hezhr)
	delete BroadCastSystem::getSingletonPtr();
	// ��̬����imageset������(hezhr)
	delete DynamicCreateImagesetMgr::getSingletonPtr();
}

EQMsgManager* GameMgr::getEQMsgManager()
{
	return &sMsgMgr;

}
