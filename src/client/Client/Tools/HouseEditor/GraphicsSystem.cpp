#include "HouseEditorPCH.h"

#include "GraphicsSystem.h"
#include "EQMain.h"
#include "SystemGcs.h"
#include "GraphicsTable.h"
#include "AvatarTable.h"
#include "OgreRootEx.h"
#include "CommFuncGcs.h"
#include "MfcInputEventMgr.h"
#include "HouseTable.h"
#include "HouseSceneDef.h"
#include "AmbientMgr.h"
#include "VideoCardCapabilities.h"
#include "ParticleMgr.h"
#include "ItemTable.h"

//-----------------------------------------------------------------------
template<> GraphicsSystem* Ogre::Singleton<GraphicsSystem>::ms_Singleton = 0;

//-----------------------------------------------------------------------
GraphicsSystem::GraphicsSystem(const std::string& logOgre) 
	: mInit(false)
	, mSetup(false)
	, mCanDraw(true)
	, mRoot(NULL)
	, mWindow(NULL)
	, mCamera(NULL)
	, mSceneMgr(NULL)
	, mFrameListener(NULL)
	, mWorldGrid(NULL)
	, mFocusGrid( NULL )
	, mShowFocusGrid( false )
{
	init(logOgre);
	SystemGcs::getSingleton().setVideoCardCapabilities(new VideoCardCapabilities);
}
//-----------------------------------------------------------------------
GraphicsSystem::~GraphicsSystem()
{
	

	if ( mWorldGrid )
	{
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(mWorldGrid->getParentNode());
		mWorldGrid->detatchFromParent();
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mWorldGrid);
		if ( node )
		{
			node->getCreator()->destroySceneNode(node);
		}

		mWorldGrid = NULL;
	}

	if (mFrameListener)
		delete mFrameListener;

	//delete OgreRootEx::getSingletonPtr();

// 	delete hvListener;
// 	delete hdrListener;
// 	delete gaussianListener;
}
//-----------------------------------------------------------------------
void GraphicsSystem::init( const std::string& logOgre )
{
	if ( mInit )
		return;

	std::string file, path;
	Ogre::StringUtil::splitFilename(logOgre, file, path);
	ForceDir(path);

	// Root
	//new OgreRootEx("","",logOgre);	//这个不只是图形系统
	mRoot = Ogre::Root::getSingletonPtr();

	//SystemGcs::getSingleton().setRoot(mRoot);


	// load common plugins
	setupPlugins();

	// RenderSystem
	setupRederSystem();

	// load the basic resource location(s)
	setupResources();

	//
	mInit = true;
}
//-----------------------------------------------------------------------
void GraphicsSystem::setupRederSystem()
{
	Ogre::RenderSystemList *renderSystems = NULL;
	Ogre::RenderSystemList::iterator r_it;
	renderSystems = mRoot->getAvailableRenderers();
	r_it = renderSystems->begin();
	mRoot->setRenderSystem(*r_it);
	mRoot->initialise(false);

}
//-----------------------------------------------------------------------
void GraphicsSystem::setupPlugins()
{
#if defined(_DEBUG)
	mRoot->loadPlugin("RenderSystem_Direct3D9_d");
	mRoot->loadPlugin("Plugin_OctreeSceneManager_d");
	mRoot->loadPlugin("Plugin_CgProgramManager_d");
	mRoot->loadPlugin("ParticleUniverse_d");
#else
	mRoot->loadPlugin("RenderSystem_Direct3D9");
	mRoot->loadPlugin("Plugin_OctreeSceneManager");
	mRoot->loadPlugin("Plugin_CgProgramManager");
	mRoot->loadPlugin("ParticleUniverse");
#endif
}
//-----------------------------------------------------------------------
void GraphicsSystem::setupResources()
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load("../Config/resources.cfg");

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}

}
//-----------------------------------------------------------------------
void GraphicsSystem::loadResources()
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	GraphicsTableManager::getSingleton().load();
	AvatarTableManager::getSingleton().load();
	HouseTableManager::getSingleton().load();
	ItemTableManager::getSingleton().load();
}
//-----------------------------------------------------------------------
void GraphicsSystem::setup( HWND hWnd )
{
	if ( mSetup )
		return;

	CRect rc;
	::GetClientRect( hWnd, rc );
	const int width = rc.Width();
	const int height = rc.Height();

	// 添加窗口
	// This window should be the current ChildView window using the externalWindowHandle
	// value pair option.
	Ogre::NameValuePairList parms;
	parms["externalWindowHandle"] = Ogre::StringConverter::toString((long)hWnd);
	parms["FSAA"] = "4";

	mWindow = mRoot->createRenderWindow("AvatarEditor Window", width, height, false, &parms);


	// 创建场景管理器
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

	SystemGcs::getSingleton().setSceneManager(mSceneMgr);

	// 创建摄像机
	mCamera = mSceneMgr->createCamera("camera");
	mCamera->setNearClipDistance(5);
	//mCamera->setLodBias(10);

	// 创建视口
	mViewport = mWindow->addViewport(mCamera);
	mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0/*28.0/255.0, 54/255.0, 93/255.0*/));		//Ogre::ColourValue(0.1,0.4,0.8) 好看的蓝色 本来都是0.5


	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio( Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

	// Set default mipmap level (NB some APIs ignore this)
	//Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//解决比如马路很模糊的问题
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(2);

	// 读取资源
	loadResources();

	// 创建监听
	createFrameListener();

	// 创建场景
	createScene();

	//
	mSetup = true;

	//
	_fireNotifySystemSetup();

	//_registerCompositors();

	//对比度加强，默认开启
	mContrast = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "Contrast");
	if(mContrast)
	{
		mContrastListener = new ContrastListener();
		mContrast->addListener(mContrastListener);
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, "Contrast", true);
	}

	ParticleMgr::getSingleton().setQuality(QUALITY_HIGH);

	return;
}
//---------------------------------------------------------------------------
void GraphicsSystem::setContact(float contrast)
{
	if(mContrast)
		mContrastListener->setContrast(contrast);
}
//-----------------------------------------------------------------------
void GraphicsSystem::render()
{
	if ( mSetup && mCanDraw )
		mRoot->renderOneFrame();

	AmbientMgr::getSingleton().update();
	ParticleMgr::getSingleton().update();
}
//-----------------------------------------------------------------------
void GraphicsSystem::sizeChange()
{
	if ( mSetup )
	{ 
		// notify "render window" instance 
		mWindow->windowMovedOrResized(); 

		// Adjust camera's aspect ratio, too 
		if (mWindow->getHeight() != 0) 
			mCamera->setAspectRatio((Ogre::Real)mWindow->getWidth() / (Ogre::Real)mWindow->getHeight());

		if ( mWindow->getWidth() == 0 || mWindow->getHeight() == 0 )
			mCanDraw = false;
		else
			mCanDraw = true;
	} 
}
//-----------------------------------------------------------------------
void GraphicsSystem::addListener(Listener *l)
{
	mListeners.push_back(l);
}
//-----------------------------------------------------------------------
void GraphicsSystem::removeListener(Listener *l)
{
	mListeners.erase(std::find(mListeners.begin(), mListeners.end(), l));
}
//--------------------------------------------------------------------------
void GraphicsSystem::createFrameListener()
{
	mFrameListener = new AppFrameListener();
	mRoot->addFrameListener(mFrameListener);

}
//--------------------------------------------------------------------------
void GraphicsSystem::createScene()
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.52));

	Ogre::Light* l = AmbientMgr::getSingleton().createMainLight();
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(-1, -1, -1);
	l->setDiffuseColour(0.98,0.85,0.7);

	mCamera->setPosition(0,1000, 1500);
	mCamera->lookAt(0,0,0);


	//------------------------
	Ogre::SceneManager* sm = SystemGcs::getSingleton().getSceneManager();
	mWorldGrid = CreateManualGrid( sm, "sdfa", 80, 20, 5,
		Ogre::ColourValue(0.6,0.6,0.6),
		Ogre::ColourValue::Black,
		Ogre::ColourValue(0.8,0.8,0.8));
	sm->getRootSceneNode()->createChildSceneNode()->attachObject(mWorldGrid);
	mWorldGrid->setVisible(false);
}
//--------------------------------------------------------------------------
void GraphicsSystem::_fireNotifySystemSetup()
{
	std::vector<Listener*>::iterator i, iend=mListeners.end();
	for(i=mListeners.begin(); i!=iend; ++i)
		(*i)->notifySystemSetup();
}
//--------------------------------------------------------------------------
void GraphicsSystem::showFocusGrid( bool bVal )
{
	mShowFocusGrid = bVal;
	if ( mFocusGrid )	
	{
		mFocusGrid->setVisible( bVal );
	}	
}
//--------------------------------------------------------------------------
void GraphicsSystem::setFocusGrid( Ogre::SceneNode * gridNode )
{
	if ( !mFocusGrid )	
	{
		mFocusGrid = CreateHollowQuad(SystemGcs::getSingleton().getSceneManager(),
										HOUSE_GRID_HALF_SIZE - 5, 5, Ogre::ColourValue(1,0,0,1));

		mFocusGrid->setQueryFlags( 0 );
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mFocusGrid);
	}

	mFocusGrid->getParentSceneNode()->setPosition( gridNode->_getDerivedPosition() + Ogre::Vector3( 0, 1, 0 ) );
	mFocusGrid->getParentSceneNode()->setOrientation( gridNode->_getDerivedOrientation() );

	showFocusGrid( mShowFocusGrid );
}
//--------------------------------------------------------------------------
void GraphicsSystem::_registerCompositors()
{
	hvListener = new HeatVisionListener();
	hdrListener = new HDRListener();
	gaussianListener = new GaussianListener();

	//iterate through Compositor Managers resources and add name keys ast Item selectors to Compositor selector view manager
	Ogre::CompositorManager::ResourceMapIterator resourceIterator =
		Ogre::CompositorManager::getSingleton().getResourceIterator();

	while (resourceIterator.hasMoreElements())
	{
		Ogre::ResourcePtr resource = resourceIterator.getNext();
		const Ogre::String& compositorName = resource->getName();

		int addPosition = -1;
		if (compositorName == "HDR")
		{
			// HDR must be first in the chain
			addPosition = 0;
		}
		Ogre::CompositorInstance *instance = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, compositorName, addPosition);
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, compositorName, false);
		// special handling for Heat Vision which uses a listener
		if(instance && (compositorName == "Heat Vision"))
			instance->addListener(hvListener);
		else if(instance && (compositorName == "HDR"))
		{
			instance->addListener(hdrListener);
			hdrListener->notifyViewportSize(mViewport->getActualWidth(), mViewport->getActualHeight());
			hdrListener->notifyCompositor(instance);

		}
		else if(instance && (compositorName == "Gaussian Blur"))
		{
			instance->addListener(gaussianListener);
			gaussianListener->notifyViewportSize(mViewport->getActualWidth(), mViewport->getActualHeight());
		}
	}

	_createTextures();
	//_createEffects();
}
void GraphicsSystem::_createTextures(void)
{
	using namespace Ogre;

	TexturePtr tex = TextureManager::getSingleton().createManual(
		"HalftoneVolume",
		"General",
		TEX_TYPE_3D,
		64,64,64,
		0,
		PF_A8
		);

	HardwarePixelBufferSharedPtr ptr = tex->getBuffer(0,0);
	ptr->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox &pb = ptr->getCurrentLock();
	uint8 *data = static_cast<uint8*>(pb.data);

	size_t height = pb.getHeight();
	size_t width = pb.getWidth();
	size_t depth = pb.getDepth();
	size_t rowPitch = pb.rowPitch;
	size_t slicePitch = pb.slicePitch;

	for (size_t z = 0; z < depth; ++z)
	{
		for (size_t y = 0; y < height; ++y)
		{
			for(size_t x = 0; x < width; ++x)
			{
				float fx = 32-(float)x+0.5f;
				float fy = 32-(float)y+0.5f;
				float fz = 32-((float)z)/3+0.5f;
				float distanceSquare = fx*fx+fy*fy+fz*fz;
				data[slicePitch*z + rowPitch*y + x] =  0x00;
				if (distanceSquare < 1024.0f)
					data[slicePitch*z + rowPitch*y + x] +=  0xFF;
			}
		}
	}
	ptr->unlock();


	TexturePtr tex2 = TextureManager::getSingleton().createManual(
		"DitherTex",
		"General",
		TEX_TYPE_2D,
		mViewport->getActualWidth(),mViewport->getActualHeight(),1,
		0,
		PF_A8
		);

	HardwarePixelBufferSharedPtr ptr2 = tex2->getBuffer(0,0);
	ptr2->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox &pb2 = ptr2->getCurrentLock();
	uint8 *data2 = static_cast<uint8*>(pb2.data);

	size_t height2 = pb2.getHeight();
	size_t width2 = pb2.getWidth();
	size_t rowPitch2 = pb2.rowPitch;

	for (size_t y = 0; y < height2; ++y)
	{
		for(size_t x = 0; x < width2; ++x)
		{
			data2[rowPitch2*y + x] = Ogre::Math::RangeRandom(64.0,192);
		}
	}

	ptr2->unlock();
}

void GraphicsSystem::_createEffects(void)
{
	/// Motion blur effect
	Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
		"Motion Blur", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		);
	{
		Ogre::CompositionTechnique *t = comp3->createTechnique();
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		/// Do the motion blur
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Combine");
			pass->setInput(0, "scene");
			pass->setInput(1, "sum");
			}
		}
		/// Copy back sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Copyback");
			pass->setInput(0, "temp");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/MotionBlur");
			pass->setInput(0, "sum");
			}
		}
	}
	/// Heat vision effect
	Ogre::CompositorPtr comp4 = Ogre::CompositorManager::getSingleton().create(
		"Heat Vision", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		);
	{
		Ogre::CompositionTechnique *t = comp4->createTechnique();
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Light to heat pass
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setIdentifier(0xDEADBABE); /// Identify pass for use in listener
				pass->setMaterialName("Fury/HeatVision/LightToHeat");
				pass->setInput(0, "scene");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Fury/HeatVision/Blur");
				pass->setInput(0, "temp");
			}
		}
	}
}
//-----------------------------------------------------------------------


/*************************************************************************
HeatVisionListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
HeatVisionListener::HeatVisionListener()
{
	timer = new Ogre::Timer();
	start = end = curr = 0.0f;
}
//---------------------------------------------------------------------------
HeatVisionListener::~HeatVisionListener()
{
	delete timer;
}
//---------------------------------------------------------------------------
void HeatVisionListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if(pass_id == 0xDEADBABE)
	{
		timer->reset();
		fpParams =
			mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	}
}
//---------------------------------------------------------------------------
void HeatVisionListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if(pass_id == 0xDEADBABE)
	{
		// "random_fractions" parameter
		fpParams->setNamedConstant("random_fractions", Ogre::Vector4(Ogre::Math::RangeRandom(0.0, 1.0), Ogre::Math::RangeRandom(0, 1.0), 0, 0));

		// "depth_modulator" parameter
		float inc = ((float)timer->getMilliseconds())/1000.0f;
		if ( (fabs(curr-end) <= 0.001) ) {
			// take a new value to reach
			end = Ogre::Math::RangeRandom(0.95, 1.0);
			start = curr;
		} else {
			if (curr > end) curr -= inc;
			else curr += inc;
		}
		timer->reset();

		fpParams->setNamedConstant("depth_modulator", Ogre::Vector4(curr, 0, 0, 0));
	}
}
//---------------------------------------------------------------------------

/*************************************************************************
HDRListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
HDRListener::HDRListener()
{
}
//---------------------------------------------------------------------------
HDRListener::~HDRListener()
{
}
//---------------------------------------------------------------------------
void HDRListener::notifyViewportSize(int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
}
//---------------------------------------------------------------------------
void HDRListener::notifyCompositor(Ogre::CompositorInstance* instance)
{
	// Get some RTT dimensions for later calculations
	Ogre::CompositionTechnique::TextureDefinitionIterator defIter =
		instance->getTechnique()->getTextureDefinitionIterator();
	while (defIter.hasMoreElements())
	{
		Ogre::CompositionTechnique::TextureDefinition* def =
			defIter.getNext();
		if(def->name == "rt_bloom0")
		{
			mBloomSize = (int)def->width; // should be square
			// Calculate gaussian texture offsets & weights
			float deviation = 3.0f;
			float texelSize = 1.0f / (float)mBloomSize;

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
					mBloomTexWeights[i][2] = 1.25f * Ogre::Math::gaussianDistribution(i, 0, deviation);
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
	}
}
//---------------------------------------------------------------------------
void HDRListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch(pass_id)
	{
		//case 994: // rt_lum4
	case 993: // rt_lum3
	case 992: // rt_lum2
	case 991: // rt_lum1
	case 990: // rt_lum0
		break;
	case 800: // rt_brightpass
		break;
	case 701: // rt_bloom1
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
	case 700: // rt_bloom0
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
void HDRListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}
//---------------------------------------------------------------------------


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