#include "EQCommPCH.h"

#include "FurnitureRTT.h"

#include "CEGUI.h"
#include "EQOgreSys.h"
#include "EQCamera.h"
#include "GUIManager.h"

#include "ModelManagerGcs.h"
#include "CommFuncGcs.h"
#include "GraphicsTable.h"
#include "ItemTable.h"
#include "HouseTable.h"
#include "Variable.h"
#include "ConfigOption.h"

FurnitureRTT::FurnitureRTT():
mModel(0),
mNode(0),
mCamera(0),
mWindow(0),
x(1000),
y(50000),
z(1000),
mStart(false)
{
	mName = "RTTWindow_1";
	mZoom.x = mZoom.y = mZoom.z = 1.0f;
}
//-------------------------------------------------------------------------
FurnitureRTT::~FurnitureRTT()
{
	if (mStart)
	{
		Ogre::TextureManager::getSingleton().remove(mRttTex->getHandle());
	}

	if (mCamera)
	{
		delete mCamera;
		mCamera = 0;
	}

	if (!mRttTex.isNull())
	{
		mRttTex->getBuffer()->getRenderTarget()->removeListener((Ogre::RenderTargetListener*)(&mListener));
	}
}
//--------------------------------------------------------------------------------
void FurnitureRTT::_init()
{
	_createRTT();
	_createRttGuiObject();
	mStart = true;
}
//-------------------------------------------------------------------------
void FurnitureRTT::_createRTT()
{
	// Setup Render To Texture for preview window
	mRttTex = Ogre::TextureManager::getSingleton().
		createManual("RttTex1", 
		Ogre::ResourceGroupManager::
		DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, 
		512, 512, 1, 0, Ogre::PF_R8G8B8A8, 
		Ogre::TU_RENDERTARGET, 0, false, 
		EQOgreSys::getSingleton().getConfigOption()->getAA());
	{
		Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
		Ogre::Camera* rttCam = sm->createCamera("RttCam1");
		Ogre::SceneNode* camNode = sm->getRootSceneNode()->
			createChildSceneNode("rttCamNode1");
		camNode->attachObject(rttCam);

		rttCam->setNearClipDistance(5);
		rttCam->setFarClipDistance(15000);
		rttCam->setAspectRatio(Ogre::Real(640.0f/480.0f));

		mCamera = new EQCamera();
		mCamera->initial(rttCam);

		mCamera->zoom(500);
		mCamera->yaw( -90  );
		mCamera->lookAt( Ogre::Vector3(x,y + 100,z));


		Ogre::Viewport *v = mRttTex->getBuffer()->getRenderTarget()->addViewport( rttCam );
		v->setOverlaysEnabled(false);
		v->setClearEveryFrame( true );
		v->setBackgroundColour( 
			Ogre::ColourValue(((float)239/255),((float)239/255),((float)239/255),0) 
			//Ogre::ColourValue::Black
			);
		//Çå³ýÌì¿ÕºÐ
		rttCam->setNearClipDistance(10.f);
		rttCam->setFOVy(Ogre::Degree(45.0f));
		rttCam->setProjectionType(Ogre::PT_PERSPECTIVE);
		v->setClearEveryFrame(true);
		v->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
		v->setOverlaysEnabled(false);
		v->setSkiesEnabled(false);

		mRttTex->getBuffer()->getRenderTarget()->addListener((Ogre::RenderTargetListener*)(&mListener));
	}

	// Retrieve CEGUI texture for the RTT
	CEGUI::OgreCEGUIRenderer* uirenderer = 
		(CEGUI::OgreCEGUIRenderer*)CEGUI::System::getSingleton().getRenderer();

	CEGUI::Texture* rttTexture = uirenderer->createTexture((CEGUI::utf8*)"RttTex1");

	CEGUI::Imageset* rttImageSet = 
		CEGUI::ImagesetManager::getSingleton().createImageset(
		(CEGUI::utf8*)"RttImageset1", rttTexture);

	int width = rttTexture->getWidth(),height = rttTexture->getHeight();
	rttImageSet->defineImage((CEGUI::utf8*)"RttImage1", 
		CEGUI::Point(0.0f, 0.0f),
		CEGUI::Size(width, height),
		CEGUI::Point(0.0f,0.0f));

	setActive(false);
}
//----------------------------------------------------------------------------
void FurnitureRTT::_createRttGuiObject()
{
	CEGUI::Imageset* rttImageSet = CEGUI::ImagesetManager::getSingleton().
		getImageset( (CEGUI::utf8*)"RttImageset1");

	mWindow = CEGUI::WindowManager::getSingleton().createWindow(
		(CEGUI::utf8*)"EQHomeLook/StaticImage", (CEGUI::utf8*)mName.c_str());

	mWindow->setSize(CEGUI::UVector2( CEGUI::UDim(1.f, 0), CEGUI::UDim(1.f, 0)));
	mWindow->setProperty("Image", CEGUI::PropertyHelper::imageToString(
		&rttImageSet->getImage((CEGUI::utf8*)"RttImage1")));

	mWindow->setProperty("FrameEnabled", CEGUI::PropertyHelper::boolToString(false));
}
//------------------------------------------------------------------------------
void FurnitureRTT::rotate(int degree)
{
	if(mModel)
		mNode->rotate(Ogre::Vector3(0,1,0), Ogre::Radian( Ogre::Math::DegreesToRadians ( degree ) ) );
}
//------------------------------------------------------------------------------
void FurnitureRTT::resetOrientation()
{
	if (mModel)
	{
		mNode->resetOrientation();
		mNode->setOrientation(mRot);
	}
}
//------------------------------------------------------------------------------
void FurnitureRTT::zoom(float scale)
{
	if(mModel)
	{
		if(mZoom.x+scale>VariableSystem::getSingleton().GetAs_Float("RTTMaxScale") ||
			mZoom.x+scale<VariableSystem::getSingleton().GetAs_Float("RTTMinScale"))
			return;

		mZoom += scale;
		mNode->setScale(mZoom);
	}
}
//------------------------------------------------------------------------------
void FurnitureRTT::resetScale()
{
	if (mModel)
	{
		mZoom.x = mZoom.y = mZoom.z = 1.0f;
		mNode->setScale(mZoom);
	}
}
//--------------------------------------------------------------------------------
void FurnitureRTT::startFurnitureRTT(uint itemid)
{
	const ItemRow* item = ItemTableManager::getSingleton().getItemRow(itemid);
	if(!item)
		return;

	const HouseCompRow* pro = HouseTableManager::getSingleton().getHouseCompRow(item->subID);
	if(!pro)
		return;
	mRot = pro->Rot;

	const ModelDisplayRow* dis = GraphicsTableManager::getSingleton().getModelDisplayRow(pro->DisID);
	if(!dis)
		return;

	if(mModel)
		ModelManagerGcs::getSingleton().destroyModel(mModel);
	else
		_init();

	SimpleCreateModelRule rule(dis->mesh, dis->mat, dis->particle);
	mModel = ModelManagerGcs::getSingleton().createModel(&rule);
	mNode = mModel->getMainNode();
	mNode->setPosition(Ogre::Vector3(x,y,z));
	mNode->setOrientation(mRot);
}
//--------------------------------------------------------------------------------
