/******************************************************************************
* 作者: Y3
* 时间: 2009-11-19
* 描述: Ogre::Root的扩展
******************************************************************************/
#ifndef OgreRootEx_H
#define OgreRootEx_H

#include "EQMainPrerequisites.h"

class HouseResManager;
class ProjectorFactory;

/** Ogre::Root的扩展
@remarks 为Root新增加一些功能. 所以创建Ogre时, 不需要OGRE_NEW Ogre::Root. 而是
	new OgreRootEx就可以了.
*/
class OgreRootEx : public Ogre::Singleton<OgreRootEx>
{
public:
	OgreRootEx(const Ogre::String& pluginFileName = "plugins.cfg", 
		const Ogre::String& configFileName = "ogre.cfg", 
		const Ogre::String& logFileName = "Ogre.log");

	~OgreRootEx();

protected:
	Ogre::Root*				mRoot;
	HouseResManager*		mHouseResManager;
	ProjectorFactory*		mProjectorFactory;
};

#endif
