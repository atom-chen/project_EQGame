#include "HouseEditorPCH.h"

#include "UIOutsideCutEvent.h"
#include "SceneManager.h"
#include "SceneEntityObject.h"
#include "GraphicsSystem.h"

//-----------------------------------------------------------------------
void loadCutLevelCfg()
{
	Ogre::ConfigFile cf;
	cf.load("../Config/cut_level.cfg");
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;

		if(secName.empty())
		{
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				const std::string& key = i->first;
				int val = Ogre::StringConverter::parseUnsignedInt(i->second);
				if ( key == "CUT_DISTANCE_FAR" )
				{
					EQSceneManager::getSingleton().getSceneInfo()->setCutRadius(CDL_FAR, val);
				}
				else if ( key == "CUT_DISTANCE_MID" )
				{
					EQSceneManager::getSingleton().getSceneInfo()->setCutRadius(CDL_MID, val);
				}
				else if ( key == "CUT_DISTANCE_CLOSE" )
				{
					EQSceneManager::getSingleton().getSceneInfo()->setCutRadius(CDL_CLOSE, val);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------
OutsideCutEvent * OutsideCutEvent::create()
{
	return new OutsideCutEvent();
}
//-----------------------------------------------------------------------
OutsideCutEvent::OutsideCutEvent() : MfcInputEvent(epOusideCut)
{
	loadCutLevelCfg();

	EQSceneManager::getSingleton().getSceneInfo()->enterOctreeScene(GraphicsSystem::getSingleton().getCamera());
}
//-----------------------------------------------------------------------
OutsideCutEvent::~OutsideCutEvent()
{
	EQSceneManager::getSingleton().getSceneInfo()->exitOctreeScene();
}



