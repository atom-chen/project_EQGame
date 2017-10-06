/******************************************************************************
* 作者: lf
* 时间: 2010-4-9
* 描述: 环境光管理
******************************************************************************/
#ifndef AmbientMgr_H
#define AmbientMgr_H

#include "EQMainPrerequisites.h"

class AmbientMgr : public Ogre::Singleton<AmbientMgr>
{
public:

	AmbientMgr();
	~AmbientMgr();

private:
	bool	mBegin;
	DWORD	mBeginTime;
	Ogre::SceneManager* mSceneMgr;

	void	_refreshFogSkybox(bool daynight);

public:
	DWORD	mChangetime;
	bool	mDayNight; // true==day   false==night
	bool	mAutoTurnOnOff;
	Ogre::ColourValue mDayAmbient;
	Ogre::ColourValue mDayDiffuse;
	Ogre::ColourValue mDaySpecular;
	Ogre::Vector3	  mDayMainDir;
	Ogre::ColourValue mDayFogColor;
	Ogre::Vector4	  mDayFogParam;
	std::string       mDaySkybox;
	Ogre::ColourValue mNightAmbient;
	Ogre::ColourValue mNightDiffuse;
	Ogre::ColourValue mNightSpecular;
	Ogre::Vector3	  mNightMainDir;
	Ogre::ColourValue mNightFogColor;
	Ogre::Vector4	  mNightFogParam;
	std::string       mNightSkybox;
	float             mContrast;

public:
	void init(Ogre::SceneManager* sm);

	Ogre::Light* createMainLight();
	Ogre::Light* getMainLight();
	
	void setAmbientLight(Ogre::ColourValue color);
	const Ogre::ColourValue& getAmbientLight();

	void notifyZoneLightDirty(uint zone);

	bool getDayNight() { return mBegin? !mDayNight : mDayNight; }
	void switchDayNight(bool immediately=false);
	void changeDayNight(bool daynight, bool immediately=false);

	void update();
	void refresh();

	void setAutoTurnOnOffLight(bool a);
};


#endif
