/**********************************************************************
* 作者: 何展然
* 时间: 2010-12-10
* 描述: 美容院玩家角色头部预览
**********************************************************************/
#ifndef Beauty_RTT_H
#include "EQCommPCH.h"

#include "BeautyHeadRTT.h"
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
#include "ConfigOption.h"

BeautyHeadRTT::BeautyHeadRTT():
mAvatar(0),
mMainModel(0),
mMainNode(0),
mRTTCamera(0),
mWindow(0),
mRenderTexture(0),
mTexture(0),
mStart(false)
{
}

BeautyHeadRTT::~BeautyHeadRTT()
{
	if (mStart)
	{
		Ogre::TextureManager::getSingleton().remove(mRttTexture->getHandle());
	}

	if (mRTTCamera)
	{
		delete mRTTCamera;
		mRTTCamera = 0;
	}

	if (mAvatar)
	{
		delete mAvatar;
		mAvatar = 0;
	}

	{
		Ogre::RenderTexture* rt= mRttTexture->getBuffer()->getRenderTarget();
		rt->removeListener((Ogre::RenderTargetListener*)(&mListener));
		EQOgreSys::getSingleton().getSceneManager()->destroyCamera("BeautyHeadRttCamera");
		EQOgreSys::getSingleton().getSceneManager()->getRootSceneNode()->removeAndDestroyChild("BeautyHeadRttCameraNode");
		CEGUI::ImagesetManager::getSingleton().destroyImageset("BeautyHeadRttImageset");
		CEGUI::WindowManager::getSingleton().destroyWindow("BeautyHeadRttWindow");
	}
}
//-------------------------------------------------------------------------
void BeautyHeadRTT::_createRTT(float x, float y, float z)
{
	// Setup Render To Texture for preview window
	// 	float pitchVal =  VariableSystem::getSingleton().GetAs_Float("CreatePitchVal");

	mRttTexture = Ogre::TextureManager::getSingleton().
		createManual("BeautyHeadRttTexture", 
		Ogre::ResourceGroupManager::
		DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, 
		96, 96, 1, 0, Ogre::PF_R8G8B8A8, 
		Ogre::TU_RENDERTARGET, 0, false,
		EQOgreSys::getSingleton().getConfigOption()->getAA()
		);
	{
		Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
		Ogre::Camera* rttCam = sm->createCamera("BeautyHeadRttCamera");
		Ogre::SceneNode* camNode = sm->getRootSceneNode()->createChildSceneNode("BeautyHeadRttCameraNode");
		camNode->attachObject(rttCam);

		rttCam->setNearClipDistance(5);
		rttCam->setFarClipDistance(15000);
		rttCam->setAspectRatio(Ogre::Real(0.5));

		mRTTCamera = new EQCamera();
		mRTTCamera->initial(rttCam);
		mRTTCamera->zoom(250);
		mRTTCamera->yaw( -90);
		mRTTCamera->lookAt( Ogre::Vector3(x,y + 100,z));

		mRenderTexture = mRttTexture->getBuffer()->getRenderTarget();
		Ogre::Viewport *vp = mRenderTexture->addViewport(rttCam);

		mRenderTexture->addListener((Ogre::RenderTargetListener*)(&mListener));

		vp->setOverlaysEnabled(false);
		vp->setClearEveryFrame(true);
		vp->setBackgroundColour(Ogre::ColourValue(((float)239/255),((float)239/255),((float)239/255),0));

		rttCam->setNearClipDistance(10.f);
		rttCam->setFOVy(Ogre::Degree(45.0f));
		rttCam->setProjectionType(Ogre::PT_PERSPECTIVE);
		
		//--------------------------------------------------
		//创建ViewPort
		vp->setClearEveryFrame(true);
		vp->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
		vp->setOverlaysEnabled(false);
		vp->setSkiesEnabled(false);
	}

	// Retrieve CEGUI texture for the RTT
	CEGUI::OgreCEGUIRenderer* uirenderer = (CEGUI::OgreCEGUIRenderer*)CEGUI::System::getSingleton().getRenderer();
	mTexture = uirenderer->createTexture("BeautyHeadRttTexture");
	CEGUI::Imageset* rttImageSet = CEGUI::ImagesetManager::getSingleton().createImageset("BeautyHeadRttImageset", mTexture);
	int width = mTexture->getWidth(),height = mTexture->getHeight();
	rttImageSet->defineImage((CEGUI::utf8*)"BeautyHeadRttImage", CEGUI::Point(0.0f, 0.0f), CEGUI::Size(width, height), CEGUI::Point(0.0f,0.0f));
}
//----------------------------------------------------------------------------
void BeautyHeadRTT::_createRttGuiObject()
{
	Ogre::String guiObjectName = "BeautyHeadRttWindow";
	CEGUI::Imageset* rttImageSet = CEGUI::ImagesetManager::getSingleton().getImageset("BeautyHeadRttImageset");
	mWindow = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"EQHomeLook/StaticImage", (CEGUI::utf8*)guiObjectName.c_str());
	mWindow->setSize(CEGUI::UVector2( CEGUI::UDim(1.f, 0), CEGUI::UDim(1.f, 0)));
	mWindow->setProperty("Image", CEGUI::PropertyHelper::imageToString(&rttImageSet->getImage((CEGUI::utf8*)"BeautyHeadRttImage")));
	mWindow->setProperty("FrameEnabled", CEGUI::PropertyHelper::boolToString(false));
}
//--------------------------------------------------------------------------------
void BeautyHeadRTT::startRTT(float x, float y, float z)
{

	if(mAvatar)
		return;

	_createRTTAvatar(x,y,z);
	_createRTT(x,y,z);
	_createRttGuiObject();

	std::vector<uint> zone;
	zone.push_back(9999);
	mAvatar->getModel()->setLightListener(zone);
	mStart = true;
}
//------------------------------------------------------------------------------
void BeautyHeadRTT::rotate(int degree,int axes)
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
void BeautyHeadRTT::resetOrientation()
{
	if (mAvatar)
	{
		mMainNode->resetOrientation();
	}
}
//------------------------------------------------------------------------------
void BeautyHeadRTT::rttScale(float scaleX,float scaleY,float scaleZ)
{
	if (mAvatar)
	{
		mMainNode->setScale(1.f,1.f,1.f);
		mMainNode->scale(scaleX,scaleY,scaleZ);
	}
}
//------------------------------------------------------------------------------
void BeautyHeadRTT::equipOn( uint slot, uint equipDisplayID, uint holdSlot )
{
	if(mAvatar)
	{
		mAvatar->equipOn(slot,equipDisplayID,holdSlot);
		//if( GameStateMgr::getSingleton().getState() == GS_MAINGAME )
		//	MainPlayer::getSingleton().equipOn( slot,equipDisplayID );
	}
}
//------------------------------------------------------------------------------
void BeautyHeadRTT::takeOff( uint slot )
{
	if(mAvatar)
	{
		mAvatar->takeOff(slot);
		//if( GameStateMgr::getSingleton().getState() == GS_MAINGAME )
		//	MainPlayer::getSingleton().takeOff( slot );
	}
}
//------------------------------------------------------------------------------
void BeautyHeadRTT::_createRTTAvatar(float x, float y, float z)
{
	mAvatar = new Avatar(1);
	mMainModel = mAvatar->getModel();
	mMainNode = mAvatar->getModel()->getMainNode();
	mMainModel->setDefAnimation("Stand");
	mMainModel->_update(0, true);	// 更新模型，在setDefAnimation后调用
	mMainNode->setPosition(Ogre::Vector3(x,y,z));
	//mMainNode->rotate(Ogre::Vector3(0,1,0),Ogre::Radian(Ogre::Math::DegreesToRadians(-25)));
}
//--------------------------------------------------------------------------------
void BeautyHeadRTT::changeSex(int sex)
{
	Ogre::Vector3 pos = mMainNode->getPosition();
	mAvatar->getAdjuster()->doChange(AvatarAdjuster::atSex,sex);

	mMainModel = mAvatar->getModel();
	mMainNode = mAvatar->getModel()->getMainNode();
	mMainModel->setDefAnimation("Stand");
	mMainModel->_update(0, true);	// 更新模型，在setDefAnimation后调用
	mMainNode->setPosition(pos);
	//mMainNode->rotate(Ogre::Vector3(0,1,0),Ogre::Radian(Ogre::Math::DegreesToRadians(-225)));

	std::vector<uint> zone;
	zone.push_back(9999);
	mAvatar->getModel()->setLightListener(zone);
}
//--------------------------------------------------------------------------------
void BeautyHeadRTT::updateModel()
{
	mMainModel = mAvatar->getModel();
	mMainNode  = mMainModel->getMainNode();
	mMainModel->enableAnimation(true);
}
//--------------------------------------------------------------------------------
void BeautyHeadRTT::takeOffAll()
{
	mAvatar->takeOffAll();
	//for (int slot=ES_HAT; slot <=ES_ORNAMENT; slot = slot * 2 )
	//	takeOff(slot);
}
//--------------------------------------------------------------------------------
void BeautyHeadRTT::syncPlayerEquip(Player *p)
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
void BeautyHeadRTT::syncPlayerEquip1(int equipArr[])
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
//--------------------------------------------------------------------------------
//void BeautyHeadRTT::createOgreTexturePtr(int flag)
//{
//	Ogre::RenderTexture* rt= mRttTexture->getBuffer()->getRenderTarget();
//	rt->removeListener((Ogre::RenderTargetListener*)(&mListener));
//
//	char buf[28] = "";
//	_snprintf_s(buf, sizeof(buf), "%d", flag);
//	std::string baseName = "BeautyHeadRttTexture";
//	std::string texName = baseName + buf;
//	mRttTexture = Ogre::TextureManager::getSingleton().
//		createManual(texName, 
//		Ogre::ResourceGroupManager::
//		DEFAULT_RESOURCE_GROUP_NAME, 
//		Ogre::TEX_TYPE_2D, 
//		512, 512, 1, 0, Ogre::PF_R8G8B8A8, 
//		Ogre::TU_RENDERTARGET
//		);
//	mRenderTexture = mRttTexture->getBuffer()->getRenderTarget();
//	mRenderTexture->setAutoUpdated(false);
//	//mRenderTexture->addListener((Ogre::RenderTargetListener*)(&mListener));
//	CEGUI::OgreCEGUIRenderer* uirenderer = (CEGUI::OgreCEGUIRenderer*)CEGUI::System::getSingleton().getRenderer();
//	/*CEGUI::Texture* rttTexture*/mTexture = uirenderer->createTexture(texName);
//}

