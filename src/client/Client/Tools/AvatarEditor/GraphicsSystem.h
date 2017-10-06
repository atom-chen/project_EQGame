/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-06
* ����: Ogre�İ�װ
* �޸�:
*		2009-11-27 by Hongjx ������ʾ��Ʒ����diplayItem
*		
******************************************************************************/
#ifndef GraphicsSystem_H
#define GraphicsSystem_H

#include "ModelManagerGcs.h"

#include "AppFrameListener.h"
#include "NodeAdjuster.h"

class OgreRootEx;
struct ModelDisplayRow;
class GraphicsSystem : public Ogre::Singleton<GraphicsSystem> 
{
public:
	class Listener
	{
	public:
		virtual ~Listener() {}

		virtual void notifySystemSetup() {};
	};

public:
	GraphicsSystem(const std::string& logOgre);
	~GraphicsSystem();

public:
	void init(const std::string& logOgre);
	void setup(HWND hWnd,int width,int height);
	void render();
	void sizeChange();

	void addListener( Listener* l );
	void removeListener(Listener *l);
	bool hasSetup() const;
public:
	Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }
	Ogre::Camera* getCamera() { return mCamera; }


	Ogre::SceneNode * getGridNode()
	{
		return mGridNode;
	}

	Ogre::ManualObject * getGrid()
	{
		return mGrid;
	}
	//ModelGcs* getModel() { return mModel; }

	/** ��Ҫ������ʾѡ�е���Ʒ
	*/
	void displayItem( const std::vector<const ModelDisplayRow *> & rows );
	
	/** ��ʾ������Ʒ
	*/
	void displayItem( uint modelDisplayID );

	void setItemNodePosition( const Ogre::Vector3 & pos );
	Ogre::SceneNode * getItemNode();
	NodeAdjuster * getNodeAdjuster() { return mNodeAdjuster; }

	Ogre::Light* getMainLight() { return mMainLight; }
protected:
	void setupRederSystem();
	void setupPlugins();
	void setupResources();
	void loadResources();
	void createFrameListener();
	void createScene();


protected:
	void _fireNotifySystemSetup();

protected:
	bool			mInit;
	bool			mSetup;
	bool			mCanDraw;

	Ogre::Root*		mRoot;
	Ogre::RenderWindow*	mWindow;
	Ogre::Camera*	mCamera;
	Ogre::SceneManager* mSceneMgr;

	AppFrameListener* mFrameListener;

	//ModelGcs*		mModel;

	std::vector<Listener*>				mListeners;

	Ogre::SceneNode *	mGridNode;
	Ogre::ManualObject * mGrid;
	Ogre::SceneNode *	mItemNode;				// ��Ʒ�ڵ�
	std::vector<ModelGcs *>	mItemEntitys;	// ��Ʒʵ�弯	
	int					mModelMorphTime;
	int					mModelMorphKeyframe;
	int					mMaxModelMorphKeyframe;
	NodeAdjuster * mNodeAdjuster;

	Ogre::Light* mMainLight; //�������
};

#endif