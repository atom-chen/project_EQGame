/******************************************************************************
* 作者: Y3
* 时间: 2009-10-06
* 描述: Ogre的包装
******************************************************************************/
#ifndef GraphicsSystem_H
#define GraphicsSystem_H

#include "ModelManagerGcs.h"

#include "AppFrameListener.h"
#include "CameraFlyEvent.h"

//Compositor
class HeatVisionListener: public Ogre::CompositorInstance::Listener
{
public:
	HeatVisionListener();
	virtual ~HeatVisionListener();
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
protected:
	Ogre::GpuProgramParametersSharedPtr fpParams;
	float start, end, curr;
	Ogre::Timer *timer;
};
//---------------------------------------------------------------------------
class HDRListener: public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	int mBloomSize;
	// Array params - have to pack in groups of 4 since this is how Cg generates them
	// also prevents dependent texture read problems if ops don't require swizzle
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexOffsetsVert[15][4];
public:
	HDRListener();
	virtual ~HDRListener();
	void notifyViewportSize(int width, int height);
	void notifyCompositor(Ogre::CompositorInstance* instance);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};
//---------------------------------------------------------------------------
class GaussianListener: public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	// Array params - have to pack in groups of 4 since this is how Cg generates them
	// also prevents dependent texture read problems if ops don't require swizzle
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexOffsetsVert[15][4];
public:
	GaussianListener();
	virtual ~GaussianListener();
	void notifyViewportSize(int width, int height);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};
//---------------------------------------------------------------------------
class ContrastListener : public Ogre::CompositorInstance::Listener
{
protected:
	Ogre::GpuProgramParametersSharedPtr fpParams;
	float mContrast;
public:
	ContrastListener():mContrast(1.0f){}
	void setContrast(float contrast) { mContrast = contrast; }
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};


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
	void setup( HWND hWnd );
	void render();
	void sizeChange();

	void addListener( Listener* l );
	void removeListener(Listener *l);

	void setContact(float contrast);

public:
	Ogre::SceneManager* getSceneMgr() { return mSceneMgr; }
	Ogre::Camera* getCamera() { return mCamera; }
	Ogre::RenderWindow* getWindow() { return mWindow; }
	Ogre::Viewport*	getViewport() { return mViewport; }

public:
	Ogre::ManualObject* getWorldGrid() { return mWorldGrid; }

	void setFocusGrid( Ogre::SceneNode * gridNode );
	void showFocusGrid( bool bVal );

protected:
	void setupRederSystem();
	void setupPlugins();
	void setupResources();
	void loadResources();
	void createFrameListener();
	void createScene();

protected:
	void _fireNotifySystemSetup();
	void _registerCompositors();
	void _createTextures();
	void _createEffects();

protected:
	bool			mInit;
	bool			mSetup;
	bool			mCanDraw;
	bool			mShowFocusGrid;

	Ogre::Root *			mRoot;
	Ogre::RenderWindow *	mWindow;
	Ogre::Camera *			mCamera;
	Ogre::SceneManager *	mSceneMgr;
	Ogre::Viewport*				mViewport;

	AppFrameListener *		mFrameListener;

	std::vector<Listener*>	mListeners;

	/// 世界网格
	Ogre::ManualObject *	mWorldGrid;

	/// 指定到某个格子
	Ogre::ManualObject *	mFocusGrid;

	HeatVisionListener *hvListener;
	HDRListener *hdrListener;
	GaussianListener *gaussianListener;

	Ogre::CompositorInstance*		mContrast;
	ContrastListener*				mContrastListener;
};

#endif