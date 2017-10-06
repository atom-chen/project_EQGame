/***********************************************************************
* 作者: 何展然
* 时间: 2010-9-8
* 描述: 负责将CEGUI扩展控件的函数透露给LUA
***********************************************************************/
#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>
#include "LuaManager.h"

#include "CEGUIEx/CEGUIEx.h"
#include "MultiLineBoxParse.h"
#include "DynamicCtrlVector.h"
#include "ImageCombineCtrl.h"
#include "ChatExpressionTable.h"
#include "BroadCastSystem.h"
#include "DynamicCreateImageset.h"
#include "Timekeeper.h"
#include "MicroBlog.h"
#include "SmallMap.h"

using namespace luabind;

bool CLuaManager::RegisterCeguiExtentControlClass()
{
	module( EQ_GLOBA_CLASS )
		[
			//	动画类型
			class_<CEGUI::Animate>("Animate")
			.def(constructor<>())
			.def("initAnimate",				&CEGUI::Animate::initAnimate)
			.def("addAnimateFrame",			(void(CEGUI::Animate::*)(std::string, std::string)) &CEGUI::Animate::addAnimateFrame)
			.def("clearUp",					&CEGUI::Animate::clearUp)
			,
			luabind::def("createAnimate",	&CEGUI::createAnimate)
			,
			//	动画控件
			class_<CEGUI::AnimateBox>("AnimateBox")
			.def(constructor<const CEGUI::String&, const CEGUI::String&>())
			.def("setSize",					&CEGUI::AnimateBox::setSizeFunc)
			.def("setPosition",				&CEGUI::AnimateBox::setPositionFunc)
			.def("setVisible",				&CEGUI::AnimateBox::setVisibleFunc)
			.def("getName",					&CEGUI::AnimateBox::getNameFunc)
			.def("addAnimate",				&CEGUI::AnimateBox::addAnimate)
			.def("clearUp",					&CEGUI::AnimateBox::clearUp)
			.def("play",					&CEGUI::AnimateBox::play)
			.def("pause",					&CEGUI::AnimateBox::pause)
			.def("stop",					&CEGUI::AnimateBox::stop)
			.def("setPlayOverHandle",		&CEGUI::AnimateBox::setPlayOverHandle)
			,
			//	图文混排控件
			class_<CEGUI::MultiLineBox>("MultiLineBox")
			.def(constructor<const CEGUI::String&, const CEGUI::String&>())
			.def("setFont",					&CEGUI::MultiLineBox::setFontFunc)
			.def("setCols",					&CEGUI::MultiLineBox::setColsFunc)
			.def("setSide",					&CEGUI::MultiLineBox::setSideFunc)
			.def("setSpecSide",				(void(CEGUI::MultiLineBox::*)(std::string, float)) &CEGUI::MultiLineBox::setSpecSide)
			.def("setMaxSize",				&CEGUI::MultiLineBox::setMaxSizeFunc)
			.def("setWidth",				&CEGUI::MultiLineBox::setWidthFunc)
			.def("setHeight",				&CEGUI::MultiLineBox::setHeightFunc)
			.def("setSize",					&CEGUI::MultiLineBox::setSizeFunc)
			.def("setPosition",				&CEGUI::MultiLineBox::setPositionFunc)
			.def("setVisible",				&CEGUI::MultiLineBox::setVisibleFunc)
			.def("setAlign",				&CEGUI::MultiLineBox::setAlign)
			.def("setEachLineHeight",		&CEGUI::MultiLineBox::setEachLineHeight)
			.def("setScrollbarVisible",		&CEGUI::MultiLineBox::setScrollbarVisible)
			.def("setScrollbarPosition",	&CEGUI::MultiLineBox::setScrollbarPosition)
			.def("addElementText",			(int(CEGUI::MultiLineBox::*)(std::string, std::string, std::string, bool)) &CEGUI::MultiLineBox::addElementText)
			.def("addElementText",			(int(CEGUI::MultiLineBox::*)(std::string, std::string, bool)) &CEGUI::MultiLineBox::addElementText)
			.def("addElementText",			(int(CEGUI::MultiLineBox::*)(std::string, bool)) &CEGUI::MultiLineBox::addElementText)
			.def("addElementImage",			(int(CEGUI::MultiLineBox::*)(std::string, std::string, float, float, bool)) &CEGUI::MultiLineBox::addElementImage)
			.def("addElementHyperLink",		(int(CEGUI::MultiLineBox::*)(std::string, std::string, std::string, std::string, bool)) &CEGUI::MultiLineBox::addElementHyperLink)
			.def("addElementHyperLink",		(int(CEGUI::MultiLineBox::*)(std::string, std::string, std::string, bool)) &CEGUI::MultiLineBox::addElementHyperLink)
			.def("addElementHyperLink",		(int(CEGUI::MultiLineBox::*)(std::string, std::string, bool)) &CEGUI::MultiLineBox::addElementHyperLink)
			.def("addElementWin",			(int(CEGUI::MultiLineBox::*)(std::string, bool)) &CEGUI::MultiLineBox::addElementWin)
			.def("getFormatHeight",			&CEGUI::MultiLineBox::getFormatHeight)
			.def("getLineSize",				&CEGUI::MultiLineBox::getLineSize)
			.def("getWidthInterval",		&CEGUI::MultiLineBox::getWidthInterval)
			.def("getHeightInterval",		&CEGUI::MultiLineBox::getHeightInterval)
			.def("getFitWidth",				(float(CEGUI::MultiLineBox::*)(std::string, std::string)) &CEGUI::MultiLineBox::getFitWidth)
			.def("getFitWidth",				(float(CEGUI::MultiLineBox::*)(float)) &CEGUI::MultiLineBox::getFitWidth)
			.def("getRenderWidth",			&CEGUI::MultiLineBox::getRenderWidth)
			.def("getRenderHeight",			&CEGUI::MultiLineBox::getRenderHeight)
			.def("getName",					&CEGUI::MultiLineBox::getNameFunc)
			.def("getFontName",				&CEGUI::MultiLineBox::getFontName)
			.def("getColour",				&CEGUI::MultiLineBox::getColour)
			.def("getLength",				&CEGUI::MultiLineBox::getLength)
			.def("getHyperLinkId",			&CEGUI::MultiLineBox::getHyperLinkId)
			.def("getTextContent",			&CEGUI::MultiLineBox::getTextContent)
			.def("getTriggerRect",			&CEGUI::MultiLineBox::getTriggerRect)
			.def("isScrollbarVisible",		&CEGUI::MultiLineBox::isScrollbarVisible)
			.def("getScrollbarDocumentSize",&CEGUI::MultiLineBox::getScrollbarDocumentSize)
			.def("getScrollbarPosition",	&CEGUI::MultiLineBox::getScrollbarPosition)
			.def("clearUp",					&CEGUI::MultiLineBox::clearUp)
			.def("eraseElement",			&CEGUI::MultiLineBox::eraseElement)
			.def("findElementText",			&CEGUI::MultiLineBox::findElementText)
			,
			//	图文混排解析类
			class_<MultiLineBoxParse>("MultiLineBoxParse")
			.def(constructor<>())
			.def("getData",					(int(MultiLineBoxParse::*)(std::string, std::string)) &MultiLineBoxParse::getData)
			,
			luabind::def("createMultiLineBoxParse",					&createMultiLineBoxParse)
			,
			//	动态控件面板容器
			class_<DynamicCtrlVector>("DynamicCtrlVector")
			.def(constructor<>())
			.def("initialise",				(void(DynamicCtrlVector::*)(std::string)) &DynamicCtrlVector::initialise)
			.def("addControl",				(void(DynamicCtrlVector::*)(std::string, float, float)) &DynamicCtrlVector::addControl)
			.def("addControl",				(void(DynamicCtrlVector::*)(std::string)) &DynamicCtrlVector::addControl)
			.def("controlTypeset",			&DynamicCtrlVector::controlTypeset)
			.def("setScrollbarVisible",		&DynamicCtrlVector::setScrollbarVisible)
			.def("setScrollbarPosition",	&DynamicCtrlVector::setScrollbarPosition)
			.def("setInterval",				&DynamicCtrlVector::setInterval)
			.def("setHorInterval",			&DynamicCtrlVector::setHorInterval)
			.def("setVerInterval",			&DynamicCtrlVector::setVerInterval)
			.def("setAlign",				&DynamicCtrlVector::setAlign)
			.def("setAtRight",				&DynamicCtrlVector::setAtRight)
			.def("getScrollbarWidth",		&DynamicCtrlVector::getScrollbarWidth)
			.def("removeAllControl",		&DynamicCtrlVector::removeAllControl)
			,
			luabind::def("createDynamicCtrlVector",			&createDynamicCtrlVector)
			,
			//	图像组合控件
			class_<ImageCombineCtrl>("ImageCombineCtrl")
			.def(constructor<>())
			.def("init",					&ImageCombineCtrl::init)
			.def("addImage",				&ImageCombineCtrl::addImage)
			.def("clear",					&ImageCombineCtrl::clear)
			.def("setWindowImage",			&ImageCombineCtrl::setWindowImage)
			,
			luabind::def("createImageCombineCtrl",			&createImageCombineCtrl)
			,
			//	聊天表情表管理器
			class_<ChatExpressionTableManager>("ChatExpressionTableManager")
			.def(constructor<>())
			.def("getChatExpression",		&ChatExpressionTableManager::getChatExpression)
			.def("getTransfer",				&ChatExpressionTableManager::getTransfer)
			.def("getExpressionCount",		&ChatExpressionTableManager::getExpressionCount)
			,
			luabind::def("getChatExpressionTableManager",	&ChatExpressionTableManager::getSingleton)
			,
			//	公告系统
			class_<Scroll::Msg>("ScrollMsg")
			.def_readonly("id",				&Scroll::Msg::id)
			.def_readonly("content",		&Scroll::Msg::content)
			.def_readonly("playTimes",		&Scroll::Msg::playTimes)
			,
			class_<BroadCastSystem>("BroadCastSystem")
			.def(constructor<>())
			.def("saveMsgAT",				&BroadCastSystem::saveMsgAT)
			.def("deleteMsgAT",				&BroadCastSystem::deleteMsgAT)
			.def("isPlayingMsgAT",			&BroadCastSystem::isPlayingMsgAT)
			.def("setPlayParamsAT",			&BroadCastSystem::setPlayParamsAT)
			.def("setCalledFuncNameAT",		&BroadCastSystem::setCalledFuncNameAT)
			.def("playAT",					&BroadCastSystem::playAT)
			.def("stopAT",					&BroadCastSystem::stopAT)
			.def("saveOldMsgAT",			&BroadCastSystem::saveOldMsgAT)
			.def("playStateAT",				&BroadCastSystem::playStateAT)
			.def("saveMsgGM",				&BroadCastSystem::saveMsgGM)
			.def("deleteMsgGM",				&BroadCastSystem::deleteMsgGM)
			.def("isPlayingMsgGM",			&BroadCastSystem::isPlayingMsgGM)
			.def("setPlayParamsGM",			&BroadCastSystem::setPlayParamsGM)
			.def("setCalledFuncNameGM",		&BroadCastSystem::setCalledFuncNameGM)
			.def("playGM",					&BroadCastSystem::playGM)
			.def("stopGM",					&BroadCastSystem::stopGM)
			.def("saveMsgBE",				&BroadCastSystem::saveMsgBE)
			.def("setPlayParamsBE",			&BroadCastSystem::setPlayParamsBE)
			.def("setCalledFuncNameBE",		&BroadCastSystem::setCalledFuncNameBE)
			.def("playBE",					&BroadCastSystem::playBE)
			.def("stopBE",					&BroadCastSystem::stopBE)
			.def("reqBroadCast",			&BroadCastSystem::reqBroadCast)
			,
			luabind::def("getBroadCastSystem",	&BroadCastSystem::getSingleton)
			,
			//	动态创建imageset管理器
			class_<DynamicCreateImagesetMgr>("DynamicCreateImagesetMgr")
			.def(constructor<>())
			.def("saveImage",				&DynamicCreateImagesetMgr::saveImage)
			.def("saveImageEx",				&DynamicCreateImagesetMgr::saveImageEx)
			.def("getImageName",			&DynamicCreateImagesetMgr::getImageName)
			.def("getImagesetName",			&DynamicCreateImagesetMgr::getImagesetName)
			.def("destroyImageset",			&DynamicCreateImagesetMgr::destroyImageset)
			.def("setResourceGroupDirectory",&DynamicCreateImagesetMgr::setResourceGroupDirectory)
			.def("setWindowImage",			(int(DynamicCreateImagesetMgr::*)(std::string, std::string, std::string)) &DynamicCreateImagesetMgr::setWindowImage)
			.def("setWindowImage",			(int(DynamicCreateImagesetMgr::*)(std::string, std::string)) &DynamicCreateImagesetMgr::setWindowImage)
			,
			luabind::def("getDynamicCreateImagesetMgr",	&DynamicCreateImagesetMgr::getSingleton)
			,
			// 暂时写这里(很囧)
			class_<Timekeeper>("Timekeeper")
			.def("getTimeDiff",				&Timekeeper::getTimeDiff)
			.def("getServerTime",			&Timekeeper::getServerTime)
			.def("getTimeNow",				&Timekeeper::getTimeNowTolua)
			,
			luabind::def("getTimekeeper",	&Timekeeper::getSingleton)
			,
			// 微博
			class_<MicroBlogManager>("MicroBlogManager")
			.def("fullScreenShot",			&MicroBlogManager::fullScreenShot)
			.def("partScreenShot",			&MicroBlogManager::partScreenShot)
			.def("SendTo",					&MicroBlogManager::SendTo)
			.def("saveAccessSecret",		&MicroBlogManager::saveAccessSecret)
			.def("getTokenOfQQ",			&MicroBlogManager::getTokenOfQQ)
			,
			luabind::def("getMicroBlogMgr",	&MicroBlogManager::getSingleton)
			,
			// 小地图
			class_<SmallMapManager>("SmallMapManager")
			.def("setActualMapCorner3D",	&SmallMapManager::setActualMapCorner3D)
			.def("getActualMapWidth2D",		&SmallMapManager::getActualMapWidth2D)
			.def("getActualMapHeight2D",	&SmallMapManager::getActualMapHeight2D)
			.def("getSceneBeginPosX",		&SmallMapManager::getSceneBeginPosX)
			.def("getSceneBeginPosY",		&SmallMapManager::getSceneBeginPosY)
			.def("getSceneEndPosX",			&SmallMapManager::getSceneEndPosX)
			.def("getSceneEndPosY",			&SmallMapManager::getSceneEndPosY)
			,
			luabind::def("getSmallMapMgr",	&SmallMapManager::getSingleton)
		];

	return true;
}
