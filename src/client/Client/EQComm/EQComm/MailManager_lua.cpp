/***********************************************************************
* 作者: xiaoMing
* 时间: 2010-09-01
* 描述: 负责将MailManager的函数透露给LUA
***********************************************************************/
#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>
#include "LuaManager.h"

#include "MailManager.h"

using namespace luabind;

bool CLuaManager::RegisterMailManagerClass()
{
	module( EQ_GLOBA_CLASS )
	[
		/**
		*/
		luabind::def("getMailManager", &MailManager::getSingleton),
		/**
		*/
		class_<Mail>("Mail")
		.def(constructor<>())
		.def_readonly("type",			&Mail::type)
		.def_readwrite("read_status",	&Mail::read_status)
		.def_readonly("remain_time",	&Mail::remain_time)
		.def_readonly("isReqContent",	&Mail::isReqContent)
		.def_readonly("name",			&Mail::name)
		.def_readonly("account",		&Mail::account)
		.def_readonly("title",			&Mail::title)
		.def_readonly("content",		&Mail::content)
		,

		class_<MailItem>("MailItem")
		.def(constructor<>())
		.def_readonly("instanceId",		&MailItem::mInstanceId)
		.def_readonly("templateId",		&MailItem::mTemplateId)
		.def_readonly("count",			&MailItem::mCount)
		,


		class_<MailManager>("MailManager")
			.def(constructor<>())
			.def_readonly("mReqMailListFlag",&MailManager::mReqMailListFlag)


			.def("sendMail",			 &MailManager::sendMail)
			.def("setSendMailItem",		 &MailManager::setSendMailItem)
			.def("getSendMailItemId",	 &MailManager::getSendMailItemId)
			.def("getSendMailItemEffectTime",&MailManager::getSendMailItemEffectTime)
			.def("getSendMailItemCount", &MailManager::getSendMailItemCount)
			.def("swapItem",			 &MailManager::swapItem)
			.def("reqMailList",			 &MailManager::reqMailList)	
			.def("getPageLastIndex",	 &MailManager::getPageLastIndex)
			.def("reqKnowMailList",		 &MailManager::reqKnowMailList)
			.def("getTotalPage",		 &MailManager::getTotalPage)
			.def("reqMailContent",		 &MailManager::reqMailContent)
			.def("getCurMailItem",		 &MailManager::getCurMailItem)
			.def("getCurMailItemEffectTime",&MailManager::getCurMailItemEffectTime)
			.def("getCurMailItemCount",	 &MailManager::getCurMailItemCount)	
			.def("reqGetMailItem",		 &MailManager::reqGetMailItem)	
			.def("reqDeleteMail",		 &MailManager::reqDeleteMail)
			.def("reqReturnMail",		 &MailManager::reqReturnMail)
			.def("getCurMailType",		 &MailManager::getCurMailType)	
			.def("getCurMail",		     &MailManager::getCurMail)
			.def("isAllReaded",			 &MailManager::isAllReaded)
			.def("isAnimating",			 &MailManager::isAnimating)
			.def("removeAnimation",		 &MailManager::removeAnimation)
			.def("backSendItem",		 &MailManager::backSendItem)
			.def("isFull",				 &MailManager::isFull)
			.def("sendMailHaveItem",	 &MailManager::sendMailHaveItem)
			.def("curMailHaveItem",		 &MailManager::curMailHaveItem)
			.def("clearSendMail",		 &MailManager::clearSendMail)
			.def("updateItems",			 &MailManager::updateItems)
	];
	return true;
}