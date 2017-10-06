#include "EQCommPCH.h"
#include "NPC.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "GameObj.h"
#include "ObjMgr.h"
#include "NetPacket.h"
//
#include "Avatar.h"
#include "AvatarRTT.h"
#include "AvatarAdjuster.h"
#include "SmallModules.h"
#include "NPCTable.h"
#include "ItemTable.h"
#include "GameEventMgr.h"
//#include "FunnyFarm.h"
#include "MainPlayer.h"
#include "DynamicUIMgr.h"
#include "ScriptFunction.h"
#include "Variable.h"
#include "CursorMgr.h"

void SmallModules::test()
{
	//sFunnyFarm.reqEnterFarm((MainPlayer::getSingleton().getAccount()));
}
//------------------------------------------------------------------------------
void _notify_npc_list(ByteArray& recBuff)
{
	static notify_npc_list npclist;
	npclist.decode(recBuff);
	vector<npc_map_mapping_info>::iterator it = npclist.npc_map_mapping_list.begin()
								,itend = npclist.npc_map_mapping_list.end();

	npc_map_mapping_info maping;
	for( ; it != itend; it++ )
	{
		maping = (*it);
		NPC *p = (NPC*)(ObjMgr::getSingleton().
					reqestObj(ObjMgr::GOT_NPC, maping.npc_name));

		p->setMaping(maping);
		p->updateLook();

		BornRow* born = LogicTableManager::getSingleton().getBornRow(maping.born_id);
		assert(born);
		p->setPos(born->pos);
		p->getMainNode()->setOrientation( 
			Ogre::Quaternion((Ogre::Radian)Ogre::Math::DegreesToRadians(Ogre::Real(born->dir)), Ogre::Vector3::UNIT_Y) );

		p->saveOrient();
	}
}
//------------------------------------------------------------------------------
void _notify_npc_show_dialog_by_option(ByteArray& recBuff)
{
	static notify_npc_show_dialog_by_option notify;
	notify.decode(recBuff);

	//notify.
	NPC* npc = (NPC*)(ObjMgr::getSingleton().getPickObj());
	
	if(npc->getType() == ObjMgr::GOT_NPC)
		npc->showDiaByOpt(notify);

}
//------------------------------------------------------------------------------
void _msg_notify_npc_close_dialog(ByteArray& recBuff)
{
	static  notify_npc_close_dialog notify;
	notify.decode(recBuff);

	//notify.
	NPC* npc = (NPC*)(ObjMgr::getSingleton().getPickObj());

	if(npc->getType() == ObjMgr::GOT_NPC)
	{
		npc->closeDia();
		npc->resetOrient();
	}
}

//------------------------------------------------------------------------------
void _notify_npc_show_dialog_by_message(ByteArray& recBuff)
{
	static notify_npc_show_dialog_by_message notify;
	notify.decode(recBuff);

	//notify.
	NPC* npc = (NPC*)(ObjMgr::getSingleton().getPickObj());

	if(npc->getType() == ObjMgr::GOT_NPC)
		npc->showDiaByMsg(notify);
}

//------------------------------------------------------------------------------
void NPC::closeDia()
{
	GameEventMgr::getSingleton().fireEvent("NPC_CLOSE_DIALOG");
}
//------------------------------------------------------------------------------
void NPC::showDiaByOpt(notify_npc_show_dialog_by_option opt)
{
	mDiaOpt = opt;
	mOptCmdSize = opt.command_list.size();
	GameEventMgr::getSingleton().fireEvent("UPDATE_NPC_UI_BY_OPT");
}
//------------------------------------------------------------------------------
void NPC::showDiaByMsg(notify_npc_show_dialog_by_message msg)
{
	mDiaMsg = msg;
	GameEventMgr::getSingleton().fireEvent("UPDATE_NPC_UI_BY_MSG");
}
//------------------------------------------------------------------------------
void NPC::updateLook()
{
	const NPCRow* row = NPCTableMgr::getSingleton().getNPCRow(mMaping.npc_id);

	if(getAvatar() != NULL)
	{
		delete(mAvatar);
		mAvatar = NULL;
	}
	createAvatar(row->roleraceid);

	Avatar* ava = getAvatar();
	AvatarAdjuster* adj = ava->getAdjuster();
	adj->doChange(AvatarAdjuster::atSex, row->gender);

	updateModel();

	ava = getAvatar();
	adj = ava->getAdjuster();

	//adj->doChange(AvatarAdjuster::atHairType, row->hair);
	//adj->doChange(AvatarAdjuster::atHairColor, row->haircolor);
	//adj->doChange(AvatarAdjuster::atFace, row->);
	//adj->doChange(AvatarAdjuster::atSkin, row->skincolor);
	//adj->doChange(AvatarAdjuster::atBeard, row->beard);

 	equipOn(ES_HAT, row->hat);
 	equipOn(ES_COAT, row->coat);
 	equipOn(ES_NOSE, row->glass);
 	equipOn(ES_PANTS, row->pants);
 	equipOn(ES_BACK, row->bag);
 	equipOn(ES_SHOES, row->shoes);
 	equipOn(ES_HAND, row->hands);

	setVisible(true);
	setState(GOS_IDLE);

	if(!row->action.empty())
		getMainModel()->playAnimation(row->action, true);
}
//------------------------------------------------------------------------------
void NPC::equipOn(uint slot, uint itemid)
{
	const ItemRow*  itemRow =  ItemTableManager::getSingleton().getItemRow(itemid);
	if(!itemRow)
		return;

	const ItemEquipRow* itemRowEquip = ItemTableManager::getSingleton().getItemEquipRow(itemRow->subID);
	if(!itemRowEquip)
		return;

	getAvatar()->equipOn(slot, itemRowEquip->equip_disp_id, itemRowEquip->equip_placeholder);
}
//------------------------------------------------------------------------------
bool NPC::onClickObj()
{
	if (sMainPlayer.getState() == GOS_WALK)
		return false;

	Ogre::Real dis  = getPos().distance(sMainPlayer.getPos());

	const NPCRow* row = NPCTableMgr::getSingleton().getNPCRow(mMaping.npc_id);
	assert(row);

	if(dis - float(row->distance) > 10.0f) //要有点误差
	{
		Ogre::Vector3 face = mMainNode->_getDerivedOrientation() * Ogre::Vector3::UNIT_X;
		face.normalise();
		face.y = 0;
		sMainPlayer.setCurWalkDestPos(getPos() + face*row->distance);
		sMainPlayer.reqStartWalk(getPos() + face*row->distance);
		
		return true;
	}
	reqNPCommand(0,0,0);

	////朝向主玩家
	Ogre::Vector3 direction = sMainPlayer.getPos() - getPos();
	direction.normalise();
	direction.y = 0;

	sMainPlayer.getMainNode()->setDirection(-direction, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_X);
	if(row->autoface)
		mMainNode->setDirection(direction, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_X);

// 	set_onNPCMouseCursor();
	return true;
}
//------------------------------------------------------------------------------
void NPC::reqNPCommand(int page_index, int command_index, uint64 unique_id)
{
	req_npc_command req;
	req.npc_key					= mMaping.npc_key;
	req.page_index				= page_index;
	req.command_index	= command_index;
	req.unique_id				= unique_id;

	sGameMgr.getEQMsgManager()->Send(req);

}
//------------------------------------------------------------------------------
void NPC::reqNPCommandByOpt()
{
	req_npc_command req;
	req.npc_key					= mMaping.npc_key;
	
	npc_command& cmd = mDiaOpt.command_list[mClickIdx];

	req.page_index				= cmd.page_index;
	req.command_index	= cmd.command_index;
	req.unique_id				= cmd.unique_id;

	sGameMgr.getEQMsgManager()->Send(req);

}
//------------------------------------------------------------------------------
void NPC::reqNPCommandBymsg()
{
	req_npc_command req;
	req.npc_key					= mMaping.npc_key;
	req.page_index				= mDiaMsg.page_index;
	req.command_index	= mDiaMsg.command_index;
	req.unique_id				= mDiaMsg.unique_id;

	sGameMgr.getEQMsgManager()->Send(req);

}
//------------------------------------------------------------------------------
bool EQMsgManager::handleByNPC()
{

	switch(mMsgtype)
	{		
	case NetMsgType::msg_notify_npc_list:
		{
			_notify_npc_list(mRecebuff);
		}
		break;
	case NetMsgType::msg_notify_npc_close_dialog:			
		{
			_msg_notify_npc_close_dialog(mRecebuff);
		}
		break;
	case NetMsgType::msg_notify_npc_show_dialog_by_option:
		{
			_notify_npc_show_dialog_by_option(mRecebuff);
		}
		break;
	case NetMsgType::msg_notify_npc_show_dialog_by_message:
		{
			_notify_npc_show_dialog_by_message(mRecebuff);
		}
		break;
	default:
		return false;
	}
	return true;
}
//--------------------------------------------------------------
void NPC::setText(std::string winname, std::string buf)
{
	CEGUI::Window* win;
	win = CEGUI::WindowManager::getSingleton().getWindow(winname.c_str());
	if(win)
		win->setText((CEGUI::utf8*)buf.c_str());
}
//-------------------------------------------------------
void NPC::setMaping(npc_map_mapping_info& map)
{
	mMaping = map;

	const NPCRow* row = NPCTableMgr::getSingleton().getNPCRow(mMaping.npc_id);
	assert(row);

	mModelHeight = row->height;
	showHeadup();
}
//-------------------------------------------------------
void NPC::showHeadup()
{
	if(!mHeadup.empty())
		DynamicUIMgr::getSingleton().destroyWindow(mHeadup);

	mHeadup = DynamicUIMgr::getSingleton().createHeadup(this, 1);
}
//-------------------------------------------------------
void NPC::setVisible(bool visible)
{
	GameObj::setVisible(visible);

	if(!mHeadup.empty() && CEGUI::WindowManager::getSingleton().isWindowPresent(mHeadup))
		CEGUI::WindowManager::getSingleton().getWindow(mHeadup)->setAlpha(visible?1.0f:0);
}

void NPC::resetOrient()
{
	mMainNode->setOrientation(mOrientation);
}

void NPC::saveOrient()
{
	mOrientation = mMainNode->getOrientation();
}
//-------------------------------------------------------
void NPC::onMovein()
{
	set_onNPCMouseCursor();
}
//-------------------------------------------------------
void NPC::onMoveout()
{
	sCursorMgr.setCurTarget(0);
	sCursorMgr.setCurTargetType(0);
	sCursorMgr.backtoLastState();
}
//-------------------------------------------------------
void NPC::set_onNPCMouseCursor()
{
	sCursorMgr.setCurTarget(1);
	sCursorMgr.setCurTargetType(ObjMgr::GOT_NPC);
	sCursorMgr.setState(CursorMgr::CS_ON_NPC);
}