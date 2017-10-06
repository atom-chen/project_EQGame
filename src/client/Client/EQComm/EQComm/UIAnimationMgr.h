/******************************************************************************
* 作者: lf
* 时间: 2010-07-05
* 描述: UI动画
******************************************************************************/
#ifndef UIAnimationMgr_H
#define UIAnimationMgr_H

#include "EQMainPrerequisites.h"
#include "GameEventMgr.h"

namespace CEGUI
{
	class Window;
	class UVector2;
	class UDim;
};



struct UIAnimationInfo : public GameEventSet
{
	enum UIAnimationType
	{
		EUIA_NONE,
		EUIA_MOVE,		//移动
		EUIA_SCALE,		//缩放
		EUIA_FADE,		//淡入淡出
		EUIA_FLOATUP,	//向上漂
		EUIA_UPDOWN	,	//上下浮动
		EUIA_POPUP,		//弹出
	};

	//c++版回调
	static const std::string EventPlayBegin;
	static const std::string EventPlayEnd;
	//lua版回调
	std::string mLuaPlayBegin;
	std::string mLuaPlayEnd;

	UIAnimationInfo():mType(EUIA_NONE),mDelayTime(0),mPlayTime(0),mBeginPlay(false){}

	// 不用设置
	int		mX;
	int		mY;
	int		mWidth;
	int		mHeight;
	DWORD	mBeginTime;
	bool	mBeginPlay;

	// 可选设置
	UIAnimationType	mType;
	DWORD	mDelayTime;
	DWORD	mPlayTime;

	// EUIA_MOVE EUIA_POPUP
	int		mStartOffsetX;
	int		mStartOffsetY;
	int		mEndOffsetX;
	int		mEndOffsetY;

	// EUIA_SCALE EUIA_POPUP
	float	mStartScale;
	float	mEndScale;

	// EUIA_FADE
	float	mStartAlpha;
	float	mEndAlpha;

	// EUIA_FLOATUP EUIA_UPDOWN
	int		mRange;

	// EUIA_UPDOWN
	DWORD	mLoopTime;
	DWORD	mPauseTime;
};

class UIAnimationMgr : public Ogre::Singleton<UIAnimationMgr>
{
public:
	typedef std::list<UIAnimationInfo> AniInfoList;
	typedef std::map<std::string, AniInfoList> UIAnimationMap;

public:
	UIAnimationMgr();
	~UIAnimationMgr();

	void playAnimation(std::string& winname, UIAnimationInfo& info, bool upd=true);
	void stopAnimation(std::string& winname);
	void updateWinPos(std::string& winname);
	bool isPlaying(const std::string & winname);

	void update();

protected:
	UIAnimationMap	mAniMap;
};

int UDim2Absolute(const CEGUI::UDim& dim, bool woh); // woh: true-width / false-height

#endif
