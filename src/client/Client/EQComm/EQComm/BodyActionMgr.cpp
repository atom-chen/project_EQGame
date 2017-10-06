#include "EQCommPCH.h"

#include "BodyActionMgr.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "NetPacket.h"
#include "MainPlayer.h"
#include "ObjMgr.h"
#include "Player.h"
#include "Ogre.h"
//-----------------------------------------------------------------------
// template<> BodyActionMgr* Ogre::Singleton<BodyActionMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
BodyActionMgr::BodyActionMgr() 
	: mBeginTime(0)
	, mPlayTime(0)
	,mLoopnum(0)
// 	,mCurActionState(0)
{	
	mAction_state_map.insert(std::pair<std::string, int>("Sit", AS_SIT));
	mAction_state_map.insert(std::pair<std::string, int>("Lie", AS_LIE));
	mAction_state_map.insert(std::pair<std::string, int>("Laugh", AS_LAUGH));
	mAction_state_map.insert(std::pair<std::string, int>("Hail", AS_HAIL));
	mAction_state_map.insert(std::pair<std::string, int>("Cry", AS_CRY));
	mAction_state_map.insert(std::pair<std::string, int>("Bow", AS_BOW));
	mAction_state_map.insert(std::pair<std::string, int>("Yawn", AS_YAWN));
	mAction_state_map.insert(std::pair<std::string, int>("Clap", AS_CLAP));
	mAction_state_map.insert(std::pair<std::string, int>("Smile", AS_SMILE));
	mAction_state_map.insert(std::pair<std::string, int>("Angry", AS_ANGRY));
	mAction_state_map.insert(std::pair<std::string, int>("Shake", AS_SHAKE));
	mAction_state_map.insert(std::pair<std::string, int>("Helpless", AS_HELPLESS));
	mAction_state_map.insert(std::pair<std::string, int>("Jump", AS_JUMP));
	mAction_state_map.insert(std::pair<std::string, int>("Wave", AS_WAVE));
	mAction_state_map.insert(std::pair<std::string, int>("Dance", AS_DANCE));
	mAction_state_map.insert(std::pair<std::string, int>("sit_lookaround", AS_SITLOOKAROUND));
	mAction_state_map.insert(std::pair<std::string, int>("sit_lie", AS_SITLIE));
	mAction_state_map.insert(std::pair<std::string, int>("lie_sit", AS_LIESIT));
	mAction_state_map.insert(std::pair<std::string, int>("Lookaround", AS_LOOKAROUND));
}
//-----------------------------------------------------------------------
BodyActionMgr::~BodyActionMgr()
{
	clear_allActionList();
}
//-----------------------------------------------------------------------
void BodyActionMgr::stop_timer()
{
	mBeginTime = 0;
}
//-----------------------------------------------------------------------
void BodyActionMgr::start_timer(DWORD ms)
{
	mBeginTime = GetTickCount();
	mPlayTime = ms;
}
//-----------------------------------------------------------------------
void BodyActionMgr::update()
{
	if(mBeginTime == 0)
		return;

	if(GetTickCount() - mBeginTime >= mPlayTime)
	{
		mBeginTime = 0;
		if(!check_AnimationList())
		{
			check_actionState();
			//reqEndBodyAction();
		}
		else
		{
			//check_actionState();
		}
		
	}
}
//-----------------------------------------------------------------------
void BodyActionMgr::reqSetBodyState(int body_state)
{
	req_sync_body_state req;
	req.body_state = body_state;
	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------------------------------
void BodyActionMgr::reqStartBodyAction(const std::string & ani, std::string action, const std::string & target,bool loop)
{
	std::string playername = MainPlayer::getSingleton().getName();
	mCurPlayAni = ani;
	mIsLoop = loop;
	req_start_body_action req;
	req.action  = action;
	req.target_account = target;
	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------------------------------
void BodyActionMgr::reqPlayAnimation(std::string ani, std::string action,const std::string & target,bool loop)
{
	req_play_animation req_play_ani;
	req_play_ani.action = action;
	req_play_ani.ani = ani;
	req_play_ani.target_account = target;
	if (loop == true)
	{
		req_play_ani.loop = 1;
	}
	else
	{
		req_play_ani.loop = 0;
	}
	sGameMgr.getEQMsgManager()->Send(req_play_ani);
	sMainPlayer.playAnimation(ani,sMainPlayer.getAccount(),target,loop);
}
//-----------------------------------------------------------------------
void BodyActionMgr::reqEndBodyAction()
{
	req_end_body_action req;

	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------------------------------
// void BodyActionMgr::notifyPlayAnimation(const std::string & ani, 
// 										const std::string & playerAccount, 
// 										const std::string & targetAccount,
// 										bool loop)
// {
// 	
// }
//-----------------------------------------------------------------------
void BodyActionMgr::notifyStartBodyAction(const std::string & action, 
										  const std::string & playerAccount, 
										  const std::string & targetAccount)
{
	reqPlayAnimation(mCurPlayAni,action,targetAccount,mIsLoop);
}
//-----------------------------------------------------------------------
void BodyActionMgr::notifyEndBodyAction(const std::string & playerAccount, 
										const std::string & targetAccount)
{
	stop_timer();

	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(playerAccount);

	Player* player = NULL;
	int player_action_state = 0;
	
	if(isMainPlayer)
	{
		player = (Player*)MainPlayer::getSingletonPtr();
	}
	else
	{
		player = (Player*)ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, playerAccount);
	}
	if(player)
	{
		switch (player->getActionState())
		{
		case AS_SIT:
			reqSetBodyState(GOS_SIT_IDLE);
			player->setState(GOS_SIT_IDLE);
			player->setActionState(0);
			break;
		case AS_DANCE:
			break;
		case AS_LIE:
			reqSetBodyState(GOS_LIE_IDLE);
			player->setState(GOS_LIE_IDLE);
			player->setActionState(0);
			break;
		case AS_SITLOOKAROUND:
			reqSetBodyState(GOS_SIT_IDLE);
			player->setState(GOS_SIT_IDLE);
			player->setActionState(0);
			break;
		case AS_SITLIE:
			reqSetBodyState(GOS_LIE_IDLE);
			player->setState(GOS_LIE_IDLE);
			player->setActionState(0);
			break;
		case AS_LIESIT:
			reqSetBodyState(GOS_SIT_IDLE);
			player->setState(GOS_SIT_IDLE);
			player->setActionState(0);
			break;
		default:
			player->stopAnimation();
			reqSetBodyState(GOS_STAND_IDLE);
			player->setState(GOS_STAND_IDLE);
		}
		
	}
	else
	{
		LOGERR("recEndBodyAction 找不到这个玩家: " + playerAccount);
	}	
}
//-----------------------------------------------------------------------
void BodyActionMgr::add_noLoopAction(Action_Name act_name,std::string action,bool loop)
{
	ActionType act(act_name,action,loop);
	mNoloopAction_list.push(act);
}
//-----------------------------------------------------------------------
void BodyActionMgr::clear_noLoopAction()
{
	while (!mNoloopAction_list.empty())
	{
		mNoloopAction_list.pop();
	}
}
//-----------------------------------------------------------------------
void BodyActionMgr::clear_loopAction()
{
	mLoopAction_list.clear();
}
//-----------------------------------------------------------------------
void BodyActionMgr::clear_allActionList()
{
	clear_noLoopAction();
	clear_loopAction();
}
//-----------------------------------------------------------------------
void BodyActionMgr::add_LoopAction(Action_Name act_name,std::string action,bool loop)
{
	ActionType act(act_name,action,loop);
	mLoopAction_list.push_back(act);
}
//----------------------------------------------------------------------
void BodyActionMgr::play_noLoopActionList()
{
	if (!mNoloopAction_list.empty())
	{
		ActionType act = mNoloopAction_list.front();
		mNoloopAction_list.pop();
		reqStartBodyAction(act.mAcitonName,act.mAction,"",act.mLoop);
	}
}
//------------------------------------------------------------------------
void BodyActionMgr::play_LoopActionList(int loop_num)
{
	static int itr_add_pos = 0;
	if (loop_num == 0)
	{
		mLoopAction_list.clear();
		itr_add_pos = 0;
		return;
	}
	int loopList_size = mLoopAction_list.size();
	int cur_pos = 0;
	if (cur_pos += itr_add_pos++, cur_pos < loopList_size)
	{
		reqStartBodyAction(mLoopAction_list[cur_pos].mAcitonName,mLoopAction_list[cur_pos].mAction, "",mLoopAction_list[cur_pos].mLoop);
		if (cur_pos == loopList_size - 1)
		{
			itr_add_pos = 0;
			if (mLoopnum > 0)
			{
				--mLoopnum;
			}
		}
	}
}
//-----------------------------------------------------------------------
void BodyActionMgr::setLoopAniNum(int loopnum)
{
	mLoopnum = loopnum;
}
//-----------------------------------------------------------------------
bool BodyActionMgr::check_AnimationList()
{
	if(mNoloopAction_list.empty())
	{
		if(!check_LoopList())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		play_noLoopActionList();
		return true;
	}
}
//-------------------------------------------------------------------------
bool BodyActionMgr::check_noLoopList()
{
	return true;
}
//-------------------------------------------------------------------------
bool BodyActionMgr::check_LoopList()
{
	if (mLoopAction_list.empty())
	{
		return false;
	}
	else
	{
		play_LoopActionList(mLoopnum);
		return true;
	}
}
//-------------------------------------------------------------------------
// void BodyActionMgr::setActionState(int sta)
// {
// 	mCurActionState = sta;
// // 	LOGMSG(Ogre::StringConverter::toString(mCurActionState));
// }
//--------------------------------------------------------------------------
// void BodyActionMgr::setActionState(std::string anima)
// {
// 	if (mAction_state_map.end() == mAction_state_map.find(anima))
// 	{
// 		LOGERR("Doesn't exsit " + anima + " state!");
// 		return;
// 	}
// 	
// 	setActionState(mAction_state_map[anima]);
// 
// }
//--------------------------------------------------------------------------
void BodyActionMgr::check_actionState()
{	
// 	std::string def_ani = "";
// 	switch (mCurActionState)
// 	{
// 	case AS_SIT:
// 		def_ani = "";//"Lookaround_sit";
// 		break;
// 	case AS_DANCE:
// 		def_ani = "Dance";
// 		break;
// 	case AS_LIE:
// 		def_ani = "";
// 		break;
// 	default:
// 		def_ani = "";
// 		
// 	}
// 	if (def_ani == "")
	{
		reqEndBodyAction();
	}
// 	else
// 	{
// 		MainPlayer::getSingletonPtr()->setDefAnimation(def_ani);
// 	}
}