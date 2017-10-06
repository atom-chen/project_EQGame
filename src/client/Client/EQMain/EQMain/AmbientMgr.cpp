#include "EQMainPCH.h"

#include "AmbientMgr.h"
#include "SystemGcs.h"
#include "ZoneLightMgr.h"
#include "SceneManager.h"

#define MAINLIGHT "MainLight"

//-----------------------------------------------------------------------
template<> AmbientMgr* Ogre::Singleton<AmbientMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
AmbientMgr::AmbientMgr()
{
	mBegin = false;
	mChangetime = 0;
	mDayNight = true;
	mDayAmbient = Ogre::ColourValue::ZERO;
	mDayDiffuse = Ogre::ColourValue::ZERO;
	mNightAmbient = Ogre::ColourValue::ZERO;
	mNightDiffuse = Ogre::ColourValue::ZERO;
	mAutoTurnOnOff = false;
	mContrast = 1.0f;
}
//-----------------------------------------------------------------------
AmbientMgr::~AmbientMgr()
{
	
}
//-----------------------------------------------------------------------
void AmbientMgr::init(Ogre::SceneManager* sm)
{
	mSceneMgr = sm;
}
//-----------------------------------------------------------------------
Ogre::Light* AmbientMgr::createMainLight()
{
	ZoneLight* zl = ZoneLightMgr::getSingleton().createZoneLight(ZoneLight::ZLT_ACTIVE, MAINLIGHT);
	zl->createLight();

	return zl->getLight();
}
//-----------------------------------------------------------------------
Ogre::Light* AmbientMgr::getMainLight()
{
	ZoneLight* light = ZoneLightMgr::getSingleton().getZoneLightByName(MAINLIGHT);
	if(!light)
		return NULL;

	return light->getLight();
}
//-----------------------------------------------------------------------
void AmbientMgr::notifyZoneLightDirty(uint zone)
{
	SystemGcs::getSingleton().getSceneManager()->_notifyLightsDirty();
	ZoneLightMgr::getSingleton()._notifyZoneLightsDirty(zone);
}
//-----------------------------------------------------------------------
void AmbientMgr::setAmbientLight(Ogre::ColourValue color)
{
	SystemGcs::getSingleton().getSceneManager()->setAmbientLight(color);
}
//-----------------------------------------------------------------------
const Ogre::ColourValue& AmbientMgr::getAmbientLight()
{
	return SystemGcs::getSingleton().getSceneManager()->getAmbientLight();
}
//-----------------------------------------------------------------------
void AmbientMgr::switchDayNight(bool immediately)
{
	if(mBegin)
		return;

	mBegin = true;
	mBeginTime = GetTickCount();
}
//-----------------------------------------------------------------------
void AmbientMgr::changeDayNight(bool daynight, bool immediately)
{
	if(immediately)
	{
		mBegin = false;
		mDayNight = daynight;
		refresh();
		return;
	}
	
	if(mBegin) return;
	else if(mDayNight==daynight) return;

	switchDayNight();
}
//-----------------------------------------------------------------------
void AmbientMgr::update()
{
	if(!mBegin)
		return;

	static DWORD cur = 0;
	if(GetTickCount() - cur < 40)
		return;
	cur = GetTickCount();

	static bool changed=false;
	if(cur - mBeginTime < mChangetime)
	{
		float per = float(cur-mBeginTime)/mChangetime;
		
		if(mDayNight)
		{
			setAmbientLight(mDayAmbient - per * (mDayAmbient - mNightAmbient));
			getMainLight()->setDiffuseColour(mDayDiffuse - per * (mDayDiffuse - mNightDiffuse));
			getMainLight()->setSpecularColour(mDaySpecular - per * (mDaySpecular - mNightSpecular));
		}
		else
		{
			setAmbientLight(mNightAmbient + per * (mDayAmbient - mNightAmbient));
			getMainLight()->setDiffuseColour(mNightDiffuse + per * (mDayDiffuse - mNightDiffuse));
			getMainLight()->setSpecularColour(mNightSpecular + per * (mDaySpecular - mNightSpecular));
		}

		//穿透光也要平滑过渡
		ZoneLightMgr::getSingleton().smoothDayNightChange(per);

		if(!changed && (cur - mBeginTime >= mChangetime/2))
		{
			if(mAutoTurnOnOff)
				EQSceneManager::getSingleton().turnAllLight(mDayNight);

			_refreshFogSkybox(!mDayNight);

			changed = true;
		}
	}
	else
	{
		mBegin = false;
		mDayNight = !mDayNight;
		refresh();
		ZoneLightMgr::getSingleton().smoothDayNightChange(1.0f);

		changed = false;
	}

	//if(EQSceneManager::getSingleton().getHouseScene()->getWatchMode() != HW_MODE_HIDE)
		notifyZoneLightDirty(0);
}
//-----------------------------------------------------------------------
void AmbientMgr::refresh()
{
	setAmbientLight(getDayNight()? mDayAmbient : mNightAmbient);
	getMainLight()->setDiffuseColour(getDayNight()? mDayDiffuse : mNightDiffuse);
	getMainLight()->setSpecularColour(getDayNight()? mDaySpecular : mNightSpecular);
	getMainLight()->setDirection(getDayNight()? mDayMainDir : mNightMainDir);
	_refreshFogSkybox(getDayNight());
}
//-----------------------------------------------------------------------
void AmbientMgr::_refreshFogSkybox(bool daynight)
{
	Ogre::ColourValue col = daynight ? mDayFogColor : mNightFogColor;
	Ogre::Vector4 fog = daynight ? mDayFogParam : mNightFogParam;
	mSceneMgr->setFog(Ogre::FogMode(int(fog.x)), col, fog.w, fog.y, fog.z);

	std::string skybox = daynight ? mDaySkybox : mNightSkybox;
	if(skybox.empty())
	{
		mSceneMgr->setSkyBox(false, "");
	}
	else
	{
		mSceneMgr->setSkyBox(true, skybox, 100);
		EQSceneManager::getSingleton().changeSkybox(skybox);
	}
}
//-----------------------------------------------------------------------
void AmbientMgr::setAutoTurnOnOffLight(bool a)
{
	mAutoTurnOnOff = a;

	EQSceneManager::getSingleton().turnAllLight(!getDayNight());
}
//-----------------------------------------------------------------------