#include "EQCommPCH.h"
#include "RecycleResource.h"

#include "CEGUI/CEGUI.h"
#include "OgreCEGUIRenderer.h"

void PrepareRecycleResource()
{
	//��Щ���ڽű�, unload��Ҳ�����ڴ�Ӳ�̶�ȡ, ����, ����ô��Ҫ����Ч������, ͬʱΪTextureManager��unloadUnreferencedResources����.
	Ogre::MaterialManager::getSingleton().unloadAll();
	Ogre::CompositorManager::getSingleton().unloadAll();
	//Ogre::FontManager::getSingleton().unloadAll();
}

void RecycleResource()
{
	//GUI��Դ����
	((CEGUI::OgreCEGUIRenderer*)CEGUI::System::getSingleton().getRenderer())->recycleTextures();

	//Ogre��Դ����
	size_t m = Ogre::MeshManager::getSingleton().getMemoryUsage();
	Ogre::MeshManager::getSingleton().unloadUnreferencedResources();
	LOGDBG("MeshManager Memory: " + Ogre::StringConverter::toString(m) + " -> "
		+ Ogre::StringConverter::toString(Ogre::MeshManager::getSingleton().getMemoryUsage()));

	//m = Ogre::SkeletonManager::getSingleton().getMemoryUsage();
	Ogre::SkeletonManager::getSingleton().unloadUnreferencedResources();
	//LOGDBG("SkeletonManager Memory: " + Ogre::StringConverter::toString(m) + " -> "
	//	+ Ogre::StringConverter::toString(Ogre::SkeletonManager::getSingleton().getMemoryUsage()));

	//��Ogre::MaterialManager::getSingleton().unloadAll(); ���ֽű���ص� �ŵ�_startLoadingִ��.

	Ogre::GpuProgramManager::getSingleton().unloadUnreferencedResources();
	Ogre::HighLevelGpuProgramManager::getSingleton().unloadUnreferencedResources();

	m = Ogre::TextureManager::getSingleton().getMemoryUsage();
	Ogre::TextureManager::getSingleton().unloadUnreferencedResources();
	LOGDBG("TextureManager Memory: " + Ogre::StringConverter::toString(m) + " -> "
		+ Ogre::StringConverter::toString(Ogre::TextureManager::getSingleton().getMemoryUsage()));

}
