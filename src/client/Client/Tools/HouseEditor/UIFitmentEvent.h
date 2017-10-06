/******************************************************************************
* ����: Y3
* ʱ��: 2009-01-04
* ����:��װ���ڶ�Ӧ���¼�
******************************************************************************/
#ifndef UIFitmentEvent_H
#define UIFitmentEvent_H

#include "MfcInputEvent.h"

#include "HouseSceneComponent.h"
#include "HSCFitmentCtrl.h"




/** �༭���ݲ����¼�
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
	/// �Ƿ����ͣ��
	bool						mCanLay;
	/// �Ƿ�����ʰȡ���
	bool						mPickingFitment;


	/// Ŀǰ���ڲ��������
	HouseSceneComponent*		mFitment;
	/// �����滻�����
	HouseSceneComponent*		mCompRep;
	HSCFitmentCtrl *			mFitmentCtrl;

	HouseReturnValue			mErr;

	/// Ŀǰ���ڵĸ���
	HouseSceneGrid *			mGrid;
	/// Ŀǰ
	int							mFace;
	/// �ڷŸ߶�
	int							mBottom;


	/// ԭ�����ڵĸ���
	HouseSceneGrid *			mOldGrid;
	/// ԭ�ȵ�����
	int							mOldFace;

	int							mOldBottom;
	/// ����ģʽ
	OpMode						mMode;
	/// 
	uint						mLastCompID;

};



/** ������
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

/** ���õƹ�
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
