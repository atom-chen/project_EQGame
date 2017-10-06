#include "AvatarEditorPCH.h"

#include "AppFrameListener.h"
#include "GraphicsSystem.h"
#include "ModelManagerGcs.h"


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
	return true;
}
//--------------------------------------------------------------------------
bool AppFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}
