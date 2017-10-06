/******************************************************************************
* 作者: Y3
* 时间: 2009-11-19
* 描述: 房屋资源管理器
******************************************************************************/
#ifndef HouseResManager_H
#define HouseResManager_H

#include "EQMainPrerequisites.h"

#include "HouseRes.h"

/** 房屋资源管理器
@remarks 从Ogre::ResourceManager继承, 遵循ogre的资源管理规则
*/
class HouseResManager : public Ogre::ResourceManager, public Ogre::Singleton<HouseResManager>
{
public:
	HouseResManager();
	~HouseResManager();

	/// 手动创建房屋资源
	HouseResPtr createManual( const Ogre::String& name, const Ogre::String& groupName, 
		Ogre::ManualResourceLoader* loader = 0);

protected:
	Ogre::Resource* createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, 
		const Ogre::String& group, bool isManual,Ogre:: ManualResourceLoader* loader, 
		const Ogre::NameValuePairList* createParams);

};


#endif
