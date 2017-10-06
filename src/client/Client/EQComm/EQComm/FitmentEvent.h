/******************************************************************************
* ����: ��_Mike
* ʱ��: 2010-01-08
* ����:��װ���ڶ�Ӧ���¼�������
******************************************************************************/
#ifndef FitmentEvent_H
#define FitmentEvent_H

//eqmain
#include "HouseSceneComponent.h"
#include "HSCFitmentCtrl.h"
#include "InputMgr.h"
#include "NetPacket.h"

//-----��װģʽ----------
enum eFitmentMode
{
	FMM_NONE	= 0,
	FMM_ADD		= 1,//���
	FMM_EDIT	= 2,//�༭

	FMM_CONUT	
};

//
//
///*****
//��װ���࣬������Ӻͱ༭���ԣ�Ŀǰ�����Ǵӹ����Ǳ�Ctrl+C������
//�پ����򵥵��޸�
//*/
/** �༭���ݲ����¼�
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
	/// �Ƿ����ͣ��
	bool						mCanLay;
	/// ʰȡ״̬
	PickState					mPickState;


	/// Ŀǰ���ڲ��������
	HouseSceneComponent*		mComp;
	uint						mCompID;
	/// �����滻�����
	HouseSceneComponent*		mCompRep;
	house_furniture				mCompRepInfo;

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

	Ogre::Camera *				mCamera;

	POINT						mPoint;
	Ogre::Vector3				mRotatePos;
};




/**
��װ�¼�ͳһ�����򵥵ļ�װģʽ�л����¼��ַ���
*/
class FitmentEventMgr : public Ogre::Singleton<FitmentEventMgr>
	,public EQInputEventListener
{
public:
	FitmentEventMgr();
	virtual ~FitmentEventMgr();

	//���ü�װģʽ
	void setFitMode(eFitmentMode fm);
	eFitmentMode getFitMode(){return mFitMode;}

	bool isFiting(){ return mFitMode!= FMM_NONE; }

	bool isPickingFitment(){return mEvent->isPickingFitment();}

	//������ӼҾ�������ʾģ��
	void setFitDro(int id);

	//ȡ��ǰ�Ҿ�id
	int getCurrFurID();

	//ֱ����ӡ�ɾ��һ���Ҿߣ�Ŀǰֻ���ڽ��շ������Ϣ
	uint addFitment(house_furniture& furni, bool turnOn=true);
	void delFitment(DWORD id);

	//�Ҿ���Ч
	void undo() { if(mEvent) mEvent->undo(); }
	void playFitmentFX(uint id);
	void delFitmentFX(uint id);
	void rotateFitment(float degree);
	void pickupFitment() { if(mEvent) mEvent->pickup(); }
	void putdownFitment() { if(mEvent) mEvent->putdown(); }
	int  getPickState() { return mEvent? mEvent->getPickState() : 0; }

public:
	//�����¼�����
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
