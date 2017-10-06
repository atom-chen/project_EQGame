/**********************************************************************
* ����: ��չȻ
* ʱ��: 2010-12-06
* ����: ����Ժ��ҽ�ɫԤ����
**********************************************************************/
#ifndef Beauty_RTT_H
#define Beauty_RTT_H

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

class BeautyRTT
{
	class BeautyRTTListener : Ogre::RenderTargetListener
	{
	public:
		BeautyRTTListener(){}
		~BeautyRTTListener(){}
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
	BeautyRTT();
	~BeautyRTT();
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
	BeautyRTTListener	mListener;
	bool				mStart;
};

#endif	//	end of guard Beauty_RTT_H