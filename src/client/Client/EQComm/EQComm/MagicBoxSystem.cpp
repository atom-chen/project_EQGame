#include "EQCommPCH.h"
#include "MagicBoxSystem.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "HouseScene.h"
#include "ModelManagerGcs.h"
#include "CommFuncGcs.h"
#include "LogicTable.h"
#include "GraphicsTable.h"
#include "SceneManager.h"
#include "DynamicUIMgr.h"
#include "MainPlayer.h"
#include "GiftSystem.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"
#include "LogicTable.h"
#include "ZoneLightMgr.h"

const std::string MagicBox::PickObjectUnit::ClassName = "MagicBox::PickObjectUnit";

MagicBox::MagicBox( MagicBoxInfo &info )
{
	Create(info);

}

MagicBox::~MagicBox( void )
{

}

void MagicBox::Create(MagicBoxInfo &info)
{
	mInfo = info;

	const ModelDisplayRow* displayRow = GraphicsTableManager::getSingleton().getModelDisplayRow(mInfo.model_id);
	SimpleCreateModelRule rule(displayRow->mesh, displayRow->mat, displayRow->particle);
	mModel = ModelManagerGcs::getSingleton().createModel(&rule);

	HouseScene * hs = EQSceneManager::getSingleton().getHouseScene();
	HouseSceneGrid * grid = hs->getGrid(mInfo.y, mInfo.x, mInfo.z);
	Ogre::Vector3 position = grid->getSceneNode()->getPosition();
	mModel->getMainNode()->setPosition(position);
	mModel->setPickObjectProxy(&mPickObjectProxy);

	HouseSceneGrid *gd = EQSceneManager::getSingleton().getHouseScene()->getGrid(mInfo.y, mInfo.x, mInfo.z);
	if(gd)
	{
		int czone = room2zone(gd->getHouseGridTypeRow()->InDoor, gd->getCreator()->getFloorNO(), gd->getRoomID());

		std::vector<uint> vzone;
		vzone.push_back(czone);
		mModel->setLightListener(vzone);
	}

	mPickObjectUnit.setCreator(this);
	mPickObjectProxy.setObject(&mPickObjectUnit);	
}

void MagicBox::Dispose()
{
	if(mModel)
	{
		ModelManagerGcs::getSingleton().destroyModel(mModel);
		mModel = NULL;
	}

}

void MagicBox::Release()
{
	Dispose();

	delete this;

}

/*****************************************************************************************************************************************
 *****************************************************************************************************************************************
 *****************************************************************************************************************************************/

template<> MagicBoxSystem* Ogre::Singleton<MagicBoxSystem>::ms_Singleton = 0;

MagicBoxSystem::MagicBoxSystem(void):mClickbox(NULL)
{
	srand(GetTickCount());

	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_house_magic_box, NewMsgHandle(MagicBoxSystem, handle_house_magic_box));
	Regist(NetMsgType::msg_notify_pick_magic_box_gain, NewMsgHandle(MagicBoxSystem, handle_pick_magic_box_gain));
	Regist(NetMsgType::msg_req_client_empty_box_positions, NewMsgHandle(MagicBoxSystem, handle_req_empty_box_positions));

	Regist(NetMsgType::msg_notify_del_house_magic_box, NewMsgHandle(MagicBoxSystem, handle_del_house_magic_box));
	Regist(NetMsgType::msg_notify_add_house_magic_box, NewMsgHandle(MagicBoxSystem, handle_add_house_magic_box));
}

MagicBoxSystem::~MagicBoxSystem(void)
{
	for(MagicBoxMap::iterator itr = mMagicBoxs.begin(); itr != mMagicBoxs.end(); ++itr)
	{
		delete itr->second;
	}

	mMagicBoxs.clear();
}

void MagicBoxSystem::handle_house_magic_box( MsgPack &recBuff )
{
	notify_house_magic_box data;
	data.decode(recBuff);

	clearMagicBoxs();
	addMagicBoxs(data.box_list);

}

void MagicBoxSystem::handle_pick_magic_box_gain( MsgPack &recBuff )
{
	notify_pick_magic_box_gain data;
	data.decode(recBuff);

	//if(data.money)
	//	DynamicUIMgr::getSingleton().createNumberPaoPao((GameObj *)MainPlayer::getSingletonPtr(), NumberPaoPaoType::ENPPT_GOLD, data.money);
	luabind::call_function<void>(sLuaMgr.getL(), "HandlePickMagicBox", data.type, data.item_id, data.money);
	sMainPlayer.setMoveForWhat(MOVE_FOR_NONE);
}

void MagicBoxSystem::handle_req_empty_box_positions( MsgPack &recBuff )
{
	req_client_empty_box_positions data;
	data.decode(recBuff);

	notify_server_empty_box_positions req;
	outMagicBoxPos(req.pos_list, data.need_n);

	sGameMgr.getEQMsgManager()->Send(req);
}

void MagicBoxSystem::handle_del_house_magic_box( MsgPack &recBuff )
{
	notify_del_house_magic_box data;
	data.decode(recBuff);

	delMagicBox(data.box_id);

}

void MagicBoxSystem::handle_add_house_magic_box( MsgPack &recBuff )
{
	notify_add_house_magic_box data;
	data.decode(recBuff);

	addMagicBoxs(data.box_list);

}

void MagicBoxSystem::addMagicBoxs( std::vector<magic_box> &boxs )
{
	for(size_t i = 0; i < boxs.size(); i ++)
	{
		if(0 == boxs[i].inst_id)
			continue;

		uint mid = 0;

		switch(boxs[i].type)
		{
		case mbt_magic_box:
			{
				const MagicBoxRow *row = LogicTableManager::getSingleton().getMagicBoxRow(boxs[i].graphic_id);

				if(NULL == row)
					row = LogicTableManager::getSingleton().getMagicBoxRow(1);

				if(row)
					mid = row->modelId;
			}
			break;
		case mbt_garbage:
			{
				const GarbageRow *row = LogicTableManager::getSingleton().getGarbageRow(boxs[i].graphic_id);

				if(NULL == row)
					row = LogicTableManager::getSingleton().getGarbageRow(1);

				if(row)
					mid = row->modelId;
			}
			break;
		}

		if(0 == mid)
			continue;

		MagicBoxInfo info;
		info.inst_id = boxs[i].inst_id;
		info.model_id = mid;
		info.x = boxs[i].pos.x;
		info.y = boxs[i].pos.y;
		info.z = boxs[i].pos.z;
		info.type = boxs[i].type;

		HouseSceneGrid *gd = EQSceneManager::getSingleton().getHouseScene()->getGrid(info.y, info.x, info.z);
		info.pt = gd->getSceneNode()->getPosition();

		MagicBox *mbox = new MagicBox(info);
		mMagicBoxs.insert(std::make_pair(info.inst_id, mbox));
	}

	if (mMagicBoxs.size() > 0)
	{
		// 运行新手教学任务(第一次看到垃圾、宝箱)
		MainPlayer::getSingleton().getTask().startNewerTeachTask(15000);		
	}
	//char msg[64];
	//sprintf(msg, "add %d magic boxs.", boxs.size());
	//LOGMSG(msg);
}

void MagicBoxSystem::outMagicBoxPos( std::vector<grid_pos> &pts, int maxCnt )
{
	HouseScene * hs = EQSceneManager::getSingleton().getHouseScene();
	std::vector<HouseSceneGrid *> hgs;

	//const std::map<int, HouseSceneFloor*> &floors = hs->getFloors();
	//for(std::map<int, HouseSceneFloor*>::iterator itr = floors.begin(); itr != floors.end(); ++itr)
	{
		//const HouseSceneFloor::GridMap &gmap = itr->second->getGridMap();
		const HouseSceneFloor::GridMap &gmap = hs->getWalkViewFloor()->getGridMap();

		for(HouseSceneFloor::GridMap::const_iterator gitr = gmap.begin(); gitr != gmap.end(); ++gitr)
		{
			HouseSceneGrid *gd = gitr->second;

			if(false == gd->hasComponents() &&
				false == gd->isFrameGrid() &&
				false == gd->isBlocked() &&
				false == GiftSystem::getSingleton().hasGiftInGrid(gd) &&
				false == hasMagicBoxInGrid(gd) &&
				gd->getRoomID() && gd->getRoomID() < 20)
			{
				hgs.push_back(gd);
			}
		}
	}

	int cnt = 0;

	while(1)
	{
		if(cnt >= maxCnt)
			break;

		if(hgs.empty())
			break;

		int idx = rand() % hgs.size();
		HouseSceneGrid *grid = hgs[idx];
		grid_pos pt;
		pt.x = grid->getGridX();
		pt.z = grid->getGridZ();
		pt.y = grid->getCreator()->getFloorNO();

		pts.push_back(pt);

		cnt ++;

		std::vector<HouseSceneGrid *>::iterator itr = hgs.begin();
		itr += idx;
		hgs.erase(itr);
		
	}

	hgs.clear();

}

void MagicBoxSystem::delMagicBox( uint64 instId )
{
	MagicBoxMap::iterator itr = mMagicBoxs.find(instId);

	if(itr == mMagicBoxs.end())
		return;

	itr->second->Release();
	mMagicBoxs.erase(itr);

}

void MagicBoxSystem::clearMagicBoxs()
{
	for(MagicBoxMap::iterator itr = mMagicBoxs.begin(); itr != mMagicBoxs.end(); ++itr)
	{
		itr->second->Release();
	}

	mMagicBoxs.clear();

}

void MagicBoxSystem::clickMagicBox( MagicBox *mbox )
{
	//HOUSE_GRID_SIZE
	/*req_pick_magic_box req;
	req.box_id = mbox->getMagickBoxInfo().inst_id;

	sGameMgr.getEQMsgManager()->Send(req);*/
	mClickbox = mbox;
	GameObj* pObj = (GameObj*)MainPlayer::getSingletonPtr();
	std::queue<Ogre::Vector3> path;
	Ogre::Vector3 vDestPos(mbox->getMagickBoxInfo().pt.x, mbox->getMagickBoxInfo().pt.y, mbox->getMagickBoxInfo().pt.z);
	sMainPlayer.setCurWalkDestPos(vDestPos);
	pObj->findPath(pObj->getPos(),vDestPos,path);

	if (path.empty())
	{
		req_pick_magic_box req;
		req.box_id = mbox->getMagickBoxInfo().inst_id;

		sGameMgr.getEQMsgManager()->Send(req);
	}
	else
	{
		reqWalkForPickMagicbox();
	}

}
//----------------------------------------------------------
void MagicBoxSystem::reqWalkForPickMagicbox()
{
	if(mClickbox)
	{
		req_walk_for_pick_magic_box pack;
		pack.curr_pos.x = MainPlayer::getSingleton().getPos().x;
		pack.curr_pos.y = MainPlayer::getSingleton().getPos().y;
		pack.curr_pos.z = MainPlayer::getSingleton().getPos().z;
		pack.dest_pos.x = mClickbox->getMagickBoxInfo().pt.x;
		pack.dest_pos.y = mClickbox->getMagickBoxInfo().pt.y;
		pack.dest_pos.z = mClickbox->getMagickBoxInfo().pt.z;
		pack.move_type = sMainPlayer.getMoveType();
		pack.box_id = mClickbox->getMagickBoxInfo().inst_id;
		sMainPlayer.setMoveForWhat(MOVE_FOR_PICKMAGICBOX);
		sGameMgr.getEQMsgManager()->Send(pack);
	}
}
bool MagicBoxSystem::hasMagicBoxInGrid(HouseSceneGrid *grid)
{
	for(MagicBoxMap::iterator itr = mMagicBoxs.begin(); itr != mMagicBoxs.end(); ++itr)
	{
		if(grid->getGridX() == itr->second->getMagickBoxInfo().x &&
			grid->getGridZ() == itr->second->getMagickBoxInfo().z &&
			grid->getCreator()->getFloorNO() == itr->second->getMagickBoxInfo().y)
		{
			return true;
		}
	}

	return false;
}

uint64 MagicBoxSystem::getMagicBoxIdInGrid( HouseSceneGrid *grid )
{
	for(MagicBoxMap::iterator itr = mMagicBoxs.begin(); itr != mMagicBoxs.end(); ++itr)
	{
		if(grid->getGridX() == itr->second->getMagickBoxInfo().x &&
			grid->getGridZ() == itr->second->getMagickBoxInfo().z &&
			grid->getCreator()->getFloorNO() == itr->second->getMagickBoxInfo().y)
		{
			return itr->first;
		}
	}

	return 0;
}

MagicBox * MagicBoxSystem::getMagicBox( uint64 id )
{
	MagicBoxMap::iterator itr = mMagicBoxs.find(id);

	if(itr == mMagicBoxs.end())
		return NULL;
	
	return itr->second;
}
