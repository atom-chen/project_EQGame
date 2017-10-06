#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "LuaManager.h"
#include "ScriptFunction.h"
#include "CEGUIEx/CEGUIEx.h"
#include "DynamicCtrlVector.h"
#include "ImageCombineCtrl.h"
#include "MultiLineBoxParse.h"
#include "CommFunc.h"

#include "EQCamera.h"

using namespace luabind;
//------------------------------------------------------------------------
void EQSrtBreak(std::string msg)
{
	//其实手动在这里设置断点就对了
	LOGDBG("Script Break! - " + msg);
}

void callFunction(std::string funName)
{
	luabind::call_function<void>(sLuaMgr.getL(), funName.c_str());
}
//------------------------------------------------------------------------
bool CLuaManager::RegisterGlobalFun()
{
	luabind::module(EQ_GLOBA_FUNC)
		[
			luabind::def("callFunction",		&callFunction),

			luabind::def("EQSrtBreak",			&EQSrtBreak),
			luabind::def("LoginRequest",		&loginRequest),
			luabind::def("CreatPlayerRequest",	&creatPlayerRequest),
			luabind::def("LoadScene",			&loadScene),
			luabind::def("RolesRequest",		&rolesRequest),
			luabind::def("VersionRequest",		&versionRequest),
			luabind::def("SetText",				&setText),
			//luabind::def("SetChatText",			&setChatText),
			//[5/12/2010 huangdj1]
			luabind::def("SetSysMsgText",		&setSysMsgText),
			//[6/12/2010 huangdj1]
			luabind::def("setTooltipProperty",	&setTooltipProperty),
			luabind::def("getMultiLineEditboxLineCount", &getMultiLineEditboxLineCount),
			luabind::def("createListboxTextItem", &createListboxTextItem),
			luabind::def("setListboxItemSelect", &setListboxItemSelect),
			luabind::def("createComboBoxTextItem", &createComboBoxTextItem),
			luabind::def("setComoboxTextItemTxtColors",&setComoboxTextItemTxtColors),
			luabind::def("locateListboxTextItem", &locateListboxTextItem),

			luabind::def("RTTSetActive",		&rttSetActive),
			luabind::def("EQStartRTT",			&startRTT),
			luabind::def("RTTSetAspectRatio",	&rttSetAspectRatio),
			luabind::def("RTTLookAt",			&rttLookAt),
			luabind::def("RTTRotate",			&rttRotate),
			luabind::def("RTTReset",			&rttReset),
			luabind::def("RTTScale",			&rttScale),
			luabind::def("RTTZoom",				&rttZoom),
			luabind::def("RTTequipOn",			&equipOn),
			luabind::def("RTTtakeOff",			&takeOff),
			luabind::def("RTTtakeOffAll",		&rttTakeOffAll),
			luabind::def("RTTSync",				&rttSync),
			luabind::def("RTTFurniture",		&rttFurniture),
			luabind::def("RTTFurniRotate",		&rttFurniRotate),
			luabind::def("RTTFurniSetAspectRatio",	&rttFurniSetAspectRatio),
			luabind::def("RTTFurniReset",		&rttFurniReset),
			luabind::def("RTTFurniZoom",		&rttFurniZoom),

			luabind::def("ExitApp",				&exitApp),
			luabind::def("HouseUp",				&houseUp),
			luabind::def("HouseDown",			&houseDown),
			luabind::def("getHouseMode",		&getHouseMode),
			luabind::def("ChangeHouseMode",		&changeHouseMode),
			luabind::def("ChangeGridMode",		&changeGridMode),
			luabind::def("ChangeFitmentMode",	&changeFitmentMode),
			luabind::def("OnMouseFitmentDro",	&onMouseFitmentDro),
			luabind::def("OnMouseFitmentRotate",&onMouseFitmentRotate),
			luabind::def("OnMouseFitmentPickup",&onMouseFitmentPickup),
			luabind::def("OnMouseFitmentPutdown",&onMouseFitmentPutdown),
			luabind::def("getPickState",		&getPickState),
			luabind::def("LoadStaticMesh",		&loadStaticMesh),
			luabind::def("LookAtMainPlayer",	&lookAtMainPlayer),
			luabind::def("PlayAnimation",		&playAnimation),
			luabind::def("AddAnimation",		&addAnimation),
			luabind::def("clear_allAnimaList",	&clear_allAnimaList),
			luabind::def("AddAnimation_loop",	&addAnimation_loop),
			luabind::def("setAnimaLoopNum",		&setAnimaLoopNum),
			luabind::def("getCurrentAnimation",	&getCurrentAnimation),
			luabind::def("check_Animation_List",&check_Animation_List),

			//luabind::def("SendNetPacket",		&sendNetPacket),
			luabind::def("avatarAdjNext",		&avatarAdjNext),
			luabind::def("avatarAdjPrev",		&avatarAdjPrev),

			luabind::def("getCurrFurID",		&getCurrFurID),
			// XiaoMing Add
			//luabind::def("LoadStaticMesh_Outdoor",	&loadStaticMesh_Outdoor),
			luabind::def("OutdoorMeshScale",	&outdoorMeshScale),	

			luabind::def("getCurrFurID",		&getCurrFurID),
			//2010-04-26(huangdj)
			luabind::def("getLoopMenuID",		&getLoopMenuID),
			luabind::def("getLoopMenuDelTime",	&getLoopMenuDelTime),
			luabind::def("getInteractingFitmentID",		&getInteractingFitmentID),
			//2010-04-27
			luabind::def("selLoopMenu",			&selLoopMenu),
			//temp
			luabind::def("getDayNight",			&getDayNight),
			luabind::def("changeDayNight",		&changeDayNight),
			luabind::def("revertDayNight",		&revertDayNight),
			luabind::def("switchDayNight",		&switchDayNight),

			luabind::def("posToSlot",			&posToSlot),
											 
			//  [5/18/2010 huangdj1]
			luabind::def("UIntToStringBitsetScript",	&UIntToStringBitsetScript),
			luabind::def("getTakeOffSlots",				&getTakeOffSlots),
			luabind::def("reqPlayerBasicData",			&reqPlayerBasicData),
			luabind::def("isKeydown",					&isKeyDown),
			luabind::def("setKeyFlag",					&setKeyFlag),
			//luabind::def("loadFarmScence",				&loadFarmScence),
			//luabind::def("logoutFarm",					&logoutFarm),
			//luabind::def("setFarmCurState",				&setFarmCurState),
			//luabind::def("setFarmSeedingCurSta",		&setFarmSeedingCurSta),
			luabind::def("playUIAnimation",				&playAnimationByName),
			luabind::def("uiAnimationIsPlaying",		&uiAnimationIsPlaying),
			luabind::def("UTF8ToGB2312Bylua",			&UTF8ToGB2312Bylua),
			luabind::def("GB2312ToUTF8Bylua",			&GB2312ToUTF8Bylua),
			luabind::def("showMsgPaoPao",				&showMsgPaoPao),
			luabind::def("showDexpDynamicUI",		&showDexpDynamicUI),
			luabind::def("getTickCount",			&getTickCount),
			luabind::def("setCreateMsgBoxTime",		&setCreateMsgBoxTime),
			luabind::def("getUTF8Len",				&getUTF8Len),
			luabind::def("returnValidString",		&returnValidString),
			luabind::def("ExitClient",				&ExitClient),
			luabind::def("OpenURL",					&OpenURL),

			//	把Window类型转换为MultiLinebox类型(hezhr)
			luabind::def("toMultiLineBox",			&CEGUI::toMultiLineBox),
			//	把Window类型转换为AnimateBox类型(hezhr)
			luabind::def("toAnimateBox",			&CEGUI::toAnimateBox),
			//	根据字体获取字符串长度(hezhr)
			luabind::def("getStrExtent",			&CEGUI::getStrExtent),
			//	根据像素获取截取的字符位置(hezhr)
			luabind::def("getCharAtPixel",			&CEGUI::getCharAtPixel),
			//	截取字符串(hezhr)
			luabind::def("subStrExtent",			&CEGUI::subStrExtent),
			//	获取字符串str中的字符个数(hezhr)
			luabind::def("getCharacterCount",		&CEGUI::getCharacterCount),
			//	截取字符串str的字串，以字符个数为单位(hezhr)
			luabind::def("subCharacter",			&CEGUI::subCharacter),
			//	获取坐标各个值(hezhr)
			luabind::def("getPositionVal",			&CEGUI::getPositionVal),
			//
			luabind::def("openFarmExe",			&openFarmExe),
			luabind::def("runExe",			    &runExe),
			luabind::def("getMainCamera",       &getMainCamera),

			//	组册lua定时器需要的函数
			luabind::def("createLuaTimer",			&createLuaTimer),
			luabind::def("startLuaTimer",			&startLuaTimer),
			luabind::def("stopLuaTimer",			&stopLuaTimer),
			luabind::def("isLuaTimerRunning",		&isLuaTimerRunning),
			luabind::def("getLuaTimerCurrentCount",	&getLuaTimerCurrentCount),
			// (add by hezhr)
			luabind::def("DateToSeconds",			&DateToSeconds),
			luabind::def("TimeDiffSeconds",			&TimeDiffSeconds),
			luabind::def("TimeDiffDayFloat",		&TimeDiffDayFloat),
			luabind::def("TimeDiffDayInt",			&TimeDiffDayInt),
			luabind::def("GetCurrentDir",			&GetCurrentDir),
			luabind::def("GetGameUserDir",			&GetGameUserDir),
			luabind::def("GetTickCountByLua",		&GetTickCountByLua),
			luabind::def("OpenDir",					&OpenDir),
			luabind::def("getModalWindowName",		&getModalWindowName),
			luabind::def("setWindowFade",			&setWindowFade),
			
			luabind::def("stringFilter",			&stringFilter),

			// 调试命令
			luabind::def("debugUseFit",				&debugUseFit),

			// 发包
			luabind::def("reqFlowerLogs",			&reqFlowerLogs),
			luabind::def("reqEggLogs",				&reqEggLogs),
			luabind::def("reqOpenGuide",			&reqOpenGuide),
			luabind::def("reqCloseGuide",			&reqCloseGuide),
			luabind::def("reqStartBuyHouse",		&reqStartBuyHouse),
			luabind::def("reqEndBuyHouse",			&reqEndBuyHouse),
			luabind::def("continueWalkForUseFurni", &continueWalkForUseFurni),
			luabind::def("continueWalkForPickMagicbox",&continueWalkForPickMagicbox)
		];

	return true;
}

