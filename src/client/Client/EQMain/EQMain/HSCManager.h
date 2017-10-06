/******************************************************************************
* ����: Y3
* ʱ��: 2009-12-03
* ����: ���ݲ���������
******************************************************************************/
#ifndef HSCManager_H
#define HSCManager_H

#include "EQMainPrerequisites.h"

#include "HouseSceneComponent.h"


class HSCManager : public Ogre::Singleton<HSCManager>
{
public:
	typedef stdext::hash_map<uint, HouseSceneComponent*> HouseSceneComponentMap;
public:
	HSCManager();
	~HSCManager();

	HouseSceneComponent* createHouseSceneComponent();

	void destroyHouseSceneComponent(uint uid);

	void destroyHouseSceneComponent(HouseSceneComponent* hsc);

	bool hasHouseSceneComponent(uint uid) const;

	HouseSceneComponent* getHouseSceneComponent(uint uid);

	const HouseSceneComponentMap & getHouseSceneComponentMap() const { return mHouseSceneComponentMap; }
protected:
	HouseSceneComponentMap	mHouseSceneComponentMap;

	ulong				mNum;
};



#endif
