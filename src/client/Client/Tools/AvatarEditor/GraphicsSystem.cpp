#include "AvatarEditorPCH.h"

#include "GraphicsSystem.h"
#include "EQMain.h"
#include "SystemGcs.h"
#include "GraphicsTable.h"
#include "AvatarTable.h"
#include "Avatar.h"
#include "OgreRootEx.h"
#include "ItemTable.h"
#include "ModelManagerGcs.h"
#include "CommFuncGcs.h"
#include "HouseTable.h"
#include "ParticleMgr.h"

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
	, mItemNode( NULL )
	, mGridNode( NULL )
	, mGrid( NULL )
	, mNodeAdjuster( NULL )
	, mMainLight(NULL)
	, mModelMorphTime(0)
	, mModelMorphKeyframe(0)
	, mMaxModelMorphKeyframe(0)
{
	init(logOgre);
}
//-----------------------------------------------------------------------
GraphicsSystem::~GraphicsSystem()
{
	if( mSceneMgr )
		mSceneMgr->destroyLight(mMainLight);

	if ( mNodeAdjuster )
		delete mNodeAdjuster;

	if (mFrameListener)
		delete mFrameListener;

	//delete OgreRootEx::getSingletonPtr();
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
	new OgreRootEx("","",logOgre);
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
#else
	mRoot->loadPlugin("RenderSystem_Direct3D9");
	mRoot->loadPlugin("Plugin_OctreeSceneManager");
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
	ItemTableManager::getSingleton().load();
	HouseTableManager::getSingleton().load();
}
//-----------------------------------------------------------------------
void GraphicsSystem::setup(HWND hWnd,int width,int height)
{
	if ( mSetup )
		return;

	// 添加窗口
	// This window should be the current ChildView window using the externalWindowHandle
	// value pair option.
	Ogre::NameValuePairList parms;
	parms["externalWindowHandle"] = Ogre::StringConverter::toString((long)hWnd);
	parms["FSAA"] = "4";
	
	mWindow = mRoot->createRenderWindow("AvatarEditor Window", width, height, false, &parms);


	// 创建场景管理器
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
	//mSceneMgr = mRoot->createSceneManager(Ogre::ST_INTERIOR);
	//mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);


	

	SystemGcs::getSingleton().setSceneManager(mSceneMgr);

	// 创建摄像机
	mCamera = mSceneMgr->createCamera("camera");
	mCamera->setNearClipDistance(5);

	// 创建视口
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0.1,0.4,0.8));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio( Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// Set default mipmap level (NB some APIs ignore this)
	//Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);

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

	return;
}
//-----------------------------------------------------------------------
void GraphicsSystem::render()
{
	if ( mSetup && mCanDraw )
	{
		if ( !mItemEntitys.empty() )
		{
			if ( mModelMorphTime == 0 )
			{
				mModelMorphTime = clock();
				mModelMorphKeyframe = 0;

				mMaxModelMorphKeyframe = 0;
				for ( std::vector<ModelGcs*>::iterator it = mItemEntitys.begin(); it != mItemEntitys.end(); ++it )
				{
					int a = (*it)->hasModelMorph();
					if ( a != 0 )
						(*it)->setModelMorph(0);
					if ( a > mMaxModelMorphKeyframe )
						mMaxModelMorphKeyframe = a;
				}
			}
			else
			{
				int t = clock();
				if ( t - mModelMorphTime > 2000 )
				{
					mModelMorphTime = t;
					mModelMorphKeyframe++;

					if ( mModelMorphKeyframe >= mMaxModelMorphKeyframe )
						mModelMorphKeyframe = 0;

					for ( std::vector<ModelGcs*>::iterator it = mItemEntitys.begin(); it != mItemEntitys.end(); ++it )
						(*it)->setModelMorph(mModelMorphKeyframe);
				}
			}
		}

		mRoot->renderOneFrame();

		ParticleMgr::getSingleton().update();
	}
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
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.6, 0.6, 0.6));
	
	// Create a light
	mMainLight = mSceneMgr->createLight("MainLight");

	mMainLight->setType(Ogre::Light::LT_DIRECTIONAL);

	mMainLight->setDirection(0, 0, -0.5);
	mMainLight->setDiffuseColour(0.6,0.6,0.6);

	mCamera->setPosition(0,0, 400);
	mCamera->lookAt(0,0,-10000);	



	mGridNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	// 网格
	//mGrid = CreateManualGrid( mSceneMgr, "mySeceneGrid", 20, 7 );

	mGrid = CreateManualGrid( mSceneMgr, "mySeceneGrid", 80, 20, 5,
						Ogre::ColourValue(128,128,128),
						Ogre::ColourValue::Black,
						Ogre::ColourValue(64,64,64));


	mGrid->setQueryFlags( 0 );	
	mGridNode->attachObject( mGrid );

	mItemNode = mGridNode->createChildSceneNode();

	mNodeAdjuster = new NodeAdjuster();

	// 创建平面
	//Ogre::Plane		plane(Ogre::Vector3::UNIT_Y, 0);

	//Ogre::MeshManager::getSingleton().createPlane(
	//	"myGround",
	//	Ogre::ResourceGroupManager::
	//	DEFAULT_RESOURCE_GROUP_NAME,
	//	plane,
	//	10000,10000,1,1,
	//	true,
	//	1,1,1,
	//	Ogre::Vector3::UNIT_Z);

	//Ogre::Entity * ent = mSceneMgr->createEntity("GroundEntity", "myGround");
	//mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3( 0, -0.1, 0 ))->attachObject(ent);
	////临时用
	//ent->setMaterialName("20220001_01");
	//ent->setQueryFlags( 0 );
}
//--------------------------------------------------------------------------
void GraphicsSystem::_fireNotifySystemSetup()
{
	std::vector<Listener*>::iterator i, iend=mListeners.end();
	for(i=mListeners.begin(); i!=iend; ++i)
		(*i)->notifySystemSetup();
}
//--------------------------------------------------------------------------
void GraphicsSystem::displayItem( uint modelDisplayID )
{
	std::vector<const ModelDisplayRow *> rows;
	rows.push_back( GraphicsTableManager::getSingleton().getModelDisplayRow( modelDisplayID ) );
	displayItem( rows );
}
//--------------------------------------------------------------------------
void GraphicsSystem::displayItem( const std::vector<const ModelDisplayRow *> & rows )
{
	// 清除旧物品实体
	if ( !mItemEntitys.empty() )
	{
		for ( uint i = 0; i < mItemEntitys.size(); ++i )
		{
			//mItemNode->detachObject( mItemEntitys[i] );
			//mSceneMgr->destroyEntity( mItemEntitys[i] );
			ModelManagerGcs::getSingleton().destroyModel(mItemEntitys[i]);
		}
	}
	mItemEntitys.clear();
	mModelMorphTime = 0;
	mModelMorphKeyframe = 0;

	// 加入新物品实体
	for ( uint i = 0; i < rows.size(); ++i )
	{
		const ModelDisplayRow * r = rows[i];
		if ( !r )
			continue;
		//Ogre::Entity * ent = mSceneMgr->createEntity( "LookItemEntity" + Ogre::StringConverter::toString( i ), r->mesh );
		//if ( !r->mat.empty() )
		//	ent->setMaterialName( r->mat );

		//mItemEntitys.push_back( ent );
		//mItemNode->attachObject( ent );

		SimpleCreateModelRule rule(r->mesh, r->mat);
		ModelGcs* mod = ModelManagerGcs::getSingleton().createModel(&rule);
		mod->setParentNode(mItemNode);

		mItemEntitys.push_back(mod);
	}
}
//--------------------------------------------------------------------------
void GraphicsSystem::setItemNodePosition( const Ogre::Vector3 & pos )
{
	if ( mItemNode )
		mItemNode->setPosition( pos );
}
//--------------------------------------------------------------------------
Ogre::SceneNode * GraphicsSystem::getItemNode()
{
	return mItemNode;
}
//--------------------------------------------------------------------------
bool GraphicsSystem::hasSetup() const
{
	return mSetup;
}
//--------------------------------------------------------------------------


