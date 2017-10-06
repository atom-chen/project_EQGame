#include "EQCommPCH.h"
#include "CEGUI.h"
#include "CEGUILua.h"

#include "LuaManager.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"

#include "EnumDef.h"
#include "ByteArray.h"
#include "NetPacket.h"
#include "ObjMgr.h"
#include "Player.h"
#include "GameState.h"
#include "MainPlayer.h"

//-------------------------------------------------------------------------
BOOL EQMsgManager::StartWalkRequest(Ogre::Vector3 vCurrPos, 
									   Ogre::Vector3 vDestination,int move_type)
{
	if( !sStateMgr.gaming() )
		return FALSE;
	if (vDestination == Ogre::Vector3(0,0,0))
	{
		return FALSE;
	}
	req_start_walk   aWalk;
	aWalk.curr_pos.x  = vCurrPos.x;
	aWalk.curr_pos.y  = vCurrPos.y;
	aWalk.curr_pos.z  = vCurrPos.z;
	aWalk.dest_pos.x  = vDestination.x;
	aWalk.dest_pos.y  = vDestination.y;
	aWalk.dest_pos.z  = vDestination.z;
	aWalk.move_type = move_type;
	sMainPlayer.setMoveForWhat(MOVE_FOR_MOVE);
	return m_Socket.Send(aWalk);
}
//-------------------------------------------------------------------------
BOOL EQMsgManager::StopWalkRequest(Ogre::Vector3 vCurrPos)
{
	if( !sStateMgr.gaming() )
		return FALSE;

	req_stop_walk   aWalk;
	aWalk.pos.x  = vCurrPos.x;
	aWalk.pos.y  = vCurrPos.y;
	aWalk.pos.z  = vCurrPos.z;
	sMainPlayer.setMoveForWhat(MOVE_FOR_NONE);
	return m_Socket.Send(aWalk);
}
//-------------------------------------------------------------------------
BOOL EQMsgManager::MoveSyncPosition(Ogre::Vector3 vCurrPos)
{
	if( !sStateMgr.gaming() )
		return FALSE;

	req_sync_position   aPosition;
	aPosition.pos.x  = vCurrPos.x;
	aPosition.pos.y  = vCurrPos.y;
	aPosition.pos.z  = vCurrPos.z;

	return m_Socket.Send(aPosition);
}
//------------------------------------------------------------------------------
void EQMsgManager::StartWalkReply(ByteArray& recBuff)
{
    notify_start_walk walk;

    walk.decode(recBuff);

	Ogre::Vector3 vDestPos(walk.dest_pos.x, walk.dest_pos.y, walk.dest_pos.z);

	GameObj* pObj ;

	if(sObjMgr.isMainPlayer(walk.account))
		pObj = (GameObj*)MainPlayer::getSingletonPtr();//.moveTo(vDestPos);
	else
		pObj = sObjMgr.findObj(ObjMgr::GOT_PLAYER,walk.account);

    if (!pObj)
        return;
	std::queue<Ogre::Vector3> path;
	pObj->stopAnimation();
	pObj->setMoveType(walk.move_type);
    pObj->findPath(pObj->getPos(),vDestPos,path);

	if (!path.empty())
	{
		bool ret = pObj->setPath(path);
		if(sObjMgr.isMainPlayer(walk.account))
		{
			sMainPlayer.showTargetPath(vDestPos);
			// (2011/7/6 add by hezhr)
			Ogre::Vector3 vCurrPos(walk.curr_pos.x, walk.curr_pos.y, walk.curr_pos.z);
			sMainPlayer.enableAutoUpDown(vCurrPos, vDestPos);
			if (!ret)
			{
				sMainPlayer.reqStopWalk();
			}
		}
	}
	else
		LOGERR("Ã»ÓÐÂ·¾¶£¡");

}
//--------------------------------------------------------------------------------
void EQMsgManager::StopWalkReply(ByteArray& recBuff)
{
    notify_stop_walk walk;

    walk.decode(recBuff);

	const bool isMainPlayer = sObjMgr.isMainPlayer(walk.account);
    GameObj* pObj = NULL;
	if (isMainPlayer)
		pObj = MainPlayer::getSingletonPtr();
	else
		pObj = sObjMgr.findObj(ObjMgr::GOT_PLAYER,walk.account);

    if (!pObj)
        return;

	pObj->stopWalk();
	pObj->setPos( Ogre::Vector3(walk.pos.x, walk.pos.y, walk.pos.z ) );
	
	//pObj->setState(GOS_STAND_IDLE);
	if (isMainPlayer)
	{
		MainPlayer::getSingleton().hideTargetPath();
		sMainPlayer.autoUpDown();	// £¨2011/7/5 add by hezhr£©
	}
}
