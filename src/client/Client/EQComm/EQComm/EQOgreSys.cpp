#include "EQCommPCH.h"
#include <OgreRoot.h>

#include "SystemGcs.h"
#include "ModelManagerGcs.h"

#include "EQOgreSys.h"
//#include "DebugPanel.h"
//#include "EventSystem.h"
#include "Variable.h"

#include "MainPlayer.h"
#include "CommFunc.h"
#include "CommonHelper.h"
#include "ConfigOption.h"
#include "SceneManager.h"
#include "SceneInfo.h"
#include "SceneEntityObject.h"

//-------------------------------------------------------------------
template<> EQOgreSys* Ogre::Singleton<EQOgreSys>::ms_Singleton = 0;
//-------------------------------------------------------------------
EQOgreSys::EQOgreSys(void)
	: 
	  mRoot(NULL)
	, mRenderWindow(NULL)
	, mSceneMgr(NULL)
	, mRootNode(NULL)
	, mCamera(NULL)
	, mViewport(NULL)
	, mGaussianBlur(NULL)
	, mMosaic(NULL)
{
	mConfigOption = new ConfigOption;
}
//----------------------------------------------------------------------
EQOgreSys::~EQOgreSys()
{
	delete mConfigOption;

	if(mGaussianBlur)
	{
		mGaussianBlur->removeListener(mGaussianListener);
		delete mGaussianListener;
	}

	if(mMosaic)
	{
		mMosaic->removeListener(mMosaicListener);
		delete mMosaicListener;
	}
}
//-----------------------------------------------------------------------
void EQOgreSys::cleanup()
{
	if (mRenderWindow)
		mRenderWindow->setActive(false);

	//delete mRoot;
}

//---------------------------------------------------------------------
void setCutDistanceLevel()
{
	EQSceneManager::getSingleton().getSceneInfo()->setCutRadius(CDL_FAR, sVariableSystem.GetAs_Int("CUT_DISTANCE_FAR"));
	EQSceneManager::getSingleton().getSceneInfo()->setCutRadius(CDL_MID, sVariableSystem.GetAs_Int("CUT_DISTANCE_MID"));
	EQSceneManager::getSingleton().getSceneInfo()->setCutRadius(CDL_CLOSE, sVariableSystem.GetAs_Int("CUT_DISTANCE_CLOSE"));
}
//---------------------------------------------------------------------
void EQOgreSys::initial(void* pParam)
{
	HWND hMainWnd = *((HWND*)pParam);
	mHwnd = hMainWnd;

	//assert(!Ogre::Root::getSingletonPtr());
	//mRoot = new Ogre::Root(	"", "", "rendersys.log");
	// Root
	//new OgreRootEx("","",logOgre);	//这个不只是图形系统
	mRoot = Ogre::Root::getSingletonPtr();

	_setupResources();

	//加载插件
	{
		#if defined(_DEBUG)
			mRoot->loadPlugin("RenderSystem_Direct3D9_d");
			mRoot->loadPlugin("Plugin_OctreeSceneManager_d");
			mRoot->loadPlugin("Plugin_CgProgramManager_d");
			mRoot->loadPlugin("Plugin_ParticleFX_d");
			mRoot->loadPlugin("ParticleUniverse_d");
		#else
			mRoot->loadPlugin("RenderSystem_Direct3D9");
			mRoot->loadPlugin("Plugin_OctreeSceneManager");
			mRoot->loadPlugin("Plugin_CgProgramManager");
			mRoot->loadPlugin("ParticleUniverse");
		#endif
	}
	//设置RenderSystem
	{
		Ogre::RenderSystemList *renderSystems = NULL;
		Ogre::RenderSystemList::iterator r_it;
		renderSystems = mRoot->getAvailableRenderers();
		r_it = renderSystems->begin();
		mRoot->setRenderSystem(*r_it);
	}

	//
	NameValuePairList ViewSetting;
	setup((Ogre::ulong)(UINT_PTR)hMainWnd, &ViewSetting);

	mRootNode = mSceneMgr->getRootSceneNode();

	setCutDistanceLevel();
}
//----------------------------------------------------------------------
void EQOgreSys::_setupResources()
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load("../config/resources.cfg");

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		const Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::const_iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName/*, recursive*/);
		}
	}
	//
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
	//	"../Media/Scene", "FileSystem", "OgreMrdx");
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
	//	"../Media/Scene/00014000", "FileSystem", "OgreMrdx");
	//Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("OgreMrdx");
}
//------------------------------------------------------------------------
void EQOgreSys::setup(ulong windowHandle, 
					const NameValuePairList* extraParams)
{
	assert(mRoot);
	Ogre::MovableObject::setDefaultVisibilityFlags(0x00000001);
	_createRenderWindow(windowHandle, extraParams);

	mConfigOption->init();

	_createSceneManager();
	SystemGcs::getSingleton().setSceneManager(mSceneMgr);

	_createCamera();
	_createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Create any resource listeners (for loading screens)
	_createResourceListener();

	// Load resources
	_loadResources();

	_initScene();

	//高斯模糊，默认关闭
	mGaussianBlur = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Gaussian Blur"); 
	if(mGaussianBlur)
	{
		mGaussianListener = new GaussianListener();
		mGaussianBlur->addListener(mGaussianListener);
		mGaussianListener->notifyViewportSize(mViewport->getActualWidth(), mViewport->getActualHeight());
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Gaussian Blur", false);
	}

	//对比度加强，默认开启
	mContrast = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Contrast");
	if(mContrast)
	{
		mContrastListener = new ContrastListener();
		mContrast->addListener(mContrastListener);
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Contrast", true);
	}

	//马赛克，默认关闭
	mMosaic = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Mosaic"); 
	if(mMosaic)
	{
		mMosaicListener = new MosaicListener();
		mMosaicListener->notifyViewportSize(mViewport->getActualWidth(), mViewport->getActualHeight());
		mMosaic->addListener(mMosaicListener);
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Mosaic", false);
	}
}
//-----------------------------------------------------------------------
void EQOgreSys::notifyViewportSize(int width, int height)
{
	if(mGaussianBlur)
		mGaussianListener->notifyViewportSize(width, height);

	if(mMosaic)
		mMosaicListener->notifyViewportSize(width, height);
}
//-----------------------------------------------------------------------
void EQOgreSys::setGaussianBlur(bool state)
{
	if(mGaussianBlur)
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Gaussian Blur", state);
}
//-----------------------------------------------------------------------
void EQOgreSys::setContact(float contrast)
{
	if(mContrast)
		mContrastListener->setContrast(contrast);
}
//-----------------------------------------------------------------------
void EQOgreSys::_createRenderWindow(ulong windowHandle, 
								 const NameValuePairList* extraParams)
{
	mRoot->initialise(false, Ogre::StringUtil::BLANK);

	Ogre::NameValuePairList miscParams;
	if (extraParams)
	{
		miscParams = *extraParams;
	}

	miscParams["externalWindowHandle"] = Ogre::StringConverter::toString(windowHandle);

	miscParams["FSAA"] = Ogre::StringConverter::toString(mConfigOption->getAA());
	LOG("FSAA = " + miscParams["FSAA"]);

	miscParams["useNVPerfHUD"] = VariableSystem::getSingleton().GetAs_String("NVPerfHUD");	//用nv工具调试

	mRenderWindow = mRoot->createRenderWindow(Ogre::StringUtil::BLANK, 0, 0, false, &miscParams);
	mRenderWindow->setActive(true);
	_setTextureFiltering(sVariableSystem.GetAs_Int("TextureFiltering"));
	 //Ogre::CompositorInstance* mCompositor;
	 //mCompositor->setEnabled(true);
}
//-----------------------------------------------------------------------
void EQOgreSys::_createSceneManager(void)
{
	mSceneMgr = mRoot->createSceneManager( "TerrainSceneManager");
	//mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "MainScene");
}
//-----------------------------------------------------------------------
void EQOgreSys::_createCamera(void)
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("MainCamera");
	mCamera->setNearClipDistance(5);
	mCamera->setFarClipDistance(50000.0f);
	// 45 degree fov
	//mCamera->setFOVy(Ogre::Degree(45));
	// Auto aspect ratio
	mCamera->setAutoAspectRatio(true);
}
//-----------------------------------------------------------------------
void EQOgreSys::_createViewports(void)
{
	// Create one viewport, entire window
	mViewport = mRenderWindow->addViewport(mCamera);

	mViewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));

	if (!mCamera->getAutoAspectRatio())
	{
		// Alter the camera aspect ratio to match the viewport
		mCamera->setAspectRatio(
			Ogre::Real(mViewport->getActualWidth()) 
		  / Ogre::Real(mViewport->getActualHeight()));
	}
}
//-----------------------------------------------------------------------
void EQOgreSys::_createResourceListener(void)
{
}
//-----------------------------------------------------------------------
void EQOgreSys::_loadResources(void)
{
	// Initialise, parse scripts etc
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//----------------------------------------------------------------------
void EQOgreSys::_initScene(void)
{
	float r = VariableSystem::getSingleton().GetAs_Float("AmbientLingtColour_R");
	float g = VariableSystem::getSingleton().GetAs_Float("AmbientLingtColour_G");
	float b = VariableSystem::getSingleton().GetAs_Float("AmbientLingtColour_B");

	mSceneMgr->setAmbientLight(Ogre::ColourValue(r, g, b));
	//mSceneMgr->setAmbientLight(Ogre::ColourValue(.8f, .8f, .8f));

	setShadow(mConfigOption->getShadow());
}
//------------------------------------------------------------------
void EQOgreSys::setShadow(int shadowTech)
{
	mSceneMgr->setShadowTechnique( (Ogre::ShadowTechnique)shadowTech );

	if (shadowTech != 0)
	{
		float a = 1,dis = 0, r,g,b;
		int  ts;

		r = VariableSystem::getSingleton().GetAs_Float("ShadowColour_R");
		g = VariableSystem::getSingleton().GetAs_Float("ShadowColour_G");
		b = VariableSystem::getSingleton().GetAs_Float("ShadowColour_B");
		a = VariableSystem::getSingleton().GetAs_Float("ShadowColour_A");
		mSceneMgr->setShadowColour(Ogre::ColourValue(r, g, b));

		dis = VariableSystem::getSingleton().GetAs_Float("ShadowFarDistance");
		mSceneMgr->setShadowFarDistance(dis);

		ts = VariableSystem::getSingleton().GetAs_Int("ShadowTextureSize");
		mSceneMgr->setShadowTextureSize(ts);
		if(0!=VariableSystem::getSingleton().GetAs_Int("ShadowTextureSelfShadow"))
			mSceneMgr->setShadowTextureSelfShadow(true);
		//mSceneMgr->setShadowColour(Ogre::ColourValue(0.6f, 0.6f, 0.6f/*,0.5f*/));
#pragma push_macro("new")
#undef new
		mSceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(OGRE_NEW Ogre::FocusedShadowCameraSetup()));
#pragma pop_macro("new")

	}
}
//------------------------------------------------------------------
bool EQOgreSys::renderOneFrame(void)
{
	return mRoot->renderOneFrame();
}
//------------------------------------------------------------------
void EQOgreSys::_setTextureFiltering(int opt)
{
	switch(opt)
	{
	case 0:	
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(1);
		break;
	case 1:
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(2);
		break;
	case 2:
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
uint EQOgreSys::addMosaic(Ogre::Vector4& rectangle)
{
	static uint index=0;
	index++;

	if(mConfigOption->isSupportSM3())
	{
		mMosaicListener->addRec(index, rectangle);
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Mosaic", true);
	}
	else
	{
		mMosaicMap.insert( std::make_pair(index, rectangle) );

		std::string winname = FormatStr("%s_win%d","Mosaic",index);
		luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "Mosaic_CreateWin", winname);
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "Mosaic_SetPos", winname, rectangle.x, rectangle.y, rectangle.z, rectangle.w);
	}

	return index;
}
//------------------------------------------------------------------
void EQOgreSys::removeMosaic(uint id)
{
	if(mConfigOption->isSupportSM3())
	{
		if(mMosaicListener->delRec(id) == 0)
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Mosaic", false);
	}
	else
	{
		std::map<uint, Ogre::Vector4>::iterator it = mMosaicMap.find(id);
		if(it != mMosaicMap.end())
		{
			std::string winname = FormatStr("%s_win%d", "Mosaic", it->first);
			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "Mosaic_DestroyWin", winname);

			mMosaicMap.erase(it);
		}
	}
}
//------------------------------------------------------------------
void EQOgreSys::updateMosaic(uint id, Ogre::Vector4& rectangle)
{
	if(mConfigOption->isSupportSM3())
	{
 		mMosaicListener->updRec(id, rectangle);
	}
	else
	{
		std::map<uint, Ogre::Vector4>::iterator it = mMosaicMap.find(id);
		if(it != mMosaicMap.end())
		{
			it->second = rectangle;

			std::string winname = FormatStr("%s_win%d", "Mosaic", it->first);
			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "Mosaic_SetPos", winname, rectangle.x, rectangle.y, rectangle.z, rectangle.w);
		}
	}
}

/*************************************************************************
GaussianListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
GaussianListener::GaussianListener()
{
}
//---------------------------------------------------------------------------
GaussianListener::~GaussianListener()
{
}
//---------------------------------------------------------------------------
void GaussianListener::notifyViewportSize(int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
	// Calculate gaussian texture offsets & weights
	float deviation = 3.0f;
	float texelSize = 1.0f / (float)std::min(mVpWidth, mVpHeight);

	// central sample, no offset
	mBloomTexOffsetsHorz[0][0] = 0.0f;
	mBloomTexOffsetsHorz[0][1] = 0.0f;
	mBloomTexOffsetsVert[0][0] = 0.0f;
	mBloomTexOffsetsVert[0][1] = 0.0f;
	mBloomTexWeights[0][0] = mBloomTexWeights[0][1] =
		mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
	mBloomTexWeights[0][3] = 1.0f;

	// 'pre' samples
	for(int i = 1; i < 8; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = Ogre::Math::gaussianDistribution(i, 0, deviation);
		mBloomTexWeights[i][3] = 1.0f;
		mBloomTexOffsetsHorz[i][0] = i * texelSize;
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = i * texelSize;
	}
	// 'post' samples
	for(int i = 8; i < 15; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
		mBloomTexWeights[i][3] = 1.0f;

		mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
	}
}
//---------------------------------------------------------------------------
void GaussianListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch(pass_id)
	{
	case 701: // blur horz
		{
			// horizontal bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
			fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
			fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	case 700: // blur vert
		{
			// vertical bloom
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			const Ogre::String& progName = mat->getBestTechnique()->getPass(0)->getFragmentProgramName();
			fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15);
			fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	}
}
//---------------------------------------------------------------------------
void GaussianListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}
//---------------------------------------------------------------------------
void ContrastListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if(pass_id == 1002)
	{
		fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	}
}
//---------------------------------------------------------------------------
void ContrastListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if(pass_id == 1002)
	{
		fpParams->setNamedConstant("contrast", mContrast);
	}
}
//---------------------------------------------------------------------------
void MosaicListener::addRec(uint id, Ogre::Vector4& rectangle)
{
	mAreaMap[id] = rectangle;
}
//---------------------------------------------------------------------------
uint MosaicListener::delRec(uint id)
{
	std::map<uint, Ogre::Vector4>::iterator it = mAreaMap.find(id);
	if(it != mAreaMap.end())
		mAreaMap.erase(it);

	return mAreaMap.size();
}
//---------------------------------------------------------------------------
void MosaicListener::updRec(uint id, Ogre::Vector4& rectangle)
{
	std::map<uint, Ogre::Vector4>::iterator it = mAreaMap.find(id);
	if(it != mAreaMap.end())
	 it->second = rectangle;
}
//---------------------------------------------------------------------------
void MosaicListener::notifyViewportSize(int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
}
//---------------------------------------------------------------------------
void MosaicListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if(pass_id == 1001)
	{
		fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
		fpParams->setNamedConstant("size", Ogre::Vector3(mVpWidth, mVpHeight, 0));
	}
}
//---------------------------------------------------------------------------
#define MOSIAC_MAXNUM 10 //最大支持马赛克数，这个值和Mosaic.cg的值相等
void MosaicListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if(pass_id == 1001)
	{
		float area[MOSIAC_MAXNUM][4];
		memset(area, 0, 4*MOSIAC_MAXNUM*sizeof(float));

		int index=0;
		std::map<uint, Ogre::Vector4>::iterator it;
		for(it=mAreaMap.begin(); it!=mAreaMap.end(); it++)
		{
			area[index][0] = it->second.x;
			area[index][1] = it->second.y;
			area[index][2] = it->second.z;
			area[index][3] = it->second.w;
			if(index++ >= MOSIAC_MAXNUM)
				break;
		}

		fpParams->setNamedConstant("area", area[0], MOSIAC_MAXNUM);
		fpParams->setNamedConstant("count", float(index));
	}
}
//---------------------------------------------------------------------------