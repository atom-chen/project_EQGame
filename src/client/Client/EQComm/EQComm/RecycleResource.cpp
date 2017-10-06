#include "EQCommPCH.h"
#include "RecycleResource.h"

#include "CEGUI/CEGUI.h"
#include "OgreCEGUIRenderer.h"

void PrepareRecycleResource()
{
	//这些属于脚本, unload掉也不会在从硬盘读取, 所以, 不怎么需要担心效率问题, 同时为TextureManager的unloadUnreferencedResources服务.
	Ogre::MaterialManager::getSingleton().unloadAll();
	Ogre::CompositorManager::getSingleton().unloadAll();
	//Ogre::FontManager::getSingleton().unloadAll();
}

void RecycleResource()
{
	//GUI资源回收
	((CEGUI::OgreCEGUIRenderer*)CEGUI::System::getSingleton().getRenderer())->recycleTextures();

	//Ogre资源回收
	size_t m = Ogre::MeshManager::getSingleton().getMemoryUsage();
	Ogre::MeshManager::getSingleton().unloadUnreferencedResources();
	LOGDBG("MeshManager Memory: " + Ogre::StringConverter::toString(m) + " -> "
		+ Ogre::StringConverter::toString(Ogre::MeshManager::getSingleton().getMemoryUsage()));

	//m = Ogre::SkeletonManager::getSingleton().getMemoryUsage();
	Ogre::SkeletonManager::getSingleton().unloadUnreferencedResources();
	//LOGDBG("SkeletonManager Memory: " + Ogre::StringConverter::toString(m) + " -> "
	//	+ Ogre::StringConverter::toString(Ogre::SkeletonManager::getSingleton().getMemoryUsage()));

	//把Ogre::MaterialManager::getSingleton().unloadAll(); 这种脚本相关的 放到_startLoading执行.

	Ogre::GpuProgramManager::getSingleton().unloadUnreferencedResources();
	Ogre::HighLevelGpuProgramManager::getSingleton().unloadUnreferencedResources();

	m = Ogre::TextureManager::getSingleton().getMemoryUsage();
	Ogre::TextureManager::getSingleton().unloadUnreferencedResources();
	LOGDBG("TextureManager Memory: " + Ogre::StringConverter::toString(m) + " -> "
		+ Ogre::StringConverter::toString(Ogre::TextureManager::getSingleton().getMemoryUsage()));

}
