#include "EQCommPCH.h"

#include "FitmentInteractMgr.h"
#include "SceneManager.h"
#include "MainPlayer.h"
#include "GameEventMgr.h"
#include "LogicTable.h"
#include "HouseScene.h"

#include "EQGameSceneMgr.h"
#include "ObjMgr.h"
#include "HSCManager.h"
#include "EQMsgManager.h"
#include "Avatar.h"
#include "FitmentEvent.h"
#include "PlayerHouse.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "EQOgreSys.h"
#include "EQVoiceMgr.h"
#include "MsgSystem.h"
#include "CommFunc.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "SceneEntityObject.h"
#include "SceneInfo.h"

//-----------------------------------------------------------------------
template<> FitmentInteractMgr* Ogre::Singleton<FitmentInteractMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
FitmentInteractMgr::FitmentInteractMgr() :
mBegin(false),
mPrepare(false)
{
	
}
//-----------------------------------------------------------------------
FitmentInteractMgr::~FitmentInteractMgr()
{

}
//-----------------------------------------------------------------------
bool FitmentInteractMgr::_isInHouse()
{
	return EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE;
}
//-----------------------------------------------------------------------
bool FitmentInteractMgr::canClick(HouseSceneComponent* com)
{
	if (!com || !LogicTableManager::getSingleton().getLoopMenuRow(com->getHouseCompRow()->LoopID))
		return false;
	
	return true;
}
//-----------------------------------------------------------------------
bool FitmentInteractMgr::canClick(SceneEntityObject* obj)
{
	if (!obj || !LogicTableManager::getSingleton().getLoopMenuRow(obj->getLoopId()))
		return false;

	return true;
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::pickFitment(HouseSceneComponent* com)
{
	mFitment = com;
	if(!sFitMgr.isFiting())//?家装模式下不显示？
		GameEventMgr::getSingleton().fireEvent("SHOW_FITMENTINTERACT_UI");
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::pickObject(SceneEntityObject* obj)
{
	mObject = obj;
	
	GameEventMgr::getSingleton().fireEvent("SHOW_FITMENTINTERACT_UI");
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::selLoopMenu(int menu)
{
	mFurniFuncRow = LogicTableManager::getSingleton().getFurniInteractRow(menu);
	if(!mFurniFuncRow)
		EQ_EXCEPT( "can't find row " + Ogre::StringConverter::toString(menu) + " in FurniInteract.xml", "pickFitment" );

	std::queue<Ogre::Vector3> result;
	if(_findPath(result))
	{
		//退出当前交互
		if(mBegin)
		{
			//清除监听
			//MainPlayer::getSingleton().setListener(NULL);
		}

		sndBeginInteract();

		mPrepare = true;
	}
	else
	{
		MsgSystem::getSingleton().showClientMsg(MsgSystem::MSG_CHATBOX, 100000);
	}
}
//-----------------------------------------------------------------------
bool FitmentInteractMgr::_findPath(std::queue<Ogre::Vector3>& result)
{
	while(result.size())
		result.pop();

	//使用距离为0就不用寻路直接使用(主要是灯)
	if(mFurniFuncRow->use_range == 0)
	{
		mPos = MainPlayer::getSingleton().getPos();
		return true;
	}

	Ogre::Vector3 start = mBegin? mPos : MainPlayer::getSingleton().getPos();

	bool found=false;
	if(_isInHouse())
	{
		std::set<const HouseSceneGrid*>& usegrid = mFitment->calcUseGrids(mFurniFuncRow->use_face, mFurniFuncRow->use_range);
		if(usegrid.size() <= 0)
			return false;

		HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
		HouseSceneGrid* playerGrid = sce->gridByPosition(start);
		if(!playerGrid)
			return false;

		//已经站在使用格就不寻路了
		if(std::find(usegrid.begin(), usegrid.end(), playerGrid) != usegrid.end())
		{
			mPos = MainPlayer::getSingleton().getPos();
			return true;
		}

		uint dis=0,time=0;
		Ogre::Vector3 dest;
		std::set<const HouseSceneGrid*>::iterator ite;
		for(ite=usegrid.begin(); ite!=usegrid.end(); ite++,time++)
		{
			//最短路径最多只找10格
			if(time>=10 && found)
				break;

			dest = (*ite)->getSceneNode()->_getDerivedPosition();
			MainPlayer::getSingleton().findPath(start, dest, result);
			uint size = result.size();
			if(result.size())
			{
				found = true;

				//找最短路径
				if(dis==0 || size<dis)
				{
					mPos = dest;
					dis = size;
				}
			}
		}
	}
	else
	{
		Ogre::Vector3 pos = mObject->_getDerivedPosition();
		Ogre::Quaternion ori = mObject->getOrientation();

		Ogre::Vector3 dest,dir;
		Ogre::Quaternion rot;
		for(int i=0; i<4; i++)
		{
			if( !(mFurniFuncRow->use_face & (1<<i)) )
				continue;

			rot = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90 * i)),Ogre::Vector3::UNIT_Y);
			dir = ori * rot * Ogre::Vector3::UNIT_X;
			dir.normalise();
			dest = pos + dir * mFurniFuncRow->use_range;
			dest.y = 0;
			MainPlayer::getSingleton().findPath(start, dest, result);
			uint size = result.size();
			if(size)
			{
				found = true;
				mPos = dest;

				//室外寻路比较耗，找到一个就够了
				break;
			}
		}
	}

	return found;
}
//-----------------------------------------------------------------------
uint FitmentInteractMgr::getLoopMenuID()
{
	if(_isInHouse())
		return mFitment->getHouseCompRow()->LoopID;
	else
		return mObject->getLoopId();
}
//-----------------------------------------------------------------------
uint FitmentInteractMgr::getInteractingFitmentID()
{
	if(_isInHouse() && mFitment)
		return mFitment->getHouseCompRow()->ID;

	return 0;
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::update()
{
	if(!mBegin || !mFurniFuncRow)
		return;

	if(mFurniFuncRow->time == 0)
		return;

	if(GetTickCount() - mBeginTime >= mFurniFuncRow->time)
	{
		mBegin = false;
// 		mPrepare = false;
		sndEndInteract();
	}
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::sndBeginInteract()
{
	req_walk_for_use_furniture pack;
	pack.curr_pos.x = MainPlayer::getSingleton().getPos().x;
	pack.curr_pos.y = MainPlayer::getSingleton().getPos().y;
	pack.curr_pos.z = MainPlayer::getSingleton().getPos().z;
	pack.dest_pos.x = mPos.x;
	pack.dest_pos.y = mPos.y;
	pack.dest_pos.z = mPos.z;
	pack.function_id = mFurniFuncRow->id;

	sMainPlayer.setCurWalkDestPos(Ogre::Vector3(mPos.x,mPos.y,mPos.z));
	pack.move_type = sMainPlayer.getMoveType();
	sMainPlayer.setMoveForWhat(MOVE_FOR_USEFURNI);
	sMainPlayer.setState(GOS_USEFURNI_INTERVAL);
	if(_isInHouse())
		pack.instance_id = EQGameSceneMgr::getSingleton().getHouse()->getInstanceID(mFitment->getAttachGrid()->getCreator()->getFloorNO(), 
																					mFitment->getAttachGrid()->getGridX(), 
																					mFitment->getAttachGrid()->getGridZ(), 
																					mFitment->getBottom());
	else
		pack.instance_id = mObject->getInteractId();

	sGameMgr.getEQMsgManager()->Send(pack);
}
//-----------------------------------------------------------------------
HouseSceneComponent* FitmentInteractMgr::_getFitmentByInstance(uint64 instance_id)
{
	uint fitid = EQGameSceneMgr::getSingleton().getHouse()->getUidByInstanceID(instance_id);
	assert(fitid);
	HouseSceneComponent* fitment = HSCManager::getSingleton().getHouseSceneComponent(fitid);
	if(!fitment)
		EQ_EXCEPT( "can't find component " + Ogre::StringConverter::toString(fitid), "FitmentInteractMgr" );

	return fitment;
}
//-----------------------------------------------------------------------
SceneEntityObject* FitmentInteractMgr::_getObjectByInstance(uint64 instance_id)
{
	SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();
	std::map<uint, SceneEntityObject*>& objectMap = scene->getSceneEntityObjects();
	std::map<uint, SceneEntityObject*>::iterator it;
	for(it=objectMap.begin(); it!=objectMap.end(); it++)
	{
		if(it->second->getInteractId() == instance_id)
			return it->second;
	}

	return NULL;
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::recBeginInteractRole(std::string& account, uint64 instance_id, int menu, int pos_index, int fur_status, bool autoUse)
{
	const FurniInteractRow* fitrow = LogicTableManager::getSingleton().getFurniInteractRow(menu);
	if(!fitrow)
		EQ_EXCEPT( "can't find row " + Ogre::StringConverter::toString(menu), "recBeginInteract" );

	//人物动画
	Player* player;
	bool ismain = ObjMgr::getSingleton().isMainPlayer(account);
	if(ismain)
		player = (Player*)MainPlayer::getSingletonPtr();
	else
		player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, account);
	
	_beginRoleAni(instance_id, fitrow, player, pos_index);

	if(ismain)
	{
		if(!autoUse)
		{
			//家具行为只有主玩家才能使用
			_beginFurniAct(instance_id, fitrow->furni_act);

 			if(fur_status)
				MsgSystem::getSingleton().showClientMsg(MsgSystem::MSG_CHATBOX, fitrow->start_hint);
 			else
				MsgSystem::getSingleton().showClientMsg(MsgSystem::MSG_CHATBOX, fitrow->end_hint);

			//播放使用家具音效
			EQVoiceMgr::getSingleton().play(fitrow->voiceid, false);

			//相机不跟随
// 			sGameMgr.getMainCamera()->setFollowMe(false);

			//LOG("开始交互");
			mBegin = true;
			mPrepare = false;
			sMainPlayer.setMoveForWhat(MOVE_FOR_NONE);
			mBeginTime = GetTickCount();
		}
	}
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::recBeginInteractFurni(uint64 instance_id, int menu, int pos_index, int fur_status)
{
	const FurniInteractRow* fitrow = LogicTableManager::getSingleton().getFurniInteractRow(menu);
	if(!fitrow)
		EQ_EXCEPT( "can't find row " + Ogre::StringConverter::toString(menu), "recBeginInteract" );

	//家具动画
	if(fur_status)
		_beginFurniAni(instance_id, fitrow, pos_index-1);
	else
		_endFurniAni(instance_id, fitrow, pos_index-1);
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::_beginRoleAni(uint64 instance_id, const FurniInteractRow* row, Player* player, int pos_index)
{
	if(row->role_ani.empty())
		return;

	Ogre::Vector3 pos;
	Ogre::Quaternion face;
	if(_isInHouse())
	{
		HouseSceneComponent* fitment = _getFitmentByInstance(instance_id);
		pos = fitment->getAttachGrid()->getSceneNode()->_getDerivedPosition();
		face = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90 * fitment->getFace())),Ogre::Vector3::UNIT_Y);
	}
	else
	{
		SceneEntityObject* object = _getObjectByInstance(instance_id);
		pos = object->_getDerivedPosition();
		face = object->getOrientation();
	}

	//交互坐标
	Ogre::Vector3 offset = player->isMale()? row->offset_male[pos_index-1] : row->offset_female[pos_index-1];
	pos += face * offset;
	player->setPos(pos);

	//交互朝向
	int f = player->isMale()?  row->face_male : row->face_female;
	player->getMainNode()->setOrientation( face * Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90 * f)),Ogre::Vector3::UNIT_Y) );

	//隐藏部件
	player->getAvatar()->hideSlot(row->hide_slot);

	//人物动作
	player->setDefAnimation(row->role_ani);

	if(row->mosaic.size() == 2)
		player->addMosaicEffect(Ogre::Vector3(row->mosaic[0]), Ogre::Vector3(row->mosaic[1]));
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::_beginFurniAni(uint64 instance_id, const FurniInteractRow* row, int pos)
{
	//室外暂不支持家具动画
	if(!_isInHouse())
		return;

	HouseSceneComponent* fitment = _getFitmentByInstance(instance_id);

	const FurniAnimationRow* ani = LogicTableManager::getSingleton().getFurniAnimationRow(row->furni_ani);
	if(!ani)
		return;

	for(int i=0; i<(int)ani->type.size(); i++)
	{
		switch(ani->type[i])
		{
		case FANI_MORPH:
			{
				int keyframe = 0;

				if(row->menu_info.size() != 2 || ani->morph_frame.size() == 1)
				{
					//只有一个交互动画表现
					keyframe = ani->morph_frame[0];
				}
				else
				{
					//多人且多种交互方式的动画表现
					std::vector<int>& flag = fitment->calcInteractMorph(pos, row->menu_info[0], row->max_user);

					//取对应偏移值
					int offset = 0;
					for(int i=0; i<(int)flag.size(); i++)
					{
						offset += flag[i] * Ogre::Math::Pow(row->menu_info[1]+1, i);
					}

					keyframe = ani->morph_frame[offset];
				}

				fitment->getModel()->setModelMorph(keyframe);
			}
			break;

		case FANI_TEXTURE:
			{
				fitment->getModel()->setTextureAnimation(ani->texture_express.c_str());
			}
			break;

		case FANI_SKELETON:
			{
				fitment->getModel()->playAnimation(ani->skeleton_name, ani->skeleton_loop);
			}
			break;

		case FANI_PARTICLE:
			{
				fitment->getModel()->addParticle(ani->particle);
			}
			break;

		case FANI_LIGHT:
			{
				fitment->turnOnLight();
				luabind::call_function<void>(sLuaMgr.getL(), "PlayerGuide_do_useLightScript");

			}
			break;
		}
	}
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::sndEndInteract()
{
	req_stop_using_furniture pack;
	pack.account = MainPlayer::getSingleton().getAccount();

	sGameMgr.getEQMsgManager()->Send(pack);
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::recEndInteract(std::string& account, uint64 instance_id, int menu, int pos_index, Ogre::Vector3 pos, int pre_status, int fur_status)
{
	const FurniInteractRow* fitrow = LogicTableManager::getSingleton().getFurniInteractRow(menu);
	if(!fitrow)
		EQ_EXCEPT( "can't find row " + Ogre::StringConverter::toString(menu), "recBeginInteract" );

	Player* player;
	bool ismain = ObjMgr::getSingleton().isMainPlayer(account);
	if(ismain)
		player = (Player*)MainPlayer::getSingletonPtr();
	else
		player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, account);

	//人物动画
	if(!fitrow->role_ani.empty())
	{
		//还原部件
		player->getAvatar()->restoreSlot();

		//还原坐标
		player->setPos(pos);

		//还原人物动作
		player->setState(GOS_USEFURNI_INTERVAL);
	}

	if(fitrow->mosaic.size() == 2)
		player->removeMosaicEffect();

	if(ismain)
	{
		if(fur_status != pre_status)
		{
 			if(fur_status)
				MsgSystem::getSingleton().showClientMsg(MsgSystem::MSG_CHATBOX, fitrow->start_hint);
 			else		
				MsgSystem::getSingleton().showClientMsg(MsgSystem::MSG_CHATBOX, fitrow->end_hint);
		}

		//相机跟随
// 		sGameMgr.getMainCamera()->setFollowMe(true);

		//LOG("退出交互");
		//这个要放在清除监听前面~!~
		mBegin = false;
		//清除监听
		//MainPlayer::getSingleton().setListener(NULL);
	}

	//家具状态改变时才播放动画(比如柜子结束时要播，灯结束时不播)
	if(fur_status != pre_status)
	{
		//家具动画
		if(fur_status)
			_beginFurniAni(instance_id, fitrow, pos_index-1);
		else
			_endFurniAni(instance_id, fitrow, pos_index-1);
	}
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::_endFurniAni(uint64 instance_id, const FurniInteractRow* row, int pos)
{
	//室外暂不支持家具动画
	if(!_isInHouse())
		return;

	HouseSceneComponent* fitment = _getFitmentByInstance(instance_id);

	const FurniAnimationRow* ani = LogicTableManager::getSingleton().getFurniAnimationRow(row->furni_ani);
	if(!ani)
		return;

	for(int i=0; i<(int)ani->type.size(); i++)
	{
		switch(ani->type[i])
		{
		case FANI_MORPH:
			{
				int keyframe = 0;

				if(row->menu_info.size() != 2 || ani->morph_frame.size() == 1)
				{
					//只有一个交互动画表现
					keyframe = 0;
				}
				else
				{
					//多人且多种交互方式的动画表现
					std::vector<int>& flag = fitment->calcInteractMorph(pos, 0, row->max_user);

					//取对应偏移值
					int offset = 0;
					for(int i=0; i<(int)flag.size(); i++)
					{
						offset += flag[i] * Ogre::Math::Pow(row->menu_info[1]+1, i);
					}

					keyframe = ani->morph_frame[offset];
				}

				fitment->getModel()->setModelMorph(keyframe);
			}
			break;

		case FANI_TEXTURE:
			{
				fitment->getModel()->clearTextureAnimation();
			}
			break;

		case FANI_SKELETON:
			{
				fitment->getModel()->clearAnimationLoop();
			}
			break;

		case FANI_PARTICLE:
			{
				fitment->getModel()->delParticleByID(ani->particle);
			}
			break;

		case FANI_LIGHT:
			{
				fitment->turnOffLight();
			}
			break;
		}
	}
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::_beginFurniAct(uint64 instance_id, uint actid)
{
	const FurniActionRow* act = LogicTableManager::getSingleton().getFurniActionRow(actid);
	if(!act)
		return;

	switch(act->type)
	{
	case FACT_TURNONBYLAMP:
		{
			req_use_mutli_furni pack;

			std::vector<int> param;
			std::string str = act->param;
			Ogre::StringUtil::trim( str );
			if ( !str.empty() )
			{
				std::vector<std::string> posArr = Ogre::StringUtil::split( str, " \r\n\t()," );
				for ( std::vector<std::string>::iterator it = posArr.begin(); it != posArr.end(); ++it )
				{
					param.push_back( Ogre::StringConverter::parseUnsignedInt(*it) );
				}
			}

			std::vector<uint64> instance;
			EQGameSceneMgr::getSingleton().getHouse()->getAllInstanceID(instance);
			for(uint j=0; j<instance.size(); j++)
			{
				uint id = EQGameSceneMgr::getSingleton().getHouse()->getUidByInstanceID(instance[j]);
				assert(id);
				HouseSceneComponent* fit = HSCManager::getSingleton().getHouseSceneComponent(id);
				assert(fit);
				const LoopMenuRow* menu = LogicTableManager::getSingleton().getLoopMenuRow(fit->getHouseCompRow()->LoopID);
				assert(menu);

				if(fit->getLight().size() == 0)
					continue;

				if(std::find(param.begin(),param.end(), fit->getHouseCompRow()->LampType) != param.end())
				{
					control_furni_info info;
					info.instance_id = instance[j];
					info.function_id = menu->fun1; //灯具一定是fun1
					pack.furni_vec.push_back(info);
				}
			}

			sGameMgr.getEQMsgManager()->Send(pack);
		}
		break;

	case FACT_TURNONBYROOM:
		{
			req_use_mutli_furni pack;

			HouseSceneComponent* fitment = _getFitmentByInstance(instance_id);

			std::vector<uint64> instance;
			EQGameSceneMgr::getSingleton().getHouse()->getAllInstanceID(instance);
			for(uint j=0; j<instance.size(); j++)
			{
				uint id = EQGameSceneMgr::getSingleton().getHouse()->getUidByInstanceID(instance[j]);
				assert(id);
				HouseSceneComponent* fit = HSCManager::getSingleton().getHouseSceneComponent(id);
				assert(fit);
				const LoopMenuRow* menu = LogicTableManager::getSingleton().getLoopMenuRow(fit->getHouseCompRow()->LoopID);
				assert(menu);

				if(fit->getLight().size() == 0)
					continue;

				if(fitment->getAttachGrid()->getRoomID() == fit->getAttachGrid()->getRoomID())
				{
					control_furni_info info;
					info.instance_id = instance[j];
					info.function_id = menu->fun1; //灯具一定是fun1
					pack.furni_vec.push_back(info);
				}
			}

			sGameMgr.getEQMsgManager()->Send(pack);
		}
		break;
	}
}
//-----------------------------------------------------------------------
std::string FitmentInteractMgr::getDelTime()
{
	if(mFitment)
	{
		const house_furniture* furniture = EQGameSceneMgr::getSingleton().getHouse()->getFurniture(
			mFitment->getAttachGrid()->getCreator()->getFloorNO(),
			mFitment->getAttachGrid()->getGridX(), 
			mFitment->getAttachGrid()->getGridZ(), 
			mFitment->getBottom());
		if (NULL == furniture)
		{
			return "";
		}
		int year = furniture->del_time.year;
		int month = furniture->del_time.month;
		int day = furniture->del_time.day;
		int hour = furniture->del_time.hour;
		int minute= furniture->del_time.minute;
		int second = furniture->del_time.second;
		return FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
	}
	return "";
}
//-----------------------------------------------------------------------
void FitmentInteractMgr::debug(float x, float y, float z)
{
	if(!isInteracting())
		return;

	Ogre::Vector3 pos;
	Ogre::Quaternion face;
	if(_isInHouse())
	{
		pos = mFitment->getAttachGrid()->getSceneNode()->_getDerivedPosition();
		face = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90 * mFitment->getFace())),Ogre::Vector3::UNIT_Y);
	}
	else
	{
		pos = mObject->_getDerivedPosition();
		face = mObject->getOrientation();
	}

	pos += face * Ogre::Vector3(x,y,z);
	sMainPlayer.setPos(pos);
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void FitMentInteractListener::exit()
{
	//if(FitmentInteractMgr::getSingleton().isInteracting())
	//	FitmentInteractMgr::getSingleton().sndEndInteract();
}
//-----------------------------------------------------------------------
void FitMentInteractListener::moveBegin()
{
	//LOG("开始寻路");
}
//-----------------------------------------------------------------------
void FitMentInteractListener::moveEnd()
{
	//LOG("结束寻路");

	//FitmentInteractMgr::getSingleton().sndBeginInteract();
}
//-----------------------------------------------------------------------