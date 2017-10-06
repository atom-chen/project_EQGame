/******************************************************************************
* ����: Y3
* ʱ��: 2010-07-30
* ����: ũ������
******************************************************************************/
#ifndef FarmScene_H
#define FarmScene_H

#include "EQMainPrerequisites.h"
#include "ModelGcs.h"
#include "PickObjectProxy.h"

class FarmScene;
#define GRASS_MAX 3
//����״̬
enum eLandState
{
	LS_WASTE = 0,		//�ĵ�
	LS_NORMAL,			//��ͨ����ֲ��
	LS_DROUGHT_NORMAL,	//�ɺ���ͨ��
	LS_RED,				//���
	LS_DROUGHT_RED,		//�ɺ����
	LS_NUM,
};

/// ũ�����ؿ�������Ϣ
struct FarmLandAreaInfo {
	Ogre::Vector3		beginPos;
	int					xSize;
	int					zSize;
};


/// ũ�������Ϣ
struct FarmObjectInfo
{
	std::string			mName;		//!< �����
	Ogre::Vector3		mPos;
	float				mYDegree;
	std::string			mMesh;
	std::string			mMtrl;
};


/// ũ����ֲ��λ
class FarmPlantUnit
{
public:
	class PickObjectUnit : public PickObject
	{
		friend class FarmPlantUnit;
	public:
		/// ����Ϊ FarmPlantUnit::PickObjectUnit
		const std::string& getClassName();

		/// ��ȡ����, ���Ը������Ʋ��ж�ѡ�е����. ����3����������Ʒֱ���"Land", "Grass", "Plant"
		const std::string& getName() {return mName;}

		FarmPlantUnit* getCreator() {return mCreator;}
	protected:
		FarmPlantUnit*	mCreator;
		std::string		mName;
	};

public:
	FarmPlantUnit(FarmScene& creator, int index);
	~FarmPlantUnit();

	/// �������
	void changeLand(const std::string& mesh, const std::string& material = "");
	/// ��λ����Ϊԭ����Ч��
	void resetLand();

	/// ��ֲֲ��, ����Ѿ���ֲ��, ���滻
	void seedPlant(const std::string& mesh, const std::string& material = "");
	/// �Ƴ�ֲ��
	void removePlant();

	/// �ֲ�
	void seedGrass();

	/// ��ʶ��չ
	void signExpand();
	/// ����
	void removeGrass();

	/// ��ʶ��չ
	void removeSignExpand();
	/// ��ȡ��λӦ�е�����
	const Ogre::Vector3& getUnitPos() {return mPos;}

	/// ��ȡ�������
	int getIndex() {return mIndex;}

	ModelGcs* getLand() {return mLand;}
	ModelGcs* getGrass(int idx) {return idx >= GRASS_MAX ? NULL : mGrass[idx];}
	ModelGcs* getPlant() {return mPlant;}

	const Ogre::Vector3& getPos(){return mPos;}

protected:
	void calcPos();

protected:
	FarmScene&		mCreator;
	Ogre::Vector3	mPos;

	int 			mIndex;
	ModelGcs*		mLand;
	ModelGcs*		mGrass[GRASS_MAX];
	ModelGcs*		mPlant;
	ModelGcs*		mSignExpand;
	PickObjectProxy	mLandPxy;
	PickObjectProxy	mGrassPxy[GRASS_MAX];
	PickObjectProxy	mPlantPxy;
	PickObjectProxy	mSignExpandPxy;
	PickObjectUnit	mLandPo;
	PickObjectUnit	mGrassPo[GRASS_MAX];
	PickObjectUnit	mPlantPo;
	PickObjectUnit	mSignExpandPo;
};


/// ũ�����
class FarmObject : public PickObject
{
public:
	FarmObject(FarmScene& creator, const std::string& name);
	~FarmObject();

	const std::string& getClassName();

	void changeObject(const std::string& mesh, const std::string& material = "");

	const std::string& getName() {return mName;}
	ModelGcs* getModel() {return mObject;}

protected:
	FarmScene&			mCreator;

	std::string			mName;
	ModelGcs*			mObject;
	PickObjectProxy		mObjectPxy;
};


/** ũ������
@remarks ���Ժ�HouseScene����, ����Ŀǰ������HouseScene�з��ݴ���. �ⷽ������д�
	�պ���չ
@par ��ũ�����ѡȡ�Ĺ����ڲٿط��洦�������.
*/
class FarmScene
{
	friend class FarmPlantUnit;
	friend class FarmObject;
public:
	FarmScene();
	~FarmScene();

	/// ��ȡ���صĸ���
	int getLandSize();

	/// �������
	void changeLand(int index, const std::string& mesh, const std::string& material = "");
	/// ��λ����Ϊԭ����Ч��
	void resetLand(int index);

	/// ��ֲֲ��, ����Ѿ���ֲ��, ���滻
	void seedPlant(int index , const std::string& mesh, const std::string& material = "");
	/// �Ƴ�ֲ��
	void removePlant(int index);

	/// �ֲ�
	void seedGrass(int index);
	
	/// ��ʶ��չ
	void signExpand(int index);
	/// ����
	void removeGrass(int index);

	/// ��ʶ��չ
	void removeSignExpand(int index);


	/// ���ӻ����ũ�����
	void changeFarmObject(const std::string& name, const std::string& mesh, const std::string& material = "");
	/// �Ƴ�ũ�����
	void removeFarmObject(const std::string& name);


	//??ֲ������صĸ�������, �Ƿ���Ҫ����һ������??

	/// ��λũ����ʼЧ��. ����������Ч��
	void setInitialState();
	/// ��������
	//TODO:


	/// ͨ������, ��ȡũ����ֲ��λ
	FarmPlantUnit* getFarmPlantUnit(int index);

	/// ��ȡũ�����
	FarmObject* getFarmObject(const std::string& name);

	Ogre::SceneNode* getSceneNode();

	/// ��xml��ʼ��
	void initialFromXml();

	//����״̬����
	void changeLand(int index, int landstate);

	//�õ��ؿ�����
	const Ogre::Vector3& getLandUintPos(int index);

protected:
	void initPlantUnit();
	void initFarmObject();

protected:
	/// ����֮��ļ��
	float									mLandInterval;
	/// ֲ����ֲ���������ĵ�ĸ߶�
	float									mSeedHeight;
	/// ���ؿ���Ϣ
	std::vector<FarmLandAreaInfo>			mLandAreaInfos;
	/// �����Ϣ
	std::map<std::string, FarmObjectInfo>	mObjectInfos;

	/// ũ����ֲ��λʵ���б�
	std::vector<FarmPlantUnit*>				mFarmPlantUnits;
	/// ũ�����ʵ���б�
	std::map<std::string, FarmObject*>		mFarmObjects;

	/// ��ʼ����ģ��
	std::string								mInitialLandMesh;
	std::string								mInitialLandMaterial;
public:
	/// �Ӳ�ģ��,������ʶ������ģ��
	std::string								mGrassMesh;
	std::string								mGrassMaterial
											,mExpandSignMesh
											,mExpandSignMaterial
											,mLandStateMesh[LS_NUM]
											,mLandStateMaterial[LS_NUM]
											;
	Ogre::Vector3 mGrassPos[GRASS_MAX];

};



#endif

