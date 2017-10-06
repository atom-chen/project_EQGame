/******************************************************************************
* 作者: Y3
* 时间: 2009-01-04
* 描述:家装窗口对应的事件
******************************************************************************/
#ifndef UIFitmentEvent_H
#define UIFitmentEvent_H

#include "MfcInputEvent.h"

#include "HouseSceneComponent.h"
#include "HSCFitmentCtrl.h"




/** 编辑房屋部件事件
*/
class EditFitmentEvent : public MfcInputEvent, public HSCFitmentCtrl::Listener
{
public:
	enum OpMode { OpNewFitment, OpEditFitment };
public:
	explicit EditFitmentEvent( HSCFitmentCtrl * c );

	~EditFitmentEvent();
public:
	static EditFitmentEvent * create();

	void createNewFitment( uint compID );
public:

	void LButtonDown( UINT nFlags, CPoint point, bool & processed );

	void LButtonUp( UINT nFlags, CPoint point, bool & processed );

	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );

	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

	//
	void notifyCanLay( HouseSceneGrid * grid, int face, int height, HouseSceneComponent * compRep );

	void notifyForbidLay(HouseReturnValue hrv);
private:
	void delFitment();
	void pickupFitment(HouseSceneComponent* comp, CPoint point);
	void undo();
protected:
	/// 是否可以停靠
	bool						mCanLay;
	/// 是否正在拾取物件
	bool						mPickingFitment;


	/// 目前正在操作的物件
	HouseSceneComponent*		mFitment;
	/// 可以替换的物件
	HouseSceneComponent*		mCompRep;
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
	/// 
	uint						mLastCompID;

};



/** 浏览物件
*/
class BrowseComponentEvent : public MfcInputEvent
{
public:
	BrowseComponentEvent();
	~BrowseComponentEvent();
	static BrowseComponentEvent * create();
public:
	void setBrowseComponent( HouseSceneComponent * comp );
public:
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );
private:
	bool exec(CPoint point);
private:
	HouseSceneComponent * mLastComp;
};

/** 设置灯光
*/
class SceneEntityObject;
class SetLightEvent : public MfcInputEvent
{
public:
	SetLightEvent();
	~SetLightEvent();
	static SetLightEvent * create();
public:
	void setLightComponent(HouseSceneComponent* comp);
	void setLightSceneEntityObject(SceneEntityObject* obj);
public:
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );
private:
	bool exec(CPoint point);
private:
	HouseSceneComponent * mLastComp;
	SceneEntityObject * mLastObj;
};

#endif
