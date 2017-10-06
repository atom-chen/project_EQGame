#include "HouseEditorPCH.h"

#include "RootManager.h"
#include "GraphicsSystem.h"
#include "EQMainSingletonManager.h"
#include "OgreRootEx.h"
#include "MfcInputEventMgr.h"
#include "TerrainPaintXMLHandler.h"
#include "SceneListener.h"
#include "SystemGcs.h"
#include "TerrainSelections.h"
#include "BrushShapes.h"
#include "SceneManager.h"
#include "IntersectPoint.h"
#include "JunctionPoints.h"
#include "IntersectGrids.h"
#include "Action.h"
#include "RaiseHeightAction.h"
#include "LowerHeightAction.h"
#include "SmoothHeightAction.h"
#include "LevelOffHeightAction.h"
#include "SetTerrainHeightAction.h"
#include "GridInfoFlagModifyAction.h"
#include "SimplePaintAction.h"
#include "AutoTexPaintAction.h"
#include "TerrainLayerOneEraserAction.h"
#include "EditorTable.h"
#include "OperatorManager.h"
#include "TerrainGrid.h"
#include "RegionEditAction.h"
#include "SceneInfo.h"
#include "CommFuncGcs.h"


//-----------------------------------------------------------------------
template<> RootManager* Ogre::Singleton<RootManager>::ms_Singleton = 0;

//-----------------------------------------------------------------------
void CreateSinglen()
{
	new OgreRootEx("","","../Log/HouseEditor.Ogre.log");

	new MfcInputEventMgr();

	new EQMainSingletonManager();

	new EditorTableMgr();

	new GraphicsSystem("../Log/HouseEditor.Ogre.log");

	new SceneListenerManager();
}
//-----------------------------------------------------------------------
void ReleaseSingleton()
{
	delete SceneListenerManager::getSingletonPtr();

	delete GraphicsSystem::getSingletonPtr();

	delete EditorTableMgr::getSingletonPtr();

	delete EQMainSingletonManager::getSingletonPtr();

	delete MfcInputEventMgr::getSingletonPtr();

	delete OgreRootEx::getSingletonPtr();
}
//-----------------------------------------------------------------------
RootManager::RootManager()
: mOperatorManager(NULL)
, mIndicatorRootSceneNode(0)
, mIndicatorNodeVisible(true)
, mTerrainGrid(NULL)
, mRegionEditManager(NULL)
, mSceneEdgeLine(NULL)
{
	CreateSinglen();

	//
	mHeightAdjustSpeed = 3;
	mBrushType = Ogre::StringUtil::BLANK;
	mBrushXSize = 3;
	mBrushZSize = 3;

	mAffectRange = 0;
	mExponentValue = 0.0f;

	mTerrainPaintInfoContainer = new TerrainPaintInfoContainer;
}
//-----------------------------------------------------------------------
RootManager::~RootManager()
{
	if(mTerrainGrid)
		delete mTerrainGrid;

	AutoTexPaintAction::clearBrushMap();

	delete mTerrainPaintInfoContainer;

	destorySelections();
	destoryBrushShapes();
	destoryHitIndicators();

	delete mOperatorManager;
	delete mRegionEditManager;

	// 清理preview image
	clearPreviewImage();

	if ( mIndicatorRootSceneNode )
	{
		assert( mIndicatorRootSceneNode->numChildren() == 0 );
		mIndicatorRootSceneNode->removeAndDestroyAllChildren();
		mIndicatorRootSceneNode->getCreator()->destroySceneNode(mIndicatorRootSceneNode);
	}

	//
	if ( mSceneEdgeLine )
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mSceneEdgeLine);

	//
	ReleaseSingleton();
}
//-----------------------------------------------------------------------
void RootManager::initScene(HWND hWnd)
{
	EditorTableMgr::getSingleton().load();

	GraphicsSystem::getSingleton().setup(hWnd);

	mIndicatorRootSceneNode = SystemGcs::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode();

	mOperatorManager = new OperatorManager;
	mRegionEditManager = new RegionEditManager;

	registerHitIndicators();
	registerBrushShapes();
	registerSelections();
}
//-----------------------------------------------------------------------
void RootManager::setIndicatorNodeVisible(bool visible)
{
	if (mIndicatorRootSceneNode)
	{
		mIndicatorRootSceneNode->setVisible(visible);
		mIndicatorNodeVisible = visible;
	}    
}
//-----------------------------------------------------------------------
void RootManager::AddTransparentTexture( const std::string &textureName, bool transparent )
{
	assert( !textureName.empty() );

	// TransparentTextures::const_iterator i = mTransparentTextures.find(textureName);

	// 插入前先检查是否该纹理已经存在，如果不是，说明是新加入的纹理
	// if ( i==mTransparentTextures.end() )
	//  {
	std::pair<TransparentTextures::iterator, bool> inserted =
		mTransparentTextures.insert( TransparentTextures::value_type(textureName,transparent) );

	assert( inserted.second );
	//   }    
}
//-----------------------------------------------------------------------
bool RootManager::IsTransparentTexture( const std::string &textureName )
{
	assert( !textureName.empty() );
	TransparentTextures::const_iterator i = mTransparentTextures.find(textureName);


	// 如果找不到，说明这个纹理还没被判断过，就读取，并判断
	if ( i==mTransparentTextures.end() )
	{
		const TextureInfo &info = mTerrainPaintInfoContainer->findTextureInfoByTexName(textureName);

		Ogre::Image image;
		image.load(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		bool transparent = image.getHasAlpha() && info.textureType != InnerTex;

		AddTransparentTexture(textureName, transparent);

		return transparent;
	}
	else
		return i->second;    
}
//-----------------------------------------------------------------------
void RootManager::reloadPaintInfo()
{
	loadTerrainPaintInfo( "TerrainTextureInfo.xml" );
}
//-----------------------------------------------------------------------
void RootManager::loadTerrainPaintInfo(const std::string& name)
{
	TerrainPaintXMLHandler handler(mTerrainPaintInfoContainer);

	handler.clearTextureInfos();

	handler.parseXMLFile(name);
}
//-----------------------------------------------------------------------
Ogre::Image* RootManager::getPreviewImage( const std::string &textureName )
{
	PreviewImageMap::iterator it = mPreviewImageMap.find(textureName);

	if ( it == mPreviewImageMap.end() )
	{
		Ogre::Image *image = new Ogre::Image;
		image->load(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		assert (image);

		std::pair< PreviewImageMap::iterator, bool > inserted = 
			mPreviewImageMap.insert(PreviewImageMap::value_type(textureName,image));

		assert (inserted.second);

		return image;
	}
	else
	{
		assert (it->second);
		return it->second;
	}
}
//-----------------------------------------------------------------------
void RootManager::clearPreviewImage(void)
{
	PreviewImageMap::iterator it = mPreviewImageMap.begin();

	while ( it != mPreviewImageMap.end() )
	{
		delete it->second;
		++it;
	}

	mPreviewImageMap.clear();
}
//-----------------------------------------------------------------------
const TextureInfos& RootManager::getTextureInfos( const std::string &brushName )
{
	// 根据组名获取该组的纹理信息
	const TextureInfoMap &textureInfoMap = mTerrainPaintInfoContainer->getTextureInfoMap();

	TextureInfoMap::const_iterator it = textureInfoMap.find(brushName);

	assert ( it != textureInfoMap.end() );

	return it->second;
}
//-----------------------------------------------------------------------
TextureInfo RootManager::getTextureInfo(const std::string &brushName, const std::string &textureName)
{
	// 根据组名和纹理名来获取纹理信息
	const TextureInfos resultInfos = getTextureInfos(brushName);

	for ( size_t i=0; i<resultInfos.size(); ++i )
	{
		if (resultInfos[i].textureName == textureName)
			return resultInfos[i];
	}

	// 能走到这里肯定出错
	OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED,
		"Can't find the texture info for brush '" + brushName + "' with texture name '" + textureName + "'",
		"SceneManipulator::getTextureInfo");
}
//-----------------------------------------------------------------------
BrushShape* RootManager::_getBrushShape(const std::string& type) const
{
	BrushShapes::const_iterator it = mBrushShapes.find(type);
	if (it == mBrushShapes.end())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND,
			"Can't found manip of '" + type + "'.",
			"SceneManipulator::getBrushShape");
	}

	return it->second;
}
//-----------------------------------------------------------------------
void RootManager::registerBrushShapes(void)
{
	registerBrushShape(new BoxBrushShape);
	registerBrushShape(new EllipseBrushShape);
	// TODO: register other usable brush shapes
}
//-----------------------------------------------------------------------
void RootManager::registerBrushShape(BrushShape* hitIndicator)
{
	std::pair<BrushShapes::iterator, bool> inserted =
		mBrushShapes.insert(BrushShapes::value_type(hitIndicator->getType(), hitIndicator));
	if (!inserted.second)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM,
			"Brush shape with type '" + hitIndicator->getType() + "' already registered",
			"SceneManipulator::registerBrushShape");
	}
}
//-----------------------------------------------------------------------
void RootManager::destoryBrushShapes(void)
{
	for (BrushShapes::const_iterator it = mBrushShapes.begin(); it != mBrushShapes.end(); ++it)
	{
		delete it->second;
	}
	mBrushShapes.clear();
}
//-----------------------------------------------------------------------
Selection* RootManager::_getSelection(const std::string& type) const
{
	Selections::const_iterator it = mSelections.find(type);
	if (it == mSelections.end())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND,
			"Can't found manip of '" + type + "'.",
			"SceneManipulator::getSelection");
	}

	return it->second;

}
//-----------------------------------------------------------------------
void RootManager::registerSelections(void)
{
	registerSelection(new JunctionSelection());
	registerSelection(new GridSelection());
	// TODO: register other usable selections
}
//-----------------------------------------------------------------------
void RootManager::registerSelection(Selection* selection)
{
	std::pair<Selections::iterator, bool> inserted =
		mSelections.insert(Selections::value_type(selection->getType(), selection));
	if (!inserted.second)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM,
			"Selection with type '" + selection->getType() + "' already registered",
			"SceneManipulator::registerSelection");
	}
}
//-----------------------------------------------------------------------
void RootManager::destorySelections(void)
{
	for (Selections::const_iterator it = mSelections.begin(); it != mSelections.end(); ++it)
	{
		delete it->second;
	}
	mSelections.clear();
}
//-----------------------------------------------------------------------
void RootManager::setBrush(const std::string& type, size_t xsize, size_t zsize)
{
	BrushShape* brushShape = _getBrushShape(type);
	if (!brushShape || xsize <= 0 || zsize <= 0)
	{
		Ogre::StringUtil::StrStreamType str;
		str << "Invalidate brush settings."
			<< " Type: " << type
			<< " XSize: " << xsize
			<< " ZSize: " << zsize;
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, str.str(), "SceneManipulator::setBrush");
	}

	mBrushType = type;
	mBrushXSize = xsize;
	mBrushZSize = zsize;
}
//-----------------------------------------------------------------------
void RootManager::_buildSelection(Selection* selection, Real xcentre, Real zcentre)
{
	assert(selection);

	if (mBrushType.empty())
	{
		selection->reset();
	}
	else
	{
		BrushShape* brushShape = _getBrushShape(mBrushType);
		assert(brushShape);
		brushShape->build(*selection, xcentre, zcentre, mBrushXSize, mBrushZSize);
	}
}
//-----------------------------------------------------------------------
void RootManager::_buildSelection(const std::string& selectionType, Real xcentre, Real zcentre)
{
	_buildSelection(_getSelection(selectionType), xcentre, zcentre);
}
//-----------------------------------------------------------------------
Ogre::Ray RootManager::getWindowToViewportRay(Real winx, Real winy) const
{
	Ogre::Camera* cam = GraphicsSystem::getSingleton().getCamera();
	Ogre::Viewport* viewport = cam->getViewport();
	return cam->getCameraToViewportRay(
		(winx - viewport->getActualLeft()) / viewport->getActualWidth(),
		(winy - viewport->getActualTop()) / viewport->getActualHeight());
}
//-----------------------------------------------------------------------
Ogre::Ray RootManager::getWindowToViewportRay(const Ogre::Vector2& pt) const
{
	return getWindowToViewportRay(pt.x, pt.y);
}
//-----------------------------------------------------------------------
bool RootManager::getTerrainIntersects(const Ogre::Ray& ray, Ogre::Vector3& position, Ogre::Vector3* normal, bool allowOutside) const
{
	return EQSceneManager::getSingleton().getTerrainData()->getIntersects(ray, position, normal, allowOutside);
}
//-----------------------------------------------------------------------
bool RootManager::getTerrainIntersects(Real winx, Real winy, Ogre::Vector3& position, Ogre::Vector3* normal, bool allowOutside) const
{
	return getTerrainIntersects(getWindowToViewportRay(winx, winy), position, normal, allowOutside);
}
//-----------------------------------------------------------------------
bool RootManager::getTerrainIntersects(const Ogre::Vector2& pt, Ogre::Vector3& position, Ogre::Vector3* normal, bool allowOutside) const
{
	return getTerrainIntersects(pt.x, pt.y, position, normal, allowOutside);
}
//-----------------------------------------------------------------------
Action* RootManager::setActiveAction(const std::string& actionName, bool active)
{
	clearActiveAction();

	if ( actionName.empty() )
		return NULL;

	Action* action = NULL;
	if ( actionName == "RaiseHeightAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<RaiseHeightAction>();
	else if ( actionName == "LowerHeightAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<LowerHeightAction>();
	else if ( actionName == "SmoothHeightAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<SmoothHeightAction>();
	else if ( actionName == "LevelOffHeightAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<LevelOffHeightAction>();
	else if ( actionName == "SetTerrainHeightAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<SetTerrainHeightAction>();
	else if ( actionName == "SimplePaintAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<SimplePaintAction>();
	else if ( actionName == "AutoTexPaintAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<AutoTexPaintAction>();
	else if ( actionName == "TerrainLayerOneEraserAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<TerrainLayerOneEraserAction>();
	else if ( actionName == "FlipDiagonalAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<FlipDiagonalAction>();
	else if ( actionName == "CRegionEditAction" )
		action = MfcInputEventMgr::getSingleton().addEvent<CRegionEditAction>();
	else
	{
		assert(0);
	}

	if ( active )
		action->setActive(true);

	return action;
}
//-----------------------------------------------------------------------
void RootManager::clearActiveAction()
{
	Action* oa = MfcInputEventMgr::getSingleton().getEvent<Action>();
	if ( !oa )
		return;

	oa->setActive(false);
	MfcInputEventMgr::getSingleton().delEvent<Action>();
}
//-----------------------------------------------------------------------
void RootManager::showHitPoint(bool show)
{
	mHitPointShown = show;

	if (!show)
	{
		// Hide all hit indicators
		for (HitIndicators::const_iterator it = mHitIndicators.begin(); it != mHitIndicators.end(); ++it)
		{
			it->second->hide();
		}
	}

	// Active/deactive current action
	Action* action = MfcInputEventMgr::getSingleton().getEvent<Action>();
	if (action)
	{
		action->setActive(show);
	}

}
//-----------------------------------------------------------------------
HitIndicator* RootManager::getHitIndicator(const std::string& name) const
{
	HitIndicators::const_iterator it = mHitIndicators.find(name);
	if (it == mHitIndicators.end())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND,
			"Can't found manip of '" + name + "'.",
			"SceneManipulator::getHitIndicator");
	}

	return it->second;
}
//-----------------------------------------------------------------------
void RootManager::registerHitIndicators(void)
{
	registerHitIndicator(new IntersectPoint());
	registerHitIndicator(new JunctionPoints());
	registerHitIndicator(new IntersectGrids());
	registerHitIndicator(new FlipDiagonalGrids());
	registerHitIndicator(new DisableDecalGrids());
	registerHitIndicator(new TerrainSelectionGrids());
	//registerHitIndicator(new StandardModelIndicator(this));
	// TODO: register other usable hit indicators
}
//-----------------------------------------------------------------------
void RootManager::registerHitIndicator(HitIndicator* hitIndicator)
{
	std::pair<HitIndicators::iterator, bool> inserted =
		mHitIndicators.insert(HitIndicators::value_type(hitIndicator->getName(), hitIndicator));
	if (!inserted.second)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_DUPLICATE_ITEM,
			"Hit indicator with name '" + hitIndicator->getName() + "' already registered",
			"SceneManipulator::registerHitIndicator");
	}
}
//-----------------------------------------------------------------------
void RootManager::destoryHitIndicators(void)
{
	for (HitIndicators::const_iterator it = mHitIndicators.begin(); it != mHitIndicators.end(); ++it)
	{
		delete it->second;
	}
	mHitIndicators.clear();
}
//-----------------------------------------------------------------------
void RootManager::OnTerrainCreate()
{
	// Give hit indicators a chance to adjust shapes
	for (HitIndicators::const_iterator it = mHitIndicators.begin(); it != mHitIndicators.end(); ++it)
	{
		it->second->reshape();
	}
}
//-----------------------------------------------------------------------
TerrainGrid* 
RootManager::getTerrainGrid()
{
	if(!mTerrainGrid)
	{
		mTerrainGrid = new TerrainGrid(SystemGcs::getSingleton().getSceneManager());
	}

	return mTerrainGrid;
}
//-----------------------------------------------------------------------
void RootManager::setSceneEdgeLine(Ogre::ManualObject* m)
{
	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
	if ( mSceneEdgeLine )
		mgr->destroyManualObject(mSceneEdgeLine);

	mSceneEdgeLine = m;
	
	Ogre::SceneNode* node = NULL;
	if ( !mgr->hasSceneNode("--SceneEdgeLineNode--") )
		node = mgr->getRootSceneNode()->createChildSceneNode("--SceneEdgeLineNode--");
	else
		node = mgr->getSceneNode("--SceneEdgeLineNode--");

	node->attachObject(mSceneEdgeLine);
}
//-----------------------------------------------------------------------
void RootManager::reshapeTerrainEdge()
{
	Ogre::Vector2& b = EQSceneManager::getSingleton().getSceneInfo()->mSceneBeginPos;
	Ogre::Vector2& e = EQSceneManager::getSingleton().getSceneInfo()->mSceneEndPos;

	if ( EQSceneManager::getSingleton().getTerrainData()->isEmpty() )
	{
		std::vector<Ogre::Vector3> ptList;
		ptList.push_back(Ogre::Vector3(b.x, 10, b.y));
		ptList.push_back(Ogre::Vector3(e.x, 10, b.y));
		ptList.push_back(Ogre::Vector3(e.x, 10, e.y));
		ptList.push_back(Ogre::Vector3(b.x, 10, e.y));
		ptList.push_back(Ogre::Vector3(b.x, 10, b.y));

		Ogre::ManualObject* m = createLine(SystemGcs::getSingleton().getSceneManager(), ptList, Ogre::ColourValue::Red);
		setSceneEdgeLine(m);
	}
	else
	{
		TerrainData* terrData = EQSceneManager::getSingleton().getTerrainData();
		std::vector<Ogre::Vector3> ptList;

		for ( float f = b.x; f < e.x; f += 80.0f )
		{
			float h = terrData->getHeightAt(f, b.y);
			ptList.push_back(Ogre::Vector3(f, h+10, b.y));
		}

		for ( float f = b.y; f < e.y; f += 80.0f )
		{
			float h = terrData->getHeightAt(e.x, f);
			ptList.push_back(Ogre::Vector3(e.x, h+10, f));
		}

		for ( float f = e.x; f > b.x; f -= 80.0f )
		{
			float h = terrData->getHeightAt(f, e.y);
			ptList.push_back(Ogre::Vector3(f, h+10, e.y));
		}

		for ( float f = e.y; f >= b.y; f -= 80.0f )
		{
			float h = terrData->getHeightAt(b.x, f);
			ptList.push_back(Ogre::Vector3(b.x, h+10, f));
		}

		Ogre::ManualObject* m = createLine(SystemGcs::getSingleton().getSceneManager(), ptList, Ogre::ColourValue::Red);
		setSceneEdgeLine(m);
	}
}



