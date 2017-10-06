#include "EQCommPCH.h"
#include "ObjMgr.h"
#include "GameObj.h"
#include "Player.h"
#include "NPC.h"
#include "MainPlayer.h"
#include "Avatar.h"
#include "EQGameSceneMgr.h"
#include "HouseScene.h"
#include "SceneManager.h"
#include "MagicBoxSystem.h"

//--------------------------------------------------------------------
template<> ObjMgr* Ogre::Singleton<ObjMgr>::ms_Singleton = 0;
//--------------------------------------------------------------------
ObjMgr::ObjMgr(void)
: mPickObj(0)
,mVisibleType(VT_ALL)
{
}

ObjMgr::~ObjMgr(void)
{
}
//---------------------------------------------------------------
GameObj* ObjMgr::reqestObj(GAMEOBJ_TYPE type,std::string account)
{
	GameObj* obj = 0;

	switch(type)
	{
	case GOT_PLAYER:
		if( NULL == (obj = _getCache(type)) )
		{
			obj = new Player();
			((Player*)obj)->init();
			mPlayer.push_back((Player*)obj);
		}
		break;
	case GOT_MAINPLAYER:
		obj = &MainPlayer::getSingleton();
		break;
	case GOT_NPC:
		if( NULL == (obj = _getCache(type)))
		{
			obj = new NPC();
			mNpc.push_back((NPC*)obj);
		}
		break;
	}

	return obj;
}
//------------------------------------------------------------------
GameObj* ObjMgr::_getCache(GAMEOBJ_TYPE type)
{
	PlayerList::iterator itor;
	NPCList::iterator npcItor;

	switch(type)
	{
	case GOT_PLAYER:
		itor = mPlayerCache.begin();
		if(itor == mPlayerCache.end())
			return NULL;
		else
		{
			Player* obj = (*itor);

			mPlayerCache.erase(itor);
			mPlayer.push_back(obj);

			return obj;
		}
		break;
	case GOT_NPC:
		if(mNpcCache.empty())
			return NULL;
		else
		{
			npcItor = mNpcCache.begin();

			NPC* obj = (*npcItor);
			mNpcCache.erase(npcItor);
			mNpc.push_back(obj);

			return obj;
		}
		break;
	}
	return NULL;
}
//------------------------------------------------------------------
void ObjMgr::update()
{
	PlayerList::iterator itor = mPlayer.begin(),itordel;
	for(; itor != mPlayer.end();)
	{
		Player* p = (*itor);
		if( p->getState() == GOS_NONE )
		{
			p->reset();
			p->getAvatar()->restoreSlot();
			p->setVisible(false);
			p->setHeadUpVisible(false);
			mPlayerCache.push_back(p);
			itordel = itor;
			itor++;
			mPlayer.erase(itordel);
			p->delSelectedEffect();
			continue;
		}
		else
		{
			setPlayerVisible(p);
			p->update();
			itor++;
		}
	}


	// Npc upDate
	NPCList::iterator npcItor = mNpc.begin(), nItorDel;
	for(; npcItor != mNpc.end();)
	{
		NPC * n = (*npcItor);
		if( n->getState() == GOS_NONE)
		{
			n->setVisible(false);
			mNpcCache.push_back(n);
			nItorDel = npcItor;
			npcItor++;
			mNpc.erase(nItorDel);
			n->delSelectedEffect();
		}
		else
		{
			n->update();
			npcItor++;
		}
	}

	// 垃圾、宝箱的隐藏(2011/7/6 add by hezhr)
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		MagicBoxMap magicBoxs = MagicBoxSystem::getSingleton().getMagicBoxs();
		for (MagicBoxMap::iterator iter=magicBoxs.begin(); magicBoxs.end()!=iter; ++iter)
		{
			if (sObjMgr.getPosFloorNO(iter->second->getMagickBoxInfo().pt) > sObjMgr.getViewFloorNO())	// 宝箱所在楼层>观察楼层，隐藏
			{
				iter->second->setVisible(false);
			}
			else	// 显示
			{
				iter->second->setVisible(true);
			}
		}
	}
}
//--------------------------------------------------------------------------
GameObj* ObjMgr::findObj(GAMEOBJ_TYPE type,std::string key)
{
	GameObj* obj = 0;
	Player*  player = 0;
	PlayerList::iterator itor;
	NPC * npc = NULL;
	NPCList::iterator npcItor;

	switch (type)
	{
		case GOT_PLAYER:
		{
			itor = mPlayer.begin();
			for(; itor != mPlayer.end(); itor++)
			{
				player = (*itor);
				if( player->getState() == GOS_NONE )
					continue;
				else if( 0 == strcmp(key.c_str(), player->getAccount().c_str()) )
				{
					obj = (GameObj*)player;
					break;
				}
			}
		}
		break;

		case GOT_MAINPLAYER:
		{
			obj = (GameObj*)&MainPlayer::getSingleton();
		}
		break;
		
		case GOT_NPC:
		{
			npcItor = mNpc.begin();
			for(; npcItor != mNpc.end(); npcItor++)
			{
				npc = (*npcItor);
				if(npc->getState() == GOS_NONE)
					continue;
				else if( 0 == strcmp(key.c_str(), npc->getName().c_str()) )
				{
					obj = (GameObj*)npc;
					break;
				}
			}

		}
		break;
	}

	return obj;
}
//--------------------------------------------------------------------------
GameObj* ObjMgr::findObj(ModelGcs* model)
{
	GameObj* obj = 0;
	PlayerList::iterator itor;
	NPCList::iterator npcItor;

	obj = (GameObj*)&MainPlayer::getSingleton();
	//主角？
	if (obj->getMainNode() == model->getMainNode())
		return obj;
	else //其它玩家？
	{
		itor = mPlayer.begin();
		for(; itor != mPlayer.end(); itor++)
		{
			obj = (GameObj*)(*itor);
			if( obj->getState() == GOS_NONE )
				continue;
			else if(obj->getMainNode() == model->getMainNode())
			{
				return obj;
			}
		}
		//NPC？
		npcItor = mNpc.begin();
		for(; npcItor != mNpc.end(); npcItor++)
		{
			obj = (GameObj*)(*npcItor);
			if(obj->getState() == GOS_NONE)
				continue;
			else if(obj->getMainNode() == model->getMainNode())
			{
				return obj;
			}
		}
	}
	return NULL;
}
//----------------------------------------------------------------
void ObjMgr::cleanup()
{
	PlayerList::iterator itor = mPlayer.begin(),itordel;
	Player* p = 0;


	for(; itor != mPlayer.end();)
	{
		p = (*itor);
		itordel = itor;
		itor++;
		mPlayer.erase(itordel);
		p->cleanup();
		delete p;
	}

	itor = mPlayerCache.begin();
	for(; itor != mPlayerCache.end();)
	{
		p = (*itor);
		itordel = itor;
		itor++;
		mPlayerCache.erase(itordel);
		p->cleanup();
		delete p;
	}

	NPCList::iterator npcItor = mNpc.begin(), npcItorDel;
	NPC * npc = NULL;

	for(; npcItor != mNpc.end();)
	{
		npc = (*npcItor);
		npcItorDel = npcItor;
		npcItor++;
		mNpc.erase(npcItorDel);
		npc->cleanup();
		delete npc;
	}

	npcItor = mNpcCache.begin();
	for(; npcItor != mNpcCache.end();)
	{
		npc = (*npcItor);
		npcItorDel = npcItor;
		npcItor++;
		mNpcCache.erase(npcItorDel);
		npc->cleanup();
		delete npc;
	}

}
//----------------------------------------------------------------
void ObjMgr::AllPlayerGosNone()
{
	PlayerList::iterator itor = mPlayer.begin();

	for(; itor != mPlayer.end();)
	{
		(*itor)->setState(GOS_NONE);
		itor++;
	}

	NPCList::iterator itor2 = mNpc.begin();
	for(; itor2 != mNpc.end();)
	{
		(*itor2)->setState(GOS_NONE);
		itor2++;
	}
}
//----------------------------------------------------------------
bool ObjMgr::isMainPlayer(std::string owner)
{
	MainPlayer* p = MainPlayer::getSingletonPtr();
	if( 0 == strcmp(owner.c_str(), p->getAccount().c_str()) )
	{
		return true;
	}
	else
		return false;
}
//----------------------------------------------------------------
NPC* ObjMgr::getPickNPC(){ 
	if(NULL != mPickObj)
		if(mPickObj->getType() == GOT_NPC)
			return (NPC*)(mPickObj);
	return NULL;
}
//----------------------------------------------------------------
Player*	ObjMgr::getPickPlayer(){ 
	if(NULL != mPickObj)
		if(mPickObj->getType() == GOT_PLAYER)
			return (Player*)(mPickObj);
	return NULL;
}
//--------------------------------------------------------------------------
ObjMgr::GAMEOBJ_TYPE ObjMgr::getObjType(GameObj* obj)
{
	if(obj == &MainPlayer::getSingleton())
		return GOT_MAINPLAYER;

	if(std::find(mPlayer.begin(), mPlayer.end(), obj) != mPlayer.end())
		return GOT_PLAYER;

	if(std::find(mNpc.begin(), mNpc.end(), obj) != mNpc.end())
		return GOT_NPC;

	return GOT_OBJ;
}
//--------------------------------------------------------------------------
int ObjMgr::getPosFloorNO(const Ogre::Vector3& pos)
{
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		HouseScene *scene = EQSceneManager::getSingleton().getHouseScene();
		if (scene)
		{
			HouseSceneGrid *grid = scene->gridByPosition(pos, 10, 10);
			if (grid)
			{
				return grid->getCreator()->getFloorNO();
			}
		}
	}
	return 0;
}
//--------------------------------------------------------------------------
int ObjMgr::getViewFloorNO(void)
{
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		HouseScene *scene = EQSceneManager::getSingleton().getHouseScene();
		if (scene)
		{
			return scene->getWalkViewFloor()->getFloorNO();
		}
	}
	return 0;
}
//--------------------------------------------------------------------------
bool ObjMgr::isPosInViewFloor(const Ogre::Vector3& pos)
{
	if (getPosFloorNO(pos) == getViewFloorNO())
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------
bool ObjMgr::isPosInStair(const Ogre::Vector3& pos)
{
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		HouseScene *scene = EQSceneManager::getSingleton().getHouseScene();
		if (scene)
		{
			HouseSceneGrid *grid = scene->gridByPosition(pos, 10, 10);
			if (grid)
			{
				HouseScene::PairGridList pariGridList = scene->getPairGridList();
				for (HouseScene::PairGridList::iterator it=pariGridList.begin(); it!=pariGridList.end(); ++it)
				{
					// 确定位置在楼梯的连通点
					if (((it->first==grid)||(it->second==grid))&&(it->first->getCreator()->getFloorNO()!=it->second->getCreator()->getFloorNO()))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
//--------------------------------------------------------------------------
void ObjMgr::setPlayerVisible(Player* p)
{
	// 室内场景
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		p->setVisible(true);
		p->setHeadUpVisible(true);
	}
	// 室外场景
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_COMMON)
	{
		switch (mVisibleType)
		{
		case VT_ALL:
			p->setVisible(true);
			p->setHeadUpVisible(true);
			break;
		case VT_NAME:
			p->setVisible(false);
			p->setHeadUpVisible(true);
			break;
		case VT_NONE:
			p->setVisible(false);
			p->setHeadUpVisible(false);
		default:
			break;
		}
	}
}
//--------------------------------------------------------------------------
void ObjMgr::setVisibleType(VisibleType type)
{
	// 室外场景
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_COMMON)
	{
		mVisibleType = type;
	}
}
//--------------------------------------------------------------------------