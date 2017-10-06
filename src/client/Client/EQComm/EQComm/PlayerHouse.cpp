#include "EQCommPCH.h"
#include "PlayerHouse.h"

#include "NetMsgType.h"

#include "EQGameMgr.h"
#include "EQCamera.h"
#include "Variable.h"
#include "OgreMaxSceneLoader.h"
#include "SceneManager.h"
#include "HouseResManager.h"
#include "FitmentEvent.h"
#include "PickMgr.h"
#include "ZoneLightMgr.h"
#include "HouseScene.h"
#include "MainPlayer.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "LogicTable.h"
#include "GameState.h"
#include "EQMsgManager.h"
#include "EQGameSceneMgr.h"
#include "EQOgreSys.h"
#include "FitmentInteractMgr.h"
#include "FriendSystem.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"

void PlayerHouseListener::postCreateScene(EQGameScene* s)
{
	sStateMgr.setState(GS_MAINGAME);
	PlayerHouse* scene = dynamic_cast<PlayerHouse*>(s);
	assert(scene);

	scene->mHc = EQSceneManager::getSingleton().getHouseScene();
	assert(scene->mHc);

	float x = VariableSystem::getSingleton().GetAs_Float("HouseTransX");
	float y = VariableSystem::getSingleton().GetAs_Float("HouseTransY");
	float z = VariableSystem::getSingleton().GetAs_Float("HouseTransZ");
	scene->mHc->getSceneNode()->translate(x,y,z);

	//室内关闭阴影
	EQOgreSys::getSingleton().setShadow(0);

	//室内不需要自动开关灯
	AmbientMgr::getSingleton().setAutoTurnOnOffLight(scene->mPreview);
}

void PlayerHouseListener::preDestroyScene(EQGameScene* s)
{
	PlayerHouse* scene = dynamic_cast<PlayerHouse*>(s);
	assert(scene);

	scene->mHc = NULL;
}

PlayerHouse::PlayerHouse(uint id, bool preview=false) : EQGameScene(id)
{
	mPreview = preview;
	addListener(new PlayerHouseListener);
}

PlayerHouse::~PlayerHouse(void)
{
	removeListener();
}
//---------------------------------------------------------------------------
void PlayerHouse::addOneFurniture(house_furniture* fit)
{
	putOneFurniture(*fit);
	mFurniture_vec.push_back(*fit);
}
//---------------------------------------------------------------------------
void PlayerHouse::recoveryOneFurniture(uint64 instance_id)
{
	int size = mFurniture_vec.size();
	std::vector<house_furniture>::iterator iter = mFurniture_vec.begin();
	std::vector<house_furniture>::iterator end = mFurniture_vec.end();

	bool bDel = false;
	for( ; iter!= end ; iter++)
	{
		if( (*iter).instance_id == instance_id)
		{
			mFurniture_vec.erase(iter);
			bDel = true;
			break;
		}
	}

	if (bDel)
	{
		delOneFurniture(instance_id);
	}
	else
	{
		LOGERR("recoveryOneFurniture() arg err. no such instance_id");
	}

}
//---------------------------------------------------------------------------
void PlayerHouse::createHousedata(std::string& owner, house_data& data)
{
	//只有进入不同房间时才重建房屋数据，避免玩家频繁进入同一房间时的重复创建
	//if(owner != mOwner)
	{
		clearHouse();
		mOwner = owner;
		mLevel = data.level;

		mInstance_id	= data.instance_id;

		mFurniture_vec.resize(data.furniture_vec.size());
		mRoom_tex_vec.resize(data.room_tex_vec.size());
		mComponent_tex_vec.resize(data.component_tex_vec.size());
		mComponent_mesh_vec.resize(data.component_mesh_vec.size());

		int size = data.furniture_vec.size();
		for(int i=0; i < size; i++)
		{
			mFurniture_vec[i].face			= data.furniture_vec[i].face;
			mFurniture_vec[i].floor			= data.furniture_vec[i].floor;
			mFurniture_vec[i].height		= data.furniture_vec[i].height;
			mFurniture_vec[i].instance_id	= data.furniture_vec[i].instance_id;
			mFurniture_vec[i].item_tempid	= data.furniture_vec[i].item_tempid;
			mFurniture_vec[i].template_id	= data.furniture_vec[i].template_id;
			mFurniture_vec[i].x				= data.furniture_vec[i].x;
			mFurniture_vec[i].z				= data.furniture_vec[i].z;
//			mFurniture_vec[i].lev			= data.furniture_vec[i].lev;
			mFurniture_vec[i].position_list = data.furniture_vec[i].position_list;
			mFurniture_vec[i].del_time		= data.furniture_vec[i].del_time;
		}

		mRoom_tex_vec = data.room_tex_vec;

		//int len = sizeof(mFurniture_vec) / sizeof(house_furniture);
		//std::sort(&mFurniture_vec[0],&mFurniture_vec[0]+len, fitSort);

		//壁纸
		putAllWallPaper();

		//家具house_furniture
		putAllFurniture();

		//门窗house_component_mesh
		putAllComponent();

		if(mOwner != MainPlayer::getSingleton().getAccount())
		{
			if (mOwner == MainPlayer::getSingleton().getAccount() + "_gm_" + FriendSystem::getSingleton().getGMFriendAcc())
			{
				luabind::call_function<void>(sLuaMgr.getL(), "FriendList_setBtnEnabledInGmHouse",false);
				luabind::call_function<void>(sLuaMgr.getL(), "PlayerGuide_do_EnterGMHouseScript");
			}

			luabind::call_function<void>(sLuaMgr.getL(), "MainUI_showWelcome", data.welcome_words);
			luabind::call_function<void>(sLuaMgr.getL(), "OnHouseWelcome");
		}
		else
		{
			if(!mPreview)
				luabind::call_function<void>(sLuaMgr.getL(), "FriendList_setBtnEnabledInGmHouse",true);
		}
	}
}
//--------------------------------------------------------------------------------
void PlayerHouse::clearHouse()
{
	std::vector<house_furniture>::iterator iter = mFurniture_vec.begin();
	while(iter != mFurniture_vec.end())
	{
		delOneFurniture(iter->instance_id);
		iter++;
	}

	PickMgr::getSingleton().clear();
	mFurniture_vec.clear();
	mCompDisUid.clear();

	clearTempFurniture();
}
//-----------------------------------------------------------------------
void PlayerHouse::putAllWallPaper()
{
	int size = mRoom_tex_vec.size();
	std::vector<room_tex>::iterator iter = mRoom_tex_vec.begin();
	std::vector<room_tex>::iterator end = mRoom_tex_vec.end();

	for( ; iter!= end ; iter++)
	{
		changeRoomMaterial((*iter).floor_id, (*iter).room_id, (*iter).type, (*iter).tex);
	}
}
//-----------------------------------------------------------------------
void PlayerHouse::putAllFurniture()
{
	int size = mFurniture_vec.size();
	std::vector<house_furniture>::iterator iter = mFurniture_vec.begin(); 
	std::vector<house_furniture>::iterator end = mFurniture_vec.end();

	//house_furniture fur;
	for( ; iter!= end ; iter++)
	{
		putOneFurniture(*iter);
	}
}
//---------------------------------------------------------------
void PlayerHouse::delOneFurniture(uint64 instance_id)
{
	std::map<uint64, uint>::iterator it;
	if( (it = mCompDisUid.find(instance_id)) != mCompDisUid.end() )
		FitmentEventMgr::getSingleton().delFitment((*it).second);
	else
	{
		LOGERR("delOneFurniture() arg: no such instance_id ");
	}
}
//---------------------------------------------------------------
void PlayerHouse::putOneFurniture(house_furniture& fur)
{
	bool lighton = mPreview? !AmbientMgr::getSingleton().getDayNight() : false;
	uint uid = FitmentEventMgr::getSingleton().addFitment(fur, lighton);
	mCompDisUid.insert(make_pair(fur.instance_id,uid));

	for(uint i=0; i<fur.position_list.size(); i++)
	{
		if(fur.position_list[i].status)
			FitmentInteractMgr::getSingleton().recBeginInteractFurni(fur.instance_id, 
																	fur.position_list[i].func_id, 
																	fur.position_list[i].position_index, 
																	fur.position_list[i].status);
	}
}
//---------------------------------------------------------------
uint PlayerHouse::getUidByInstanceID(uint64 instance_id)
{
	std::map<uint64, uint>::iterator it;
	if( (it = mCompDisUid.find(instance_id)) != mCompDisUid.end() )
		return it->second;
	else
		return 0;
}
//--------------------------------------------------------------------------------
void PlayerHouse::putAllComponent()
{
	int size = mComponent_mesh_vec.size();
	std::vector<house_component_mesh>::iterator iter = mComponent_mesh_vec.begin();
	std::vector<house_component_mesh>::iterator end = mComponent_mesh_vec.end();

	//house_furniture fur;
	for( ; iter!= end ; iter++)
	{
		putOneComponent(*iter);
	}

	size = mComponent_tex_vec.size();
	std::vector<house_component_tex>::iterator iter2 = mComponent_tex_vec.begin();
	std::vector<house_component_tex>::iterator end2 = mComponent_tex_vec.end();

	//house_furniture fur;
	for( ; iter2!= end2 ; iter2++)
	{
		putOneComponent(*iter2);
	}
}
//---------------------------------------------------------------
void PlayerHouse::putOneComponent(house_component_tex& fur)
{

}
//---------------------------------------------------------------
void PlayerHouse::putOneComponent(house_component_mesh& fur)
{

}
//-----------------------------------------------------------------------
uint64 PlayerHouse::getInstanceID( int floor, int x, int z, int bottom)
{
	int size = mFurniture_vec.size();
	std::vector<house_furniture>::iterator iter = mFurniture_vec.begin();
	std::vector<house_furniture>::iterator end = mFurniture_vec.end();

	//house_furniture fur;
	for( ; iter!= end ; iter++)
	{
		if( (*iter).x == x && (*iter).z == z
			&& (*iter).height == bottom
			&& (*iter).floor == floor
			)
			return (*iter).instance_id;
	}

	LOGERR("getInstanceID return 0.");
	return 0;

}
//-----------------------------------------------------------------------
const house_furniture* PlayerHouse::getFurniture(int floor, int x, int z, int bottom)
{
	int size = mFurniture_vec.size();
	std::vector<house_furniture>::iterator iter = mFurniture_vec.begin();
	std::vector<house_furniture>::iterator end = mFurniture_vec.end();

	for(; end!=iter; ++iter)
	{
		if((*iter).floor==floor && (*iter).x==x && (*iter).z==z && (*iter).height==bottom)
			return &(*iter);
	}
	return NULL;
}
//-----------------------------------------------------------------------
void PlayerHouse::getAllInstanceID(std::vector<uint64>& ins)
{
	ins.clear();
	std::vector<house_furniture>::iterator it;
	for(it=mFurniture_vec.begin(); it!=mFurniture_vec.end(); it++)
		ins.push_back((*it).instance_id);
}
//-----------------------------------------------------------------------
uint64 PlayerHouse::moveFit( HouseSceneGrid * oldGrid, 
							float	x, 
							float	z, 
							int		face, 
							int		height,
							int		floor,
							int		bottom)
{
	int size = mFurniture_vec.size();
	std::vector<house_furniture>::iterator iter = mFurniture_vec.begin();
	std::vector<house_furniture>::iterator end = mFurniture_vec.end();

	//house_furniture fur;
	for( ; iter!= end ; iter++)
	{
		if( (*iter).x == oldGrid->getGridX() 
			&& (*iter).z == oldGrid->getGridZ()
			&& (*iter).height == bottom
			&& (*iter).floor == oldGrid->getCreator()->getFloorNO()
			)
		{
			(*iter).face	= face;
			(*iter).floor	= floor;
			(*iter).height	= height;
			(*iter).x		= x;
			(*iter).z		= z;

			return (*iter).instance_id;
		}
	}

	LOGERR("getInstanceID return 0.");
	return 0;

}
//-----------------------------------------------------------------------
house_furniture* PlayerHouse::getFitByGird( int x, int z)
{
	int size = mFurniture_vec.size();
	std::vector<house_furniture>::iterator iter = mFurniture_vec.begin();
	std::vector<house_furniture>::iterator end = mFurniture_vec.end();

	//house_furniture fur;
	for( ; iter!= end ; iter++)
	{
		if( (*iter).x == x && (*iter).z == z)
			return (house_furniture*) &(*iter);
	}
	return NULL;
}
//-----------------------------------------------------------------------
void PlayerHouse::reqMoveFit( int x, int z)
{
	//
	//house_furniture* fit = 0;

	//std::vector<house_furniture>::iterator iter = mFurniture_vec.begin();
	//std::vector<house_furniture>::iterator end = mFurniture_vec.end();

	////house_furniture fur;
	//for( ; iter!= end ; iter++)
	//{
	//	if( (*iter).x == x && (*iter).z == z)
	//	{
	//		fit = &(*iter);
	//		break;
	//	}
	//}

	//if(fit == NULL)
	//{
	//	LOGERR("reqMoveFit() got no fit ");
	//	return;
	//}

	//req_move_furniture move;
	//move.instance_id	= fit->instance_id;
	//move.x				= mGrid->getGridX();
	//move.z				= mGrid->getGridZ();
	//move.height			= mBottom;
	//move.floor			= mGrid->getCreator()->getFloorNO();
	//move.face			= mFace;
	//sGameMgr.getEQMsgManager()->Send(move);
}
//-----------------------------------------------------------------------
bool PlayerHouse::changeRoomMaterial(int floor_id, int room_id, int type, std::string tex)
{
	if(!mHc)
		return false;

	std::map<int, HouseSceneFloor*> floors = mHc->getFloors();
	std::map<int, HouseSceneFloor*>::const_iterator iter = floors.begin();
	for (iter; floors.end()!=iter; ++iter)
	{
		if (iter->second->getFloorNO()==floor_id && iter->second->getRoomComponent(room_id))
		{
			switch(type)
			{
			case HCP_FLOOR:	// 地板
				iter->second->changeRoomFloorMaterial(room_id, tex);
				break;
			case HCP_OUT_WALL:	// 外墙
			case HCP_IN_WALL:	// 内墙
				iter->second->changeRoomWallMaterial(room_id, tex);
				break;
			case HCP_TCX:	// T脚线
				iter->second->changeRoomTcxMaterial(room_id, tex);
				break;
			default:
				LOGERR("Can't change the room's component type : " + Ogre::StringConverter::toString(type) + " , which id is : " + Ogre::StringConverter::toString(room_id));
				return false;
			}
			return true;
		}
	}
	LOGERR("There is no room that id is : " + Ogre::StringConverter::toString(room_id));
	return false;
}
//-----------------------------------------------------------------------
std::string PlayerHouse::getRoomMaterial(int floor_id, int room_id, int type)
{
	if(!mHc)
		return false;

	std::map<int, HouseSceneFloor*> floors = mHc->getFloors();
	std::map<int, HouseSceneFloor*>::const_iterator iter = floors.begin();
	for (iter; floors.end()!=iter; ++iter)
	{
		if (iter->second->getFloorNO()==floor_id && iter->second->getRoomComponent(room_id))
		{
			switch(type)
			{
			case HCP_FLOOR:	// 地板
				return iter->second->getRoomFloorMaterial(room_id);
			case HCP_OUT_WALL:	// 外墙
			case HCP_IN_WALL:	// 内墙
				return iter->second->getRoomWallMaterial(room_id);
			case HCP_TCX:	// T脚线
				return iter->second->getRoomTcxMaterial(room_id);
			default:
				return "";
			}
		}
	}
	return "";
}
//-----------------------------------------------------------------------
void PlayerHouse::showAllTempMark()
{
	TempCompTable::iterator it;
	for(it=mTempComp.begin(); it!=mTempComp.end(); it++)
		FitmentEventMgr::getSingleton().playFitmentFX(it->first);
}
//-----------------------------------------------------------------------
void PlayerHouse::hideAllTempMark()
{
	TempCompTable::iterator it;
	for(it=mTempComp.begin(); it!=mTempComp.end(); it++)
		FitmentEventMgr::getSingleton().delFitmentFX(it->first);
}
//-----------------------------------------------------------------------
uint PlayerHouse::addTempFurniture(house_furniture& comp, house_furniture& rep)
{
	uint uid = FitmentEventMgr::getSingleton().addFitment(comp);
	if(mTempMark)
		FitmentEventMgr::getSingleton().playFitmentFX(uid);

	TempCompInfo info;
	info.comp = comp;
	info.rep = rep;
	mTempComp.insert(std::make_pair(uid, info));

	return uid;
}
//-----------------------------------------------------------------------
void PlayerHouse::delTempFurniture(uint uid)
{
	TempCompTable::iterator it = mTempComp.find(uid);
	if(it != mTempComp.end())
	{
		if(it->second.rep.template_id)
			FitmentEventMgr::getSingleton().addFitment(it->second.rep);
		FitmentEventMgr::getSingleton().delFitment(it->first);

		mTempComp.erase(it);
	}
}
//-----------------------------------------------------------------------
bool PlayerHouse::getTempFurniture(uint uid, house_furniture& comp)
{
	TempCompTable::iterator it = mTempComp.find(uid);
	if(it != mTempComp.end())
	{
		comp = it->second.comp;
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------
void PlayerHouse::clearTempFurniture()
{
	TempCompTable::iterator it;
	for(it=mTempComp.begin(); it!=mTempComp.end(); it++)
	{
		if(it->second.rep.template_id)
			FitmentEventMgr::getSingleton().addFitment(it->second.rep);
		FitmentEventMgr::getSingleton().delFitment(it->first);
	}

	mTempComp.clear();
}
//-----------------------------------------------------------------------
uint PlayerHouse::isTempFurniture(HouseSceneGrid* grid, int bottom)
{
	TempCompTable::iterator it;
	for(it=mTempComp.begin(); it!=mTempComp.end(); it++)
	{
		if(it->second.comp.x==grid->getGridX() && it->second.comp.z==grid->getGridZ() && it->second.comp.height==bottom)
			return it->first;
	}

	return 0;
}
//-----------------------------------------------------------------------
void PlayerHouse::moveTempFurniture(uint uid, int floor, HouseSceneGrid* grid, int bottom, int face)
{
	TempCompTable::iterator it = mTempComp.find(uid);
	if(it == mTempComp.end())
		return;

	it->second.comp.floor = floor;
	it->second.comp.x = grid->getGridX();
	it->second.comp.z = grid->getGridZ();
	it->second.comp.height = bottom;
	it->second.comp.face = face;
}
//-----------------------------------------------------------------------
uint PlayerHouse::fetchTempFurnitureRep(uint uid)
{
	TempCompTable::iterator it = mTempComp.find(uid);
	if(it != mTempComp.end())
	{
		uint rep = it->second.rep.template_id;
		it->second.rep.template_id = 0;
		return rep;
	}

	return 0;
}
//-----------------------------------------------------------------------