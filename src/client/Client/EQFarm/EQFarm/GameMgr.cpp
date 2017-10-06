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
	{//单件体都在这里new出来，这里从理论上来讲没有顺序要球
	 //但最好也按比较合理的方式
		new OgreRootEx("", "", GetGameUserDir() + "EQFarm.Ogre.log");
		new EQMainSingletonManager();

		//EQClient
		new VariableSystem;				//游戏设置	
		new EQWndProc;					//window
		new EQOIS;						//OIS
		new EQOgreSys;					//Ogre系统
		new AppListener;				//帧监听器
		new GameStateMgr;				//游戏状态管理

		new PickMgr();

		mLogin = new Login();
		mMainCamera = new EQCamera;	//主控相机;
		mMainCamera->listenerEvt();
		mAvatarRTT  = new AvatarRTT;	//Render to texture
		mFurnitureRTT = new FurnitureRTT;
		mOtherPlayerRTT = new OtherPlayerRTT;
		
		//thirdpart lib UI 管理 和 Lua管理
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

		// 创建聊天表情表管理器(hezhr)
		new ChatExpressionTableManager;
		//	动态创建imageset管理器(hezhr)
		new DynamicCreateImagesetMgr;
		// 公告系统(hezhr)
		new BroadCastSystem;
		//
		new EQVoiceMgr;
	}

	{/*
	 初始化工作加在这里，这里的初始化顺序取决于依赖关系
	 也就是说被依赖方必须放在前头先初始化好来。
	 一般来说直接往最后面加
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


			//这个要放在LUA之前
			GameEventMgr::getSingleton().regAllEvent();
			//这个要放在EQOgreSys之后，在CLuaManager之前
			EQGameSceneMgr::getSingleton().init(EQOgreSys::getSingleton().getSceneManager());

			//LOGMSG("UIManager initial...");
			CGUIManager::getSingleton().Initialize(GetGameUserDir() + "EQFarm.CEGUI.log");

			// 在游戏初始化时注册扩展的cegui控件的类厂(hezhr)
			CEGUI::registerAllFactories();
			// 以下要放在cegui控件类厂注册的后面(hezhr)
			ChatExpressionTableManager::getSingleton().load();


			//
			//mAvatarRTT->startRTT(0,50000,0);

			//LOGMSG("LuaManager initial...");
			CLuaManager::getSingleton().Initialize();
		}
		catch(...)
		{
			// 错误分类
			EQErrorInfoEx::instance().errorType = "资源不完整";
			// 不做任何处理，将原本错误再次抛出
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
	打扫，这里的扫地顺序取决于依赖关系
	也就是说被依赖方必须放在后头来打扫。
	一般来说往最前面加
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
	//删除场景
	HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	if ( scene )
		EQSceneManager::getSingleton().destroyScene();
	////删除资源组
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

	// 删除聊天表情表管理器(hezhr)
	delete ChatExpressionTableManager::getSingletonPtr();
	// 公告系统(hezhr)
	delete BroadCastSystem::getSingletonPtr();
	// 动态创建imageset管理器(hezhr)
	delete DynamicCreateImagesetMgr::getSingletonPtr();
}

EQMsgManager* GameMgr::getEQMsgManager()
{
	return &sMsgMgr;

}
