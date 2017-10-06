#include "EQFarmPCH.h"
#include "MessageManager.h"
#include "EnumDef.h"
#include "ByteArray.h"
#include "NetPacket.h"
#include "SmallModules.h"
#include "GameEventMgr.h"
#include "GameState.h"
#include "Login.h"
#include "MsgSystem.h"

bool CMessageManager::logicProc()
{
    //重复登入
    if(handleLoginRepeat())
        return true;
    //打开界面相关
    if(handleOpenUIMsg())
        return true;
    //聊天相关
    if(handleByChat())
        return true;
    if(handleByNPC())
        return true;
    //角色属性
    if(handleByProPerty())
        return true;
    //HDJ Add
    if (handleByFriendSys())
        return true;
    //农场
    if(handleByFarm())
        return true;
    //农场
    if(handleByTask())
        return true;
    //登录，创建角色
    if(!sStateMgr.gaming())
    {
		if(Login::getSingleton().handleMsg())
			return true;
    }
    //消息系统
    if(handleByMsgSys())
		return true;
    //其它小模块
    if(handleByGM())
		return true;
    //公告系统
    if(handleBroadCast())
        return true;

	return false;
}
