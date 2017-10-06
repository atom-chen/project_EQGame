/******************************************************************************
* 作者: Y3
* 时间: 2010-05-20
* 描述: 房屋资源
******************************************************************************/
#ifndef SceneInfo_H
#define SceneInfo_H

#include "EQMainPrerequisites.h"
#include "EQOctreeManager.h"


class Terrain;
class TerrainData;
class SceneObject;
class SceneEntityObject;


/** 场景信息, 指室外的
*/
class SceneInfo
{
public:
	class Listener
	{
	public:
		virtual ~Listener(){}
		
		virtual void onAddSceneObject(SceneObject* obj) {}
		virtual void onDestroySceneObject(uint id) {}
	};

	class OctreeCameraNodeListener : public Ogre::Node::Listener
	{
	public:
		void nodeUpdated(const Ogre::Node*);

		Ogre::Vector3 lastPos;
		SceneInfo* owner;
	};
public:
	SceneInfo(void);
	~SceneInfo();

	/** Destroys all the created render instance state (reverse of initialise). 
	@remarks
	You can call initialise() again after this.
	*/
	void destroy(void);

	/** Clears any of the objects added to this scene and 
	destroys anything which has already been initialised.
	*/
	void reset(void);

	/** Initialise the scene. 
	*/
	void initialise(bool inEditer);

	void addListener(Listener* l);
	void removeListener(Listener* l);

	Ogre::SceneNode* getSceneNode();


	Terrain* getTerrain(void) const { return mTerrain; }

	TerrainData* getTerrainData(void) const {	return mTerrainData;}

	bool isTerrainEmpty();


	Ogre::SceneNode* getSceneObjectNode();
	std::map<uint, SceneObject*>& getSceneObjects() {return mSceneObjects;}
	std::map<uint, SceneEntityObject*>& getSceneEntityObjects() {return mSceneEntityObjects;}


	uint addSceneObject(SceneObject* obj);
	void destroySceneObject(uint id);
	void clearSceneObject();
	bool hasSceneObject(uint id);

	SceneObject* getSceneObject(uint id);
	SceneEntityObject* getSceneEntityObject(uint id);

	void update(float delta);

	void bakeStaticGeometries(float range);
	bool isInStaticScene() {return (mStaticGeom!=NULL);}

	void enterOctreeScene(Ogre::Camera* cam);
	void exitOctreeScene();
	void setCutRadius(int lvl, float radius);
	bool isInOctreeScene() { return mInOctreeScene; }

	void turnAllLight(bool on);
	void changeSkybox(std::string& mat);

public:
	/// 场景的边界
	Ogre::Vector2	mSceneBeginPos;
	Ogre::Vector2	mSceneEndPos;

protected:
	uint distributeId();
	void cutScene();

protected:
	std::vector<Listener*> mListenerList;


	Terrain*		mTerrain;
	TerrainData*	mTerrainData;

	Ogre::StaticGeometry*	mStaticGeom;
	std::map<int, EQOctreeManager*> mOctreeMgrs;
	bool			mInOctreeScene;
	Ogre::Camera*	mCamera;
	Ogre::Vector3	mLastPos;

	Ogre::SceneNode* mSceneObjectNode;
	std::map<uint, SceneObject*>		mSceneObjects;
	std::map<uint, SceneEntityObject*>	mSceneEntityObjects;
};

#endif

