/******************************************************************************
* 作者: 麦_Mike
* 时间: 2010-01-08
* 描述:家装窗口对应的事件及管理
******************************************************************************/
#ifndef FitmentEvent_H
#define FitmentEvent_H

//eqmain
#include "HouseSceneComponent.h"
#include "HSCFitmentCtrl.h"
#include "InputMgr.h"
#include "NetPacket.h"

//-----家装模式----------
enum eFitmentMode
{
	FMM_NONE	= 0,
	FMM_ADD		= 1,//添加
	FMM_EDIT	= 2,//编辑

	FMM_CONUT	
};

//
//
///*****
//家装基类，集合添加和编辑共性，目前基本是从工具那边Ctrl+C过来的
//再经过简单的修改
//*/
/** 编辑房屋部件事件
*/
class FitmentEvent : public HSCFitmentCtrl::Listener
{
	friend class FitmentEventMgr;
public:
	enum OpMode { OpNewFitment, OpEditFitment };
	enum PickState {PS_NONE, PS_PICKUP, PS_ROTATE};
public:

	FitmentEvent();

	~FitmentEvent();
public:
	void createNewFitment( uint compID );
	HouseSceneComponent * getCurrentFitment() { return mComp; }
public:
	
	void LButtonDown( POINT point );
	void LButtonUp( POINT point );
	void RButtonUp( POINT point );
	void MouseMove(UINT& flags, POINT delta, POINT point );
	void KeyUp( UINT nChar, UINT nRepCnt );
	void update();

	void onMovein();
	void onMoveout();
	void setMouseCursor();
	bool isPickingFitment(){return mPickState!=PS_NONE;}

	void pickup();
	void putdown();
	int  getPickState() { return mPickState; }

	void notifyCanLay( HouseSceneGrid * grid, int face, int height, HouseSceneComponent * compRep );
	void notifyForbidLay(HouseReturnValue hrv);

private:
	void delFitment();
	void pickupFitment(HouseSceneComponent* comp, POINT point);
	void undo();
	Ogre::Ray getRay( POINT point );
	HouseSceneComponent * getComponentByPoint( POINT point );

	void rotateFitment(float degree);
protected:
	/// 是否可以停靠
	bool						mCanLay;
	/// 拾取状态
	PickState					mPickState;


	/// 目前正在操作的物件
	HouseSceneComponent*		mComp;
	uint						mCompID;
	/// 可以替换的物件
	HouseSceneComponent*		mCompRep;
	house_furniture				mCompRepInfo;

	HSCFitmentCtrl *			mFitmentCtrl;

	HouseReturnValue			mErr;

	/// 目前所在的格子
	HouseSceneGrid *			mGrid;
	/// 目前
	int							mFace;
	/// 摆放高度
	int							mBottom;


	/// 原先所在的格子
	HouseSceneGrid *			mOldGrid;
	/// 原先的面向
	int							mOldFace;

	int							mOldBottom;
	/// 操作模式
	OpMode						mMode;

	Ogre::Camera *				mCamera;

	POINT						mPoint;
	Ogre::Vector3				mRotatePos;
};




/**
家装事件统一管理，简单的家装模式切换，事件分发等
*/
class FitmentEventMgr : public Ogre::Singleton<FitmentEventMgr>
	,public EQInputEventListener
{
public:
	FitmentEventMgr();
	virtual ~FitmentEventMgr();

	//设置家装模式
	void setFitMode(eFitmentMode fm);
	eFitmentMode getFitMode(){return mFitMode;}

	bool isFiting(){ return mFitMode!= FMM_NONE; }

	bool isPickingFitment(){return mEvent->isPickingFitment();}

	//设置添加家具拖拉显示模型
	void setFitDro(int id);

	//取当前家具id
	int getCurrFurID();

	//直接添加、删除一个家具，目前只用于接收服务端消息
	uint addFitment(house_furniture& furni, bool turnOn=true);
	void delFitment(DWORD id);

	//家具特效
	void undo() { if(mEvent) mEvent->undo(); }
	void playFitmentFX(uint id);
	void delFitmentFX(uint id);
	void rotateFitment(float degree);
	void pickupFitment() { if(mEvent) mEvent->pickup(); }
	void putdownFitment() { if(mEvent) mEvent->putdown(); }
	int  getPickState() { return mEvent? mEvent->getPickState() : 0; }

public:
	//输入事件处理
	virtual void LButtonDown( UINT& flags, POINT point);
	virtual void LButtonUp( UINT& flags, POINT point);
	virtual void RButtonUp( UINT& flags, POINT point);
	virtual void MouseMove( UINT& flags, POINT delta, POINT point);
	virtual void KeyUp( UINT& flags, UINT nChar, UINT nRepCnt = 0);
	virtual void Update	(float timElapsed);
	void onMovein();
	void onMoveout();

protected:
	FitmentEvent	* mEvent;
	eFitmentMode	mFitMode;
};

#define sFitMgr FitmentEventMgr::getSingleton()
#endif
