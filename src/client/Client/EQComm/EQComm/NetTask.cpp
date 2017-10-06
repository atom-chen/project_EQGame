/*******************************************************************
* 作者: lf
* 时间: 2010-06-24
* 描述: 任务消息处理
********************************************************************/

#include "EQCommPCH.h"
#include "EQMsgManager.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "NetMsgType.h"
#include "NetPacket.h"
#include "MainPlayer.h"

//-------------------------------------------------------------------------
bool EQMsgManager::handleByTask()
{
	switch(mMsgtype)
	{
	case NetMsgType::msg_notify_task_list:
		{
			notify_task_list data;
			data.decode(mRecebuff);

			MainPlayer::getSingleton().getTask().recTask(data.task_list);
			luabind::call_function<void>(sLuaMgr.getL(), "Task_OpenUI");
		}
		break;

	case NetMsgType::msg_notify_give_up_task:
		{
			notify_give_up_task data;
			data.decode(mRecebuff);

			MainPlayer::getSingleton().getTask().recGiveup(data.id);
		}
		break;

	case NetMsgType::msg_notify_task_complete:
		{
			notify_task_complete data;
			data.decode(mRecebuff);

			MainPlayer::getSingleton().getTask().recComplete(data.id);
		}
		break;

	case NetMsgType::msg_notify_update_task_progress:
		{
			notify_update_task_progress data;
			data.decode(mRecebuff);

			MainPlayer::getSingleton().getTask().recUpdateTaskProgress(data);
		}
		break;

	case NetMsgType::msg_notify_give_task:
		{
			notify_give_task data;
			data.decode(mRecebuff);

			MainPlayer::getSingleton().getTask().handle_give_task(data.task);
		}

	default:
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------