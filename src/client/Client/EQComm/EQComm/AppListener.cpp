#include "EQCommPCH.h"

#include "CEGUI.h"
#include <OIS.h>
#include <OgreOverlayManager.h>
#include <OgreRenderTarget.h>
#include <OgreOverlayElement.h>
#include <OgreStringConverter.h>
#include <OgreRoot.h>
#include "EQOIS.h"
#include "EQCamera.h"
//EQMain
#include "SceneManager.h"
#include "ModelManagerGcs.h"

#include "AppListener.h"
#include "MainPlayer.h"
#include "EQGameMgr.h"
//----------------------------------------------------------------------
template<> AppListener* Ogre::Singleton<AppListener>::ms_Singleton = 0;
//----------------------------------------------------------------------
void AppListener::initial(Ogre::RenderTarget* renderTarget)
{
	try
	{
		mDebugOverlay = Ogre::OverlayManager::getSingleton().
			getByName("Core/DebugOverlay");

		mStatPanel = Ogre::OverlayManager::getSingleton().
			getOverlayElement("Core/StatPanel");

		mLogoPanel = Ogre::OverlayManager::getSingleton().
			getOverlayElement("Core/LogoPanel");
	}
	catch (...)
	{
		// ignore
	}

	showDebugOverlay(true);
	Ogre::Root::getSingleton().addFrameListener(this);

	 mRenderTarget = renderTarget;
	 showStatPanel(false);
	 showLogoPanel(false);
}
//----------------------------------------------------------------------
AppListener::AppListener()
    : mDebugOverlay(NULL)
    , mStatPanel(NULL)
    , mLogoPanel(NULL)
{
}
//----------------------------------------------------------------------
void AppListener::cleanup()
{
	Ogre::Root::getSingleton().removeFrameListener(this);
}
//---------------------------------------------------------------------
AppListener::~AppListener()
{
}
//----------------------------------------------------------------------
void AppListener::updateStats(void)
{
	static Ogre::String currFps = "FPS: ";
	static Ogre::String avgFps = "x: ";
	static Ogre::String bestFps = "z: ";
	static Ogre::String worstFps = "y: ";
	//static Ogre::String tris = "Player State: ";
	static Ogre::String tris = "Triangle Count: ";
	static Ogre::String batches = "Batch Count: ";

    // update stats when necessary
    try {
        Ogre::OverlayElement* guiAvg = Ogre::OverlayManager::
			getSingleton().getOverlayElement("Core/AverageFps");
		Ogre::OverlayElement* guiCurr = Ogre::OverlayManager::
			getSingleton().getOverlayElement("Core/CurrFps");
		Ogre::OverlayElement* guiBest = Ogre::OverlayManager::
			getSingleton().getOverlayElement("Core/BestFps");
		Ogre::OverlayElement* guiWorst = Ogre::OverlayManager::
			getSingleton().getOverlayElement("Core/WorstFps");

		const Ogre::RenderTarget::FrameStats& stats = mRenderTarget->
			getStatistics();

		Ogre::OverlayElement* guiTris = Ogre::OverlayManager::getSingleton().
			getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + Ogre::StringConverter::toString(stats.triangleCount));

		Ogre::OverlayElement* guiBatches = Ogre::OverlayManager::getSingleton()
			.getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + Ogre::StringConverter::toString(stats.batchCount));

		Ogre::Vector3 v2 = MainPlayer::getSingleton().getPos();
		
		CEGUI::Point ceMousePoint = CEGUI::MouseCursor::getSingleton().getPosition();
		OIS::MouseState ms = EQOIS::getSingleton().mMouse->getMouseState();
		
		POINT curpos;
		::GetCursorPos(&curpos);

		guiAvg->setCaption  ("ms.X.abs: "				
			+ Ogre::StringConverter::toString(v2.x)
			+ "  ms.Y.abs: "
			+ Ogre::StringConverter::toString(ms.Y.abs)
			);

		guiWorst->setCaption("ceui d_x: "
			+ Ogre::StringConverter::toString(ceMousePoint.d_x)
			+ "  cegui d_y: "
			+ Ogre::StringConverter::toString(ceMousePoint.d_y)			
			);

		guiBest->setCaption("mouse x: "				
			+ Ogre::StringConverter::toString(v2.z)
			+ "  wmouse y: "				
			+ Ogre::StringConverter::toString(curpos.y)
			);


		guiCurr->setCaption(currFps + Ogre::StringConverter::
			toString(stats.lastFPS)
			//MainPlayer::getSingleton().getState())
			);
		//guiCurr->setCaption(currFps + Ogre::StringConverter::
		//	toString(stats.lastFPS));
    }
    catch (...)
    {
        // ignore
    }
}
//----------------------------------------------------------------------
void AppListener::showDebugOverlay(bool show)
{
    if (mDebugOverlay)
    {
        if (show)
        {
            mDebugOverlay->show();
        }
        else
        {
            mDebugOverlay->hide();
        }
    }
}
//----------------------------------------------------------------------
bool AppListener::isDebugOverlayShown(void) const
{
    return mDebugOverlay && mDebugOverlay->isVisible();
}
//----------------------------------------------------------------------
void AppListener::showStatPanel(bool show)
{
    if (mStatPanel)
    {
        if (show)
        {
            mStatPanel->show();
        }
        else
        {
            mStatPanel->hide();
        }
    }
}
//----------------------------------------------------------------------
bool AppListener::isStatPanelShown(void) const
{
    return mStatPanel && mStatPanel->isVisible();
}
//----------------------------------------------------------------------
void AppListener::showLogoPanel(bool show)
{
    if (mLogoPanel)
    {
        if (show)
        {
            mLogoPanel->show();
        }
        else
        {
            mLogoPanel->hide();
        }
    }
}
//----------------------------------------------------------------------
bool AppListener::isLogoPanelShown(void) const
{
    return mLogoPanel && mLogoPanel->isVisible();
}
//----------------------------------------------------------------------
bool AppListener::frameEnded(const Ogre::FrameEvent& evt)
{
    if (isStatPanelShown() || isLogoPanelShown())
    {
        updateStats();
    }

    return true;
}
//----------------------------------------------------------------
bool AppListener::frameStarted(const Ogre::FrameEvent& evt)
{
	ModelManagerGcs::getSingleton().
					updateAllModels(evt.timeSinceLastFrame);

	EQSceneManager::getSingleton().update(evt.timeSinceLastFrame);

	EQGameMgr::getSingleton().setTimElapsed(evt.timeSinceLastFrame);
	EQGameMgr::getSingleton().getMainCamera()->updateLookAt(evt.timeSinceLastFrame);


	EQOIS::getSingleton().update(evt.timeSinceLastFrame);
	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

	return true;
}