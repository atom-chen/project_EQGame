#include "EQMainPCH.h"

#include "SceneInfo.h"

#include "Terrain.h"
#include "SystemGcs.h"
#include "SceneEntityObject.h"
#include "EQOctreeNode.h"

#define CUT_SCENE_UNIT_DISTANCE		40.0f


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
SceneInfo::SceneInfo()
: mTerrain(NULL)
, mTerrainData(NULL)
, mSceneObjectNode(NULL)
, mStaticGeom(NULL)
, mInOctreeScene(false)
, mCamera(NULL)
{
	//创建了3个管理类
	mOctreeMgrs[CDL_FAR] = new EQOctreeManager();
	mOctreeMgrs[CDL_MID] = new EQOctreeManager();
	mOctreeMgrs[CDL_CLOSE] = new EQOctreeManager();


	mSceneBeginPos = Ogre::Vector2(-2000.0f, -2000.0f);
	mSceneEndPos = Ogre::Vector2(2000.0f, 2000.0f);

	mTerrain = new Terrain;
	mTerrainData = mTerrain->getData();
}
//-----------------------------------------------------------------------
SceneInfo::~SceneInfo()
{
	destroy();

	delete mTerrain;

	for( std::map<int, EQOctreeManager*>::iterator it = mOctreeMgrs.begin(); it != mOctreeMgrs.end(); ++it )
	{
		delete it->second;
	}
	mOctreeMgrs.clear();
}
//-----------------------------------------------------------------------
bool SceneInfo::isTerrainEmpty()
{
	return mTerrainData->isEmpty();
}
//-----------------------------------------------------------------------
void SceneInfo::destroy(void)
{
	if ( mInOctreeScene )
		exitOctreeScene();


	if ( mStaticGeom )
	{
		Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
		mgr->destroyStaticGeometry(mStaticGeom->getName());
		mStaticGeom = NULL;
	}

	clearSceneObject();

	if ( mSceneObjectNode )
	{
		assert(mSceneObjectNode->numChildren()==0 );
		mSceneObjectNode->getCreator()->destroySceneNode(mSceneObjectNode);
		mSceneObjectNode = NULL;
	}

	getTerrain()->clearGeometry();
}
//-----------------------------------------------------------------------
void SceneInfo::reset(void)
{
	destroy();

	getTerrain()->reset();
}
//-----------------------------------------------------------------------
void SceneInfo::initialise( bool inEditer)
{
	if ( !isTerrainEmpty() )
	{
		getTerrain()->setLightmapQuality(Terrain::LMQ_HIGH);
		getTerrain()->buildGeometry(getSceneNode(), inEditer);
	}

	bool useCutScene = false;
	for ( std::map<uint, SceneEntityObject*>::iterator it = mSceneEntityObjects.begin(); it != mSceneEntityObjects.end(); ++it )
	{
		it->second->createInst();
		if ( it->second->getCutDistanceLevel() != CDL_NONE )
			useCutScene = true;
	}

	//新场景不在合并静态物件
// 	const float range = 80*32;
// 	if ( !inEditer && !useCutScene )	//在非编辑状态, 且都没有裁切场景, 才能合并
// 		bakeStaticGeometries(range);
}
//-----------------------------------------------------------------------
void SceneInfo::addListener(Listener* l)
{
	for (std::vector<Listener*>::iterator it = mListenerList.begin(); it!=mListenerList.end(); ++it)
	{
		if (l == (*it))
			return;
	}
	mListenerList.push_back(l);
}
//-----------------------------------------------------------------------
void SceneInfo::removeListener(Listener* l)
{
	for (std::vector<Listener*>::iterator it = mListenerList.begin(); it!=mListenerList.end();)
	{
		if (l == (*it))
			it = mListenerList.erase(it);
		else
			++it;
	}
}
//-----------------------------------------------------------------------
Ogre::SceneNode* SceneInfo::getSceneNode()
{
	static const std::string nodeName = "__SceneInfoNode__";

	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
	if ( !mgr->hasSceneNode(nodeName) )
		mgr->getRootSceneNode()->createChildSceneNode(nodeName);
	
	return mgr->getSceneNode(nodeName);
}
//-----------------------------------------------------------------------
Ogre::SceneNode* SceneInfo::getSceneObjectNode()
{
	if ( mSceneObjectNode )
		return mSceneObjectNode;

	mSceneObjectNode = getSceneNode()->createChildSceneNode();
	return mSceneObjectNode;
}
//-----------------------------------------------------------------------
bool SceneInfo::hasSceneObject(uint id)
{
	return (mSceneObjects.find(id)!=mSceneObjects.end());
}
//-----------------------------------------------------------------------
void SceneInfo::destroySceneObject(uint id)
{
	std::map<uint, SceneObject*>::iterator it = mSceneObjects.find(id);
	if ( it == mSceneObjects.end() )
		return;

	//fire
	for (std::vector<Listener*>::iterator it = mListenerList.begin(); it!=mListenerList.end(); ++it)
		(*it)->onDestroySceneObject(id);


	SceneObject* obj = it->second;

	mSceneObjects.erase(it);
	if ( obj->getClassName() == "SceneEntityObject" )
	{
		mSceneEntityObjects.erase(id);
	}
	else
	{
		assert(0);
	}

	delete obj;
}
//-----------------------------------------------------------------------
uint SceneInfo::distributeId()
{
	static uint id = 0;
	return ++id;
}
//-----------------------------------------------------------------------
uint SceneInfo::addSceneObject(SceneObject* obj)
{
	uint id = obj->getId();
	if ( id == 0 )	//如果为0, 则自动分配一个id
	{
		id = distributeId();
		obj->setId(id);
	}

	if ( mSceneObjects.find(id) != mSceneObjects.end() )
	{
		EQ_EXCEPT("SceneObject id duplicate!", "SceneInfo::addSceneObject");
	}

	mSceneObjects.insert(std::pair<uint, SceneObject*>(id, obj));

	//
	if ( obj->getClassName() == "SceneEntityObject" )
	{
		mSceneEntityObjects.insert(std::pair<uint, SceneEntityObject*>(id, static_cast<SceneEntityObject*>(obj)));
	}
	else
	{
		assert(0);
	}

	//fire
	for (std::vector<Listener*>::iterator it = mListenerList.begin(); it!=mListenerList.end(); ++it)
		(*it)->onAddSceneObject(obj);


	return id;
}
//-----------------------------------------------------------------------
void SceneInfo::clearSceneObject()
{
	for ( std::map<uint, SceneObject*>::iterator it = mSceneObjects.begin(); it != mSceneObjects.end(); ++it )
	{
		delete it->second;
	}
	mSceneObjects.clear();

	mSceneEntityObjects.clear();
}
//-----------------------------------------------------------------------
SceneObject* SceneInfo::getSceneObject(uint id)
{
	std::map<uint, SceneObject*>::iterator it = mSceneObjects.find(id);
	if ( it ==  mSceneObjects.end() )
		return NULL;
	else
		return it->second;
}
//-----------------------------------------------------------------------
SceneEntityObject* SceneInfo::getSceneEntityObject(uint id)
{
	std::map<uint, SceneEntityObject*>::iterator it = mSceneEntityObjects.find(id);
	if ( it ==  mSceneEntityObjects.end() )
		return NULL;
	else
		return it->second;
}
//-----------------------------------------------------------------------
void SceneInfo::update(float delta)
{
	for (std::map<uint, SceneEntityObject*>::iterator it = mSceneEntityObjects.begin(); it != mSceneEntityObjects.end(); ++it)
		it->second->updateAnimation(delta);

	if ( mInOctreeScene )
		cutScene();
}
//-----------------------------------------------------------------------
void SceneInfo::bakeStaticGeometries(float range)
{
	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();

	if ( mStaticGeom )
	{
		mgr->destroyStaticGeometry(mStaticGeom->getName());
		mStaticGeom = NULL;
	}

	mStaticGeom = mgr->createStaticGeometry("EQ/StaticGeometry/0");
	mStaticGeom->setCastShadows(true);
	mStaticGeom->setRegionDimensions(Ogre::Vector3(range, range, range));

	for (std::map<uint, SceneEntityObject*>::iterator it = mSceneEntityObjects.begin(); it != mSceneEntityObjects.end(); ++it)
	{
		it->second->bakeStaticGeometry(mStaticGeom);
	}

	mStaticGeom->build();
}
//-----------------------------------------------------------------------
void SceneInfo::enterOctreeScene(Ogre::Camera* cam)
{
	mInOctreeScene = true;
	
	//
	SystemGcs::getSingleton().getSceneManager()->getRootSceneNode()->_update(true, false);

	//
	mCamera = cam;

	for (std::map<uint, SceneEntityObject*>::iterator it = mSceneEntityObjects.begin(); it != mSceneEntityObjects.end(); ++it)
	{
		SceneEntityObject* obj = it->second;
		if ( obj->getCutDistanceLevel() != CDL_NONE )
		{
			EQOctreeManager& mgr = *mOctreeMgrs[obj->getCutDistanceLevel()];
			EQOctreeNode* node = static_cast<EQOctreeNode*>(mgr.getRootNode()->createChild());

			obj->goinCutDistance(node);
		}
	}

	std::map<int, EQOctreeManager*>::iterator it;
	for( it = mOctreeMgrs.begin(); it != mOctreeMgrs.end(); ++it )
	{
		it->second->_updateSceneGraph();
	}

	//需要先剔除一次
	mLastPos = Ogre::Vector3(100000, 100000, 100000);
	//执行剔除函数
	cutScene();
}
//-----------------------------------------------------------------------
void SceneInfo::exitOctreeScene()
{
	for (std::map<uint, SceneEntityObject*>::iterator it = mSceneEntityObjects.begin(); it != mSceneEntityObjects.end(); ++it)
	{
		it->second->gooutCutDistance();
	}

	std::map<int, EQOctreeManager*>::iterator it;
	for( it = mOctreeMgrs.begin(); it != mOctreeMgrs.end(); ++it )
	{
		it->second->clearScene();
	}

	///
	mLastPos = Ogre::Vector3(100000, 100000, 100000);

	//
	mInOctreeScene = false;
}
//-----------------------------------------------------------------------
void SceneInfo::cutScene()
{
	const Ogre::Vector3& currPos = mCamera->getPosition();
	float offsetLenSqu = (mLastPos - currPos).squaredLength();

	if ( offsetLenSqu > CUT_SCENE_UNIT_DISTANCE )
	{
		std::map<int, EQOctreeManager*>::iterator it;
		for( it = mOctreeMgrs.begin(); it != mOctreeMgrs.end(); ++it )
		{
			EQOctreeManager& mgr = *it->second;
			mgr.cutNodes(currPos);	//场景剔除
		}

		//
		mLastPos = currPos;
	}
}
//-----------------------------------------------------------------------
void SceneInfo::setCutRadius(int lvl, float radius)
{
	mOctreeMgrs[lvl]->setRadius(radius);
}
//-----------------------------------------------------------------------
void SceneInfo::turnAllLight(bool on)
{
	for(std::map<uint, SceneEntityObject *>::iterator it=mSceneEntityObjects.begin(); it!=mSceneEntityObjects.end(); it++)
		on ? it->second->turnOnLight() : it->second->turnOffLight();
}

void SceneInfo::changeSkybox(std::string& mat)
{
	if(Ogre::MaterialManager::getSingleton().resourceExists(mat))
	{
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(mat);
		Ogre::TextureUnitState* texture = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);

		std::string texName;
		std::string frameName = material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getFrameTextureName(0);
		if(texture->is3D())
		{
			texName = frameName;
		}
		else
		{
			std::string ext;
			std::string baseName;
			size_t pos = frameName.find_last_of(".");
			if( pos != std::string::npos )
			{
				baseName = frameName.substr(0, pos-3);
				ext = frameName.substr(pos);
			}
			texName = baseName + ext;
		}

		for(std::map<uint, SceneEntityObject *>::iterator it=mSceneEntityObjects.begin(); it!=mSceneEntityObjects.end(); it++)
		{
			Ogre::Entity* ent = it->second->getEntity();
			if(ent)
			{
				Ogre::TextureUnitState* tex = ent->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState("skybox");
				if(tex)
				{
					tex->setCubicTextureName(texName, true);
					//需要手动load一遍，否则会和场景资源回收冲突
					ent->getSubEntity(0)->getMaterial()->load();
				}
			}
		}
	}
}
//-----------------------------------------------------------------------