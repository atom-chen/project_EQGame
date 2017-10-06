#include "EQCommPCH.h"
#include "SmallModules.h"

#include "ByteArray.h"
#include "NetMsgType.h"
#include "NetPacket.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "GameEventMgr.h"

#include "ObjMgr.h"
#include "Player.h"
#include "GameObj.h"
#include "CommFunc.h"

//-------------------------------------------------------------------------------
//bool SmallModules::handleByTrade(UINT16 msgType, ByteArray &recBuff)
//{
//	notify_start_trade startTrade;
//	notify_put_trade_item notifyPutItem;
//	notify_update_trade_money notifyUpdateMoney;
//	notify_unput_trade_item notifyUnputItem;
//	notify_swap_trade_item notifySwapItem;
//	notify_other_swap_trade_item notifyOtherSwap;
//
//	switch(msgType)
//	{
//		// 通知交易开始
//	case NetMsgType::msg_notify_start_trade:
//		initTradePack();
//
//		endCheckAnswer();
//
//		startTrade.decode(recBuff);
//		mTraderAcc = startTrade.account;
//
//		GameEventMgr::getSingleton().fireEvent("TRADE_START");
//		return true;
//
//		// 通知更新己方交易的钱
//	case NetMsgType::msg_notify_update_trade_money:
//		// 重新计时
//		endCheckAnswer();
//
//		notifyUpdateMoney.decode(recBuff);
//		mPutMoney = notifyUpdateMoney.money;
//
//		GameEventMgr::getSingleton().fireEvent("MY_TRADE_MONEY");
//		return true;
//
//		// 通知更新对方交易的钱
//	case NetMsgType::msg_notify_other_update_trade_money:
//		// 结束交易响应检测
//		endCheckAnswer();
//
//		notifyUpdateMoney.decode(recBuff);
//		mGetMoney = notifyUpdateMoney.money;
//
//		GameEventMgr::getSingleton().fireEvent("OTHER_TRADE_MONEY");
//		return true;
//
//		// 通知对方交易栏更新
//		// 添加一个交易物品
//	case NetMsgType::msg_notify_other_put_trade_item:
//		{
//			// 结束交易响应检测
//			endCheckAnswer();
//
//			notifyPutItem.decode(recBuff);
//			int year = notifyPutItem.item_data.del_time.year;
//			int month = notifyPutItem.item_data.del_time.month;
//			int day = notifyPutItem.item_data.del_time.day;
//			int hour = notifyPutItem.item_data.del_time.hour;
//			int minute= notifyPutItem.item_data.del_time.minute;
//			int second = notifyPutItem.item_data.del_time.second;
//			std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
//			setTradePack(notifyPutItem.trade_index, notifyPutItem.item_data.template_id, delTime, notifyPutItem.item_count, OTHER);
//
//			GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
//			return true;
//		}
//		//撤除一个交易物品
//	case NetMsgType::msg_notify_other_unput_trade_item:
//		// 结束交易响应检测
//		endCheckAnswer();
//
//		notifyUnputItem.decode(recBuff);
//		setTradePack(notifyUnputItem.trade_index, -1, 0, OTHER);
//
//		GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
//		return true;
//
//		// 通知己方交易栏更新
//		// 添加一个交易物品
//	case NetMsgType::msg_notify_put_trade_item:
//		{
//			// 重新计时
//			endCheckAnswer();
//
//			notifyPutItem.decode(recBuff);
//			int year = notifyPutItem.item_data.del_time.year;
//			int month = notifyPutItem.item_data.del_time.month;
//			int day = notifyPutItem.item_data.del_time.day;
//			int hour = notifyPutItem.item_data.del_time.hour;
//			int minute= notifyPutItem.item_data.del_time.minute;
//			int second = notifyPutItem.item_data.del_time.second;
//			std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
//			setTradePack(notifyPutItem.trade_index, notifyPutItem.item_data.template_id, delTime, notifyPutItem.item_count, MYSELF);
//
//			GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
//			return true;
//		}
//		//撤除一个交易物品
//	case NetMsgType::msg_notify_unput_trade_item:
//		// 重新计时
//		endCheckAnswer();
//
//		notifyUnputItem.decode(recBuff);
//		setTradePack(notifyUnputItem.trade_index, -1, 0, MYSELF);
//
//		GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
//		return true;
//
//	case NetMsgType::msg_notify_swap_trade_item:
//		//重新计时
//		endCheckAnswer();
//		notifySwapItem.decode(recBuff);
//
//		swapPackItem(notifySwapItem.index1, notifySwapItem.index2, MYSELF);
//		GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
//		return true;
//	case NetMsgType::msg_notify_other_swap_trade_item:
//		// 结束交易响应检测
//		endCheckAnswer();
//		notifyOtherSwap.decode(recBuff);
//
//		swapPackItem(notifyOtherSwap.index1, notifyOtherSwap.index2, OTHER);
//		GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
//		return true;
//		// 通知已方确定交易
//	case NetMsgType::msg_notify_sure_trade:
//		endCheckAnswer();
//
//		GameEventMgr::getSingleton().fireEvent("MY_TRADE_SURE");
//		return true;
//		// 通知对方确定交易
//	case NetMsgType::msg_notify_other_sure_trade:
//		GameEventMgr::getSingleton().fireEvent("OTHER_TRADE_SURE");
//
//		// 开始检测响应时间
//		startCheckAnswer(ANSWER_TRADE);
//
//		return true;
//
//		// 通知取消交易
//	case NetMsgType::msg_notify_cancel_trade:
//		endCheckAnswer();
//
//		GameEventMgr::getSingleton().fireEvent("TRADE_CANCEL");
//		return true;
//
//		// 通知交易成功
//	case NetMsgType::msg_notify_trade_success:
//		endCheckAnswer();
//
//		GameEventMgr::getSingleton().fireEvent("TRADE_SUCCESS");
//		return true;
//
//	default:
//		return false;
//	}
//}

void SmallModules::handle_start_trade(MsgPack &recBuff)
{
	// 通知交易开始
	initTradePack();

	endCheckAnswer();

	notify_start_trade startTrade;
	startTrade.decode(recBuff);
	mTraderAcc = startTrade.account;

	GameEventMgr::getSingleton().fireEvent("TRADE_START");
}

void SmallModules::handle_update_trade_money(MsgPack &recBuff)
{
	// 通知更新己方交易的钱
	// 重新计时
	endCheckAnswer();

	notify_update_trade_money notifyUpdateMoney;
	notifyUpdateMoney.decode(recBuff);
	mPutMoney = notifyUpdateMoney.money;

	GameEventMgr::getSingleton().fireEvent("MY_TRADE_MONEY");
}

void SmallModules::handle_other_update_trade_money(MsgPack &recBuff)
{
	// 通知更新对方交易的钱
	// 结束交易响应检测
	endCheckAnswer();

	notify_update_trade_money notifyUpdateMoney;
	notifyUpdateMoney.decode(recBuff);
	mGetMoney = notifyUpdateMoney.money;

	GameEventMgr::getSingleton().fireEvent("OTHER_TRADE_MONEY");
}

void SmallModules::handle_other_put_trade_item(MsgPack &recBuff)
{
	// 通知对方交易栏更新
	// 添加一个交易物品

	// 结束交易响应检测
	endCheckAnswer();

	notify_put_trade_item notifyPutItem;
	notifyPutItem.decode(recBuff);
	int year = notifyPutItem.item_data.del_time.year;
	int month = notifyPutItem.item_data.del_time.month;
	int day = notifyPutItem.item_data.del_time.day;
	int hour = notifyPutItem.item_data.del_time.hour;
	int minute= notifyPutItem.item_data.del_time.minute;
	int second = notifyPutItem.item_data.del_time.second;
	std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
	setTradePack(notifyPutItem.trade_index, notifyPutItem.item_data.template_id, delTime, notifyPutItem.item_count, OTHER);

	GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
}

void SmallModules::handle_other_unput_trade_item(MsgPack &recBuff)
{
	//撤除一个交易物品
	// 结束交易响应检测
	endCheckAnswer();

	notify_unput_trade_item notifyUnputItem;
	notifyUnputItem.decode(recBuff);
	setTradePack(notifyUnputItem.trade_index, -1, 0, OTHER);

	GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
}

void SmallModules::handle_put_trade_item(MsgPack &recBuff)
{
	// 通知己方交易栏更新
	// 添加一个交易物品

	// 重新计时
	endCheckAnswer();

	notify_put_trade_item notifyPutItem;
	notifyPutItem.decode(recBuff);
	int year = notifyPutItem.item_data.del_time.year;
	int month = notifyPutItem.item_data.del_time.month;
	int day = notifyPutItem.item_data.del_time.day;
	int hour = notifyPutItem.item_data.del_time.hour;
	int minute= notifyPutItem.item_data.del_time.minute;
	int second = notifyPutItem.item_data.del_time.second;
	std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
	setTradePack(notifyPutItem.trade_index, notifyPutItem.item_data.template_id, delTime, notifyPutItem.item_count, MYSELF);

	GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
}

void SmallModules::handle_unput_trade_item(MsgPack &recBuff)
{
	//撤除一个交易物品
	// 重新计时
	endCheckAnswer();

	notify_unput_trade_item notifyUnputItem;
	notifyUnputItem.decode(recBuff);
	setTradePack(notifyUnputItem.trade_index, -1, 0, MYSELF);

	GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
}

void SmallModules::handle_swap_trade_item(MsgPack &recBuff)
{
	//重新计时
	endCheckAnswer();

	notify_swap_trade_item notifySwapItem;
	notifySwapItem.decode(recBuff);

	swapPackItem(notifySwapItem.index1, notifySwapItem.index2, MYSELF);
	GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
}

void SmallModules::handle_other_swap_trade_item(MsgPack &recBuff)
{
	// 结束交易响应检测
	endCheckAnswer();

	notify_other_swap_trade_item notifyOtherSwap;
	notifyOtherSwap.decode(recBuff);

	swapPackItem(notifyOtherSwap.index1, notifyOtherSwap.index2, OTHER);
	GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
}

void SmallModules::handle_sure_trade(MsgPack &recBuff)
{
	// 通知已方确定交易
	endCheckAnswer();

	GameEventMgr::getSingleton().fireEvent("MY_TRADE_SURE");
}

void SmallModules::handle_other_sure_trade(MsgPack &recBuff)
{
	// 通知对方确定交易
	GameEventMgr::getSingleton().fireEvent("OTHER_TRADE_SURE");

	// 开始检测响应时间
	startCheckAnswer(ANSWER_TRADE);
}

void SmallModules::handle_cancel_trade(MsgPack &recBuff)
{
	endCheckAnswer();

	GameEventMgr::getSingleton().fireEvent("TRADE_CANCEL");
}

void SmallModules::handle_trade_success(MsgPack &recBuff)
{
	endCheckAnswer();

	GameEventMgr::getSingleton().fireEvent("TRADE_SUCCESS");
}

//-------------------------------------------------------------------------------
void SmallModules::sendPutItem(int packetIndex, int tradeIndex)
{
	req_put_trade_item reqPutItem;
	reqPutItem.packet_index = packetIndex;
	reqPutItem.trade_index  = tradeIndex;

	sGameMgr.getEQMsgManager()->Send((INetPacket &)reqPutItem);
}

//-------------------------------------------------------------------------------
void SmallModules::sendUpdateMoney(int money)
{
	req_update_trade_money reqMoney;
	reqMoney.money = money;

	sGameMgr.getEQMsgManager()->Send((INetPacket&) reqMoney);
}

//-------------------------------------------------------------------------------
void SmallModules::sendUnputItem(int tradeIndex, int storageIndex)
{
	req_unput_trade_item reqUnputItem;
	reqUnputItem.trade_index  = tradeIndex;
	reqUnputItem.packet_index = storageIndex;
	sGameMgr.getEQMsgManager()->Send((INetPacket&) reqUnputItem);
}

//-------------------------------------------------------------------------------
void SmallModules::reqCancelTrade()
{
	req_cancel_trade reqCancelTrade;

	sGameMgr.getEQMsgManager()->Send((INetPacket&) reqCancelTrade);
}

//-------------------------------------------------------------------------------
void SmallModules::reqSureTrade()
{
	req_sure_trade reqSureTrade;

	sGameMgr.getEQMsgManager()->Send((INetPacket&) reqSureTrade);
}

//-------------------------------------------------------------------------------
void SmallModules::sendSwap(int srcIndex, int destIndex)
{
	req_swap_trade_item reqSwap;
	reqSwap.index1 = srcIndex;
	reqSwap.index2 = destIndex;

	sGameMgr.getEQMsgManager()->Send((INetPacket &) reqSwap);
}
//-------------------------------------------------------------------------------
void SmallModules::setTradePack(int tradeIndex, int itemId, uint count, GridType type)
{
	if(tradeIndex < 0 || tradeIndex > MAX_COUNT)
	{
		// 报错
	}

	switch(type)
	{
	case MYSELF:
		{
			mMyPack[tradeIndex].templateId = itemId;
			mMyPack[tradeIndex].count	   = count;
			// 更新UI
			GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
			return;
		}
	case OTHER:
		{
			mOtherPack[tradeIndex].templateId = itemId;
			mOtherPack[tradeIndex].count	  = count;
			GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
			return;
		}
	}
}
//-------------------------------------------------------------------------------
void SmallModules::setTradePack(int tradeIndex, int itemId, std::string effTime, uint count, GridType type)
{
	if(tradeIndex < 0 || tradeIndex > MAX_COUNT)
	{
		// 报错
	}

	switch(type)
	{
	case MYSELF:
		{
			mMyPack[tradeIndex].templateId = itemId;
			mMyPack[tradeIndex].count	   = count;
			mMyPack[tradeIndex].effectTime = effTime;
			// 更新UI
			GameEventMgr::getSingleton().fireEvent("UPDATE_MY_TRADE");
			return;
		}
	case OTHER:
		{
			mOtherPack[tradeIndex].templateId = itemId;
			mOtherPack[tradeIndex].count	  = count;
			mOtherPack[tradeIndex].effectTime = effTime;
			GameEventMgr::getSingleton().fireEvent("UPDATE_OTHER_TRADE");
			return;
		}
	}
}
//-------------------------------------------------------------------------------
int SmallModules::getTradeItemId(int tradeIndex, SmallModules::GridType type) const
{
	if(tradeIndex < 0 || tradeIndex > MAX_COUNT)
	{
		// 报错
	}

	switch (type)
	{
	case MYSELF:
		return mMyPack[tradeIndex].templateId;
	case OTHER:
		return mOtherPack[tradeIndex].templateId;
	}

	return -1;
}
//-------------------------------------------------------------------------------
uint SmallModules::getTradeItemCount(int tradeIndex, GridType type) const
{
	if(tradeIndex < 0 || tradeIndex > MAX_COUNT)
	{
		// 报错
	}

	switch (type)
	{
	case MYSELF:
		return mMyPack[tradeIndex].count;
	case OTHER:
		return mOtherPack[tradeIndex].count;
	}

	return 0;
}
//-------------------------------------------------------------------------------
std::string SmallModules::getTradeItemEffectTime(int tradeIndex, GridType type)
{
	if(tradeIndex < 0 || tradeIndex > MAX_COUNT)
	{
		// 报错
	}

	switch (type)
	{
	case MYSELF:
		return mMyPack[tradeIndex].effectTime;
	case OTHER:
		return mOtherPack[tradeIndex].effectTime;
	}

	return "";	
}
//-------------------------------------------------------------------------------
void SmallModules::inviteDealByPicked()
{
	GameObj* obj = ObjMgr::getSingleton().getPickObj();
	if(obj != NULL)
	{
		std::string acc = ((Player*)(obj))->getAccount();
		inviteDeal(acc);
	}
}
//-------------------------------------------------------------------------------
void SmallModules::inviteDeal(const std::string &acc)
{
	if("" == acc)
	{
		//报错
	}
	inviteOtherPlayer(1, acc);

}
//-------------------------------------------------------------------------------
void SmallModules::clearTradePack()
{
	for(int i = 0; i < MAX_COUNT; i++)
	{
		mMyPack[i].templateId = -1;
		mMyPack[i].count = 0;
		mOtherPack[i].templateId = -1;
		mOtherPack[i].count = 0;
	}
}
//-------------------------------------------------------------------------------
void SmallModules::initTradePack()
{
	mGetMoney = 0;
	mPutMoney = 0;
	clearTradePack();
}
//-------------------------------------------------------------------------------
void SmallModules::checkTradeAnswer()
{
	unsigned long time = mAnswerTimer.getMilliseconds();

	if(time >= 30000)
	{
		reqCancelTrade();
		mAnswerTimer.reset();
	}
}
//-------------------------------------------------------------------------------
void SmallModules::checkInviteAnswer()
{
	unsigned long time = mAnswerTimer.getMilliseconds();

	if(time >= 15000)
	{
		disagreeInvite();
		mAnswerTimer.reset();
	}

}
//-------------------------------------------------------------------------------
void SmallModules::checkAnswerTime()
{
	if(ANSWER_NONE == mAnswerType)
	{
		return;
	}
	else
	{
		switch(mAnswerType)
		{
		case ANSWER_INVITE:
			{
				checkInviteAnswer();
				return;
			}

		case ANSWER_TRADE:
			{
				checkTradeAnswer();
				return;
			}
		default:
			return;
		}
	}
}

//-------------------------------------------------------------------------------
void SmallModules::startCheckAnswer(SmallModules::CheckAnswerType answerType)
{
	mAnswerType = answerType;
	mAnswerTimer.reset();
}
//-------------------------------------------------------------------------------
void SmallModules::endCheckAnswer()
{
	mAnswerType = ANSWER_NONE;
}
//-------------------------------------------------------------------------------
std::string SmallModules::getTraderName()
{
	std::string userName = "";
	// 根据帐号获得名字
	GameObj* obj = ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER, mTraderAcc);
	if(NULL != obj)
		userName = ((Player*)(obj))->getName();

	return userName;
}
//-------------------------------------------------------------------------------
void SmallModules::swapPackItem(int fromIndex, int toIndex, GridType type)
{
	TradePack temp;
	if(MYSELF == type)
	{
		temp = mMyPack[fromIndex];
		mMyPack[fromIndex] = mMyPack[toIndex];
		mMyPack[toIndex] = temp;
	}
	else
	{
		temp = mOtherPack[fromIndex];
		mOtherPack[fromIndex] = mOtherPack[toIndex];
		mOtherPack[toIndex] = temp;
	}
}
