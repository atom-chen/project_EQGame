#include "HouseEditorPCH.h"

#include "AppFrameListener.h"
#include "GraphicsSystem.h"
#include "ModelManagerGcs.h"
#include "SceneManager.h"


//--------------------------------------------------------------------------
AppFrameListener::AppFrameListener()
{

}
//--------------------------------------------------------------------------
AppFrameListener::~AppFrameListener()
{

}
//--------------------------------------------------------------------------
bool AppFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	ModelManagerGcs::getSingleton().updateAllModels(evt.timeSinceLastFrame);
	EQSceneManager::getSingleton().update(evt.timeSinceLastFrame);

	const Ogre::RenderTarget::FrameStats& stats = GraphicsSystem::getSingleton().getWindow()->getStatistics();

	std::string fps = " -FPS: " + Ogre::StringConverter::toString(stats.lastFPS);

	CString tile = "HouseEditor";
	tile += fps.c_str();
	::AfxGetMainWnd()->SetWindowText(tile);

	return true;
}
//--------------------------------------------------------------------------
bool AppFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}
