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
	Ogre::Vector3			mPoint;
	float					mDistance;
	Ogre::ColourValue		mHighLight;

public:
	Ogre::MovableObject* getPickObject();
	Ogre::MovableObject* getLastPickObject();
	Ogre::Vector3& getPickPoint() { return mPoint; }
	float getPickDistance() { return mDistance; }

	void init();
	bool pick(const Ogre::Ray &ray, Ogre::uint32 flag=-1);
	void movein();
	void moveout();
	bool click();
	
	//by 麦_mike
	bool clickGameObj(GameObj* model);

	//bool isPickObject(ModelGcs* model);
	//void clear() { mPick = NULL; mLast = NULL; }
	void clear() { mPickName = ""; mLastName = ""; }


	virtual void LButtonUp	( UINT& flags, POINT point );

	virtual void MouseMove	( UINT& flags, POINT delta, POINT point );
};

#endif
