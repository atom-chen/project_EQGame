/******************************************************************************
* ����: lf
* ʱ��: 2010-4-22
* ����: �Ҿ߽���
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
		FANI_MORPH		= 1,	//���ζ���
		FANI_TEXTURE	= 2,	//������
		FANI_SKELETON	= 3,	//��������
		FANI_PARTICLE	= 4,	//���Ӷ���
		FANI_LIGHT		= 5,	//�ƹ⶯��
	};

	enum FITMENTACTION_TYPE
	{
		FACT_TURNONBYLAMP	= 1, //���ƾ����Ϳ���
		FACT_TURNONBYROOM	= 2, //�����俪�ƣ�Ŀǰֻ֧�����ڷ���
		FACT_OPENUI			= 3, //��UI
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
	std::string getDelTime();	// ��ȡ�Ҿߵ���ʱ��(2011/9/22 add by hezr)
	void	selLoopMenu(int menu);
	bool	isInteracting() { return mBegin; }
	bool	isPreparing(){return mPrepare;}
	HouseSceneComponent* getFitment() { return mFitment; }
	Ogre::Vector3& getInteractPos() { return mPos; }

	void	sndBeginInteract();
	void	sndEndInteract();

	//һ��ʼ�úõĽṹ���ĳ����������߰���������ȥ�����˰ɨr(������)�q
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
