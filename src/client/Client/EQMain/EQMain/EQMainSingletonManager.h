/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-22
* ����: EQMain����������
******************************************************************************/
#ifndef EQMainSingletonManager_H
#define EQMainSingletonManager_H

#include "EQMainPrerequisites.h"

/** 
@remarks ����б�Ҫ, ���Ӵ���ͼ�εĽӿ�
*/
class EQMainSingletonManager : public Ogre::Singleton<EQMainSingletonManager>
{
public:
	EQMainSingletonManager();
	~EQMainSingletonManager();
};

#endif