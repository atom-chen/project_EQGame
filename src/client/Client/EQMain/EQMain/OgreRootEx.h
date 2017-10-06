/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-19
* ����: Ogre::Root����չ
******************************************************************************/
#ifndef OgreRootEx_H
#define OgreRootEx_H

#include "EQMainPrerequisites.h"

class HouseResManager;
class ProjectorFactory;

/** Ogre::Root����չ
@remarks ΪRoot������һЩ����. ���Դ���Ogreʱ, ����ҪOGRE_NEW Ogre::Root. ����
	new OgreRootEx�Ϳ�����.
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
