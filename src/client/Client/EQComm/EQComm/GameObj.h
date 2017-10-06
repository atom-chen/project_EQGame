/*******************************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: GameOBJ
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
// ��Ϸ����״̬
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
// ��Ϸ�����������
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

// ��Ϸ�������
class GameObj : public PickObject
{
public:
/*
��ʼ�����ͷ����----------------------------------------------------------------
*/
	GameObj(void);
	virtual ~GameObj(void);
	virtual void		cleanup();
	void				createAvatar(int id);
	const std::string&	getClassName();
/*
�����������--------------------------------------------------------------------
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
	float				getYaw(void);	// ֵ�ķ�Χ[0,180],[0,-180](2011/5/26 add by hezhr)
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
	//�����ƶ����ͣ���Or��
	int					mMoveType;
	// ��������
	void				_setPos(Ogre::Vector3& pos) { mBaseData.pos = pos;  }
	// ����״̬
	void				_setState(GameObjState s) {mBaseData.state=s;}
/*
�ƶ�������ͬ�����--------------------------------------------------------------
*/
public:
	//����trueʱ�����и��£�PlayerҪ�������ͬ������
	virtual bool		update();
	virtual void		moveTo(Ogre::Vector3& dest);
	virtual bool 		updateMove();

	/*Ѱ·�ӿ�
		@params startPos ��ʼλ�� endPos �յ�λ�� path Ѱ·���
	*/
	void findPath(const Ogre::Vector3 & startPos, const Ogre::Vector3 & endPos, std::queue<Ogre::Vector3> & path);

	//���Ŷ���
	void				playAnima(std::string ani, bool loop = false);
	void				stopAnimation();

	//��ȡ��ǰ������
	std::string			getCurrentAnima();
	
	/** ȡ��������ʱ��(��λ������)
	*/
	DWORD				getAnimationLength( const std::string& name );
	void				setDefAnimation(const std::string& ani);

	Ogre::Vector3&		getTargetWalk()		{return mTargetWalk;}
	Ogre::Vector3&		getWalkDir()		{return mWalkDirection;}

	//��ȡ��ǰλ�õ���һ��Ŀ���ľ���(hezhr)
	Ogre::Real getDistanceToTarget(const Ogre::Vector3& dest);
	//����˲��ʱ����(2011/6/13 add by hezhr)
	virtual void		stuffedBear(void){}

	virtual void		setVisible(bool visible);

	//�Զ�Ѱ·�ӿ�,����·���������ã������ɹ�����true
	virtual bool		setPath(std::queue<Ogre::Vector3> &path);
	
	/*ǿ��ֹͣ�ƶ�
	@param ifNotifySev �Ƿ�֪ͨ������������ǿͻ���������������ã�һ����Ҫ֪ͨ
	��������յ���������ֹ֪ͣͨ����һ�㲻��Ҫ
	*/
	void				stopWalk(bool ifNotifySev=false);
	//
	virtual void		reqStartWalk(Ogre::Vector3& dest){}
	virtual void		reqStopWalk(){}
	void				reset();

	/**����¼����
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
	/**��ǰ����Ŀ��㣬���룬����
	*/
	Ogre::Vector3		mTargetWalk;
	Ogre::Real			mTargetDist;
	Ogre::Vector3		mWalkDirection;

	//��·��ʼ��ʶ
	bool				mMoveStart;
	DWORD				mStandTime;
	//
	std::queue<Ogre::Vector3> mPathqueue;
/*
ģ�ͣ���װ���------------------------------------------------------------------
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
�������------------------------------------------------------------------------
*/
public:
	void				setSceneName(std::string name){mScenename=name;}
	const std::string&	getScencName(){return mScenename;}

protected:
	std::string			mScenename;

/*
�ŵ׹�Ȧ����ʱ�������Ժ�Ҫ�Ƶ���Ч������
*/
private:
	Ogre::ManualObject*		mEffect;
	Ogre::AnimationState*	mAnimState;
public:
	void					addSelectedEffect();
	void					delSelectedEffect();

/*
���ӽӿ�
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