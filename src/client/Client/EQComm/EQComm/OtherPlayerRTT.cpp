
#include "EQCommPCH.h"

#include "OtherPlayerRTT.h"
#include "Player.h"

#include "EQOgreSys.h"

#include "AvatarTable.h"
#include "EQCamera.h"
#include "AppListener.h"
#include "EQWndProc.h"
#include "EQOIS.h"
#include "GUIManager.h"
#include "EQMsgManager.h"
#include "ObjMgr.h"
#include "Avatar.h"
#include "GameState.h"
#include "AvatarAdjuster.h"

#include "ItemTable.h"
#include "ScriptFunction.h"
#include "Variable.h"
#include "AvatarRTT.h"
#include "ConfigOption.h"

OtherPlayerRTT::OtherPlayerRTT():
mAvatar(0),
mMainModel(0),
mMainNode(0),
mRTTCamera(0),
mWindow(0),
mStart(false)
{
	mName = "RTTWindow_2";
}

OtherPlayerRTT::~OtherPlayerRTT()
{
	if (mStart)
	{
		Ogre::TextureManager::getSingleton().remove(rttTex->getHandle());
	}

	if (mRTTCamera)
	{
		delete mRTTCamera;
		mRTTCamera = 0;
	}

	if (mAvatar)
	{

		if (!rttTex.isNull())
		{
			Ogre::RenderTexture* rt= rttTex->getBuffer()->getRenderTarget();
			rt->removeListener((Ogre::RenderTargetListener*)(&mListener));
		}

		delete mAvatar;
		mAvatar = 0;
	}
}
//-------------------------------------------------------------------------
void OtherPlayerRTT::_createRTT(float x, float y, float z)
{
	// Setup Render To Texture for preview window
	// 	float pitchVal =  VariableSystem::getSingleton().GetAs_Float("CreatePitchVal");

	rttTex = Ogre::TextureManager::getSingleton().
		createManual("RttTex2", 
		Ogre::ResourceGroupManager::
		DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, 
		512, 512, 1, 0, Ogre::PF_R8G8B8A8, 
		Ogre::TU_RENDERTARGET, 0, false, 
		EQOgreSys::getSingleton().getConfigOption()->getAA()
		);
	{
		Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
		Ogre::Camera* rttCam = sm->createCamera("RttCam2");
		Ogre::SceneNode* camNode = sm->getRootSceneNode()->
			createChildSceneNode("rttCamNode2");
		camNode->attachObject(rttCam);

		rttCam->setNearClipDistance(5);
		rttCam->setFarClipDistance(15000);
		rttCam->setAspectRatio(Ogre::Real(0.5));

		mRTTCamera = new EQCamera();
		mRTTCamera->initial(rttCam);

		mRTTCamera->zoom(250);
		mRTTCamera->yaw( -90 );
		// 		mRTTCamera->pitch(Ogre::Math::DegreesToRadians ( pitchVal ));

		mRTTCamera->lookAt( Ogre::Vector3(x,y + 100,z));

		Ogre::RenderTexture* rt= rttTex->getBuffer()->getRenderTarget();
		uint fsaa = rt->getFSAA();
		Ogre::Viewport *v = rt->addViewport( rttCam );

		rt->addListener((Ogre::RenderTargetListener*)(&mListener));

		v->setOverlaysEnabled(false);
		v->setClearEveryFrame( true );
		v->setBackgroundColour( 
			Ogre::ColourValue(((float)239/255),((float)239/255),((float)239/255),0) 
			//Ogre::ColourValue::Black
			);

		//
		rttCam->setNearClipDistance(10.f);
		//mRTTCamera->setAspectRatio((float)s_nTexWidth/s_nTexHeight);
		rttCam->setFOVy(Ogre::Degree(45.0f));
		rttCam->setProjectionType(Ogre::PT_PERSPECTIVE);

		//--------------------------------------------------
		//´´½¨ViewPort
		v->setClearEveryFrame(true);
		v->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
		v->setOverlaysEnabled(false);
		v->setSkiesEnabled(false);
	}

	// Retrieve CEGUI texture for the RTT
	CEGUI::OgreCEGUIRenderer* uirenderer = 
		(CEGUI::OgreCEGUIRenderer*)CEGUI::System::getSingleton().getRenderer();

	CEGUI::Texture* rttTexture = uirenderer->createTexture((CEGUI::utf8*)"RttTex2");

	CEGUI::Imageset* rttImageSet = 
		CEGUI::ImagesetManager::getSingleton().createImageset(
		(CEGUI::utf8*)"RttImageset2", rttTexture);

	int width = rttTexture->getWidth(),height = rttTexture->getHeight();
	rttImageSet->defineImage((CEGUI::utf8*)"RttImage2", 
		CEGUI::Point(0.0f, 0.0f),
		CEGUI::Size(width, height),
		CEGUI::Point(0.0f,0.0f));

	setActive(false);
}
//----------------------------------------------------------------------------
void OtherPlayerRTT::_createRttGuiObject()
{
	CEGUI::Imageset* rttImageSet = CEGUI::ImagesetManager::getSingleton().
		getImageset( (CEGUI::utf8*)"RttImageset2");

	mWindow = CEGUI::WindowManager::getSingleton().createWindow(
		(CEGUI::utf8*)"EQHomeLook/StaticImage", (CEGUI::utf8*)mName.c_str());

	mWindow->setSize(CEGUI::UVector2( CEGUI::UDim(1.f, 0), CEGUI::UDim(1.f, 0)));
	mWindow->setProperty("Image", CEGUI::PropertyHelper::imageToString(
		&rttImageSet->getImage((CEGUI::utf8*)"RttImage2")));

	mWindow->setProperty("FrameEnabled", CEGUI::PropertyHelper::boolToString(false));
}
//--------------------------------------------------------------------------------
void OtherPlayerRTT::startRTT(float x, float y, float z)
{

	if(mAvatar)
		return;

	_createRTTAvatar(x,y,z);
	_createRTT(x,y,z);

	_createRttGuiObject();

	//CEGUI::Window* window = CEGUI::WindowManager::getSingleton().
	//	getWindow((CEGUI::utf8*)"MainRoot/MTxt_ShowChatbox"/*"Page1"*/);

	//window->addChildWindow(mWindow);
	//mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
	std::vector<uint> zone;
	zone.push_back(9999);
	mAvatar->getModel()->setLightListener(zone);
	mStart = true;
}
//------------------------------------------------------------------------------
void OtherPlayerRTT::rotate(int degree,int axes)
{
	if(mAvatar)
	{
		switch (axes)
		{
		case 1://x
			mMainNode->rotate(Ogre::Vector3(1,0,0),
				Ogre::Radian( Ogre::Math::DegreesToRadians ( degree ) ) );
			break;
		case 2://y
			mMainNode->rotate(Ogre::Vector3(0,1,0),
				Ogre::Radian( Ogre::Math::DegreesToRadians ( degree ) ) );
			break;
		case 3://z
			mMainNode->rotate(Ogre::Vector3(0,0,1),
				Ogre::Radian( Ogre::Math::DegreesToRadians ( degree ) ) );
			break;
		default:
			break;
		}
	}
}

//------------------------------------------------------------------------------
void OtherPlayerRTT::rttScale(float scaleX,float scaleY,float scaleZ)
{
	if (mAvatar)
	{
		mMainNode->setScale(1.f,1.f,1.f);
		mMainNode->scale(scaleX,scaleY,scaleZ);
	}

}
//------------------------------------------------------------------------------
void OtherPlayerRTT::equipOn( uint slot, uint equipDisplayID, uint holdSlot )
{
	if(mAvatar)
	{
		mAvatar->equipOn(slot,equipDisplayID,holdSlot);
		//if( GameStateMgr::getSingleton().getState() == GS_MAINGAME )
		//	MainPlayer::getSingleton().equipOn( slot,equipDisplayID );
	}
}
//------------------------------------------------------------------------------
void OtherPlayerRTT::takeOff( uint slot )
{
	if(mAvatar)
	{
		mAvatar->takeOff(slot);
		//if( GameStateMgr::getSingleton().getState() == GS_MAINGAME )
		//	MainPlayer::getSingleton().takeOff( slot );
	}
}
//------------------------------------------------------------------------------
void OtherPlayerRTT::_createRTTAvatar(float x, float y, float z)
{

	mAvatar = new Avatar(1);
	mMainModel = mAvatar->getModel();
	mMainNode = mAvatar->getModel()->getMainNode();

	mMainModel->setDefAnimation("Stand1");

	mMainNode->setPosition(Ogre::Vector3(x,y,z));
	//mMainNode->rotate(Ogre::Vector3(0,1,0),Ogre::Radian(Ogre::Math::DegreesToRadians(-25)));
}
//--------------------------------------------------------------------------------
void OtherPlayerRTT::changeSex(int sex)
{
	Ogre::Vector3 pos = mMainNode->getPosition();
	mAvatar->getAdjuster()->doChange(AvatarAdjuster::atSex,sex);

	mMainModel = mAvatar->getModel();
	mMainNode = mAvatar->getModel()->getMainNode();

	mMainModel->setDefAnimation("Stand1");
	mMainNode->setPosition(pos);
	//mMainNode->rotate(Ogre::Vector3(0,1,0),Ogre::Radian(Ogre::Math::DegreesToRadians(-225)));

	std::vector<uint> zone;
	zone.push_back(9999);
	mAvatar->getModel()->setLightListener(zone);
}
//--------------------------------------------------------------------------------
void OtherPlayerRTT::updateModel()
{
	mMainModel = mAvatar->getModel();
	mMainNode  = mMainModel->getMainNode();
	mMainModel->enableAnimation(true);
}
//--------------------------------------------------------------------------------
void OtherPlayerRTT::takeOffAll()
{
	mAvatar->takeOffAll();
	//for (int slot=ES_HAT; slot <=ES_ORNAMENT; slot = slot * 2 )
	//	takeOff(slot);
}
//--------------------------------------------------------------------------------
void OtherPlayerRTT::syncPlayerEquip(Player *p)
{
	takeOffAll();

	for(int i=0; i<DEFAULT_BODY_SIZE-1; i++)
	{
		uint itemid = p->getItemID(i, ItemPack::PT_BODY);
		if(itemid)
		{
			const ItemRow *  itemRow =  ItemTableManager::getSingleton().getItemRow(itemid);
			uint subID =  itemRow->subID;
			const ItemEquipRow * itemRowEquip = ItemTableManager::getSingleton().getItemEquipRow(subID);
			uint equipSID = itemRowEquip->equip_disp_id;
			uint slot = posToSlot(itemRowEquip->equip_pos);

			equipOn(slot, equipSID, itemRowEquip->equip_placeholder);
		}
	}
}
//--------------------------------------------------------------------------------
void OtherPlayerRTT::syncPlayerEquip1(int equipArr[])
{
	takeOffAll();

	for(int i=0; i<DEFAULT_BODY_SIZE-1; i++)
	{
		uint itemid = equipArr[i];
		if(itemid)
		{
			const ItemRow *  itemRow =  ItemTableManager::getSingleton().getItemRow(itemid);
			uint subID =  itemRow->subID;
			const ItemEquipRow * itemRowEquip = ItemTableManager::getSingleton().getItemEquipRow(subID);
			uint equipSID = itemRowEquip->equip_disp_id;
			uint slot = posToSlot(itemRowEquip->equip_pos);

			equipOn(slot, equipSID, itemRowEquip->equip_placeholder);
		}
	}
}
