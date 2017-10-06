/****************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 主玩家单件体
*		因为游戏客户端操作大多针对主玩家，因此抽出来做
*****************************************************************/
#ifndef MainPlayer_H
#define MainPlayer_H

#include "EQMainPrerequisites.h"
#include "Player.h"
#include "InputMgr.h"

enum MOVE_FOR_WHAT
{
	MOVE_FOR_NONE,
	MOVE_FOR_MOVE,
	MOVE_FOR_PICKMAGICBOX,
	MOVE_FOR_USEFURNI,
};
/*
*/
struct CreateEquip 
{
	CreateEquip()
		: curHatEquip(0)
		, curClothEquip(0)
		, curTrouserEquip(0)
		, curShoesEquip(0)
		, curGlassesEquip(0)
		, curBackEquip(0)
		, curHandEquip(0)
		, curTrinketEquip(0)
		
	{
	}

	uint curHatEquip;
	uint curClothEquip;
	uint curTrouserEquip;
	uint curShoesEquip;
	uint curGlassesEquip;
	uint curBackEquip;
	uint	curHandEquip;
	uint curTrinketEquip;

};

class MainPlayerListener
{
public:
	MainPlayerListener() {};
	virtual ~MainPlayerListener(){};

public:
	virtual void exit() {};
	virtual void moveBegin(){}
	virtual void moveEnd(){};
};

class MainPlayer : public Player, public Ogre::Singleton<MainPlayer>
	,public EQInputEventListener
{
public:
			MainPlayer(void);
	virtual ~MainPlayer(void);

	void	init();
	virtual void LButtonUp	( UINT& flags, POINT point );
/*
移动，坐标同步相关--------------------------------------------------------------
*/
public:
	//直接移动接口
	virtual void	moveTo(Ogre::Vector3& dest, 
							MainPlayerListener* listener=NULL);

	//自动寻路接口
	virtual bool	setPath(std::queue<Ogre::Vector3> &path, 
							MainPlayerListener* listener=NULL);

	//请示移动开始，结束，同不
	virtual void	reqStartWalk(Ogre::Vector3& dest);
	virtual void	reqStopWalk();
	void			syncPosToServer();
	void			reqContinueWalk();
	void			setCurWalkDestPos(Ogre::Vector3 dest_pos){mCurDestPos = dest_pos;}

	//更新坐标，状态
	virtual bool	update();
	virtual bool	updateMove();
	//检查传送点
	void			_checkTeleport();

	void			setListener(MainPlayerListener* listener);
	
	//
	CreateEquip &	getCreateEquip(){return mCreateEquip;}
	void			setCeateEquip(uint,uint,uint,uint,uint,uint,uint,uint);

	// 室内自动上下楼相关(2011/6/13 add by hezhr)
	void			enableAutoUpDown(const Ogre::Vector3& curr, const Ogre::Vector3& dest);
	void			autoUpDown(void);
	virtual void	stuffedBear(void);	// 玩家位置瞬移时调用
	/*
	**/
	void			setMoveForWhat(int move_for){mMoveForWhat = move_for;}
	int				getMoveForWhat(){return mMoveForWhat;}

private:		
	MainPlayerListener* mListener;
	bool				mAutoUpDown;
	int					mMoveForWhat;

public:
	CreateEquip			mCreateEquip;

/*
目标点提示动画
*/
private:
	Ogre::SceneNode*		mTargetNode;
	Ogre::AnimationState*	mArrowAnimState;
	Ogre::ManualObject*		mTargetEffect;
	Ogre::AnimationState*	mCircleAnimState;
	void					_TargetPoint();
public:
	void					showTargetPath(Ogre::Vector3& pos);
	void					hideTargetPath();

/*
脚底光圈，临时方案，以后要移到特效管理里
*/
public:
	std::string			mTargetName;
	uint				mTargetType;
	void				clearTarget();
	void				setTarget(std::string& name, uint type);

/*
鲜花鸡蛋
*/
public:
	uint				getFlowerCount() { return mFlowerCount; }
	void				setFlowerCount(uint flower);
	uint				getDecoration() { return mDecoration; }
	void				setDecoration(uint decoration);
	uint				getFlowerToday() { return mFlowerToday; }
	void				setFlowerToday(notify_house_flower_change& flower);
	uint				getEggToday() { return mEggToday; }
	void				setEggToday(notify_house_egg_change& egg);
	void				showFlowerAni(notify_send_flower_success& data);
	void				showEggAni(notify_send_egg_success& data);
	void				showFlowerLog(notify_flower_logs& log);
	void				showEggLog(notify_egg_logs& log);
	void				showFne(notify_today_flower_egg_count& fne);

protected:
	uint				mFlowerCount;
	uint				mDecoration;
	uint				mFlowerToday;
	uint				mEggToday;
	Ogre::Vector3		mCurDestPos;

//日常重置
public:
	void				resetDaily(notify_new_day_reset& reset);
	void				reqPlayBigExpression(int id);
};
#define sMainPlayer MainPlayer::getSingleton()
#endif