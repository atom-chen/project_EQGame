#include "EQCommPCH.h"

#include "UIAnimationMgr.h"
#include "DynamicUIMgr.h"
#include "CEGUI.h"
#include "EQGameMgr.h"
#include "EQCamera.h"

#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"

int UDim2Absolute(const CEGUI::UDim& dim, bool woh)
{
	Ogre::Camera* camera = EQGameMgr::getSingleton().getMainCamera()->getOgreCam();
	if(woh)
		return dim.asAbsolute(camera->getViewport()->getActualWidth());
	else
		return dim.asAbsolute(camera->getViewport()->getActualHeight());
}

//-----------------------------------------------------------------------
template<> UIAnimationMgr* Ogre::Singleton<UIAnimationMgr>::ms_Singleton = 0;

const std::string UIAnimationInfo::EventPlayBegin("PlayBegin");
const std::string UIAnimationInfo::EventPlayEnd("PlayEnd");
//-----------------------------------------------------------------------
UIAnimationMgr::UIAnimationMgr()
{

}
//-----------------------------------------------------------------------
UIAnimationMgr::~UIAnimationMgr()
{

}
//-----------------------------------------------------------------------
void UIAnimationMgr::playAnimation(std::string& winname, UIAnimationInfo& info, bool upd)
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingletonPtr()->getWindow(winname);
	if(!win)
		return;
	
	if(info.mType == UIAnimationInfo::EUIA_NONE)
		return;

	if(info.mPlayTime == 0 && info.mType < UIAnimationInfo::EUIA_UPDOWN)
	{
		assert(0 && "Animation type is not recyclable");
		return;
	}

	info.mBeginTime = GetTickCount();
	info.mX = UDim2Absolute(win->getXPosition(), true);
	info.mY = UDim2Absolute(win->getYPosition(), false);
	info.mWidth = UDim2Absolute(win->getWidth(), true);
	info.mHeight = UDim2Absolute(win->getHeight(), false);

	UIAnimationMap::iterator it = mAniMap.find(winname);
	if(it != mAniMap.end())
	{
		bool found=false;

		AniInfoList::iterator il;
		for(il=it->second.begin(); il!=it->second.end(); il++)
		{
			if((*il).mType == info.mType)
			{
				it->second.erase(il);
				found = true;
				break;
			}
		}

		it->second.push_back(info);
	}
	else
	{
		AniInfoList l;
		l.push_back(info);
		mAniMap.insert(std::make_pair(winname, l));
	}

	if(upd)
		update();
}
//-----------------------------------------------------------------------
void UIAnimationMgr::stopAnimation(std::string& winname)
{
	UIAnimationMap::iterator it = mAniMap.find(winname);
	if(it != mAniMap.end())
	{
		AniInfoList::iterator il;
		for(il=it->second.begin(); il!=it->second.end(); il++)
		{
			il->fireEvent(UIAnimationInfo::EventPlayEnd);
			if(!il->mLuaPlayEnd.empty())
				luabind::call_function<void>(CLuaManager::getSingleton().getL(), il->mLuaPlayEnd.c_str(), winname);
		}

		it->second.clear();
		mAniMap.erase(it);
	}
}
//-----------------------------------------------------------------------
void UIAnimationMgr::updateWinPos(std::string& winname)
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingletonPtr()->getWindow(winname);
	if(!win)
		return;

	UIAnimationMap::iterator im = mAniMap.find(winname);
	if(im != mAniMap.end())
	{
		AniInfoList::iterator il;
		for(il=im->second.begin(); il!=im->second.end(); il++)
		{
			il->mX = UDim2Absolute(win->getXPosition(), true);
			il->mY = UDim2Absolute(win->getYPosition(), false);
		}
	}
	update();
}
//-----------------------------------------------------------------------
void UIAnimationMgr::update()
{
	DWORD now = GetTickCount();

	UIAnimationMap::iterator im;
	AniInfoList::iterator il;
	bool end;
	for(im=mAniMap.begin(); im!=mAniMap.end();)
	{
		CEGUI::Window* win = CEGUI::WindowManager::getSingletonPtr()->getWindow(im->first);
		if(!win)
		{
			il=im->second.erase(il);
			continue;
		}

		for(il=im->second.begin(); il!=im->second.end(); )
		{
			end = false;
			UIAnimationInfo* info = &(*il);
			DWORD begintime = info->mBeginTime + info->mDelayTime;
			if(info->mPlayTime != 0)
			{
				if(now < begintime)
				{
					//未开始
					il++;
					continue;
				}
				else if(now - begintime >= info->mPlayTime)
				{
					//结束
					end = true;
				}
				else
				{
					if(info->mBeginPlay == false)
					{
						//刚开始
						il->fireEvent(UIAnimationInfo::EventPlayBegin);
						if(!il->mLuaPlayBegin.empty())
							luabind::call_function<void>(CLuaManager::getSingleton().getL(), il->mLuaPlayBegin.c_str(), im->first);

						info->mBeginPlay = true;
					}
				}
			}

			float step = end ? 1.0f : float(now - begintime)/info->mPlayTime;
			switch (info->mType)
			{
			case UIAnimationInfo::EUIA_MOVE:
				{
					int x = info->mX + info->mStartOffsetX + (info->mEndOffsetX - info->mStartOffsetX) * step;
					int y = info->mY + info->mStartOffsetY + (info->mEndOffsetY - info->mStartOffsetY) * step;
					win->setPosition(CEGUI::UVector2( CEGUI::UDim(0, x), CEGUI::UDim(0, y) ));
				}
				break;
			case UIAnimationInfo::EUIA_SCALE:
				{
					float scale = info->mStartScale + (info->mEndScale - info->mStartScale) * step;
					win->setSize(CEGUI::UVector2( CEGUI::UDim(0, info->mWidth*scale), CEGUI::UDim(0, info->mHeight*scale) ));
				}
				break;
			case UIAnimationInfo::EUIA_FADE:
				{
					float alpha = info->mStartAlpha + (info->mEndAlpha - info->mStartAlpha) * step;
					win->setAlpha(alpha);
				}
				break;
			case UIAnimationInfo::EUIA_FLOATUP:
				{
					int x = info->mX;
					int y = info->mY - info->mRange * step;
					win->setPosition(CEGUI::UVector2( CEGUI::UDim(0, x), CEGUI::UDim(0, y) ));
				}
				break;
			case UIAnimationInfo::EUIA_UPDOWN:
				{
					DWORD curtime = (now - begintime)%(info->mLoopTime + info->mPauseTime);
					if(curtime < info->mLoopTime)
					{
						float section = info->mLoopTime/4;
						int y;
						if(curtime <= section)
							y = info->mY - info->mRange * curtime/section;	
						else if(curtime <= section*2)
							y = info->mY - info->mRange + info->mRange * (curtime - section)/section;
						else if(curtime <= section*3)
							y = info->mY + info->mRange * (curtime - section*2)/section;	
						else
							y = info->mY + info->mRange - info->mRange * (curtime - section*3)/section;

						win->setYPosition( CEGUI::UDim(0, y) );
					}
					else
					{
						win->setYPosition( CEGUI::UDim(0, info->mY) );
					}
				}
				break;
			case UIAnimationInfo::EUIA_POPUP:
				{
					int x = info->mX + info->mStartOffsetX + (info->mEndOffsetX - info->mStartOffsetX) * step;
					int y = info->mY + info->mStartOffsetY + (info->mEndOffsetY - info->mStartOffsetY) * step;
					win->setPosition(CEGUI::UVector2( CEGUI::UDim(0, x), CEGUI::UDim(0, y) ));

					float scale = info->mStartScale + (info->mEndScale - info->mStartScale) * step;
					win->setSize(CEGUI::UVector2( CEGUI::UDim(0, info->mWidth*scale), CEGUI::UDim(0, info->mHeight*scale) ));
				}
				break;
			}

			if(end)
			{
				il->fireEvent(UIAnimationInfo::EventPlayEnd);
				if(!il->mLuaPlayEnd.empty())
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), il->mLuaPlayEnd.c_str(), im->first);
				il=im->second.erase(il);
			}
			else
			{
				il++;
			}
		}

		im++;
	}

	for(im=mAniMap.begin(); im!=mAniMap.end();)
	{
		if(im->second.size() == 0)
		{
			im = mAniMap.erase(im);
			if(im == mAniMap.end())
				break;
			else
				continue;
		}

		im++;
	}
}
//-----------------------------------------------------------------------
bool UIAnimationMgr::isPlaying(const std::string & winname)
{
	return mAniMap.find(winname) != mAniMap.end();
}