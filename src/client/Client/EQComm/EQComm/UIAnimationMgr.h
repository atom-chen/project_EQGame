/******************************************************************************
* ����: lf
* ʱ��: 2010-07-05
* ����: UI����
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
		EUIA_MOVE,		//�ƶ�
		EUIA_SCALE,		//����
		EUIA_FADE,		//���뵭��
		EUIA_FLOATUP,	//����Ư
		EUIA_UPDOWN	,	//���¸���
		EUIA_POPUP,		//����
	};

	//c++��ص�
	static const std::string EventPlayBegin;
	static const std::string EventPlayEnd;
	//lua��ص�
	std::string mLuaPlayBegin;
	std::string mLuaPlayEnd;

	UIAnimationInfo():mType(EUIA_NONE),mDelayTime(0),mPlayTime(0),mBeginPlay(false){}

	// ��������
	int		mX;
	int		mY;
	int		mWidth;
	int		mHeight;
	DWORD	mBeginTime;
	bool	mBeginPlay;

	// ��ѡ����
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
