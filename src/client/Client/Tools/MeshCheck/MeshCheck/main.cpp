#include "Ogre.h"
using namespace Ogre;

int main()
{
	Ogre::Root *mRoot = OGRE_NEW Ogre::Root();

#ifdef _DEBUG
	mRoot->loadPlugin("RenderSystem_Direct3D9_d");
#else
	mRoot->loadPlugin("RenderSystem_Direct3D9");
#endif

	Ogre::RenderSystemList *renderSystems = NULL;
	Ogre::RenderSystemList::iterator r_it;
	renderSystems = mRoot->getAvailableRenderers();
	r_it = renderSystems->begin();
	mRoot->setRenderSystem(*r_it);

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

	std::vector<std::string> targentmesh;
	ArchiveManager::ArchiveMapIterator ait = ArchiveManager::getSingleton().getArchiveIterator();
	while(ait.hasMoreElements())
	{
		Archive* arch = ait.getNext();

		FileInfoListPtr list = arch->listFileInfo();
		FileInfoList::iterator lit;
		for(lit=list->begin(); lit!=list->end(); lit++)
		{
			FileInfo file = *lit;
			if(file.basename.find(".mesh") != std::string::npos)
			{
				MeshPtr mesh = MeshManager::getSingleton().load(file.filename, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
				unsigned short src, dest;
				if (mesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
				{
					targentmesh.push_back(file.filename);
				}
			}
		}
	}

	for(uint i=0; i<targentmesh.size(); i++)
	{
		printf("%s 模型带有切线，必须重导!\n", targentmesh[i].c_str());
	}

	while (getchar())
		return 0;
}