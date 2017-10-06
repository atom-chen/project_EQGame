/**********************************************************************
* 作者: 何展然
* 时间: 2010-12-10
* 描述: 美容院玩家角色头部预览
**********************************************************************/
#ifndef Beauty_Head_RTT_H
#define Beauty_Head_RTT_H

#include "EQOgreSys.h"
#include "CEGUI.h"

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

class BeautyHeadRTT
{
	class BeautyHeadRTTListener : Ogre::RenderTargetListener
	{
	public:
		BeautyHeadRTTListener(){}
		~BeautyHeadRTTListener(){}
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
	BeautyHeadRTT();
	~BeautyHeadRTT();
	void		startRTT(float x, float y, float z);
	EQCamera*	getRTTCamera(){return mRTTCamera;}

	void		rotate(int degree,int axes);
	void		resetOrientation(void);
	void		rttScale(float scaleX,float scaleY,float scaleZ);
	void		equipOn( uint slot, uint equipDisplayID, uint holdSlot );
	void		takeOff( uint slot );
	void		takeOffAll();
	void		syncPlayerEquip(Player *p);
	void		syncPlayerEquip1(int equipArr[]);

	void		changeSex(int sex);
	Avatar*		getAvatar(){return mAvatar;}
	void		updateModel();

	Ogre::TexturePtr& getOgreTexturePtr(void){return mRttTexture;}
	Ogre::RenderTexture* getOgreRenderTexture(void){return mRenderTexture;}
	//void		createOgreTexturePtr(int flag);

private:
	void		_createRTT(float x, float y, float z);
	void		_createRttGuiObject();
	void		_createRTTAvatar(float x, float y, float z);

private:
	ModelGcs*		mMainModel;
	Ogre::SceneNode*mMainNode;
	EQCamera		*mRTTCamera;
	CEGUI::Window	* mWindow;
	Ogre::TexturePtr mRttTexture;
	BeautyHeadRTTListener	mListener;
	Avatar*			mAvatar;
	Ogre::RenderTexture*	mRenderTexture;
	CEGUI::Texture*	mTexture;
	bool mStart;
};

#endif	//	end of guard Beauty_Head_RTT_H