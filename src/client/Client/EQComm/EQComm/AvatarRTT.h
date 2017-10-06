/**********************************************************************
* 作者: 麦_Mike
* 时间: 2009-12-22
* 描述: AvatarRTT
*		角色一览框
**********************************************************************/
#ifndef AvatarRTT_H
#define AvatarRTT_H

#include "Ogre/OgreRenderTargetListener.h"
#include "EQOgreSys.h"
#include "ZoneLightListener.h"
namespace CEGUI
{
	class Window;
	class UVector2;
}
class EQCamera;
class Player;
class Avatar;
class ModelGcs;

class AvatarRTT
{
public:
	class RTTListener : Ogre::RenderTargetListener
	{
	public:
		RTTListener(){}
		~RTTListener(){}
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


public:
	AvatarRTT();
	virtual ~AvatarRTT();

	void		startRTT(float x, float y, float z);
	EQCamera*	getRTTCamera(){return mRTTCamera;}

	void		rotate(int degree,int axes);
	void		resetOrientation(void);
	void		rttScale(float scaleX,float scaleY,float scaleZ);
	void		zoom(float scale);
	void		resetScale();
	void		equipOn( uint slot, uint equipDisplayID, uint holdSlot );
	void		takeOff( uint slot );
	void		takeOffAll();
	void		syncPlayerEquip();

	void		changeSex(int sex);
	int			getSex();
	Avatar*		getAvatar(){return mAvatar;}
	void		updateModel();
	void		update();

	Avatar*			mAvatar;

	void setActive(bool active) { rttTex->getBuffer()->getRenderTarget()->setActive(active); }
	Ogre::String& getName(){ return mName; }

private:
	void		_createRTT(float x, float y, float z);
	void		_createRttGuiObject();
	void		_createRTTAvatar(float x, float y, float z);
private:
	Ogre::String			mName;
	ModelGcs*				mMainModel;
	Ogre::SceneNode*		mMainNode;
	EQCamera*				mRTTCamera;
	CEGUI::Window*			mWindow;
	RTTListener				mListener;
	Ogre::TexturePtr		rttTex;
	bool					mStart;
	Ogre::Vector3			mScale;
	Ogre::Vector3			mZoom;
};

#endif
