/******************************************************************************
* 作者: lf
* 时间: 2010-4-22
* 描述: 家具交互
******************************************************************************/
#ifndef FitmentInteractMgr_H
#define FitmentInteractMgr_H

#include "EQMainPrerequisites.h"
#include "MainPlayer.h"

class HouseSceneComponent;
class SceneEntityObject;
struct FurniInteractRow;

class FitmentInteractMgr : public Ogre::Singleton<FitmentInteractMgr>
{
	enum FITMENTANIMATION_TYPE
	{
		FANI_MORPH		= 1,	//变形动画
		FANI_TEXTURE	= 2,	//纹理动画
		FANI_SKELETON	= 3,	//骨骼动画
		FANI_PARTICLE	= 4,	//粒子动画
		FANI_LIGHT		= 5,	//灯光动画
	};

	enum FITMENTACTION_TYPE
	{
		FACT_TURNONBYLAMP	= 1, //按灯具类型开灯
		FACT_TURNONBYROOM	= 2, //按房间开灯，目前只支持所在房间
		FACT_OPENUI			= 3, //打开UI
	};

public:
	FitmentInteractMgr();
	~FitmentInteractMgr();

	void debug(float x, float y, float z);

private:
	HouseSceneComponent*	mFitment;
	SceneEntityObject*		mObject;
	const FurniInteractRow*	mFurniFuncRow;
	Ogre::Vector3			mPos;
	bool					mBegin;
	bool					mPrepare;
	DWORD					mBeginTime;

private:
	bool	_isInHouse();
	bool	_findPath(std::queue<Ogre::Vector3>& result);
	void	_beginRoleAni(uint64 instance_id, const FurniInteractRow* row, Player* player, int pos);
	void	_beginFurniAni(uint64 instance_id, const FurniInteractRow* row, int pos);
	void	_beginFurniAct(uint64 instance_id, uint actid);
	void	_endFurniAni(uint64 instance_id, const FurniInteractRow* row, int pos);
	HouseSceneComponent* _getFitmentByInstance(uint64 instance_id);
	SceneEntityObject* _getObjectByInstance(uint64 instance_id);

public:
	bool	canClick(HouseSceneComponent* com);
	bool	canClick(SceneEntityObject* obj);
	void	pickFitment(HouseSceneComponent* com);
	void	pickObject(SceneEntityObject* obj);

	void	update();
	uint	getInteractingFitmentID();
	uint	getLoopMenuID();
	std::string getDelTime();	// 获取家具到期时间(2011/9/22 add by hezr)
	void	selLoopMenu(int menu);
	bool	isInteracting() { return mBegin; }
	bool	isPreparing(){return mPrepare;}
	HouseSceneComponent* getFitment() { return mFitment; }
	Ogre::Vector3& getInteractPos() { return mPos; }

	void	sndBeginInteract();
	void	sndEndInteract();

	//一开始好好的结构被改成这样，杯具啊，看不懂去抽服务端吧╮(￣▽￣)╭
	void	recBeginInteractRole(std::string& account, uint64 instance_id, int menu, int pos_index, int fur_status=0, bool autoUse=false);
	void	recBeginInteractFurni(uint64 instance_id, int menu, int pos_index, int fur_status);
	void	recEndInteract(std::string& account, uint64 instance_id, int menu, int pos_index, Ogre::Vector3 pos, int pre_status, int fur_status);
};

class FitMentInteractListener : public MainPlayerListener
{
public:
	virtual void exit();
	virtual void moveBegin();
	virtual void moveEnd();
};

#endif
