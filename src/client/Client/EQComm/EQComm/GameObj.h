/*******************************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: GameOBJ
*******************************************************************************/
#ifndef GameObj_H
#define GameObj_H

#include <string>
#include <queue>

#include "PickObjectProxy.h"
#include "BodyActionMgr.h"

//#define NORMAL_WALK_SPEED 100
using namespace std;
class Avatar;
class ModelGcs;
// 游戏物体状态
enum GameObjState
{
	GOS_NONE,
	GOS_IDLE,
	GOS_STAND_IDLE,
	GOS_SIT_IDLE,
	GOS_LIE_IDLE,
	GOS_WALK,
	GOS_RUN,
	GOS_ANIMATION,
	GOS_USEFURNI_INTERVAL,
};
enum GameObjMoveType
{
	GOMT_WALK,
	GOMT_RUN,
};
// 游戏物体基本数据
struct GameObjBaseData
{
	friend class GameObj;
	friend class Player;
	friend class Npc;
protected:
	//GUID	guid;
	int						id;
	int						type;
	std::string				name;
	Ogre::Vector3			pos;
	GameObjState			state;
};

// 游戏物体基类
class GameObj : public PickObject
{
public:
/*
初始化，释放相关----------------------------------------------------------------
*/
	GameObj(void);
	virtual ~GameObj(void);
	virtual void		cleanup();
	void				createAvatar(int id);
	const std::string&	getClassName();
/*
基本属性相关--------------------------------------------------------------------
*/
public:
	void				setName(string& name){mBaseData.name=name;}
	string				getName(){return mBaseData.name;}
	void				setType(int t){mBaseData.type=t;}
	int					getType(){return mBaseData.type;}
	GameObjBaseData&	getObjBase()		{return mBaseData; }

	virtual void		setPos(Ogre::Vector3& v);
	Ogre::Vector3&		getPos()			{return mBaseData.pos;}
	void				setState(GameObjState state,std::string anima = "");
	void				forwardSetState(std::string& anima);
	GameObjState		getState()			{return mBaseData.state;}
	/**
	*/
	void				setActionState(int sta);
	/**
	*/
	void				setActionState(std::string anima);
	/**
	*/
	int					getActionState(){return mCurActionState;}
	BodyActionMgr*		getBodyActionMgr(){return mpBodyActionMgr;}
	/**
	*/
	float				getYaw(void);	// 值的范围[0,180],[0,-180](2011/5/26 add by hezhr)
	//
	void				setMoveType(int move_type){mMoveType = move_type;}
	int					getMoveType(){return mMoveType;}

protected:
	GameObjBaseData 	mBaseData;
	//
	int					mCurActionState;
	//
	std::string			mCurIdleActionName;
	//
	BodyActionMgr*		mpBodyActionMgr;
	//人物移动类型，跑Or走
	int					mMoveType;
	// 设置坐标
	void				_setPos(Ogre::Vector3& pos) { mBaseData.pos = pos;  }
	// 设置状态
	void				_setState(GameObjState s) {mBaseData.state=s;}
/*
移动，坐标同步相关--------------------------------------------------------------
*/
public:
	//返回true时坐标有更新，Player要向服务器同步坐标
	virtual bool		update();
	virtual void		moveTo(Ogre::Vector3& dest);
	virtual bool 		updateMove();

	/*寻路接口
		@params startPos 起始位置 endPos 终点位置 path 寻路结果
	*/
	void findPath(const Ogre::Vector3 & startPos, const Ogre::Vector3 & endPos, std::queue<Ogre::Vector3> & path);

	//播放动画
	void				playAnima(std::string ani, bool loop = false);
	void				stopAnimation();

	//获取当前动作名
	std::string			getCurrentAnima();
	
	/** 取动画播放时间(单位：毫秒)
	*/
	DWORD				getAnimationLength( const std::string& name );
	void				setDefAnimation(const std::string& ani);

	Ogre::Vector3&		getTargetWalk()		{return mTargetWalk;}
	Ogre::Vector3&		getWalkDir()		{return mWalkDirection;}

	//获取当前位置到另一个目标点的距离(hezhr)
	Ogre::Real getDistanceToTarget(const Ogre::Vector3& dest);
	//坐标瞬移时调用(2011/6/13 add by hezhr)
	virtual void		stuffedBear(void){}

	virtual void		setVisible(bool visible);

	//自动寻路接口,传入路径队列引用，操作成功返回true
	virtual bool		setPath(std::queue<Ogre::Vector3> &path);
	
	/*强制停止移动
	@param ifNotifySev 是否通知服务器，如果是客户端主玩家主动调用，一般需要通知
	，如果是收到服务器的停止通知，则一般不需要
	*/
	void				stopWalk(bool ifNotifySev=false);
	//
	virtual void		reqStartWalk(Ogre::Vector3& dest){}
	virtual void		reqStopWalk(){}
	void				reset();

	/**鼠标事件相关
	*/
	virtual void onMovein(){}
	virtual void onMoveout(){}

public:
	static float		NORMAL_WALK_SPEED;
	static float		NORMAL_RUN_SPEED;

protected:
	bool				_moveStep();
	void 				_setDirectionLength(Ogre::Vector3& tarPos);
	void 				_walk();
	/**当前行走目标点，距离，方向
	*/
	Ogre::Vector3		mTargetWalk;
	Ogre::Real			mTargetDist;
	Ogre::Vector3		mWalkDirection;

	//走路开始标识
	bool				mMoveStart;
	DWORD				mStandTime;
	//
	std::queue<Ogre::Vector3> mPathqueue;
/*
模型，换装相关------------------------------------------------------------------
*/
public:
	Ogre::SceneNode*	getMainNode(){return mMainNode;}
	ModelGcs*			getMainModel(){return mMainModel;}
	void				equipOn( uint slot, uint equipDisplayID, uint holdSlot );
	void				takeOff( uint slot );
	Avatar* 		    getAvatar(){return mAvatar;}
	// call this func when avatar changed.
	void				updateModel();
	uint				getModelHeight() { return mModelHeight; }
protected:
	//obj avatar
	Avatar*				mAvatar;
	//
	Ogre::SceneNode*	mMainNode;
	ModelGcs*			mMainModel;
	PickObjectProxy		mProxy;
	uint				mModelHeight;

/*
场景相关------------------------------------------------------------------------
*/
public:
	void				setSceneName(std::string name){mScenename=name;}
	const std::string&	getScencName(){return mScenename;}

protected:
	std::string			mScenename;

/*
脚底光圈，临时方案，以后要移到特效管理里
*/
private:
	Ogre::ManualObject*		mEffect;
	Ogre::AnimationState*	mAnimState;
public:
	void					addSelectedEffect();
	void					delSelectedEffect();

/*
粒子接口
*/
public:
	uint addParticle(uint id);
	void delParticleByHandle(uint handle);
	void delParticleByID(uint id);
	void startParticle();
	void pauseParticle();
	void stopParticle();
};

#endif