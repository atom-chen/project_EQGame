#include "EQCommPCH.h"
#include <ShellAPI.h>
#include "ScriptFunction.h"

#include "GUIManager.h"
#include "EQMsgManager.h"

#include <string>
#include <iostream>

#include "EQGameMgr.h"
#include "GameState.h"
#include "AvatarRTT.h"
#include "FurnitureRTT.h"
#include "EQCamera.h"
#include "FitmentEvent.h"
#include "EQOgreSys.h"
#include "MainPlayer.h"
#include "EQGameSceneMgr.h"
#include "Variable.h"
#include "FitmentInteractMgr.h"
//#include "FunnyFarm.h"
//EQMain
#include "SceneManager.h"
#include "HouseScene.h"
#include "luaManager.h"
#include "Avatar.h"
#include "OtherPlayerRTT.h"
#include "FurnitureRTT.h"
#include "BeautyRTT.h"
#include "AvatarAdjuster.h"
#include "SystemGcs.h"
#include "ItemTable.h"
#include "CommFunc.h"
#include "ItemPack.h"

#include "EQLogin.h"
#include "Hotkey.h"
#include "UIAnimationMgr.h"
#include "DynamicUIMgr.h"
#include "MsgSystem.h"
#include "BodyActionMgr.h"
#include "StringHandle.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "MagicBoxSystem.h"

extern bool	 g_bAppQuit;
//---------------------------------------------------------------------------
bool loginRequest(std::string account, 
					std::string pw,
					std::string serverIP, 
					int serverPort)
{
	EQLogin* login = sGameMgr.getLogin();
	if (login)
	{
		login->setAccount(account);
		login->setPassword(pw);
		login->setServerIP(serverIP);
		login->setPort(serverPort);
		return login->LoginRequest() == TRUE;
	}
	return false;
}
//---------------------------------------------------------------------------
bool creatPlayerRequest(std::string account, 
							std::string username)
{
	EQLogin* login = sGameMgr.getLogin();
	if (login)
	{
		login->setAccount(account);
		login->setUsername(username);
		return login->CreatPlayerRequest() == TRUE;
	}
	return false;
}
//---------------------------------------------------------------------------
bool loadScene()
{
	GameStateMgr::getSingleton().setState(GS_MAINGAME);
	return true; 
	//CClientShell::getSingleton().LoadScene();
}
//---------------------------------------------------------------------------
bool rolesRequest()
{
	EQLogin* login = sGameMgr.getLogin();
	if (login)
	{
		return login->RolesRequest() == TRUE;
	}
	return false;
}
//---------------------------------------------------------------------------
bool versionRequest(std::string account,
					std::string pw,
						std::string serverIP, 
						int serverPort)
{
	EQLogin* login = sGameMgr.getLogin();
	if (login)
	{
		login->setAccount(account);
		login->setPassword(pw);
		login->setServerIP(serverIP);
		login->setPort(serverPort);
		return login->VersionRequest() == TRUE;
	}
	return false;
}
//---------------------------------------------------------------------------
void setText(const char* szWindows, 
				 const char* szText)
{
	CGUIManager::getSingleton().SetText(szWindows,szText);
}
//---------------------------------------------------------------------------
void startRTT(int x, int y, int z)
{
	EQGameMgr::getSingleton().getRTT()->startRTT(x,y,z);
}
//---------------------------------------------------------------------------
void rttScale(float scaleX,float scaleY,float scaleZ)
{
	EQGameMgr::getSingleton().getRTT()->rttScale(scaleX, scaleY, scaleZ);
}
//---------------------------------------------------------------------------
void rttZoom(float scale)
{
	EQGameMgr::getSingleton().getRTT()->zoom(scale);
}
//---------------------------------------------------------------------------
void rttReset()
{
	EQGameMgr::getSingleton().getRTT()->resetOrientation();
	EQGameMgr::getSingleton().getRTT()->resetScale();
}
//---------------------------------------------------------------------------
void rttSetAspectRatio(int w, int h)
{
	EQGameMgr::getSingleton().getRTT()->getRTTCamera()
		->getOgreCam()->setAspectRatio(Ogre::Real((float)w / h));
}
//---------------------------------------------------------------------------
void rttLookAt(int x, int y, int z)
{
	EQGameMgr::getSingleton().getRTT()->getRTTCamera()
		->lookAt(Ogre::Vector3(x,y,z));
}
//---------------------------------------------------------------------------
void rttRotate(int degree,int axes)
{
	EQGameMgr::getSingleton().getRTT()->rotate(degree,axes);
}
//---------------------------------------------------------------------------
void equipOn( uint slot, uint equipDisplayID, uint holdSlot )
{
	EQGameMgr::getSingleton().getRTT()->equipOn(slot, equipDisplayID, holdSlot);
}
//---------------------------------------------------------------------------
void takeOff( uint slot )
{
	EQGameMgr::getSingleton().getRTT()->takeOff(slot);
}
//---------------------------------------------------------------------------
void rttTakeOffAll()
{
	EQGameMgr::getSingleton().getRTT()->takeOffAll();
}
//---------------------------------------------------------------------------
void rttSync()
{
	EQGameMgr::getSingleton().getRTT()->syncPlayerEquip();
}
//---------------------------------------------------------------------------
void rttFurniture(uint itemid)
{
	EQGameMgr::getSingleton().getFurnitureRTT()->startFurnitureRTT(itemid);
}
//---------------------------------------------------------------------------
void rttFurniRotate(int degree)
{
	EQGameMgr::getSingleton().getFurnitureRTT()->rotate(degree);
}
//---------------------------------------------------------------------------
void rttFurniSetAspectRatio(int w, int h)
{
	EQGameMgr::getSingleton().getFurnitureRTT()->getCamera()
		->getOgreCam()->setAspectRatio(Ogre::Real((float)w / h));
}
//---------------------------------------------------------------------------
void rttFurniReset()
{
	EQGameMgr::getSingleton().getFurnitureRTT()->resetOrientation();
	EQGameMgr::getSingleton().getFurnitureRTT()->resetScale();
}
//---------------------------------------------------------------------------
void rttFurniZoom(float scale)
{
	EQGameMgr::getSingleton().getFurnitureRTT()->zoom(scale);
}
//---------------------------------------------------------------------------
void rttSetActive(const char* name, bool active)
{
	if(EQGameMgr::getSingleton().getRTT()->getName() == name)
	{
		EQGameMgr::getSingleton().getRTT()->setActive(active);
	}

	if(EQGameMgr::getSingleton().getFurnitureRTT()->getName() == name)
	{
		EQGameMgr::getSingleton().getFurnitureRTT()->setActive(active);
	}

	if(EQGameMgr::getSingleton().getOtherPlayerRTT()->getName() == name)
	{
		EQGameMgr::getSingleton().getOtherPlayerRTT()->setActive(active);
	}

	if(EQGameMgr::getSingleton().getBeautyRTT()->getName() == name)
	{
		EQGameMgr::getSingleton().getBeautyRTT()->setActive(active);
	}
}
//---------------------------------------------------------------------------
void avatarAdjNext(int slot)
{
	AvatarAdjuster* adj = EQGameMgr::getSingleton().getRTT()->mAvatar->getAdjuster();
	switch (slot)
	{
	case 1:
		adj->nextHairType();
		break;
	case 2:
		adj->nextHairColor();
		break;
	case 3:
		adj->nextFace();
		break;
	case 4:
		adj->nextBeard();
		break;
	case 5:
		adj->nextSkin();
		break;

	}
}
//---------------------------------------------------------------------------
void avatarAdjPrev(int slot)
{
	AvatarAdjuster* adj = EQGameMgr::getSingleton().getRTT()->mAvatar->getAdjuster();
	switch (slot)
	{
	case 1:
		adj->nextHairType();
		break;
	case 2:
		adj->nextHairColor();
		break;
	case 3:
		adj->nextFace();
		break;
	case 4:
		adj->nextBeard();
		break;
	case 5:
		adj->nextSkin();
		break;

	}
}
//---------------------------------------------------------------------------
void exitApp()
{
	//g_bAppQuit = true;
	sGameMgr.getEQMsgManager()->LogoutRequest();
}
//---------------------------------------------------------------------------
void houseUp()
{
	// TODO: Add your control notification handler code here
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce )
	{
		bool b = sce->upView();
		HouseSceneFloor* floor = sce->getWalkViewFloor();

	}
}
//---------------------------------------------------------------------------
void houseDown()
{
	// TODO: Add your control notification handler code here
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce )
	{
		bool b = sce->downView();
		HouseSceneFloor* floor = sce->getWalkViewFloor();

	}
}
//---------------------------------------------------------------------------
int getHouseMode()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce )
		return (int)sce->getWatchMode();
	else
		return 0;
}
//---------------------------------------------------------------------------
void changeHouseMode(int sel)
{
	if ( sel < 0 )
	{
		sScriptExcept.scriptExcept(SE_SHOW_MESSBOX, "changeHouseMode 参数小于0，请检查");
		return;
	}

	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce )
	{ 
		sce->changeWatchMode((HouseWatchMode)(sel));
	}
}
//---------------------------------------------------------------------------
void changeGridMode(int sel)
{
	if ( sel < 0 )
		return;

	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce )
	{
		sce->changeGridDisplayMode((GridDisplayMode)(sel));
	}
}
//---------------------------------------------------------------------------
void changeFitmentMode(int sel)
{
	switch (sel)
	{
	case 1://普通游戏模式
		FitmentEventMgr::getSingleton().setFitMode(FMM_NONE);
		EQGameMgr::getSingleton().getMainCamera()->resetCamMovSpeed();
		break;
	case 2://家装
		FitmentEventMgr::getSingleton().setFitMode(FMM_ADD);
		EQGameMgr::getSingleton().getMainCamera()->resetCamMovSpeed(10);
		if (sMainPlayer.getState() == GOS_ANIMATION)
		{
			sMainPlayer.stopAnimation();
		}
		sMainPlayer.setState(GOS_IDLE);
		break;
	case 3://家装编辑
		FitmentEventMgr::getSingleton().setFitMode(FMM_EDIT);
		EQGameMgr::getSingleton().getMainCamera()->resetCamMovSpeed(10);
		if (sMainPlayer.getState() == GOS_ANIMATION)
		{
			sMainPlayer.stopAnimation();
		}
		sMainPlayer.setState(GOS_IDLE);
		break;
	}
}
//---------------------------------------------------------------------------
void onMouseFitmentDro(int id)
{
	FitmentEventMgr::getSingleton().setFitDro(id);
}
//---------------------------------------------------------------------------
void onMouseFitmentRotate(float degree)
{
	FitmentEventMgr::getSingleton().rotateFitment(degree);
}
//---------------------------------------------------------------------------
void onMouseFitmentPickup()
{
	FitmentEventMgr::getSingleton().pickupFitment();
}
//---------------------------------------------------------------------------
void onMouseFitmentPutdown()
{
	FitmentEventMgr::getSingleton().putdownFitment();
}
//---------------------------------------------------------------------------
int getPickState()
{
	return FitmentEventMgr::getSingleton().getPickState();
}
//---------------------------------------------------------------------------

void loadStaticMesh(std::string meshfile, bool castShadows)
{
	Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
	Ogre::Entity* ent = 0;
	ent = sm->createEntity(meshfile,meshfile);
	assert(ent);
	sm->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setCastShadows(castShadows);
	
}
//---------------------------------------------------------------------------
// void loadStaticMesh_Outdoor(std::string meshfile, bool castShadows, float x, float y, float z, float rotationX, float rotationY, float rotationZ, float rotationW)
// {
// 	Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
// 	Ogre::Entity* ent = 0;
// 	ent = sm->createEntity(meshfile,meshfile);
// 	assert(ent);
// 	EQGameSceneMgr::getSingleton().getCommon()->addMesh(meshfile);
// 
// 	Ogre::SceneNode *sn = NULL;
// 	if ( !sm->hasSceneNode("TEMP_OUTDOOR_NODE") )
// 	{
// 		sn = sm->getRootSceneNode()->createChildSceneNode("TEMP_OUTDOOR_NODE");
// 		sn->setScale(0.5,0.5,0.5);
// 	}
// 	else
// 		sn = sm->getSceneNode("TEMP_OUTDOOR_NODE");
// 
// 	Ogre::SceneNode *node = sn->createChildSceneNode(meshfile);
// 	node->attachObject(ent);
// 	node->translate(x, y, z);
// 	Ogre::Quaternion quat(rotationW, rotationX, rotationY, rotationZ);
// 	node->rotate(quat);
// 	ent->setCastShadows(castShadows);
// }
//---------------------------------------------------------------------------
void outdoorMeshScale(std::string nodeName, float x, float y, float z)
{
	Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
	Ogre::SceneNode *node = sm->getSceneNode(nodeName);
	node->scale(x, y, z);
}
//---------------------------------------------------------------------------
void lookAtMainPlayer()
{
	if (sGameMgr.getMainCamera()->getFollowMe())
	{
		return;
	}
	Ogre::Vector3 pos = MainPlayer::getSingleton().getPos();
	pos.y = 120;
	//EQGameMgr::getSingleton().getMainCamera()->lookAt(pos);
	EQGameMgr::getSingleton().getMainCamera()->setNextLookAt(pos);
}
//---------------------------------------------------------------------------
void playAnimation(std::string ani, std::string target)
{
// 	sMainPlayer.getBodyActionMgr()->reqStartBodyAction(ani, target,false);
}
//---------------------------------------------------------------------------
void addAnimation(std::string ani,std::string action,bool loop)
{
	sMainPlayer.getBodyActionMgr()->add_noLoopAction(ani,action,loop);
}
//---------------------------------------------------------------------------
void clear_allAnimaList()
{
	sMainPlayer.getBodyActionMgr()->clear_allActionList();
}
//---------------------------------------------------------------------------
void clearLoopAnima()
{

}
//---------------------------------------------------------------------------
std::string getCurrentAnimation()
{
	return sMainPlayer.getCurrentAnimation();
}
//---------------------------------------------------------------------------
void addAnimation_loop(std::string ani,std::string action,bool loop)
{
	sMainPlayer.getBodyActionMgr()->add_LoopAction(ani,action,loop);
}
//---------------------------------------------------------------------------
void setAnimaLoopNum(int loopnum)
{
	sMainPlayer.getBodyActionMgr()->setLoopAniNum(loopnum);
}
//---------------------------------------------------------------------------
void check_Animation_List()
{
	sMainPlayer.getBodyActionMgr()->check_AnimationList();
}
//---------------------------------------------------------------------------
bool sendNetPacket(/*NumberPrinter&*/INetPacket& packet)
{
	//packet.print();
	return (sGameMgr.getEQMsgManager()->Send(packet) == TRUE);
}
//-------------------------------------------------------------------
int getCurrFurID()
{
	return	FitmentEventMgr::getSingleton().getCurrFurID();
}
//-------------------------------------------------------------------
bool getDayNight()
{
	return EQGameSceneMgr::getSingleton().getDayNight();
}
//-------------------------------------------------------------------
void changeDayNight(bool daynight, bool immediately)
{
	EQGameSceneMgr::getSingleton().changeDayNight(daynight, immediately);
}
//-------------------------------------------------------------------
void revertDayNight()
{
	EQGameSceneMgr::getSingleton().initDayNightTimer();
}
//-------------------------------------------------------------------
void switchDayNight(bool immediately)
{
	EQGameSceneMgr::getSingleton().switchDayNight(immediately);
}

//---------------------------------2010-04-26(huangdj)------------------------------------------
uint getLoopMenuID()
{
	return FitmentInteractMgr::getSingleton().getLoopMenuID();
}
std::string getLoopMenuDelTime()
{
	return FitmentInteractMgr::getSingleton().getDelTime();
}
uint getInteractingFitmentID()
{
	return FitmentInteractMgr::getSingleton().getInteractingFitmentID();
}
//---------------2010-04-27(huangdj)-------------------------------------------------------
void selLoopMenu(int menu)
{
	FitmentInteractMgr::getSingleton().selLoopMenu(menu);
}
//-------------------------------------------------------------
void continueWalkForUseFurni()
{
	FitmentInteractMgr::getSingleton().sndBeginInteract();
}
//-----------------------
void continueWalkForPickMagicbox()
{
	MagicBoxSystem::getSingleton().reqWalkForPickMagicbox();
}
//-----------------------
int posToSlot(int pos)
{
	switch(pos)
	{
	case Player::BODY_HAT:
		return ES_HAT;
	case Player::BODY_CLOTHES:
		return ES_COAT;
	case Player::BODY_GLASSES:
		return ES_NOSE;
	case Player::BODY_TROUSERS:
		return ES_PANTS;
	case Player::BODY_BACK:
		return ES_BACK;
	case Player::BODY_SHOES:
		return ES_SHOES;
	case Player::BODY_HAND:
		return ES_HAND;
	case Player::BODY_TRINKET:
		return ES_ORNAMENT;

	default:
		LOGERR("错误的装备位置参数给posToSlot()");

	}
	return 0;
}

//  [5/12/2010 huangdj1]
void setSysMsgText(std::string sysMsg)
{
	CGUIManager::getSingleton().setSysMsgText(sysMsg);
}

//  [5/18/2010 huangdj1]
std::string UIntToStringBitsetScript(uint aSet)
{
	if (aSet != NULL)
	{
		return UIntToStringBitset(aSet);
	}
	return "";

}

//  [5/19/2010 huangdj1]
uint getTakeOffSlots(uint equipOnSlots)
{
	ItemPack itemPack;
	return ItemPackLogic::getTakeOffSlots(itemPack,equipOnSlots);
}

void reqPlayerBasicData()
{
	req_player_basic_data data;
	sGameMgr.getEQMsgManager()->Send(data);
}

//  [6/13/2010 huangdj1]
void setTooltipProperty(const std::string szWindow, const std::string szText)
{
	CGUIManager::getSingleton().setTooltipProperty(szWindow, szText);
}

int getMultiLineEditboxLineCount(const char* winname)
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	if(!win)
		return 0;

	CEGUI::MultiLineEditbox* mleb = dynamic_cast<CEGUI::MultiLineEditbox*>(win);
	if(!mleb)
		return 0;

	return mleb->getFormattedLines().size();
}

void setListboxTextItemBrush(CEGUI::ListboxTextItem *item)
{
	if(false == CEGUI::ImagesetManager::getSingleton().isImagesetPresent("TaharezLook"))
		return;

	CEGUI::Imageset *sel_imgset = CEGUI::ImagesetManager::getSingleton().getImageset("TaharezLook");

	if(sel_imgset->isImageDefined("MultiListSelectionBrush"))
		item->setSelectionBrushImage(&sel_imgset->getImage("MultiListSelectionBrush"));
}
//------------------------------------------------------------------------------
void createListboxTextItem(const char* listname, const char* text, int id)
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(listname);
	if(!win)
		return;

	CEGUI::Listbox* list = dynamic_cast<CEGUI::Listbox*>(win);
	if(!list)
		return;

	

	CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem((CEGUI::utf8*)text, id);
	setListboxTextItemBrush(item);
	list->addItem(item);
}
//------------------------------------------------------------------------------
bool locateListboxTextItem(std::string listname, int id)
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(listname);
	if(!win)
		return false;

	CEGUI::Listbox* list = dynamic_cast<CEGUI::Listbox*>(win);
	if(!list)
		return false;

	for (size_t i=0; list->getItemCount()!=i; ++i)
	{
		CEGUI::ListboxItem* item = list->getListboxItemFromIndex(i);
		if (item->getID() == id)
		{
			list->setItemSelectState(i, true);
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------
void createComboBoxTextItem( const char* comboBoxName, const char* text, int id,std::string txtCol )
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(comboBoxName);
	if(!win)
		return;

	CEGUI::Combobox* comboBox = dynamic_cast<CEGUI::Combobox*>(win);
	if(!comboBox)
		return;

	CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem((CEGUI::utf8*)text, id);
	if(!txtCol.empty())
		item->setTextColours(CEGUI::PropertyHelper::stringToColourRect(txtCol));
	setListboxTextItemBrush(item);
	comboBox->addItem(item);
}
//------------------------------------------------------------------------------
void setComoboxTextItemTxtColors( std::string combobox_name )
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(combobox_name);
	CEGUI::Combobox* comboBox = dynamic_cast<CEGUI::Combobox*>(win);
	CEGUI::ListboxTextItem* txtItm = static_cast<CEGUI::ListboxTextItem*>(comboBox->getSelectedItem());
	CEGUI::ColourRect colRect = txtItm->getTextColours();
	CEGUI::colour txtCol = colRect.d_bottom_left;
	comboBox->getEditbox()->setProperty("NormalTextColour",CEGUI::PropertyHelper::colourToString(txtCol));
}
//------------------------------------------------------------------------------
bool isKeyDown(int keycode)
{
	return sHotkey.isKeydown(keycode);
}
//------------------------------------------------------------------------------
void setKeyFlag(int keycode, bool flag)
{
	sHotkey.setKeyFlag(keycode, flag);
}
//-------------------------------------------------------------------------------
void playAnimationByName(std::string winName , UIAnimationInfo&info)
{
	UIAnimationMgr::getSingleton().playAnimation(winName, info);
}
//-------------------------------------------------------------------------------
bool uiAnimationIsPlaying(std::string winName)
{
	return UIAnimationMgr::getSingleton().isPlaying(winName);
}

std::string UTF8ToGB2312Bylua( const char * str )
{
	return UTF8ToGB2312(str);
	
}

std::string GB2312ToUTF8Bylua( const char * str )
{
	return GB2312ToUTF8(str);
}
//-------------------------------------------------------------------------------
EQCamera* getMainCamera()
{
	return sGameMgr.getMainCamera();
}
//-------------------------------------------------------------------------------
void showMsgPaoPao(int index,int x,int y, int z, const char* content, const char* imageset, const char* icon)
{
	DynamicUIMgr::getSingleton().createFarmPaoPao(index,x,y,z,content,imageset,icon);
}
//---------------------------------------------------------------------------------
void showDexpDynamicUI(int index,int exp,int x_3d,int y_3d,int z_3d)
{
	Ogre::Vector3 pos(x_3d,y_3d,z_3d);
	DynamicUIMgr::getSingleton().createFarmHarvest(index,pos,EFPPT_EXP,exp);
}
//---------------------------------------------------------------------------------
DWORD getTickCount()
{
	return GetTickCount();
}
//---------------------------------------------------------------------------------
void setCreateMsgBoxTime()
{
	sSysMsg.setCreateMsgBoxTime();
}
//
int getUTF8Len(const char * str)
{
	return UTF8Len(str);
}
//
std::string returnValidString(std::string fullStr)
{
	 return CGUIManager::getSingleton().returnValidString(fullStr);
}

void openFarmExe()
{
	//string ac = sMainPlayer.getAccount();
	//string pw = sMainPlayer.getPassword();
	ShellExecuteA( NULL,"open","EQFarm.exe","-FuckingAutoRun","", SW_SHOW );
}

bool runExe(const char *lpOperation, const char *lpFile, const char *lpParameters, const char *lpDirectory, int nShowCmd)
{
	if(ShellExecuteA(NULL, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd) <= (HINSTANCE)32)
		return false;

	return true;
}

void ExitClient(void)
{
	::PostQuitMessage(0);
}

void OpenURL(std::string url)
{
	ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//--------------------------------------------------------------------------
std::string getModalWindowName(void)
{
	CEGUI::Window *win = CEGUI::System::getSingleton().getModalTarget();
	if (NULL == win)
	{
		return "";
	}
	return (char*)win->getName().c_str();
}
//--------------------------------------------------------------------------
void setWindowFade(std::string winname, int delayTime, int fadeTime, bool autoDel)
{
	DynamicUIInfo dinfo;
	dinfo.type = EDUI_COMMON;
	dinfo.winname = winname;
	dinfo.life = delayTime + fadeTime;
	dinfo.autoRemove = true;
	dinfo.autoDel = autoDel;
	DynamicUIMgr::getSingleton().createCommon(dinfo);

	UIAnimationInfo ainfo;
	ainfo.mType = UIAnimationInfo::EUIA_FADE;
	ainfo.mDelayTime = delayTime;
	ainfo.mPlayTime = fadeTime;
	ainfo.mStartAlpha = 1.0;
	ainfo.mEndAlpha = 0.0;
	UIAnimationMgr::getSingleton().playAnimation(winname, ainfo);
}
//--------------------------------------------------------------------------
void debugUseFit(float x, float y, float z)
{
	FitmentInteractMgr::getSingleton().debug(x, y, z);
}
//--------------------------------------------------------------------------
std::string stringFilter(std::string txt,int mode)
{
	return sStringHandle.stringFilter(const_cast<char*>(txt.c_str()),mode);
}
//--------------------------------------------------------------------------
void setListboxItemSelect( std::string listbox_name,std::string itm_txt )
{
	CEGUI::Listbox *listbox = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().getWindow(listbox_name));
	if (listbox)
	{
		listbox->clearAllSelections();
		CEGUI::ListboxItem* listbox_itm = listbox->findItemWithText((CEGUI::utf8*)itm_txt.c_str(),NULL);
		if (listbox_itm)
		{
			if (!listbox_itm->isSelected())
				listbox_itm->setSelected(true);
		}
	}
}
//--------------------------------------------------------------------------
void reqFlowerLogs()
{
	req_flower_logs pack;
	sGameMgr.getEQMsgManager()->Send(pack);
}

void reqEggLogs()
{
	req_egg_logs pack;
	sGameMgr.getEQMsgManager()->Send(pack);
}

void reqOpenGuide()
{
	req_open_change_scene_guide pack;
	sGameMgr.getEQMsgManager()->Send(pack);
}

void reqCloseGuide()
{
	req_close_change_scene_guide pack;
	sGameMgr.getEQMsgManager()->Send(pack);
}

void reqStartBuyHouse()
{
	req_start_buy_house pack;
	sGameMgr.getEQMsgManager()->Send(pack);
}

void reqEndBuyHouse()
{
	req_end_buy_house pack;
	sGameMgr.getEQMsgManager()->Send(pack);
}
//--------------------------------------------------------------------------