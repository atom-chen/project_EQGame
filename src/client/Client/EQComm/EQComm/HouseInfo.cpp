#include "EQCommPCH.h"
#include "SmallModules.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "NetPacket.h"
#include "ByteArray.h"
#include "NetMsgType.h"
#include "HouseTable.h"
#include "luabind/luabind/luabind.hpp"
#include "LogicTable.h"
#include "LuaManager.h"
#include "CommFunc.h"
#include "EnumDef.h"
#include "GiftLogManager.h"
#include "GiftSystem.h"
#include "MainPlayer.h"
#include "StringHandle.h"
//------------------------------------------------------------------------
// 查看房屋消息
void SmallModules::reqHouseBaseInfo()
{
	req_house_basic_data req;
	sGameMgr.getEQMsgManager()->Send(req);
}
void SmallModules::reqHouseList()
{
	reqHouseListByAcc(MainPlayer::getSingleton().getPlayerAccount().c_str());
}
void SmallModules::reqHouseListByAcc(const char *acc)
{
	req_house_list req;
	req.account = acc;
	sGameMgr.getEQMsgManager()->Send(req);
}
void SmallModules::reqChangeHouse(size_t idx, size_t type)
{
	req_change_house req;
	req.inst_id = mHouseDataList[idx].instance_id;
	req.type = type;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
void SmallModules::reqClearHouseLog()
{
	req_clear_house_log req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
void SmallModules::reqSetHousePermission(int furPerMission, int accessPerMission)
{
	req_set_house_permission req;
	req.furniture_permission = furPerMission;
	req.house_permission = accessPerMission;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------
void SmallModules::reqSetHouseWelWords(const std::string& welWord )
{
	req_set_house_welcome_words req;

	std::string output;
	output = sStringHandle.stringFilter(const_cast<char *>(welWord.c_str()),0);

	req.words = output;
	sGameMgr.getEQMsgManager()->Send(req);	
}
//------------------------------------------------------------------------
//bool SmallModules::handleHouseInfo(UINT16 msgType, ByteArray& recBuff)
//{
//	notify_clear_house_log clearResult;
//	notify_set_house_welcome_words welWords;
//	notify_set_house_permission perMission;
//	notify_house_basic_data houseBaseData;
//	switch(msgType)
//	{
//	case NetMsgType::msg_notify_house_basic_data:
//		{
//			houseBaseData.decode(recBuff);			
//			const PlayerHouseRow* houseRow = HouseTableManager::getSingleton().getPlayerHouseRow( houseBaseData.data.template_id );
//
//			if(NULL == houseRow)
//				break;
//
//			char buyTime[11];
//			sprintf(buyTime, "%04d-%02d-%02d", houseBaseData.data.buy_date.year, 
//				houseBaseData.data.buy_date.month,houseBaseData.data.buy_date.day);
//
//			uint price = 0;
//			HouseSalesTable t = LogicTableManager::getSingleton().getHouseSalesTable();
//			HouseSalesTable::const_iterator iter = t.begin();
//			while(t.end() != iter )
//			{
//				if( iter->second.houseId == houseRow->id)
//				{
//					price = iter->second.price;
//					break;
//				}
//				iter++;
//			}
//
//			// 设置文本
//			CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
//			winMgr->getWindow("housedata/tab1/Tab 1/txt6a")->setText((CEGUI::utf8*)(buyTime));
//			winMgr->getWindow("housedata/tab1/Tab 1/txt8a")->setText((CEGUI::utf8*)(houseBaseData.data.welcome_words.c_str()));
//			winMgr->getWindow("housedata/tab1/Tab 1/txt1a")->setText((CEGUI::utf8*)(houseRow->name.c_str()));
//
//			luabind::call_function<void>(sLuaMgr.getL(), "SetHouseBaseInfo",houseBaseData.data.house_permission, 
//				houseBaseData.data.furniture_permission, houseRow->level, houseRow->maxPlayer, houseRow->type, price);
//			// 保存访问日志
//			std::string time;
//			uint size = houseBaseData.data.visit_log.size();
//			for(uint i = 0; i < size; i++)
//			{
//				time = getTimeString(houseBaseData.data.visit_log[i].datetime);
//				luabind::call_function<void>(sLuaMgr.getL(), "SaveHouseLog", (GB2312ToUTF8)(time.c_str()), 
//					houseBaseData.data.visit_log[i].username, houseBaseData.data.visit_log[i].type, i, size);
//			}
//		}
//		break;
//	case NetMsgType::msg_notify_clear_house_log:
//		{
//			clearResult.decode(recBuff);
//			if( chlr_ok == clearResult.result )
//			{
//				CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
//				winMgr->getWindow("housedata/tab1/Tab 3/txt1")->setText("");
//			}
//		}
//		break;
//	case NetMsgType::msg_notify_set_house_welcome_words:
//		{
//			welWords.decode(recBuff);
//			luabind::call_function<void>(sLuaMgr.getL(), "CloseHouseInfoUI");
//		}
//		break;
//	case NetMsgType::msg_notify_set_house_permission:
//		{
//			perMission.decode(recBuff);
//			luabind::call_function<void>(sLuaMgr.getL(), "CloseHouseInfoUI");
//		}
//		break;
//
//	//------------------------------------------------------------------------
//	// 礼物记录
//	case NetMsgType::msg_notify_get_send_express_vec:
//		{// 收到的快递型礼物
//			notify_get_send_express_vec data;
//			data.decode(recBuff);
//
//			GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::MAIL_SEND, data.send_express_vec, data.total);
//		}
//		break;
//
//	case NetMsgType::msg_notify_get_send_box_vec:
//		{// 收到的手工制作的礼物
//			notify_get_send_box_vec data;
//			data.decode(recBuff);
//
//			GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::DIY_SEND, data.send_box_vec, data.total);
//		}
//		break;
//
//	case NetMsgType::msg_notify_get_receive_box_vec:
//		{
//			notify_get_receive_box_vec data;	
//			data.decode(recBuff);
//
//			GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::DIY_RECIVE_NO_VIEW, data.receive_box_vec, data.total);
//		}
//		break;
//
//	case NetMsgType::msg_notify_get_viewed_box_vec:
//		{
//			notify_get_viewed_box_vec data;
//			data.decode(recBuff);
//
//			GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::DIY_RECIVE_VIEW, data.viewed_box_vec, data.total);
//		}
//		break;
//
//	case NetMsgType::msg_notify_get_receive_express_vec:
//		{
//			notify_get_receive_express_vec data;
//			data.decode(recBuff);
//
//			GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::MAIL_RECIVE_NO_VIEW, data.receive_express_vec, data.total);
//		}
//		break;
//
//	case NetMsgType::msg_notify_get_viewed_express_vec:
//		{
//			notify_get_viewed_express_vec data;
//			data.decode(recBuff);
//
//			GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::MAIL_RECIVE_VIEW, data.viewed_express_vec, data.total);
//		}
//		break;
//
//	default:return false;
//	}
//	return true;
//}

void SmallModules::handle_house_basic_data(MsgPack &recBuff)
{
	notify_house_basic_data houseBaseData;
	houseBaseData.decode(recBuff);			
	const PlayerHouseRow* houseRow = HouseTableManager::getSingleton().getPlayerHouseRow( houseBaseData.data.template_id );

	if(NULL == houseRow)
		return;

	char buyTime[11];
	sprintf(buyTime, "%04d-%02d-%02d", houseBaseData.data.buy_date.year, 
		houseBaseData.data.buy_date.month,houseBaseData.data.buy_date.day);

	uint price = 0;
	HouseSalesTable t = LogicTableManager::getSingleton().getHouseSalesTable();
	HouseSalesTable::const_iterator iter = t.begin();
	while(t.end() != iter )
	{
		if( iter->second.houseId == houseRow->id)
		{
			price = iter->second.price;
			break;
		}
		iter++;
	}

	// 设置文本
	CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
	winMgr->getWindow("housedata/tab1/Tab 1/txt6a")->setText((CEGUI::utf8*)(buyTime));
	winMgr->getWindow("housedata/tab1/Tab 1/txt8a")->setText((CEGUI::utf8*)(houseBaseData.data.welcome_words.c_str()));
	winMgr->getWindow("housedata/tab1/Tab 1/txt1a")->setText((CEGUI::utf8*)(houseRow->name.c_str()));

	luabind::call_function<void>(sLuaMgr.getL(), "SetHouseBaseInfo",houseBaseData.data.house_permission, 
		houseBaseData.data.furniture_permission, houseRow->level, houseRow->maxPlayer, houseRow->type, price, houseBaseData.data.house_clean);
	// 保存访问日志
	std::string time;
	uint size = houseBaseData.data.visit_log.size();
	for(uint i = 0; i < size; i++)
	{
		time = getTimeString(houseBaseData.data.visit_log[i].datetime);
		luabind::call_function<void>(sLuaMgr.getL(), "SaveHouseLog", (GB2312ToUTF8)(time.c_str()), 
			houseBaseData.data.visit_log[i].username,
			houseBaseData.data.visit_log[i].type,
			houseBaseData.data.visit_log[i].item_id);
	}
}

void SmallModules::handle_house_clean(MsgPack &recBuff)
{
	notify_house_clean data;
	data.decode(recBuff);
	luabind::call_function<void>(sLuaMgr.getL(), "HouseInfo_setClean", data.house_clean);
}

void SmallModules::handle_clear_house_log(MsgPack &recBuff)
{
	notify_clear_house_log clearResult;
	clearResult.decode(recBuff);
	if( chlr_ok == clearResult.result )
	{
		CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
		winMgr->getWindow("housedata/tab1/Tab 3/txt1")->setText("");
	}
}

void SmallModules::handle_set_house_welcome_words(MsgPack &recBuff)
{
	notify_set_house_welcome_words welWords;
	welWords.decode(recBuff);
	luabind::call_function<void>(sLuaMgr.getL(), "CloseHouseInfoUI");
}

void SmallModules::handle_set_house_permission(MsgPack &recBuff)
{
	notify_set_house_permission perMission;
	perMission.decode(recBuff);
	luabind::call_function<void>(sLuaMgr.getL(), "CloseHouseInfoUI");
}

// 礼物记录
void SmallModules::handle_get_send_express_vec(MsgPack &recBuff)
{// 收到的快递型礼物
	notify_get_send_express_vec data;
	data.decode(recBuff);

	GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::MAIL_SEND, data.send_express_vec, data.total);
}

void SmallModules::handle_get_send_box_vec(MsgPack &recBuff)
{// 收到的手工制作的礼物
	notify_get_send_box_vec data;
	data.decode(recBuff);

	GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::DIY_SEND, data.send_box_vec, data.total);
}

void SmallModules::handle_get_receive_box_vec(MsgPack &recBuff)
{
	notify_get_receive_box_vec data;	
	data.decode(recBuff);

	GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::DIY_RECIVE_NO_VIEW, data.receive_box_vec, data.total);
}

void SmallModules::handle_get_viewed_box_vec(MsgPack &recBuff)
{
	notify_get_viewed_box_vec data;
	data.decode(recBuff);

	GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::DIY_RECIVE_VIEW, data.viewed_box_vec, data.total);
}

void SmallModules::handle_get_receive_express_vec(MsgPack &recBuff)
{
	notify_get_receive_express_vec data;
	data.decode(recBuff);

	GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::MAIL_RECIVE_NO_VIEW, data.receive_express_vec, data.total);
}

void SmallModules::handle_get_viewed_express_vec(MsgPack &recBuff)
{
	notify_get_viewed_express_vec data;
	data.decode(recBuff);

	GiftLogManager::getSingleton().recGiftLogData(GiftLogManager::MAIL_RECIVE_VIEW, data.viewed_express_vec, data.total);
}

void SmallModules::handle_house_list(MsgPack &recBuff)
{
	notify_house_list data;
	data.decode(recBuff);

	THouseDataList *pHouseDataList;
	std::string sCallScriptFun;

	if(data.account == MainPlayer::getSingleton().getPlayerAccount())
	{
		pHouseDataList = &mHouseDataList;
		sCallScriptFun = "OnUpdateHouseList";
	}
	else
	{
		pHouseDataList = &mFHouseDataList;
		sCallScriptFun = "OnUpdateFriendHouseList";
	}

	pHouseDataList->clear();
	for(size_t i = 0; i < data.house_vec.size(); i ++)
	{
		THouseData hdata;
		hdata.used = 0;
		hdata.empty = 1;
		const PlayerHouseRow* houseRow = HouseTableManager::getSingleton().getPlayerHouseRow( data.house_vec[i].template_id );
		if(houseRow)
		{
			hdata.empty = 0;
			hdata.name = houseRow->name;
			hdata.level = houseRow->level;
			hdata.max_player = houseRow->maxPlayer;
			hdata.type = houseRow->type;
			hdata.used = data.house_vec[i].instance_id && !data.house_vec[i].plot_id ? 1 : 0;
			hdata.template_id = data.house_vec[i].template_id;
			hdata.instance_id = data.house_vec[i].instance_id;

			char buyTime[11];
			sprintf(buyTime, "%04d-%02d-%02d",
				data.house_vec[i].buy_date.year, 
				data.house_vec[i].buy_date.month,
				data.house_vec[i].buy_date.day);

			hdata.buy_time = buyTime;

			HouseSalesTable t = LogicTableManager::getSingleton().getHouseSalesTable();
			HouseSalesTable::const_iterator iter = t.begin();
			while(t.end() != iter )
			{
				if( iter->second.houseId == houseRow->id)
				{
					hdata.price = iter->second.price;
					break;
				}
				iter++;
			}
		}
		pHouseDataList->push_back(hdata);
	}

	luabind::call_function<void>(sLuaMgr.getL(), sCallScriptFun.c_str());

}

void SmallModules::handle_change_house(MsgPack &recBuff)
{
	notify_change_house data;
	data.decode(recBuff);

	int i = 0;
	for(THouseDataList::iterator itr = mHouseDataList.begin(); itr != mHouseDataList.end(); ++itr)
	{
		if(itr->instance_id == data.inst_id)
		{
			for(size_t x = 0; x < mHouseDataList.size(); x ++)
				mHouseDataList[x].used = 0;
			mHouseDataList[i].used = 1;
			break;
		}

		i ++;
	}

	luabind::call_function<void>(sLuaMgr.getL(), "OnChangeHouse");

}

//------------------------------------------------------------------------
void SmallModules::reqGiftLog(int type, uint pageNum) const
{
	// 不需要请求
	if(!GiftLogManager::getSingleton().needReqLog(static_cast<GiftLogManager::GiftLogType>(type), pageNum))
	{
		GiftLogManager::getSingleton().showLog(static_cast<GiftLogManager::GiftLogType>(type), pageNum);
		return;
	}

	switch(static_cast<GiftLogManager::GiftLogType>(type))
	{
	case GiftLogManager::DIY_SEND:
		{
			req_get_send_box_vec req;
			req.page_count = GiftLogManager::PerPageLogCount;
			req.page_index = pageNum;

			sGameMgr.getEQMsgManager()->Send(req);
		}
		break;

	case GiftLogManager::DIY_RECIVE_NO_VIEW:
		{
			req_get_receive_box_vec req;
			req.page_count = GiftLogManager::PerPageLogCount;
			req.page_index = pageNum;

			sGameMgr.getEQMsgManager()->Send(req);
		}
		break;

	case GiftLogManager::DIY_RECIVE_VIEW:
		{
			req_get_viewed_box_vec req;
			req.page_count = GiftLogManager::PerPageLogCount;
			req.page_index = pageNum;

			sGameMgr.getEQMsgManager()->Send(req);
		}
		break;

	case GiftLogManager::MAIL_SEND:
		{
			req_get_send_express_vec req;
			req.page_count = GiftLogManager::PerPageLogCount;
			req.page_index = pageNum;

			sGameMgr.getEQMsgManager()->Send(req);
		}
		break;

	case GiftLogManager::MAIL_RECIVE_NO_VIEW:
		{
			req_get_receive_express_vec req;
			req.page_count = GiftLogManager::PerPageLogCount;
			req.page_index = pageNum;

			sGameMgr.getEQMsgManager()->Send(req);
		}
		break;

	case GiftLogManager::MAIL_RECIVE_VIEW:
		{
			req_get_viewed_express_vec req;
			req.page_count = GiftLogManager::PerPageLogCount;
			req.page_index = pageNum;

			sGameMgr.getEQMsgManager()->Send(req);
		}
		break;
	}
}
//------------------------------------------------------------------------
void SmallModules::showLogGift(const char *instanceIDStr) const
{
	uint64 instanceId = _atoi64(instanceIDStr);
	const GiftInfo* giftInfo = GiftLogManager::getSingleton().getGiftInfo(instanceId);

	if(NULL == giftInfo)
		return;

	GiftLogManager::getSingleton().setCurGiftInstanceId(instanceId);

	luabind::call_function<void>(sLuaMgr.getL(), "ShowLogGift"
								 ,giftInfo->greetings
								 ,giftInfo->giverName
								 ,giftInfo->itemTemplateId
								 ,giftInfo->effecTime
								 ,giftInfo->itemOverlay
								 ,giftInfo->cardId
								 ,giftInfo->giverAcc
								 ,giftInfo->expressType);
}
//------------------------------------------------------------------------
void SmallModules::showLogGiftByIcon(const char *iid) const
{
	uint64 instanceId = _atoi64(iid);

	std::map<uint64, gift_express>::const_iterator itr = mRecvExpressMap.find(instanceId);
	if(itr == mRecvExpressMap.end())
		return;

	GiftLogManager::getSingleton().setCurGiftInstanceId(instanceId);

	GiftInfo info;
	GiftSystem::getSingleton().converDataToGiftInfo(itr->second, info);
	luabind::call_function<void>(sLuaMgr.getL(), "ShowLogGift"
		,info.greetings
		,info.giverName
		,info.itemTemplateId
		,info.effecTime
		,info.itemOverlay
		,info.cardId
		,info.giverAcc
		,info.expressType);
}
void SmallModules::delLogGiftByIcon(const char *iid)
{
	uint64 instanceId = _atoi64(iid);

	std::map<uint64, gift_express>::iterator itr = mRecvExpressMap.find(instanceId);
	if(itr == mRecvExpressMap.end())
		return;

	mRecvExpressMap.erase(itr);

}
//------------------------------------------------------------------------
void SmallModules::initGiftLog()
{
	GiftLogManager::getSingleton().cleanIndex();
}
//------------------------------------------------------------------------
void SmallModules::reqCleanGiftLog(int type) const
{
	req_clean_gift_log req;
	req.type = type;

	sGameMgr.getEQMsgManager()->Send(req);
}