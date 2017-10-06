/********************************************************************
* 作者: lf
* 时间: 2010-12-14
* 描述: 游戏配置
**********************************************************************/
#ifndef ConfigOption_H
#define ConfigOption_H

#include "EQMainPrerequisites.h"
#include "VideoCardCapabilities.h"
#include "Variable.h"

class ConfigOption : public VideoCardCapabilities
{
public:
	ConfigOption();
	~ConfigOption(void);

	void load(const char* file);
	void init();
	void insertAAOption(const char* cbname);
	void save();

	bool setFullScreen();
	bool changeResolution(int width, int height, bool title);
	bool changeAntiAliasing(int type);
	bool changeShadow(int type);
	bool changeParticleQuality(int quality);

	bool isFullScreen() { return mFullScreen; }
	int  getWidth();
	int  getWidthWithBorder();
	int  getHeight();
	int  getHeightWithBorder();
	int  getAA();
	int  getShadow();
	uint getSysVolume();
	uint getBgVolume();
	const char* getIP();
	int  getParticleQuality();
private:
	const char* _getValue(VariableSystem::VARIABLE_MAP& map, const char* key, const char* def);
protected:
	bool mFullScreen;
	int  mWidth,mHeight;
	int  mAA;
	int  mShadow;
	int  mParticleQuality;
	int  mSysVolume;
	int  mBgVolume;
	std::string	mCfgName;
	VariableSystem::VARIABLE_MAP mCfgMap;

	std::string mIP;
	int			mConsole;
	std::string	mStartName;
	VariableSystem::VARIABLE_MAP mStartMap;
};

#endif