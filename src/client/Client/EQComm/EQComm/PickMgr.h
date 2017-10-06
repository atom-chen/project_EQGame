/******************************************************************************
* ����: lf
* ʱ��: 2010-4-21
* ����: pick����
******************************************************************************/
#ifndef PickMgr_H
#define PickMgr_H

#include "EQMainPrerequisites.h"
#include "InputMgr.h" 

class ModelGcs;
class GameObj;

/** ϵͳ̫����, �����ع���!      --Y3
*/
class PickMgr : public Ogre::Singleton<PickMgr>
	,public EQInputEventListener
{
public:

	PickMgr();
	~PickMgr();

private:
	//Ogre::MovableObject*	mPick;	//ԭ������ʽ���ܱ���ָ��İ�ȫ��
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
	
	//by ��_mike
	bool clickGameObj(GameObj* model);

	//bool isPickObject(ModelGcs* model);
	//void clear() { mPick = NULL; mLast = NULL; }
	void clear() { mPickName = ""; mLastName = ""; }


	virtual void LButtonUp	( UINT& flags, POINT point );

	virtual void MouseMove	( UINT& flags, POINT delta, POINT point );
};

#endif
