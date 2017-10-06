/******************************************************************************
* 作者: hjx
* 时间: 2010-10-09
* 描述: 人物动作
******************************************************************************/
#ifndef BodyActionMgr_H
#define BodyActionMgr_H

#include "EQMainPrerequisites.h"
// #include "MainPlayer.h"
class BodyActionMgr/* : public Ogre::Singleton<BodyActionMgr>*/
{
public:
	BodyActionMgr();
	~BodyActionMgr();
public:
	typedef std::string Action_Name ;

	/**动作结构体
	*/
	typedef struct ActionStruct 
	{
		ActionStruct(std::string name, std::string action,bool loop)
			:mAcitonName(name)
			,mAction(action)
			,mLoop(loop)
		{

		}

		std::string mAcitonName;
		std::string mAction;
		bool		mLoop;
	} ActionType;
	/**
	*/
	enum ACTSTATE
	{
		AS_SIT = 1,
		AS_LIE,
		AS_LAUGH,
		AS_HAIL,
		AS_CRY,
		AS_BOW,
		AS_YAWN,
		AS_CLAP,
		AS_SMILE,
		AS_ANGRY,
		AS_SHAKE,
		AS_HELPLESS,
		AS_JUMP,
		AS_WAVE,
		AS_DANCE,
		AS_SITLOOKAROUND,
		AS_SITLIE,
		AS_LIESIT,
		AS_LOOKAROUND,
	};

public:
	void update();
	void reqSetBodyState(int body_state);
	void reqStartBodyAction(const std::string & ani, std::string action, const std::string & target,bool loop);
	void reqPlayAnimation(std::string ani, std::string action,const std::string & target,bool loop);
	void notifyStartBodyAction(const std::string & action, const std::string & playerAccount, const std::string & targetAccount);
	void reqEndBodyAction();
	void notifyEndBodyAction(const std::string & playerAccount, const std::string & targetAccount);

	/**
	*/
	void add_noLoopAction(Action_Name act_name,std::string action,bool loop);
	/**
	*/
	void add_LoopAction(Action_Name act_name,std::string action,bool loop);
	/**
	*/
	void clear_noLoopAction();
	/**
	*/
	void clear_loopAction();
	/**
	*/
	void clear_allActionList();
	/**
	*/
	void setLoopAniNum(int loopnum);
	/**
	*/
	void play_noLoopActionList();
	/**
	*/
	void play_LoopActionList(int loop_num);
	/**
	*/
	bool check_AnimationList();
// 	/**
// 	*/
// 	void setActionState(int sta);
// 	/**
// 	*/
// 	void setActionState(std::string anima);
// 	/**
// 	*/
// 	int getActionState(){return mCurActionState;}
	/**
	*/
	void check_actionState();
	/**
	*/
	std::map<std::string,int>& getActionStateMap(){return mAction_state_map;}

	/**
	*/
// 	std::string getCurrentAnimation(){return MainPlayer::getSingletonPtr()->getCurrentAnima();}
	void start_timer(DWORD ms);
private:
	void stop_timer();
	bool check_LoopList();
	bool check_noLoopList();
private:
	DWORD	mBeginTime;
	DWORD	mPlayTime;
	std::queue<ActionType> mNoloopAction_list;
	std::vector<ActionType> mLoopAction_list;
	int						mLoopnum;
// 	int						mCurActionState;
	std::map<std::string,int> mAction_state_map;
// 	Player					*mActionPlayer;
	std::string				mCurPlayAni;
	bool					mIsLoop;
};


#endif
