/**********************************************************************
* 作者: lf
* 时间: 2010-7-1
* 描述: FurnitureRTT
*		家具预览框
**********************************************************************/
#ifndef FurnitureRTT_H
#define FurnitureRTT_H

#include "EQOgreSys.h"

namespace CEGUI
{
	class Window;
	class UVector2;
}
class EQCamera;
class ModelGcs;

class FurnitureRTT
{
public:
	class FurnitureRTTListener : Ogre::RenderTargetListener
	{
	public:
		FurnitureRTTListener(){}
		~FurnitureRTTListener(){}
		virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) 
		{
			Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
			oldcv = sm->getAmbientLight();

			Ogre::ColourValue newcv(1.f,1.f,1.f,1.f);
			sm->setAmbientLight(newcv);
		}

		virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
		{ 
			Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
			sm->setAmbientLight(oldcv);
		}
	private:
		Ogre::ColourValue oldcv;
	};

	FurnitureRTT();
	virtual ~FurnitureRTT();

	void			startFurnitureRTT(uint itemid);
	void			rotate(int degree);
	void			resetOrientation(void);
	void			zoom(float scale);
	void			resetScale();
	void			setActive(bool active) { mRttTex->getBuffer()->getRenderTarget()->setActive(active); }
	Ogre::String&	getName(){ return mName; }
	EQCamera*		getCamera(){return mCamera;}

private:
	void		_init();
	void		_createRTT();
	void		_createRttGuiObject();

private:
	FurnitureRTTListener	mListener;
	Ogre::String			mName;
	int						x,y,z;
	ModelGcs*				mModel;
	Ogre::SceneNode*		mNode;
	EQCamera*				mCamera;
	CEGUI::Window*			mWindow;
	Ogre::TexturePtr		mRttTex;
	bool					mStart;
	Ogre::Vector3			mZoom;
	Ogre::Quaternion		mRot;
};

#endif
