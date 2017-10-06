/******************************************************************************
* 作者: lf
* 时间: 2010-4-21
* 描述: pick管理
******************************************************************************/
#ifndef PickMgr_H
#define PickMgr_H

#include "EQMainPrerequisites.h"
#include "InputMgr.h" 

class ModelGcs;
class GameObj;

/** 系统太乱了, 必须重构啊!      --Y3
*/
class PickMgr : public Ogre::Singleton<PickMgr>
	,public EQInputEventListener
{
public:

	PickMgr();
	~PickMgr();

private:
	//Ogre::MovableObject*	mPick;	//原来的样式不能保障指针的安全性
	//Ogre::MovableObject*	mLast;
	std::string				mPickName;
	std::string				mLastName;
	std::string				mPickPlayerName;
	Ogre::Vector3			mPoint;
	float					mDistance;
	Ogre::ColourValue		mObjHighLight;
	Ogre::ColourValue		mComHighLight;
	unsigned __int64		mPickGarbageId;
	unsigned __int64		mPickLastGarbageId;

public:
	Ogre::MovableObject* getPickObject();
	Ogre::MovableObject* getLastPickObject();
	Ogre::MovableObject* getObject(std::string &name);
	Ogre::Vector3& getPickPoint() { return mPoint; }
	float getPickDistance() { return mDistance; }
	std::string getPickPlayerName() {return mPickPlayerName; }

	void init();
	bool pick(const Ogre::Ray &ray, Ogre::uint32 flag=-1);
	bool pickGarbage(const Ogre::Ray &ray);
	void movein();
	void moveinGarbage();
	void moveout();
	void moveoutGarbage();
	bool click();
	bool clickGarbage();
	
	//by 麦_mike
	bool clickGameObj(GameObj* model);

	//bool isPickObject(ModelGcs* model);
	//void clear() { mPick = NULL; mLast = NULL; }
	void clear() { mPickName = ""; mLastName = ""; }
	void update();

	virtual void LButtonUp	( UINT& flags, POINT point );

	virtual void MouseMove	( UINT& flags, POINT delta, POINT point );
};

#endif
