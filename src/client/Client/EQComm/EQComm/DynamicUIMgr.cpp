#include "EQCommPCH.h"

#include "DynamicUIMgr.h"
#include "CEGUI.h"

#include "EQGameMgr.h"
#include "EQCamera.h"
#include "GameObj.h"
#include "Player.h"
#include "NPC.h"
#include "ObjMgr.h"
#include "UIAnimationMgr.h"
#include "CommonHelper.h"
#include "LuaManager.h"
#include "MsgSystem.h"
#include "luabind/luabind/luabind.hpp"
#include "CEGUIEx/CEGUIMultiLineBox.h"

//-----------------------------------------------------------------------
template<> DynamicUIMgr* Ogre::Singleton<DynamicUIMgr>::ms_Singleton = 0;

const std::string DynamicUIInfo::EventTimeOut("TimeOut");
//-----------------------------------------------------------------------
DynamicUIMgr::DynamicUIMgr():
mCounter(0)
{

}
//-----------------------------------------------------------------------
DynamicUIMgr::~DynamicUIMgr()
{
	DynamicUIMap::iterator it;
	for(it=mUIMap.begin(); it!=mUIMap.end(); it++)
		CEGUI::WindowManager::getSingleton().destroyWindow(it->second.winname);

	mUIMap.clear();
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createCommon(DynamicUIInfo& info)
{
	DynamicUIMap::iterator it = mUIMap.find(info.winname);
	if(it == mUIMap.end())
	{
		info.time = GetTickCount();
		mUIMap.insert(std::make_pair(info.winname, info));
	}
	else
	{
		it->second.time = GetTickCount();
	}

	return info.winname;
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createChatPaoPao(GameObj* obj, std::string& winname, const char* text)
{
	std::string objname,titlename;
	ObjMgr::GAMEOBJ_TYPE objtype = ObjMgr::getSingleton().getObjType(obj);
	if(objtype == ObjMgr::GOT_MAINPLAYER || objtype == ObjMgr::GOT_PLAYER)
	{
		objname = ((Player*)obj)->getAccount();
		titlename = ((Player*)obj)->getName();
	}
	else if(objtype == ObjMgr::GOT_NPC)
		objname = titlename = obj->getName();
	else
		return "";

	int x,y;
	Ogre::Vector3 headup = obj->getPos() + Ogre::Vector3(0,obj->getModelHeight(),0);
	if(!CommonHelper::Vector3ToScreenPoint(headup, x, y))
		return "";

	winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "DynamicUI_CreateChatPaoPao", winname, titlename, text);
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetChatPaoPaoPos", winname, x, y);

	DynamicUIMap::iterator it = mUIMap.find(winname);
	if(it == mUIMap.end())
	{
		DynamicUIInfo info;
		info.type = EDUI_CHATPAOPAO;
		info.winname = winname;
		info.time = GetTickCount();
		info.life = 7000;
		info.objname = objname;
		info.objtype = objtype;
		info.autoRemove = true;

		mUIMap.insert(std::make_pair(winname, info));
	}
	else
	{
		it->second.time = GetTickCount();
		it->second.life = 7000;
	}

	UIAnimationInfo info1;
 	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(winname.c_str());
 	info1.mType = UIAnimationInfo::EUIA_POPUP;
  	info1.mPlayTime = 200;
  	info1.mStartOffsetX = UDim2Absolute(win->getWidth(), true)/2;
  	info1.mStartOffsetY = UDim2Absolute(win->getHeight(), false)/2;
  	info1.mEndOffsetX = 0;
  	info1.mEndOffsetY = 0;
 	info1.mStartScale = 0;
 	info1.mEndScale = 1;
	UIAnimationMgr::getSingleton().playAnimation(winname, info1, false);

	UIAnimationInfo info2;
	info2.mType = UIAnimationInfo::EUIA_FADE;
	info2.mDelayTime = 5000;
	info2.mPlayTime = 2000;
	info2.mStartAlpha = 1;
	info2.mEndAlpha = 0;
	UIAnimationMgr::getSingleton().playAnimation(winname, info2);

	return winname;
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createHeadupBigExpression( GameObj* obj,std::string& winname,int id )
{
	std::string objname,titlename;
	ObjMgr::GAMEOBJ_TYPE objtype = ObjMgr::getSingleton().getObjType(obj);
	if(objtype == ObjMgr::GOT_MAINPLAYER || objtype == ObjMgr::GOT_PLAYER)
	{
		objname = ((Player*)obj)->getAccount();
		titlename = ((Player*)obj)->getName();
	}
	else
		return "";

	int x,y;
	Ogre::Vector3 headup = obj->getPos() + Ogre::Vector3(0,obj->getModelHeight(),0);
	if(!CommonHelper::Vector3ToScreenPoint(headup, x, y))
		return "";

	winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "BigExpression_CreateBigExpressionWin", winname,id);
	if (((Player*)obj)->getChatPaoPaoIsVisible())
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "BigExpression_SetBigExpressionWinPos", winname, x, y,true);
	else
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "BigExpression_SetBigExpressionWinPos", winname, x, y,false);

	DynamicUIMap::iterator it = mUIMap.find(winname);
	if(it == mUIMap.end())
	{
		DynamicUIInfo info;
		info.type = EDUI_BIGEXPRESSION;
		info.winname = winname;
		info.time = GetTickCount();
		info.life = 7000;
		info.objname = objname;
		info.objtype = objtype;
		info.autoRemove = true;

		mUIMap.insert(std::make_pair(winname, info));
	}
	else
	{
		it->second.time = GetTickCount();
		it->second.life = 7000;
	}

	UIAnimationInfo info1;
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(winname.c_str());
	info1.mType = UIAnimationInfo::EUIA_POPUP;
	info1.mPlayTime = 200;
	info1.mStartOffsetX = UDim2Absolute(win->getWidth(), true)/2;
	info1.mStartOffsetY = UDim2Absolute(win->getHeight(), false)/2;
	info1.mEndOffsetX = 0;
	info1.mEndOffsetY = 0;
	info1.mStartScale = 0;
	info1.mEndScale = 1;
	UIAnimationMgr::getSingleton().playAnimation(winname, info1, false);

	UIAnimationInfo info2;
	info2.mType = UIAnimationInfo::EUIA_FADE;
	info2.mDelayTime = 5000;
	info2.mPlayTime = 2000;
	info2.mStartAlpha = 1;
	info2.mEndAlpha = 0;
	UIAnimationMgr::getSingleton().playAnimation(winname, info2);

	return winname;
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createNumberPaoPao(GameObj* obj, NumberPaoPaoType type, int num)
{
	std::string objname;
	ObjMgr::GAMEOBJ_TYPE objtype = ObjMgr::getSingleton().getObjType(obj);
	if(objtype == ObjMgr::GOT_MAINPLAYER || objtype == ObjMgr::GOT_PLAYER)
		objname = ((Player*)obj)->getAccount();
	else if(objtype == ObjMgr::GOT_NPC)
		objname = obj->getName();
	else
		return "";

	std::string winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "DynamicUI_CreateNumberPaoPao", int(type), num);
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	win->setVisible(false);

	DynamicUIInfo info;
	info.type = EDUI_NUMBERPAOPAO;
	info.winname = winname;
	info.life = 3000;
	info.objname = objname;
	info.objtype = objtype;
	info.autoDel = true;

	mNumberPaoPaoBuffer.push(info);

	return winname;
}
//-----------------------------------------------------------------------
bool DynamicUIMgr::_popNumberPaoPao()
{
	if(mNumberPaoPaoBuffer.empty())
		return false;

	DynamicUIInfo info = mNumberPaoPaoBuffer.front();
	mNumberPaoPaoBuffer.pop();
	info.time = GetTickCount();
	mUIMap.insert(std::make_pair(info.winname, info));

	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(info.winname);
	if(!win)
		return true;
	win->setVisible(true);

	GameObj* obj = ObjMgr::getSingleton().findObj((ObjMgr::GAMEOBJ_TYPE)info.objtype, info.objname);
	if(!obj)
		return true;

	int x,y;
	Ogre::Vector3 headup = obj->getPos() + Ogre::Vector3(0,obj->getModelHeight(),0);
	if(!CommonHelper::Vector3ToScreenPoint(headup, x, y))
		return true;
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetNumberPaoPaoPos", info.winname, x, y);

	UIAnimationInfo ani;
	ani.mType = UIAnimationInfo::EUIA_FLOATUP;
	ani.mPlayTime = 3000;
	ani.mRange = 50;
	UIAnimationMgr::getSingleton().playAnimation(info.winname, ani);	

	return true;
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createHeadup(GameObj* obj, int type)
{
	std::string objname,headupname;
	ObjMgr::GAMEOBJ_TYPE objtype = ObjMgr::getSingleton().getObjType(obj);
	if(objtype == ObjMgr::GOT_MAINPLAYER || objtype == ObjMgr::GOT_PLAYER)
	{
		objname = ((Player*)obj)->getAccount();
		headupname = ((Player*)obj)->getName();
	}
	else if(objtype == ObjMgr::GOT_NPC)
	{
		objname = headupname = ((NPC*)obj)->getName();
	}
	else
		return "";

	std::string winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "DynamicUI_CreateHeadup", headupname, type);

	int x,y;
	Ogre::Vector3 headup = obj->getPos() + Ogre::Vector3(0,obj->getModelHeight(),0);
	if(!CommonHelper::Vector3ToScreenPoint(headup, x, y))
		x = y = -10000;
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetHeadupPos", winname, x, y);

	DynamicUIMap::iterator it = mUIMap.find(winname);
	if(it == mUIMap.end())
	{
		DynamicUIInfo info;
		info.type = EDUI_HEADUP;
		info.winname = winname;
		info.time = GetTickCount();
		info.objname = objname;
		info.objtype = objtype;

		mUIMap.insert(std::make_pair(winname, info));
	}
	else
	{
		it->second.time = GetTickCount();
	}

	return winname;
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createCircleMenuDynamicInfo(GameObj*obj,int type, std::string winName)
{
	std::string objname;
	ObjMgr::GAMEOBJ_TYPE objtype = ObjMgr::getSingleton().getObjType(obj);
	if(objtype == ObjMgr::GOT_PLAYER)
	{
		objname = ((Player*)obj)->getAccount();
	}
	else
		return "";

	std::string winname = "circleMenu";

	int x,y;
	Ogre::Vector3 headup = obj->getPos() + Ogre::Vector3(0,obj->getModelHeight(),0);
	if(!CommonHelper::Vector3ToScreenPoint(headup, x, y))
		return "";
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "setDisplayPos", winname, x, y);

	DynamicUIMap::iterator it = mUIMap.find(winname);
	if(it != mUIMap.end())
	{
		mUIMap.erase(it);
	}

	DynamicUIInfo info;
	info.type = EDUI_CIRCLEMENU;
	info.winname = winname;
	info.time = GetTickCount();
	info.objname = objname;
	info.objtype = objtype;
	mUIMap.insert(std::make_pair(winname, info));

	return winname;
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createFarmPaoPao(int index,int x1, int y1, int z, const char* content, const char* set, const char* image)
{
	int x,y;
	//
	Ogre::Vector3 pos(x1,y1,z);

	if(!CommonHelper::Vector3ToScreenPoint(pos, x, y))
		return "";
	//创建泡泡
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	if (strcmp( set,"") == 0)
	{
		std::string winname1 = "DynamicUI_Farm_";
		std::string winname = winname1 + Ogre::StringConverter::toString(index).c_str();
		winMgr.destroyWindow(winname);
		CEGUI::MultiLineBox* farmPaoPao = static_cast<CEGUI::MultiLineBox*>(winMgr.createWindow("EQHomeLook/FarmPaoPao",winname));
		//设置泡泡属性
		winMgr.getWindow("FarmmainUI")->addChildWindow(farmPaoPao);
		farmPaoPao->setMinSize(CEGUI::UVector2(CEGUI::UDim(0,50),CEGUI::UDim(0,15)));
		farmPaoPao->setWidth(CEGUI::UDim(0,92)); //6个汉字宽
		farmPaoPao->setMousePassThroughEnabled(true);
		//设置泡泡文字
		CEGUI::Font* font = CEGUI::FontManager::getSingleton().getFont("simsun-10");
		CEGUI::colour cols(0,0,0);
		float width = farmPaoPao->getFitWidth("simsun-10", content);
		if (width < 92)
		{
			farmPaoPao->setWidthFunc(width);
		}
		farmPaoPao->addElementText(font,(CEGUI::utf8*)content,cols);
		farmPaoPao->setAlign("AT_CENTER_HOR");
		farmPaoPao->setHeightFunc(farmPaoPao->getFormatHeight());
		// 	std::string winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "DynamicUI_CreateFarmPaoPao",index, content, set, image);
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetFarmPaoPaoPos", winname, x, y);

		DynamicUIMap::iterator it = mUIMap.find(winname);
		if(it == mUIMap.end())
		{
			DynamicUIInfo info;
			info.type = EDUI_FARMPAOPAO;
			info.winname = winname;
			info.life = 3000;
			info.time = GetTickCount();
			info.pos = pos;
			info.autoDel = true;

			mUIMap.insert(std::make_pair(winname, info));
		}
		else
		{
			it->second.time = GetTickCount();
			it->second.pos = pos;
		}

		UIAnimationInfo info;
		info.mType = UIAnimationInfo::EUIA_UPDOWN;
		info.mRange = 10;
		info.mLoopTime = 1000;
		info.mPauseTime = 500;
		UIAnimationMgr::getSingleton().playAnimation(winname, info);
		return winname;
	}
	else
	{
		std::string winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "DynamicUI_CreateFarmPaoPao",index, set, image);
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetFarmPaoPaoPos", winname, x, y);

		DynamicUIMap::iterator it = mUIMap.find(winname);
		if(it == mUIMap.end())
		{
			DynamicUIInfo info;
			info.type = EDUI_FARMPAOPAO;
			info.winname = winname;
			// 			info.life = 3000;
			info.time = GetTickCount();
			info.pos = pos;

			mUIMap.insert(std::make_pair(winname, info));
		}
		else
		{
			it->second.time = GetTickCount();
			it->second.pos = pos;
		}

		UIAnimationInfo info;
		info.mType = UIAnimationInfo::EUIA_UPDOWN;
		info.mRange = 10;
		info.mLoopTime = 1000;
		info.mPauseTime = 500;
		UIAnimationMgr::getSingleton().playAnimation(winname, info);
		return winname;
		// 		std::string winname1 = "DynamicUI_PestPaoPao_";
		// 		std::string winname = winname1 + Ogre::StringConverter::toString(index).c_str();
		// 		winMgr.destroyWindow(winname);
		// 		CEGUI::Window* pestPoPo = winMgr.createWindow("EQHomeLook/StaticImage",winname); 
		// 		winMgr.getWindow("FarmmainUI")->addChildWindow(pestPoPo);
		// 		pestPoPo->setSize(CEGUI::UVector2(CEGUI::UDim(0,100),CEGUI::UDim(0,50)));
		// 		pestPoPo->setMousePassThroughEnabled(true);
		// 		pestPoPo->setProperty("Image", "set:" + set + " image:" + image);
	}
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createFarmHarvest(int index,Ogre::Vector3& pos, FarmPaoPaoType type, int num, const char* set, const char* image)
{
	int x,y;
	if(!CommonHelper::Vector3ToScreenPoint(pos, x, y))
		return "";

	std::string winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "DynamicUI_CreateFarmHarvest", index,int(type), num, set, image);
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	win->setVisible(false);

	DynamicUIInfo info;
	info.type = EDUI_FARMHARVEST;
	info.winname = winname;
	info.life = 1500;
	info.pos = pos;
	info.autoDel = true;

	mFarmPaoPaoBuffer.push(info);

	return winname;
}
//-----------------------------------------------------------------------
bool DynamicUIMgr::_popFarmPaoPao()
{
	if(mFarmPaoPaoBuffer.empty())
		return false;

	DynamicUIInfo info = mFarmPaoPaoBuffer.front();
	mFarmPaoPaoBuffer.pop();
	info.time = GetTickCount();
	mUIMap.insert(std::make_pair(info.winname, info));

	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(info.winname);
	if(!win)
		return true;
	win->setVisible(true);

	int x,y;
	if(!CommonHelper::Vector3ToScreenPoint(info.pos, x, y))
		return true;
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetFarmHarvestPos", info.winname, x, y);

	UIAnimationInfo ani;
	ani.mType = UIAnimationInfo::EUIA_FLOATUP;
	ani.mPlayTime = 1500;
	ani.mRange = 75;
	UIAnimationMgr::getSingleton().playAnimation(info.winname, ani);	

	UIAnimationInfo ani2;
	ani2.mType = UIAnimationInfo::EUIA_FADE;
	ani2.mDelayTime = 500;
	ani2.mPlayTime = 1000;
	ani2.mStartAlpha = 1;
	ani2.mEndAlpha = 0;
	UIAnimationMgr::getSingleton().playAnimation(info.winname, ani2);

	return true;
}
//-----------------------------------------------------------------------
std::string DynamicUIMgr::createFarmHeadup(int index,const Ogre::Vector3& pos, FarmHeadupType type, const char* set1, const char* image1, 
										   const char* set2, const char* image2, const char* set3, const char* image3)
{
	std::string winname = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "DynamicUI_CreateFarmHeadup", index,
																type, set1, image1, set2, image2, set3, image3);

	int x,y;
	if(!CommonHelper::Vector3ToScreenPoint(pos, x, y))
		x = y = -10000;
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetFarmHeadupPos", winname, x, y);

	DynamicUIMap::iterator it = mUIMap.find(winname);
	if(it == mUIMap.end())
	{
		DynamicUIInfo info;
		info.type = EDUI_FARMHEADUP;
		info.winname = winname;
		info.time = GetTickCount();
		info.pos = pos;

		mUIMap.insert(std::make_pair(winname, info));
	}
	else
	{
		it->second.time = GetTickCount();
	}
	
	return winname;
}
//-----------------------------------------------------------------------
bool DynamicUIMgr::hasWindow(std::string& winname)
{
	return mUIMap.find(winname) != mUIMap.end();
}
//-----------------------------------------------------------------------
void DynamicUIMgr::destroyWindow(std::string& winname)
{
	DynamicUIMap::iterator it = mUIMap.find(winname);
	if(it != mUIMap.end())
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(it->second.winname);
		UIAnimationMgr::getSingleton().stopAnimation(it->second.winname);
		mUIMap.erase(it);
	}
}
//
void DynamicUIMgr::clearFarmPaoPaoBuffer()
{
	if (!mFarmPaoPaoBuffer.empty())
	{
		int size = mFarmPaoPaoBuffer.size();
		for (int i = 0;i < size; i++)
		{
			mFarmPaoPaoBuffer.pop();
		}
	}
}
//-----------------------------------------------------------------------
void DynamicUIMgr::update()
{
	DWORD now = GetTickCount();

	//数值泡泡动画缓冲
	static DWORD lastNumber = 0;
	if(now - lastNumber > 1000)
	{
		if(_popNumberPaoPao())
			lastNumber = now;
	}

	bool valid;
	DynamicUIMap::iterator it;
	for(it=mUIMap.begin(); it!=mUIMap.end(); )
	{
		valid = true;

		CEGUI::Window* win=NULL; 
		if(!CEGUI::WindowManager::getSingleton().isWindowPresent(it->second.winname))
			valid = false;
		else
			win = CEGUI::WindowManager::getSingleton().getWindow(it->second.winname);

		GameObj* obj=NULL;
		if(!it->second.objname.empty())
		{
			obj = ObjMgr::getSingleton().findObj((ObjMgr::GAMEOBJ_TYPE)it->second.objtype, it->second.objname);
			if(!obj)
				valid = false;
		}

		if(valid==false || ( it->second.life != 0 && now - it->second.time >= it->second.life ))
		{
			it->second.fireEvent(DynamicUIInfo::EventTimeOut);
			
			UIAnimationMgr::getSingleton().stopAnimation(it->second.winname);

			if(it->second.autoRemove && win && win->getParent())
				win->getParent()->removeChildWindow(win);

			if (it->second.type == EDUI_CHATPAOPAO)
			{
				ObjMgr::GAMEOBJ_TYPE objtype = ObjMgr::getSingleton().getObjType(obj);
				if(objtype == ObjMgr::GOT_MAINPLAYER || objtype == ObjMgr::GOT_PLAYER)
					((Player*)obj)->setChatPaoPaoIsVisible(false);
			}

			if(it->second.autoDel)
				CEGUI::WindowManager::getSingleton().destroyWindow(it->second.winname);

			it = mUIMap.erase(it);
			continue;
		}

		//同步坐标
		if(it->second.type != EDUI_COMMON)
		{
			int x,y;
			Ogre::Vector3 pos;
			
			if(obj)
				pos = obj->getPos() + Ogre::Vector3(0,obj->getModelHeight(),0);
			else
				pos = it->second.pos;

			if(CommonHelper::Vector3ToScreenPoint(pos, x, y))
			{
				switch (it->second.type)
				{
				case EDUI_CHATPAOPAO:
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetChatPaoPaoPos", it->second.winname, x, y);
					break;
				case EDUI_HEADUP:
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetHeadupPos", it->second.winname, x, y);
					break;
				case EDUI_FARMPAOPAO:
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetFarmPaoPaoPos", it->second.winname, x, y);
					break;
				case EDUI_FARMHEADUP:
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DynamicUI_SetFarmHeadupPos", it->second.winname, x, y);
					break;
				case EDUI_CIRCLEMENU:
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "setDisplayPos", it->second.winname, x, y);
					break;
				case EDUI_BIGEXPRESSION:
					if (((Player*)obj)->getChatPaoPaoIsVisible())
					{
						luabind::call_function<void>(CLuaManager::getSingleton().getL(), "BigExpression_SetBigExpressionWinPos", it->second.winname, x, y,true);
					}
					else
						luabind::call_function<void>(CLuaManager::getSingleton().getL(), "BigExpression_SetBigExpressionWinPos", it->second.winname, x, y,false);
					break;
				default:
					it++;
					continue;
				}

				//农场头顶是显3秒隐3秒
// 				if(it->second.type == EDUI_FARMHEADUP)
// 					win->setVisible( (now-it->second.time)%6000<3000 );
// 				else
					win->setVisible(true);

				UIAnimationMgr::getSingleton().updateWinPos(it->second.winname);
			}
			else
			{
				win->setVisible(false);
			}
		}

		it++;
	}
}
//-----------------------------------------------------------------------
