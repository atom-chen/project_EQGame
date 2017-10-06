/****************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ����ҵ�����
*		��Ϊ��Ϸ�ͻ��˲�������������ң���˳������
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
�ƶ�������ͬ�����--------------------------------------------------------------
*/
public:
	//ֱ���ƶ��ӿ�
	virtual void	moveTo(Ogre::Vector3& dest, 
							MainPlayerListener* listener=NULL);

	//�Զ�Ѱ·�ӿ�
	virtual bool	setPath(std::queue<Ogre::Vector3> &path, 
							MainPlayerListener* listener=NULL);

	//��ʾ�ƶ���ʼ��������ͬ��
	virtual void	reqStartWalk(Ogre::Vector3& dest);
	virtual void	reqStopWalk();
	void			syncPosToServer();
	void			reqContinueWalk();
	void			setCurWalkDestPos(Ogre::Vector3 dest_pos){mCurDestPos = dest_pos;}

	//�������꣬״̬
	virtual bool	update();
	virtual bool	updateMove();
	//��鴫�͵�
	void			_checkTeleport();

	void			setListener(MainPlayerListener* listener);
	
	//
	CreateEquip &	getCreateEquip(){return mCreateEquip;}
	void			setCeateEquip(uint,uint,uint,uint,uint,uint,uint,uint);

	// �����Զ�����¥���(2011/6/13 add by hezhr)
	void			enableAutoUpDown(const Ogre::Vector3& curr, const Ogre::Vector3& dest);
	void			autoUpDown(void);
	virtual void	stuffedBear(void);	// ���λ��˲��ʱ����
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
Ŀ�����ʾ����
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
�ŵ׹�Ȧ����ʱ�������Ժ�Ҫ�Ƶ���Ч������
*/
public:
	std::string			mTargetName;
	uint				mTargetType;
	void				clearTarget();
	void				setTarget(std::string& name, uint type);

/*
�ʻ�����
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

//�ճ�����
public:
	void				resetDaily(notify_new_day_reset& reset);
	void				reqPlayBigExpression(int id);
};
#define sMainPlayer MainPlayer::getSingleton()
#endif