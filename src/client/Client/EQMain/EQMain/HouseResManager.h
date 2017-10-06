/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-19
* ����: ������Դ������
******************************************************************************/
#ifndef HouseResManager_H
#define HouseResManager_H

#include "EQMainPrerequisites.h"

#include "HouseRes.h"

/** ������Դ������
@remarks ��Ogre::ResourceManager�̳�, ��ѭogre����Դ�������
*/
class HouseResManager : public Ogre::ResourceManager, public Ogre::Singleton<HouseResManager>
{
public:
	HouseResManager();
	~HouseResManager();

	/// �ֶ�����������Դ
	HouseResPtr createManual( const Ogre::String& name, const Ogre::String& groupName, 
		Ogre::ManualResourceLoader* loader = 0);

protected:
	Ogre::Resource* createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, 
		const Ogre::String& group, bool isManual,Ogre:: ManualResourceLoader* loader, 
		const Ogre::NameValuePairList* createParams);

};


#endif
