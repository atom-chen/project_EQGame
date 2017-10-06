#include "EQCommPCH.h"
#include "ConfigOption.h"
#include "EQWndProc.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "CommFunc.h"
#include "GUIManager.h"
#include "EQOIS.h"
#include "EQOgreSys.h"
#include "ScriptFunction.h"
#include "GameEventMgr.h"
#include "SystemGcs.h"
#include "ParticleMgr.h"
#include "EQVoiceMgr.h"

#define SYS_VOLUME "SysVolume"
#define BG_VOLUME "BgVolume"

//-----------------------------------------------------------------------
template<> ConfigOption* Ogre::Singleton<ConfigOption>::ms_Singleton = 0;
//-----------------------------------------------------------------------
ConfigOption::ConfigOption()
{
	SystemGcs::getSingleton().setVideoCardCapabilities(this);

	mStartName = "..\\config\\EQStart.cfg";
	if(sVariableSystem.LoadVariable(mStartName.c_str(), mStartMap) == false)
	{
		mIP = "10.35.12.224";
		mConsole = 0;
	}
	else
	{
		mIP.clear();
		mConsole = atoi(_getValue(mStartMap, "Console", "0"));
	}

	if(mConsole)
		::EQLogManager::getLogger().getConsole().open();
}
//-----------------------------------------------------------------------
ConfigOption::~ConfigOption()
{
}
//-----------------------------------------------------------------------
const char* ConfigOption::_getValue(VariableSystem::VARIABLE_MAP& map, const char* key, const char* def)
{
	VariableSystem::VARIABLE_MAP::iterator it = map.find(key);
	if(it == map.end())
		return def;
	else
		return it->second.vValue.c_str();
}
//-----------------------------------------------------------------------
void ConfigOption::load(const char* file)
{
	mCfgName = file;
	if(sVariableSystem.LoadVariable(mCfgName.c_str(), mCfgMap) == false)
	{
		mWidth = 800;
		mHeight = 600;
		mFullScreen = false;
		mShadow = 0;
		mAA = 0;
		mParticleQuality = 10;
		mSysVolume = 80;
		mBgVolume = 80;
	}
	else
	{
		mWidth = mHeight = mShadow = mAA = mParticleQuality = mSysVolume = mBgVolume = -1;
	}
}
//-----------------------------------------------------------------------
void ConfigOption::init()
{
	const Ogre::RenderSystemCapabilities* caps = EQOgreSys::getSingleton().getRoot()->getRenderSystem()->getCapabilities();
	if (!caps->hasCapability(Ogre::RSC_VERTEX_PROGRAM) || !(caps->hasCapability(Ogre::RSC_FRAGMENT_PROGRAM)))
	{
		LOG("Video card does not support vertex and fragment programs");
		mSM2 =false;
		mSM3 =false;
	}
	else
	{
		if (!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("arbfp1") &&
			!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0"))
		{
			LOG("Video card does not support shader model 2");
			mSM2 = false;
		}

		if (!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("arbfp1") &&
			!Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_3_0"))
		{
			LOG("Video card does not support shader model 3");
			mSM3 = false;
		}
	}

	changeParticleQuality(getParticleQuality());
}
//-----------------------------------------------------------------------
const char* ConfigOption::getIP()
{
	if(mIP.empty())
	{
		mIP = _getValue(mStartMap, "IP", "");
	}

	return mIP.c_str();
}
//-----------------------------------------------------------------------
int ConfigOption::getWidth()
{
	if(mWidth == -1)
	{
		mFullScreen = atoi(_getValue(mCfgMap, "FullScreen", "0")) == 1;
		if(mFullScreen)
			mWidth = GetSystemMetrics(SM_CXSCREEN);
		else
			mWidth = atoi(_getValue(mCfgMap, "WindowWidth", "640"));
	}

	return mWidth;
}
//-----------------------------------------------------------------------
int ConfigOption::getWidthWithBorder()
{
	return mWidth + GetSystemMetrics(SM_CXFRAME)*2 - 2;
}
//-----------------------------------------------------------------------
int ConfigOption::getHeight()
{
	if(mHeight == -1)
	{
		mFullScreen = atoi(_getValue(mCfgMap, "FullScreen", "0")) == 1;
		if(mFullScreen)
			mHeight = GetSystemMetrics(SM_CYSCREEN);
		else
			mHeight = atoi(_getValue(mCfgMap, "WindowHeigh", "480"));
	}

	return mHeight;
}
//-----------------------------------------------------------------------
int ConfigOption::getHeightWithBorder()
{
	return mHeight + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) + 2;
}
//-----------------------------------------------------------------------
int ConfigOption::getAA()
{
	if(mAA == 0)
	{
		Ogre::ConfigOptionMap& option = EQOgreSys::getSingleton().getRoot()->getRenderSystem()->getConfigOptions();
		Ogre::ConfigOptionMap::iterator it = option.find( "Anti aliasing" );
		Ogre::ConfigOption* optFSAA = &it->second;

		int type;
		for(uint i=0; i<optFSAA->possibleValues.size(); i++)
		{
			if(optFSAA->possibleValues[i] == "None")
				continue;

			if(optFSAA->possibleValues[i].find("NonMaskable") != std::string::npos)
				continue;

			sscanf(optFSAA->possibleValues[i].c_str(), "Level %d", &type);
			if(type > mAA)
				mAA = type;
		}
	}
	else if(mAA == -1)
	{
		mAA = atoi(_getValue(mCfgMap, "FSAA", "0"));

		Ogre::ConfigOptionMap& option = EQOgreSys::getSingleton().getRoot()->getRenderSystem()->getConfigOptions();
		Ogre::ConfigOptionMap::iterator it = option.find( "Anti aliasing" );
		Ogre::ConfigOption* optFSAA = &it->second;

		if(mAA == 0)
		{
			
		}
		else if(mAA == 1)
		{
			//不用这种模式
			mAA = 0;
		}
		else
		{
			Ogre::String str = "Level " + Ogre::StringConverter::toString(mAA);
			if(std::find(optFSAA->possibleValues.begin(), optFSAA->possibleValues.end(), str) != optFSAA->possibleValues.end())
			{
				
			}
			else
			{
				mAA = 0;
			}
		}
	}

	return mAA;
}
//-----------------------------------------------------------------------
int ConfigOption::getShadow()
{
	if(mShadow == -1)
	{
		mShadow = atoi(_getValue(mCfgMap, "ShadowTechnique", "0"));

		if(mSM2 == false)
			mShadow = 0;
	}

	return mShadow;
}
//-----------------------------------------------------------------------
int ConfigOption::getParticleQuality()
{
	if(mParticleQuality == -1)
	{
		mParticleQuality = atoi(_getValue(mCfgMap, "ParticleQuality", "1"));

		if(mParticleQuality!=10 && mParticleQuality!=5 && mParticleQuality!=1)
			mParticleQuality = 1;

		if(mSM2 == false)
			mParticleQuality = 1;
	}

	return mParticleQuality;
}
//-----------------------------------------------------------------------
uint ConfigOption::getSysVolume()
{
	if(mSysVolume == -1)
	{
		mSysVolume = atoi(_getValue(mCfgMap, SYS_VOLUME, "80"));
	}

	return (uint)mSysVolume;
}
//-----------------------------------------------------------------------
uint ConfigOption::getBgVolume()
{
	if(mBgVolume == -1)
	{
		mBgVolume = atoi(_getValue(mCfgMap, BG_VOLUME, "80"));
	}

	return (uint)mBgVolume;
}
//-----------------------------------------------------------------------
bool ConfigOption::setFullScreen()
{
	changeResolution( GetSystemMetrics(SM_CXSCREEN),  GetSystemMetrics(SM_CYSCREEN), false);
	mFullScreen = true;

	return true;
}
//-----------------------------------------------------------------------
bool ConfigOption::changeResolution(int width, int height, bool title)
{
	mFullScreen = false;
	mWidth = width;
	mHeight = height;

	int actWidth,actHeight;

	HWND wnd = EQWndProc::getSingleton().getHWnd();
	RECT rect;
	GetWindowRect(wnd, &rect);

	if(title)
	{
		actWidth = getWidthWithBorder();
		actHeight = getHeightWithBorder();

		LONG style = GetWindowLong(wnd, GWL_STYLE);
		style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		SetWindowLong(wnd,GWL_STYLE,style);

		rect.left = (GetSystemMetrics(SM_CXFULLSCREEN) - mWidth) / 2;
		rect.top = (GetSystemMetrics(SM_CYFULLSCREEN) - mHeight) / 2;
	}
	else
	{
		actWidth = mWidth;
		actHeight = mHeight;

		LONG style = GetWindowLong(wnd, GWL_STYLE);   
		style &= ~(WS_CAPTION);
		SetWindowLong(wnd,GWL_STYLE,style);

		rect.left = 0;
		rect.top = 0;
	}
	MoveWindow(wnd, rect.left, rect.top, actWidth, actHeight, TRUE);
	EQWndProc::getSingleton().setWndSize(width, height);

	EQOgreSys::getSingleton().getRenderWindow()->windowMovedOrResized();
	EQGameMgr::getSingleton().getMainCamera()->getOgreCam()->setAspectRatio(
		(Ogre::Real)EQOgreSys::getSingleton().getRenderWindow()->getWidth() / EQOgreSys::getSingleton().getRenderWindow()->getHeight());
	CGUIManager::getSingleton().WindowResized(EQOgreSys::getSingleton().getRenderWindow());

	EQOIS::getSingleton().getMouse()->getMouseState().width = EQWndProc::getSingleton().getWndWidth();
	EQOIS::getSingleton().getMouse()->getMouseState().height = EQWndProc::getSingleton().getWndHeight();

	GameEventMgr::getSingleton().fireEvent("CHANGE_RESOLUTION");

	EQOgreSys::getSingleton().notifyViewportSize(mWidth, mHeight);

	return true;
}
//-----------------------------------------------------------------------
bool ConfigOption::changeAntiAliasing(int type)
{
	mAA = type;

	Ogre::ConfigOptionMap& option = EQOgreSys::getSingleton().getRoot()->getRenderSystem()->getConfigOptions();
	Ogre::ConfigOptionMap::iterator it = option.find( "Anti aliasing" );
	Ogre::ConfigOption* optFSAA = &it->second;
	for(uint i=0; i<optFSAA->possibleValues.size(); i++)
	{
		LOG(optFSAA->possibleValues[i]);
	}
	Ogre::String str = "Level " + Ogre::StringConverter::toString(type);
	EQOgreSys::getSingleton().getRoot()->getRenderSystem()->setConfigOption("Anti aliasing", str);
	//EQOgreSys::getSingleton().getRoot()->getRenderSystem()->reinitialise();

	return true;
}
//-----------------------------------------------------------------------
bool ConfigOption::changeShadow(int type)
{
	mShadow = type;
	EQOgreSys::getSingleton().setShadow(type);
	return true;
}
//-----------------------------------------------------------------------
bool ConfigOption::changeParticleQuality(int quality)
{
	mParticleQuality = quality;
	ParticleMgr::getSingleton().setQuality(eParticleQuality(quality));
	return true;
}
//-----------------------------------------------------------------------
void ConfigOption::insertAAOption(const char* cbname)
{
	Ogre::ConfigOptionMap& option = EQOgreSys::getSingleton().getRoot()->getRenderSystem()->getConfigOptions();
	Ogre::ConfigOptionMap::iterator it = option.find( "Anti aliasing" );
	Ogre::ConfigOption* optFSAA = &it->second;

	int type;
	for(uint i=0; i<optFSAA->possibleValues.size(); i++)
	{
		if(optFSAA->possibleValues[i] == "None")
			continue;

		if(optFSAA->possibleValues[i].find("NonMaskable") != std::string::npos)
			continue;

		sscanf(optFSAA->possibleValues[i].c_str(), "Level %d", &type);
		Ogre::String str = Ogre::StringConverter::toString(type) + "x";
		createComboBoxTextItem(cbname, str.c_str(), type,"");
	}
}
//-----------------------------------------------------------------------
void ConfigOption::save()
{
	mCfgMap["FullScreen"].bTemp = FALSE;
	mCfgMap["FullScreen"].vValue = mFullScreen? "1" : "0";

	mCfgMap["WindowWidth"].bTemp = FALSE;
	mCfgMap["WindowWidth"].vValue = Ogre::StringConverter::toString(mWidth).c_str();

	mCfgMap["WindowHeigh"].bTemp = FALSE;
	mCfgMap["WindowHeigh"].vValue = Ogre::StringConverter::toString(mHeight).c_str();

	mCfgMap["FSAA"].bTemp = FALSE;
	mCfgMap["FSAA"].vValue = Ogre::StringConverter::toString(mAA).c_str();

	mCfgMap["ShadowTechnique"].bTemp = FALSE;
	mCfgMap["ShadowTechnique"].vValue = Ogre::StringConverter::toString(mShadow).c_str();

	mCfgMap["ParticleQuality"].bTemp = FALSE;
	mCfgMap["ParticleQuality"].vValue = Ogre::StringConverter::toString(mParticleQuality).c_str();

	mCfgMap[SYS_VOLUME].bTemp = FALSE;
	mCfgMap[SYS_VOLUME].vValue = Ogre::StringConverter::toString(EQVoiceMgr::getSingleton().getVolume(EQVOICE_VOLUME_SYS)).c_str();

	mCfgMap[BG_VOLUME].bTemp = FALSE;
	mCfgMap[BG_VOLUME].vValue = Ogre::StringConverter::toString(EQVoiceMgr::getSingleton().getVolume(EQVOICE_VOLUME_BG)).c_str();


	sVariableSystem.SaveVariable(mCfgName.c_str(), mCfgMap);
}
//-----------------------------------------------------------------------