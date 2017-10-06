#include <stdio.h>
#include <iostream>

#include "OgreRoot.h"
#include "OgreConfigFile.h"
#include "OgreResourceManager.h"
#include "OgreResourceGroupManager.h"
#include "MyResourceLoadingListener.h"
#include "OgreGpuProgramManager.h"


void setupResources(void)
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load( "../config/resources.cfg");

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	std::string secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		
		
	
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;

			if(archName == "../Media/OgreCore.zip")
				continue;

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}

void loadResources(void)
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

int main()
{
	// 清空 ResourceCollision.txt
	std::ofstream outPut;
	outPut.open("ResourceCollision.txt");
	outPut.clear();
	outPut.close();
	
	// 清空 CompilerError.txt
	outPut.open("CompilerError.txt");
	outPut.clear();
	outPut.close();

	// 清空 brackMatch.txt
	outPut.open("brackMatch.txt");
	outPut.clear();
	outPut.close();
	
	// 清空 noPath.txt
	outPut.open("noPath.txt");
	outPut.clear();
	outPut.close();
	

	Ogre::Root *mRoot = OGRE_NEW Ogre::Root();

#ifdef _DEBUG
	mRoot->loadPlugin("RenderSystem_Direct3D9_d");
	mRoot->loadPlugin("Plugin_CgProgramManager_d");
#else
	mRoot->loadPlugin("RenderSystem_Direct3D9");
	mRoot->loadPlugin("Plugin_CgProgramManager");
#endif

	{
		Ogre::RenderSystemList *renderSystems = NULL;
		Ogre::RenderSystemList::iterator r_it;
		renderSystems = mRoot->getAvailableRenderers();
		r_it = renderSystems->begin();
		mRoot->setRenderSystem(*r_it);
	}

	if(mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mRoot->initialise(true);
	}
	else
	{
		return 0;
	}

	Ogre::GpuProgramManager::getSingleton();


	setupResources();
	
	MyResourceLoadingListener *listener  = new MyResourceLoadingListener; 
	MyScriptCompilerListener  *cListener = new MyScriptCompilerListener;

	Ogre::ResourceGroupManager::getSingleton().setLoadingListener(listener);
	Ogre::ScriptCompilerManager::getSingleton().setListener(cListener);
		
	// Load resources	
	loadResources();
	
	std::cout<<"Checked Out";
	delete listener;
	delete cListener;
	delete mRoot;

	system("pause");
}