#include "EQCommPCH.h"

#include "AvatarRTT.h"
#include "Player.h"

#include "EQOgreSys.h"

#include "AvatarTable.h"
#include "MainPlayer.h"
#include "EQCamera.h"
#include "AppListener.h"
#include "EQWndProc.h"
#include "EQOIS.h"
//#include "LuaManager.h"
#include "GUIManager.h"
#include "EQMsgManager.h"
#include "ObjMgr.h"
#include "Avatar.h"
#include "GameState.h"
#include "AvatarAdjuster.h"

#include "ItemTable.h"
#include "ScriptFunction.h"
#include "Variable.h"
#include "ConfigOption.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"

AvatarRTT::AvatarRTT():
	mAvatar(0),
	mMainModel(0),
	mMainNode(0),
	mRTTCamera(0),
	mWindow(0),
	mStart(false)
{
	mName = "RTTWindow_0";
	mScale.x = mScale.y = mScale.z = mZoom.x = mZoom.y = mZoom.z = 1.0f;
}

AvatarRTT::~AvatarRTT()
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
void AvatarRTT::_createRTT(float x, float y, float z)
{
	// Setup Render To Texture for preview window
// 	float pitchVal =  VariableSystem::getSingleton().GetAs_Float("CreatePitchVal");

	rttTex = Ogre::TextureManager::getSingleton().
									createManual("RttTex", 
									Ogre::ResourceGroupManager::
												DEFAULT_RESOURCE_GROUP_NAME, 
									Ogre::TEX_TYPE_2D, 
									512, 512, 1, 0, Ogre::PF_R8G8B8A8, 
									Ogre::TU_RENDERTARGET,
									0,
									false,
									EQOgreSys::getSingleton().getConfigOption()->getAA()
									);
	{
		Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
		Ogre::Camera* rttCam = sm->createCamera("RttCam");
		Ogre::SceneNode* camNode = sm->getRootSceneNode()->
			createChildSceneNode("rttCamNode");
		camNode->attachObject(rttCam);

		rttCam->setNearClipDistance(5);
		rttCam->setFarClipDistance(15000);
		rttCam->setAspectRatio(Ogre::Real(0.5));

		mRTTCamera = new EQCamera();
		mRTTCamera->initial(rttCam);

		mRTTCamera->zoom(250);
		mRTTCamera->yaw( -90  );
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
		//创建ViewPort
		v->setClearEveryFrame(true);
		v->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
		v->setOverlaysEnabled(false);
		v->setSkiesEnabled(false);
	}

	// Retrieve CEGUI texture for the RTT
	CEGUI::OgreCEGUIRenderer* uirenderer = 
		(CEGUI::OgreCEGUIRenderer*)CEGUI::System::getSingleton().getRenderer();

	CEGUI::Texture* rttTexture = uirenderer->createTexture((CEGUI::utf8*)"RttTex");

	CEGUI::Imageset* rttImageSet = 
		CEGUI::ImagesetManager::getSingleton().createImageset(
		(CEGUI::utf8*)"RttImageset", rttTexture);

	int width = rttTexture->getWidth(),height = rttTexture->getHeight();
	rttImageSet->defineImage((CEGUI::utf8*)"RttImage", 
		CEGUI::Point(0.0f, 0.0f),
		CEGUI::Size(width, height),
		CEGUI::Point(0.0f,0.0f));

	setActive(false);
}
//----------------------------------------------------------------------------
void AvatarRTT::_createRttGuiObject()
{
	CEGUI::Imageset* rttImageSet = CEGUI::ImagesetManager::getSingleton().
		getImageset( (CEGUI::utf8*)"RttImageset");

	mWindow = CEGUI::WindowManager::getSingleton().createWindow(
		(CEGUI::utf8*)"EQHomeLook/StaticImage", (CEGUI::utf8*)mName.c_str());

	mWindow->setSize(CEGUI::UVector2( CEGUI::UDim(1.f, 0), CEGUI::UDim(1.f, 0)));
	mWindow->setProperty("Image", CEGUI::PropertyHelper::imageToString(
		&rttImageSet->getImage((CEGUI::utf8*)"RttImage")));

	mWindow->setProperty("FrameEnabled", CEGUI::PropertyHelper::boolToString(false));
}
//--------------------------------------------------------------------------------
void AvatarRTT::startRTT(float x, float y, float z)
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
void AvatarRTT::rotate(int degree,int axes)
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
void AvatarRTT::resetOrientation()
{
	if (mAvatar)
	{
		mMainNode->resetOrientation();
	}
}
//------------------------------------------------------------------------------
void AvatarRTT::rttScale(float scaleX,float scaleY,float scaleZ)
{
	if (mAvatar)
	{
		mMainNode->setScale(1.f,1.f,1.f);
		mMainNode->scale(scaleX,scaleY,scaleZ);
		mScale.x = scaleX;
		mScale.y = scaleY;
		mScale.z = scaleZ;
	}
}
//------------------------------------------------------------------------------
void AvatarRTT::zoom(float scale)
{
	if(mAvatar)
	{
		if(mZoom.x+scale>VariableSystem::getSingleton().GetAs_Float("RTTMaxScale") ||
			mZoom.x+scale<VariableSystem::getSingleton().GetAs_Float("RTTMinScale"))
			return;

		mZoom += scale;
		mMainNode->setScale(mZoom*mScale);
	}
}
//------------------------------------------------------------------------------
void AvatarRTT::resetScale()
{
	if (mAvatar)
	{
		mMainNode->setScale(1.f,1.f,1.f);
		mMainNode->scale(mScale);
		mZoom.x = 1.0f;
		mZoom.y = 1.0f;
		mZoom.z = 1.0f;
	}
}
//------------------------------------------------------------------------------
void AvatarRTT::equipOn( uint slot, uint equipDisplayID, uint holdSlot )
{
	if(mAvatar)
	{
		mAvatar->equipOn(slot,equipDisplayID,holdSlot);
		//if( GameStateMgr::getSingleton().getState() == GS_MAINGAME )
		//	MainPlayer::getSingleton().equipOn( slot,equipDisplayID );

		//这句会导致换装的时候人物闪烁，如果注释掉会引起其他问题，需要考虑其他解决方案
		//EQOgreSys::getSingleton().renderOneFrame();
	}
}
//------------------------------------------------------------------------------
void AvatarRTT::takeOff( uint slot )
{
	if(mAvatar)
	{
		mAvatar->takeOff(slot);
		//if( GameStateMgr::getSingleton().getState() == GS_MAINGAME )
		//	MainPlayer::getSingleton().takeOff( slot );
	}
}
//------------------------------------------------------------------------------
void AvatarRTT::_createRTTAvatar(float x, float y, float z)
{

	mAvatar = new Avatar(1);
	mMainModel = mAvatar->getModel();
	mMainNode = mAvatar->getModel()->getMainNode();

	mMainModel->setDefAnimation("Stand1");
	mMainModel->_update(0, true);

	mMainNode->setPosition(Ogre::Vector3(x,y,z));
	//mMainNode->rotate(Ogre::Vector3(0,1,0),Ogre::Radian(Ogre::Math::DegreesToRadians(-25)));
}
//--------------------------------------------------------------------------------
void AvatarRTT::changeSex(int sex)
{
	Ogre::Vector3 pos = mMainNode->getPosition();
	mAvatar->getAdjuster()->doChange(AvatarAdjuster::atSex,sex);

	mMainModel = mAvatar->getModel();
	mMainNode = mAvatar->getModel()->getMainNode();

	mMainModel->setDefAnimation("Stand1");
	mMainModel->_update(0, true);
	mMainNode->setPosition(pos);
	//mMainNode->rotate(Ogre::Vector3(0,1,0),Ogre::Radian(Ogre::Math::DegreesToRadians(-225)));

	std::vector<uint> zone;
	zone.push_back(9999);
	mAvatar->getModel()->setLightListener(zone);
}
//--------------------------------------------------------------------------------
void AvatarRTT::update()
{
	static size_t lastTime = 0; 
	size_t now = GetTickCount();
	if (!luabind::call_function<bool>(sLuaMgr.getL(), "CreatePlayer_getInCPFlag"))
	{
		lastTime = now;
		return;
	}
	if (sStateMgr.gaming())
	{
	}
	else
	{
		size_t time_interval = VariableSystem::getSingleton().GetAs_Float("CP_PlayAni_TimeInterVal");
		if (now - lastTime >= time_interval)
		{
			std::string anima =  luabind::call_function<std::string>(sLuaMgr.getL(), "CreatePlayer_getRttAnimaName");
			if(!anima.empty())
				mMainModel->playAnimation(anima);
			lastTime = now;
		}
	}
}
//--------------------------------------------------------------------------------
void AvatarRTT::updateModel()
{
	mMainModel = mAvatar->getModel();
	mMainNode  = mMainModel->getMainNode();
	mMainModel->enableAnimation(true);
}
//--------------------------------------------------------------------------------
void AvatarRTT::takeOffAll()
{
	mAvatar->takeOffAll();
	//for (int slot=ES_HAT; slot <=ES_ORNAMENT; slot = slot * 2 )
	//	takeOff(slot);
}
//--------------------------------------------------------------------------------
void AvatarRTT::syncPlayerEquip()
{
	takeOffAll();

	for(int i=0; i<DEFAULT_BODY_SIZE-1; i++)
	{
		uint itemid = MainPlayer::getSingleton().getItemID(i, ItemPack::PT_BODY);
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
int AvatarRTT::getSex()
{
	return mAvatar->getAdjuster()->getSex();
}
