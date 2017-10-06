#include "EQCommPCH.h"
#include "ItemInteractSys.h"
#include "ObjMgr.h"
#include "GameEventMgr.h"
#include "CommFunc.h"
#include "ItemTable.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"

//---------------------------------------------------------------------
IITDataConnect::IITDataConnect()
{
	regist_msgHandler();
}
//---------------------------------------------------------------------
void IITDataConnect::regist_msgHandler()
{
	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_active_game_result, NewMsgHandle(IITDataConnect, rec_msg_FarmIsActive));
	Regist(NetMsgType::msg_notify_farm_money, NewMsgHandle(IITDataConnect, rec_msg_FarmMoney));
}
//---------------------------------------------------------------------
void IITDataConnect::rec_msg_FarmMoney(MsgPack &recBuff)
{
	notify_farm_money farm_money;
	farm_money.decode(recBuff);
	sIITSys.addmoney(farm_money.money);
	if(ObjMgr::getSingleton().isMainPlayer(farm_money.account))
		_notify();
}
//---------------------------------------------------------------------
void IITDataConnect::rec_msg_FarmIsActive(MsgPack &recBuff)
{
	notify_active_game_result result;
	result.decode(recBuff);
	if (result.result == NOACTIVE)
	{
		GameEventMgr::getSingleton().fireEvent("NOTIFY_FARM_NOACTIVE");
	}
	else if (result.result == ACTIVE)
	{
		
		GameEventMgr::getSingleton().fireEvent("NOTIFY_FARM_ISACTIVE");
	}
}
//---------------------------------------------------------------------
void IITDataConnect::rec_msg_update_playercoin(MsgPack &recBuff)
{
// 	notify_update_coin coin;
// 	coin.decode(recBuff);

}
//---------------------------------------------------------------------
bool IITDataConnect::notifyInteractItems(notify_package &pack,std::string acc)
{
	std::vector<pack_grid>::size_type items_count = pack.grid_vec.size();
	sIITSys.clearItemList();
	sIITSys.resetItemList(items_count);
	
	for (std::vector<pack_grid>::size_type idx = 0; idx < items_count; idx++)
	{
		sIITSys.addItem(idx, 
						pack.grid_vec[idx].item_data, 
						pack.grid_vec[idx].count, 
						pack.grid_vec[idx].lock);
	}
	sIITSys.setPackCurPage(sIITSys.getCurpage());
	if(ObjMgr::getSingleton().isMainPlayer(acc))
		_notify();
	return true;

}
//---------------------------------------------------------------------
void IITDataConnect::reqFarmisActive()
{
	req_is_active_game req;
	req.type = IST_Farm;
	sGameMgr.getEQMsgManager()->Send(req);
}
//--------------------------------------------------------------------
void IITDataConnect::reqMoveMoney(int money,int fromType,int toType)
{
	req_move_money move_money;
	move_money.money = money;
	move_money.pack1_type = fromType;
	move_money.pack2_type = toType;
	sGameMgr.getEQMsgManager()->Send(move_money);
}
void IITDataConnect::reqSwithItem(int sourceType,std::string insIdStr, int targetType)
{
	req_switch_item req;
	uint64 instanceId = sIITSys.transformInsIdToUi64(insIdStr);
	if (instanceId != 0)
	{
		req.instance_id = instanceId;
		req.pack1_type = sourceType;
		req.pack2_type = targetType;
		sGameMgr.getEQMsgManager()->Send(req);
	}
}
//---------------------------------------------------------------------
void IITDataConnect::_notify()
{
	sIITSys.notifyDataUpdate();
}


/**
//
//
//
//
*/
//---------------------------------------------------------------------
ItemInteractSys::ItemInteractSys():mMoney(0),mCurPage(1)
{
	_init();
}
//---------------------------------------------------------------------
void ItemInteractSys::_init()
{
	mItemList.init(DEFAULT_IITGRID_SIZE);
}
//---------------------------------------------------------------------
void ItemInteractSys::setCurpage(int page)
{
	mCurPage = page;
}
//---------------------------------------------------------------------
void ItemInteractSys::_setItemSize(uint size)
{
	mItemSize = size;
}
//---------------------------------------------------------------------
void ItemInteractSys::resetItemList(int size)
{
	_setItemSize(size);
	mItemList.resize(size);
}
//---------------------------------------------------------------------
void ItemInteractSys::clearItemList()
{
	mItemList.clear();
}
//---------------------------------------------------------------------
int ItemInteractSys::getItemID(uint index)
{
	if(index >=0 && index < mItemList.getGridNumPerPage() )
	{
		Item* item = NULL;
		if (mItemList.getItemGirdOfCurPage(index) != NULL)
		{
			item = mItemList.getItemGirdOfCurPage(index)->mItem;
		}
		if(item)
		{
			return item->getItemInfo()->id;
		}
		else
		{
			return 0;//
		}
	}
	else
	{
		return 0;//qaj
	}
}
//---------------------------------------------------------------------
uint64 ItemInteractSys::getItemInstanceID(uint index)
{
	if(index >=0 && index < mItemList.getGridNumPerPage() )
	{
		Item* item = NULL;
		if (mItemList.getItemGirdOfCurPage(index) != NULL)
		{
			item = mItemList.getItemGirdOfCurPage(index)->mItem;
		}
		if(item)
		{
			return item->getInstanceID();
		}
		else
		{
			return 0;//
		}
	}
	else
	{
		return 0;//qaj
	}
}
//---------------------------------------------------------------------
std::string ItemInteractSys::getItemDelTime(uint index)
{
	if(index >=0 && index < mItemList.getGridNumPerPage() )
	{
		Item* item = NULL;
		if (mItemList.getItemGirdOfCurPage(index) != NULL)
		{
			item = mItemList.getItemGirdOfCurPage(index)->mItem;
		}
		if(item)
		{
			return item->getItemDelTime();
		}
		else
		{
			return "";//
		}
	}
	else
	{
		return "";//qaj
	}
}
//---------------------------------------------------------------------
std::string ItemInteractSys::getItemInsIdStr(uint index)
{
	uint64 insid = getItemInstanceID(index);
	std::string outIdxStr = transformInsIdToStr(insid);
	return outIdxStr;
}
//---------------------------------------------------------------------
std::string ItemInteractSys::transformInsIdToStr(uint64 insID)
{
	char temp[20] = {0};
	_ui64toa( insID, temp, 10);
	std::string insIdStr(temp);
	return insIdStr;
}
//---------------------------------------------------------------------
uint64 ItemInteractSys::transformInsIdToUi64(std::string insId_str)
{
	uint64 instanceId = _atoi64(insId_str.c_str());
	return instanceId;
}
//---------------------------------------------------------------------
const ItemGrid* ItemInteractSys::getItemGird(uint index)
{	
	if(index >=0 && index < mItemSize/*mItemList.getGridNumPerPage()*/ )
	{
		return mItemList.getItemGirdOfCurPage(index);
	}
	else
	{
		return NULL;
		//qaj
	}
}
//---------------------------------------------------------------------
int ItemInteractSys::getPackCurPage()
{
	return mItemList.getCurPage();
}
//---------------------------------------------------------------------
int ItemInteractSys::getPackPageCount()
{
	return mItemList.getPageNum();
}
//---------------------------------------------------------------------
bool ItemInteractSys::setPackCurPage(int page)
{
	bool ret = false;
	ret = mItemList.changePage(page);
	setCurpage(page);
	//if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
		notifyDataUpdate();

	return ret;
}
//---------------------------------------------------------------------
bool ItemInteractSys::addItem(int idx,item& item_data,int count, int lock)
{
	int year = item_data.del_time.year;
	int month = item_data.del_time.month;
	int day = item_data.del_time.day;
	int hour = item_data.del_time.hour;
	int minute= item_data.del_time.minute;
	int second = item_data.del_time.second;
	std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
	mItemList.setItem(
						idx,
						item_data.instance_id,
						delTime,
						item_data.template_id,
						count,
						lock);
	return true;

}
//--------------------------------------------------------------------
bool ItemInteractSys::addmoney(int money)
{
	mMoney = money;
	return true;
}
//--------------------------------------------------------------------
int ItemInteractSys::getmoney()
{
	return mMoney;
}
//--------------------------------------------------------------------
void ItemInteractSys::notifyDataUpdate()
{
	GameEventMgr::getSingleton().fireEvent("UPDATE_IITDATA_UI");
}
//--------------------------------------------------------------------