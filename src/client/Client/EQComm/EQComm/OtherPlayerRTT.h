/**********************************************************************
* 作者: xiaoMing
* 时间: 2010-08-10
* 描述: OtherPlayerRTT
*		其他玩家角色预览框
**********************************************************************/
#ifndef Other_Player_RTT_H
#define Other_Player_RTT_H

#include "EQOgreSys.h"

namespace CEGUI
{
	class Window;
	class UVector2;
}
class EQCamera;
class Player;
class Avatar;
class ModelGcs;
class RTTListener;

class OtherPlayerRTT
{
	class OtherRTTListener : Ogre::RenderTargetListener
	{
	public:
		OtherRTTListener(){}
		~OtherRTTListener(){}
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
	OtherPlayerRTT();
	~OtherPlayerRTT();
	void		startRTT(float x, float y, float z);
	EQCamera*	getRTTCamera(){return mRTTCamera;}

	void		rotate(int degree,int axes);
	void		rttScale(float scaleX,float scaleY,float scaleZ);
	void		equipOn( uint slot, uint equipDisplayID, uint holdSlot );
	void		takeOff( uint slot );
	void		takeOffAll();
	void		syncPlayerEquip(Player *p);
	void		syncPlayerEquip1(int equipArr[]);

	void		changeSex(int sex);
	Avatar*		getAvatar(){return mAvatar;}
	void		updateModel();

	Avatar*			mAvatar;

	void setActive(bool active) { rttTex->getBuffer()->getRenderTarget()->setActive(active); }
	Ogre::String& getName(){ return mName; }

private:
	void		_createRTT(float x, float y, float z);
	void		_createRttGuiObject();
	void		_createRTTAvatar(float x, float y, float z);

private:
	Ogre::String		mName;
	ModelGcs*			mMainModel;
	Ogre::SceneNode*	mMainNode;
	EQCamera*			mRTTCamera;
	CEGUI::Window*		mWindow;
	Ogre::TexturePtr	rttTex;
	OtherRTTListener	mListener;
	bool				mStart;
};

#endif