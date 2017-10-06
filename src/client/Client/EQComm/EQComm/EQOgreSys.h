/**********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: EQOGRE系统

**********************************************************************/
#ifndef EQOgreSys_H
#define EQOgreSys_H

#include "EQMainPrerequisites.h"

class ModelGcs;
class GaussianListener;
class ContrastListener;
class MosaicListener;
class ConfigOption;

class EQOgreSys : public Ogre::Singleton<EQOgreSys>
{

public:
	EQOgreSys(void);
	virtual ~EQOgreSys();
	virtual void 		initial(void* pParam);

	typedef std::map<Ogre::String, Ogre::String> NameValuePairList;
	virtual void 		setup(Ogre::ulong windowHandle, 
							  const NameValuePairList* extraParams);
	void				cleanup();

	void				setShadow(int shadowTech);

public:
	virtual bool 		renderOneFrame(void);

	Ogre::Root*			getRoot(void) const			{ return mRoot; }
	Ogre::RenderWindow* getRenderWindow(void) const { return mRenderWindow; }
	Ogre::SceneManager* getSceneManager(void) const { return mSceneMgr; }
	Ogre::Camera*		getCamera(void) const		{ return mCamera; }
	Ogre::Viewport*		getViewport(void) const		{ return mViewport; }
	ConfigOption*		getConfigOption(void) const { return mConfigOption; }

public:
	void				setGaussianBlur(bool state);
	void				setContact(float contrast);
	uint				addMosaic(Ogre::Vector4& rectangle);
	void				updateMosaic(uint id, Ogre::Vector4& rectangle);
	void				removeMosaic(uint id);
	void				notifyViewportSize(int width, int height);

protected:
	virtual void 		_setupResources();
	virtual void 		_createRenderWindow(
							Ogre::ulong windowHandle, 
							const NameValuePairList* extraParams);
	virtual void 		_createSceneManager(void);
	virtual void 		_createCamera(void);
	virtual void 		_createViewports(void);
	virtual void 		_createResourceListener(void);
	virtual void 		_loadResources(void);
	virtual void 		_initScene(void);
	void				_setTextureFiltering(int opt);

protected:
	Ogre::Root*			mRoot;
	Ogre::RenderWindow* mRenderWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneNode*	mRootNode;
	Ogre::Camera*		mCamera;
	Ogre::Viewport*		mViewport;
	ConfigOption*		mConfigOption;

	Ogre::CompositorInstance*		mGaussianBlur;
	GaussianListener*				mGaussianListener;

	Ogre::CompositorInstance*		mContrast;
	ContrastListener*				mContrastListener;

	Ogre::CompositorInstance*		mMosaic;
	MosaicListener*					mMosaicListener;
	std::map<uint, Ogre::Vector4>	mMosaicMap;
protected:
	HWND				mHwnd;
	ModelGcs*			mModel;
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

class MosaicListener : public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	Ogre::GpuProgramParametersSharedPtr fpParams;
	std::map<uint, Ogre::Vector4> mAreaMap;
public:
	void addRec(uint id, Ogre::Vector4& rectangle);
	uint delRec(uint id);
	void updRec(uint id, Ogre::Vector4& rectangle);
	void notifyViewportSize(int width, int height);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

#define sEQOgreSys EQOgreSys::getSingleton()
#endif