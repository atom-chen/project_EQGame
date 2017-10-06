#include "EQCommPCH.h"
#include "Player.h"
#include "MainPlayer.h"
#include "Avatar.h"
#include "AvatarRTT.h"
#include "ObjMgr.h"
#include "GameObj.h"
#include "EQGameMgr.h"
#include "EQMsgManager.h"
#include "AvatarAdjuster.h"
#include "GameEventMgr.h"
#include "ItemTable.h"
#include "SceneManager.h"
#include "ZoneLightMgr.h"
#include "ScriptFunction.h"
#include "EQGameSceneMgr.h"
#include "HouseScene.h"

#include "LogicTable.h"
#include "AvatarTable.h"
#include "DynamicUIMgr.h"
#include "CommFunc.h"
#include "PlayerHouse.h"
#include "StringHandle.h"
#include "GameState.h"
#include "ItemInteractSys.h"
#include "BodyActionMgr.h"

#include "luabind/luabind/luabind.hpp"
#include "MsgSystem.h"


//--------------------------------------------------------------------
Player::Player(void):
mGameCoin(0),
mEQCoin(0),
mStorageCoin(0),
mMosaicID(0),
mDiseaseParticle(0),
mChatPaoPaoIsVisible(false)
{
	//GameObj::GameObj();
	setType(ObjMgr::GOT_PLAYER);
}
//--------------------------------------------------------------------
void Player::cleanup()
{
	GameObj::cleanup();
	removeMosaicEffect();
}
//--------------------------------------------------------------------
Player::~Player(void)
{
	
}
//--------------------------------------------------------------------
void Player::init()
{
	mWarehouse.init(DEFAULT_WAREHOUSE_SIZE);
	mBody.init(DEFAULT_BODY_SIZE);
	mStorage.init(DEFAULT_STORAGE_SIZE);
	initShopCenter();
}
//--------------------------------------------------------------------
void	Player::setAccount(std::string a)	
{ 
	mPlayerBD.account = a;
}
//--------------------------------------------------------------------
const std::string&	Player::getAccount()	
{ 
	return mPlayerBD.account;
}
//--------------------------------------------------------------------
void Player::setPlayerData(player_basic_data& data)
{
	mPlayerBD = data;

	if(isMale())
		mModelHeight = 170;
	else
		mModelHeight = 160;

	showHeadup();
	setHealtyState(mPlayerBD.health_status);
}
//--------------------------------------------------------------------
void Player::setHealtyState(int healthState)
{
	mPlayerBD.health_status = healthState;

	if(mDiseaseParticle)
	{
		delParticleByHandle(mDiseaseParticle);
		mDiseaseParticle = 0;
	}
	const DiseaseRow* row = LogicTableManager::getSingleton().getDiseaseRow(healthState);
	if(row && row->particle)
		mDiseaseParticle = addParticle(row->particle);
}
//--------------------------------------------------------------------
int Player::getHealtyState() const
{
	return mPlayerBD.health_status;
}
//--------------------------------------------------------------------
bool Player::isMale() 
{
	return mPlayerBD.sex==1; 
}
//--------------------------------------------------------------------
void Player::moveTo(Ogre::Vector3& dest)
{
	GameObj::moveTo(dest); 
}
//--------------------------------------------------------------------
bool Player::update()
{
	bool ret = GameObj::update();

	_updateMosaicEffect();
	return ret;
}
//--------------------------------------------------------------------
void Player::updateBaseLook()
{
	AvatarAdjuster* aad = mAvatar->getAdjuster();
	aad->doChange(AvatarAdjuster::atHairType,mPlayerBD.hair);
	//aad->doChange(atHairColor,mPlayerBD.hair);
	aad->doChange(AvatarAdjuster::atFace,mPlayerBD.face);
	aad->doChange(AvatarAdjuster::atSkin,mPlayerBD.skin_color);
	aad->doChange(AvatarAdjuster::atBeard,mPlayerBD.beard);

}
//--------------------------------------------------------------------
void Player::updateWarehouse()
{
	GameEventMgr::getSingleton().fireEvent("UPDATE_STORAGE_UI");
}
//--------------------------------------------------------------------
void Player::updateBodyItem()
{
	GameEventMgr::getSingleton().fireEvent("UPDATE_BODY_UI");
	//_updateEquipLook();
}
//--------------------------------------------------------------------
void Player::updateStorage() const
{
	GameEventMgr::getSingleton().fireEvent("UPDATE_STORAGE");
}
//--------------------------------------------------------------------
//void Player::_updateEquipLook()
//{
//	
//}
//--------------------------------------------------------------------
bool Player::isInItemPack(int id)
{
	for (size_t i=0; i<mBody.getTotalItemGrids().size(); ++i)
	{
		Item* item = mBody.getItemGirdByGlobaIdx(i).mItem;
		if (item)
		{
			if (id == item->getItemInfo()->id)
				return true;
		}
	}
	for (size_t i=0; i<mWarehouse.getTotalItemGrids().size(); ++i)
	{
		Item* item = mWarehouse.getItemGirdByGlobaIdx(i).mItem;
		if (item)
		{
			if (id == item->getItemInfo()->id)
				return true;
		}
	}
	for (size_t i=0; i<mStorage.getTotalItemGrids().size(); ++i)
	{
		Item* item = mStorage.getItemGirdByGlobaIdx(i).mItem;
		if (item)
		{
			if (id == item->getItemInfo()->id)
				return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
const ItemGrid* Player::getItemGird(uint index,ItemPack::PackType pt)
{
	switch(pt)
	{
	case ItemPack::PT_WAREHOUSE:
		{
			if(index >=0 && index < mWarehouse.getGridNumPerPage() )
			{
				return mWarehouse.getItemGirdOfCurPage(index);
			}
			else
			{
				//qaj
			}
		}
		break;
		//
	case ItemPack::PT_BODY:
		{
			if(index >=0 && index < mBody.getGridNumPerPage() )
			{
				return mBody.getItemGirdOfCurPage(index);
			}
			else
			{
				//qaj
			}
		}
		break;
	case ItemPack::PT_STORAGE:
		{
			if(index >=0 && index < mStorage.getGridNumPerPage() )
			{
				return mStorage.getItemGirdOfCurPage(index);
			}
			else
			{
				//qaj
			}
		}
	break;
	}
	return NULL;
}
//--------------------------------------------------------------------
const ItemRow* Player::getItemRow(uint index,ItemPack::PackType pt)
{
	switch(pt)
	{
		//
		case ItemPack::PT_WAREHOUSE:
			{
				if(index >=0 && index < mWarehouse.getGridNumPerPage() )
				{
					Item* item = NULL;
					if (mWarehouse.getItemGirdOfCurPage(index) != NULL)
					{
						item = mWarehouse.getItemGirdOfCurPage(index)->mItem;
					}
					if(item)
						return item->getItemInfo();
					else
					{
						//
					}
				}
				else
				{
					//qaj
				}
		}
		break;
		//
		case ItemPack::PT_BODY:
			{
				if(index >=0 && index < mBody.getGridNumPerPage() )
				{
					Item* item = NULL;
					if (mBody.getItemGirdOfCurPage(index) != NULL)
					{
						item = mBody.getItemGirdOfCurPage(index)->mItem;
					}
					if(item)
						return item->getItemInfo();
					else
					{
						//
					}
				}
				else
				{
					//qaj
				}
		}
		break;
		// 仓库
		case ItemPack::PT_STORAGE:
			{
				if(index >=0 && index < mBody.getGridNumPerPage() )
				{
					Item* item = NULL;
					if (mStorage.getItemGirdOfCurPage(index) != NULL)
					{
						item = mStorage.getItemGirdOfCurPage(index)->mItem;
					}
					if(item)
						return item->getItemInfo();
					else
					{
						//
					}
				}
				else
				{
					//qaj
				}
			}
		break;
	}
	return NULL;
}
//--------------------------------------------------------------------
uint64 Player::getItemInstanceID(uint index,ItemPack::PackType pt)
{
	switch(pt)
	{
		//
	case ItemPack::PT_WAREHOUSE:
		{
			if(index >=0 && index < mWarehouse.getGridNumPerPage() * mWarehouse.getPageNum())
			{
				Item* item = NULL;
				//if (mWarehouse.getItemGirdByGlobaIdx(index) != NULL)
				{
					item = mWarehouse.getItemGirdByGlobaIdx(index).mItem;
				}
				if(item)
					return item->getInstanceID();
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
		break;
	}
	return 0;
}
//--------------------------------------------------------------------
std::string Player::getItemInsIdStr(uint index,ItemPack::PackType pt)
{
	uint64 insid = getItemInstanceID(index,pt);
	std::string outIdxStr = sIITSys.transformInsIdToStr(insid);
	return outIdxStr;
}
//--------------------------------------------------------------------
const ItemGrid* Player::getItemGridByGlobaIdx(uint index, ItemPack::PackType pt /* = ItemPack::PT_NONE */)
{
	switch(pt)
	{
	case ItemPack::PT_WAREHOUSE:
		{
			if( 0 <= index && index < mWarehouse.getGridNumPerPage() * mWarehouse.getPageNum() )
			{
				return (&mWarehouse.getItemGirdByGlobaIdx(index));
			}
			else
			{
				return NULL;
			}
		}
		break;

	case ItemPack::PT_STORAGE:
		{
			if(index >=0 && index < mStorage.getGridNumPerPage() * mStorage.getPageNum() )
			{
				return (&mStorage.getItemGirdByGlobaIdx(index));	
			}
			else
			{
				return NULL;
			}
		}
		break;

	default: return NULL;
	}

	return NULL;
}
//--------------------------------------------------------------------
uint Player::getItemIDByGlobaIdx(uint index,ItemPack::PackType pt)
{
	switch(pt)
	{
		//
	case ItemPack::PT_WAREHOUSE:
		{
			if(index >=0 && index < mWarehouse.getGridNumPerPage() * mWarehouse.getPageNum() )
			{
				Item* item = mWarehouse.getItemGirdByGlobaIdx(index).mItem;
				if(item)
					return item->getItemInfo()->id;
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
		break;
	case ItemPack::PT_STORAGE:
		{
			if(index >=0 && index < mStorage.getGridNumPerPage() * mStorage.getPageNum() )
			{
				Item* item = mStorage.getItemGirdByGlobaIdx(index).mItem;
				if(item)
					return item->getItemInfo()->id;
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
	break;
	}
	return 0;
}
//--------------------------------------------------------------------
std::string Player::getItemDelTimeByGlobalIdx(uint index,ItemPack::PackType pt)
{
	switch(pt)
	{
		//
	case ItemPack::PT_WAREHOUSE:
		{
			if(index >=0 && index < mWarehouse.getGridNumPerPage() * mWarehouse.getPageNum() )
			{
				Item* item = mWarehouse.getItemGirdByGlobaIdx(index).mItem;
				if(item)
					return item->getItemDelTime();
				else
					return "";//
			}
			else
			{
				return "";//qaj
			}
		}
		break;
	case ItemPack::PT_STORAGE:
		{
			if(index >=0 && index < mStorage.getGridNumPerPage() * mStorage.getPageNum() )
			{
				Item* item = mStorage.getItemGirdByGlobaIdx(index).mItem;
				if(item)
					return item->getItemDelTime();
				else
					return "";//
			}
			else
			{
				return "";//qaj
			}
		}
		break;
	}
	return "";
}
//--------------------------------------------------------------------
uint Player::getItemID(uint index,ItemPack::PackType pt)
{
	switch(pt)
	{
		//
	case ItemPack::PT_WAREHOUSE:
		{
			if(index >=0 && index < mWarehouse.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mWarehouse.getItemGirdOfCurPage(index) != NULL)
				{
					item = mWarehouse.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
		break;
		//
	case ItemPack::PT_BODY:
		{
			if(index >=0 && index < mBody.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mBody.getItemGirdOfCurPage(index) != NULL)
				{
					item = mBody.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
		break;
		//
	case ItemPack::PT_SHOPCENTER:
		{
			if(index >=0 && index < mShopCenter.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mShopCenter.getItemGirdOfCurPage(index) != NULL)
				{
					item = mShopCenter.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
		break;
		//
	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		{
			if(index >=0 && index < mShopCenterRecommend.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mShopCenterRecommend.getItemGirdOfCurPage(index) != NULL)
				{
					item = mShopCenterRecommend.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
		break;
		//
	case ItemPack::PT_FITMODE_SHOPCENTER:
		{
			if(index >=0 && index < mFitModeShopCenter.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mFitModeShopCenter.getItemGirdOfCurPage(index) != NULL)
				{
					item = mFitModeShopCenter.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
		break;
		//
	case ItemPack::PT_NPCSHOP:
		{
			if(index >=0 && index < mNpcShop.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mNpcShop.getItemGirdOfCurPage(index) != NULL)
				{
					item = mNpcShop.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
		break;
		//
	case ItemPack::PT_FITMODE_SHOPPINGCART:
		{
			if(index >=0 && index < mFitModeShoppingCart.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mFitModeShoppingCart.getItemGirdOfCurPage(index) != NULL)
				{
					item = mFitModeShoppingCart.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
		break;
		//
	case ItemPack::PT_STORAGE:
		{
			if(index >=0 && index < mStorage.getGridNumPerPage() )
			{
				Item* item = NULL;
				if (mStorage.getItemGirdOfCurPage(index) != NULL)
				{
					item = mStorage.getItemGirdOfCurPage(index)->mItem;
				}
				if(item)
					return item->getItemInfo()->id;
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
	break;
	}
	return NULL;
}
//--------------------------------------------------------------------
std::string Player::getItemDelTime(uint index,ItemPack::PackType pt)
{
	ItemPack* itemPackTemp = NULL;
	switch(pt)
	{
		//
	case ItemPack::PT_WAREHOUSE:
		itemPackTemp = &mWarehouse;
		break;
		//
	case ItemPack::PT_BODY:
		itemPackTemp = &mBody;
		break;
		//
	case ItemPack::PT_SHOPCENTER:
		itemPackTemp = &mShopCenter;
		break;
		//
	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		itemPackTemp = &mShopCenterRecommend;
		break;
		//
	case ItemPack::PT_FITMODE_SHOPCENTER:
		itemPackTemp = &mFitModeShopCenter;
		break;
		//
	case ItemPack::PT_NPCSHOP:
		itemPackTemp = &mNpcShop;
		break;
		//
	case ItemPack::PT_FITMODE_SHOPPINGCART:
		itemPackTemp = &mFitModeShoppingCart;
		break;
		//
	case ItemPack::PT_STORAGE:
		itemPackTemp = &mStorage;
		break;
	}

	for (uint i=0; i<itemPackTemp->getTotalItemGrids().size(); ++i)
	{
		Item* item = itemPackTemp->getItemGirdByGlobaIdx(i).mItem;
		if (item)
		{
			if (index == item->getItemInfo()->id)
				return item->getItemDelTime();
		}
	}
	return "";
}
//--------------------------------------------------------------------
void Player::setPack(notify_package& pack , ItemPack::PackType pt)
{
	if (pt == ItemPack::PT_NONE)
		pt = (ItemPack::PackType)pack.type;

	switch(pt)
	{
	//
	case ItemPack::PT_WAREHOUSE:
		{
			mWarehouse.resize(pack.grid_vec.size());
			for(uint i=0; i < pack.grid_vec.size(); i++)
			{
				int year = pack.grid_vec[i].item_data.del_time.year;
				int month = pack.grid_vec[i].item_data.del_time.month;
				int day = pack.grid_vec[i].item_data.del_time.day;
				int hour = pack.grid_vec[i].item_data.del_time.hour;
				int minute= pack.grid_vec[i].item_data.del_time.minute;
				int second = pack.grid_vec[i].item_data.del_time.second;
				std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
				mWarehouse.setItem(
					i,
					pack.grid_vec[i].item_data.instance_id,
					delTime,
					pack.grid_vec[i].item_data.template_id,
					pack.grid_vec[i].count,pack.grid_vec[i].lock);
			}
			if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
				updateWarehouse();
		}
		break;
	//
	case ItemPack::PT_BODY:
		{
			_takeOffAll();
			for(uint i=0; i < pack.grid_vec.size(); i++)
			{
				//
				uint itemID = pack.grid_vec[i].item_data.template_id;
				if (itemID != 0)
				{
					const ItemRow *  itemRow =  ItemTableManager::getSingleton().getItemRow(itemID);
					uint subID =  itemRow->subID;
					const ItemEquipRow * itemRowEquip = ItemTableManager::getSingleton().getItemEquipRow(subID);
					uint equipID = itemRowEquip->equip_pos;
					uint equipSID = itemRowEquip->equip_disp_id;
					uint slot = posToSlot(itemRowEquip->equip_pos);
					
					//主控玩家多个RTT
					if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
						EQGameMgr::getSingleton().getRTT()->equipOn(slot, equipSID, itemRowEquip->equip_placeholder);
					getAvatar()->equipOn(slot, equipSID, itemRowEquip->equip_placeholder);
					//
					if(itemRowEquip->equip_pos == i)
					{
						int year = pack.grid_vec[i].item_data.del_time.year;
						int month = pack.grid_vec[i].item_data.del_time.month;
						int day = pack.grid_vec[i].item_data.del_time.day;
						int hour = pack.grid_vec[i].item_data.del_time.hour;
						int minute= pack.grid_vec[i].item_data.del_time.minute;
						int second = pack.grid_vec[i].item_data.del_time.second;
						std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
						mBody.setItem(
							i,
							pack.grid_vec[i].item_data.instance_id,
							delTime,
							pack.grid_vec[i].item_data.template_id,
							pack.grid_vec[i].count,pack.grid_vec[i].lock);
					}
					else
					{
						LOGERR("服务器发错装备栏位置上对应的装备物品！");
					}
				}
				else
					mBody.dropItem(i);

			}
			if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
				updateBodyItem();
		}
		break;
	case ItemPack::PT_FARMDEPOT:
		{
			
			if (sStateMgr.isFunnyFarm())
			{
				int templateId, count, lock;
				luabind::call_function<void>(sLuaMgr.getL(), "Dec_clearShowBufferData");
				for(uint i=0; i < pack.grid_vec.size(); i++)
				{
					count = pack.grid_vec[i].count;
					lock  = pack.grid_vec[i].lock;
					templateId = pack.grid_vec[i].item_data.template_id;
					int year = pack.grid_vec[i].item_data.del_time.year;
					int month = pack.grid_vec[i].item_data.del_time.month;
					int day = pack.grid_vec[i].item_data.del_time.day;
					int hour = pack.grid_vec[i].item_data.del_time.hour;
					int minute= pack.grid_vec[i].item_data.del_time.minute;
					int second = pack.grid_vec[i].item_data.del_time.second;
					std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
					char instanceId[20] = {0};
					_ui64toa( pack.grid_vec[i].item_data.instance_id, instanceId, 10);
					luabind::call_function<void>(sLuaMgr.getL(), "lua_addFarmStorage",instanceId, delTime, templateId, count, lock, i+1, pack.grid_vec.size());
				}
			}
			else
			{
				sIITDataConnect.notifyInteractItems(pack,getAccount());
			}
		}
		break;
	case ItemPack::PT_STORAGE:
		{
			mStorage.resize(pack.grid_vec.size());

			for(uint i=0; i < pack.grid_vec.size(); i++)
			{
				int year = pack.grid_vec[i].item_data.del_time.year;
				int month = pack.grid_vec[i].item_data.del_time.month;
				int day = pack.grid_vec[i].item_data.del_time.day;
				int hour = pack.grid_vec[i].item_data.del_time.hour;
				int minute= pack.grid_vec[i].item_data.del_time.minute;
				int second = pack.grid_vec[i].item_data.del_time.second;
				std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
				mStorage.setItem(
					i,
					pack.grid_vec[i].item_data.instance_id,
					delTime,
					pack.grid_vec[i].item_data.template_id,
					pack.grid_vec[i].count,pack.grid_vec[i].lock);
			}

			if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
				updateStorage();
		}
	break;
	}
}
//---------------------------------------
void Player::_takeOffAll()
{
	//for (int slot=ES_HAT; slot <=ES_ORNAMENT; slot = slot * 2 )
	//{
	//	if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
	//		EQGameMgr::getSingleton().getRTT()->takeOff(slot);

	//	getAvatar()->takeOff(slot);
	//}

	if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
		EQGameMgr::getSingleton().getRTT()->takeOffAll();

	getAvatar()->takeOffAll();
}
//---------------------------------------
void Player::setPos(Ogre::Vector3& v)
{
	GameObj::setPos(v);

	_calcLightListener(v);
}
//-------------------------------------------------------
void Player::_calcLightListener(Ogre::Vector3& pos)
{
	if(!mMainModel)
		return;

	static uint lzone=0;
	int czone = 0; //默认室外

	if(EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
		if(sce)
		{
			HouseSceneGrid* grid;
			//Ogre::Vector3 cpoint;
			//sce->rayGrid(Ogre::Ray(pos+Ogre::Vector3(0,10,0), Ogre::Vector3::NEGATIVE_UNIT_Y), grid, cpoint, true);
			grid = sce->gridByPosition(pos, 10, 10);
			if(grid)
				czone = room2zone(grid->getHouseGridTypeRow()->InDoor, grid->getCreator()->getFloorNO(), grid->getRoomID());
		}
	}

	if(czone != lzone)
	{
		lzone = czone;

		std::vector<uint> vzone;
		vzone.push_back(czone);
		mMainModel->setLightListener(vzone);

		LOG("玩家 " + mBaseData.name + " 改变至区域 " + Ogre::StringConverter::toString(czone));
	}
}
//-------------------------------------------------------
void Player::initShopCenter()
{
	mShopCenter.init(DEFAULT_SHOPCENTER_SIZE);
	mShopCenterRecommend.init(DEFAULT_SHOPCENTER_RECOMMEND_SIZE);
	mFitModeShopCenter.init(DEFAULT_FITMODE_SHOPCENTER_SIZE);
	mFitModeShoppingCart.init(DEFAULT_FITMODE_SHOPPINGCART_SIZE);
	mNpcShop.init(DEFAULT_NPCSHOP_SIZE);

	std::vector<uint> vID; 
	ShopCenterTable::const_iterator it;
	for(it=LogicTableManager::getSingleton().getShopCenterTable().begin(); it!=LogicTableManager::getSingleton().getShopCenterTable().end(); it++)
	{
		if(it->second.recommend)
			vID.push_back(it->second.good_id);
	}
	std::sort(vID.begin(), vID.end());
	for(uint i=0; i<vID.size(); i++)
		mShopCenterRecommend.insertItem(LogicTableManager::getSingleton().getShopCenterRow(vID[i])->item_id, vID[i]);
}
//-------------------------------------------------------
void Player::setShopCenterType(uint type, int sex, ItemPack::PackType pt, int arg)
{
	std::vector<uint> vID; 
	ShopCenterTable::const_iterator it;

	switch(pt)
	{
	case ItemPack::PT_SHOPCENTER:
		{
			for(it=LogicTableManager::getSingleton().getShopCenterTable().begin(); 
					it!=LogicTableManager::getSingleton().getShopCenterTable().end(); it++)
			{
				if(it->second.type != type)
					continue;

				if(arg != 0 && it->second.money_type != arg)
					continue;

				if(sex==3 || it->second.sex==3 ||it->second.sex==sex)
					vID.push_back(it->second.good_id);
			}

			mShopCenter.clear();
			std::sort(vID.begin(), vID.end());
			for(uint i=0; i<vID.size(); i++)
				mShopCenter.insertItem(LogicTableManager::getSingleton().getShopCenterRow(vID[i])->item_id, vID[i]);
		}
		break;

	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		{
			for(it=LogicTableManager::getSingleton().getShopCenterTable().begin(); 
				it!=LogicTableManager::getSingleton().getShopCenterTable().end(); it++)
			{
				if(!it->second.recommend)
					continue;

				if(arg != 0 && it->second.money_type != arg)
					continue;

				if(sex==3 || it->second.sex==3 ||it->second.sex==sex)
					vID.push_back(it->second.good_id);
			}

			mShopCenterRecommend.clear();
			std::sort(vID.begin(), vID.end());
			for(uint i=0; i<vID.size(); i++)
				mShopCenterRecommend.insertItem(LogicTableManager::getSingleton().getShopCenterRow(vID[i])->item_id, vID[i]);
		}
		break;

	case ItemPack::PT_FITMODE_SHOPCENTER:
		{
			for(it=LogicTableManager::getSingleton().getShopCenterTable().begin(); 
					it!=LogicTableManager::getSingleton().getShopCenterTable().end(); it++)
			{
				if(it->second.type != type)
					continue;

				if(sex==3 || it->second.sex==3 ||it->second.sex==sex)
					vID.push_back(it->second.good_id);
			}

			mFitModeShopCenter.clear();
			std::sort(vID.begin(), vID.end());
			for(uint i=0; i<vID.size(); i++)
				mFitModeShopCenter.insertItem(LogicTableManager::getSingleton().getShopCenterRow(vID[i])->item_id, vID[i]);
		}
		break;

	case ItemPack::PT_NPCSHOP:
		{
			const NpcShopMgrRow* row = LogicTableManager::getSingleton().getNpcShopMgrRow(arg);
			assert(row);

			for(it=LogicTableManager::getSingleton().getNpcShopTable().begin(); 
				it!=LogicTableManager::getSingleton().getNpcShopTable().end(); it++)
			{
				if(it->second.type != type)
					continue;

				if( std::find(row->sale_type.begin(), row->sale_type.end(), it->second.sale_type) == row->sale_type.end() )
					continue;

				if(sex==3 || it->second.sex==3 ||it->second.sex==sex)
					vID.push_back(it->second.good_id);
			}

			mNpcShop.clear();
			std::sort(vID.begin(), vID.end());
			for(uint i=0; i<vID.size(); i++)
				mNpcShop.insertItem(LogicTableManager::getSingleton().getNpcShopRow(vID[i])->item_id, vID[i]);
		}
		break;
	}
}
//-------------------------------------------------------
const ShopCenterRow* Player::getShopCenterRow(uint index,ItemPack::PackType pt)
{
	switch(pt)
	{
	case ItemPack::PT_SHOPCENTER:
		{
			if(index >=0 && index < mShopCenter.getGridNumPerPage() )
			{
				if(mShopCenter.getItemGirdOfCurPage(index) != NULL && mShopCenter.getItemGirdOfCurPage(index)->mItem)
				{
					uint goodid = mShopCenter.getItemGirdOfCurPage(index)->mData;
					return LogicTableManager::getSingleton().getShopCenterRow(goodid);
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		break;

	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		{
			if(index >=0 && index < mShopCenterRecommend.getGridNumPerPage() )
			{
				if(mShopCenterRecommend.getItemGirdOfCurPage(index) != NULL && mShopCenterRecommend.getItemGirdOfCurPage(index)->mItem)
				{
					uint goodid = mShopCenterRecommend.getItemGirdOfCurPage(index)->mData;
					return LogicTableManager::getSingleton().getShopCenterRow(goodid);
				}
			}
			else
				return NULL;
		}
		break;

	case ItemPack::PT_FITMODE_SHOPCENTER:
		{
			if(index >=0 && index < mFitModeShopCenter.getGridNumPerPage() )
			{
				if(mFitModeShopCenter.getItemGirdOfCurPage(index) != NULL && mFitModeShopCenter.getItemGirdOfCurPage(index)->mItem)
				{
					uint goodid = mFitModeShopCenter.getItemGirdOfCurPage(index)->mData;
					return LogicTableManager::getSingleton().getShopCenterRow(goodid);
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		break;

	case ItemPack::PT_FITMODE_SHOPPINGCART:
		{
			if(index >=0 && index < mFitModeShoppingCart.getGridNumPerPage() )
			{
				if(mFitModeShoppingCart.getItemGirdOfCurPage(index) != NULL && mFitModeShoppingCart.getItemGirdOfCurPage(index)->mItem)
				{
					return (ShopCenterRow*)mFitModeShoppingCart.getItemGirdOfCurPage(index)->mPData;
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		break;

	case ItemPack::PT_NPCSHOP:
		{
			if(index >=0 && index < mNpcShop.getGridNumPerPage() )
			{
				if(mNpcShop.getItemGirdOfCurPage(index) != NULL && mNpcShop.getItemGirdOfCurPage(index)->mItem)
				{
					uint goodid = mNpcShop.getItemGirdOfCurPage(index)->mData;
					return LogicTableManager::getSingleton().getNpcShopRow(goodid);
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		break;
	}
	return NULL;
}
//-------------------------------------------------------
bool Player::setPackLastPage(ItemPack::PackType pt)
{
	switch(pt)
	{
	case ItemPack::PT_SHOPCENTER:
		{
			return mShopCenter.lastPage();
		}
		break;
	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		{
			return mShopCenterRecommend.lastPage();
		}
		break;
	case ItemPack::PT_FITMODE_SHOPCENTER:
		{
			return mFitModeShopCenter.lastPage();
		}
		break;
	case ItemPack::PT_NPCSHOP:
		{
			return mNpcShop.lastPage();
		}
		break;
	}

	return false;
}
//-------------------------------------------------------
bool Player::setPackNextPage(ItemPack::PackType pt)
{
	switch(pt)
	{
	case ItemPack::PT_SHOPCENTER:
		{
			return mShopCenter.nextPage();
		}
		break;
	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		{
			return mShopCenterRecommend.nextPage();
		}
		break;
	case ItemPack::PT_FITMODE_SHOPCENTER:
		{
			return mFitModeShopCenter.nextPage();
		}
		break;
	case ItemPack::PT_NPCSHOP:
		{
			return mNpcShop.nextPage();
		}
		break;
	}

	return false;
}
//-------------------------------------------------------
bool Player::setPackCurPage(ItemPack::PackType pt, uint page)
{
	bool ret = false;
	switch(pt)
	{
	case ItemPack::PT_FITMODE_SHOPPINGCART:
		{
			return mFitModeShoppingCart.changePage(page);
		}
		break;

	case ItemPack::PT_WAREHOUSE:
		{
			ret = mWarehouse.changePage(page);

			if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
				updateWarehouse();

			return ret;
			
		}
		break;
	case ItemPack::PT_STORAGE:
		{
			ret = mStorage.changePage(page);

			if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
				updateStorage();

			return ret;
		}
	break;
	}

	return false;
}
//-------------------------------------------------------
uint Player::getPackCurPage(ItemPack::PackType pt)
{
	switch(pt)
	{
	case ItemPack::PT_SHOPCENTER:
		{
			return mShopCenter.getCurPage();
		}
		break;
	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		{
			return mShopCenterRecommend.getCurPage();
		}
		break;
	case ItemPack::PT_FITMODE_SHOPCENTER:
		{
			return mFitModeShopCenter.getCurPage();
		}
		break;
	case ItemPack::PT_WAREHOUSE:
		{
			return mWarehouse.getCurPage();
		}
		break;
	case ItemPack::PT_NPCSHOP:
		{
			return mNpcShop.getCurPage();
		}
		break;
	case ItemPack::PT_STORAGE:
		{
			return mStorage.getCurPage();
		}
	break;
	}

	return 0;
}
//-------------------------------------------------------
uint Player::getPackPageCount(ItemPack::PackType pt)
{
	switch(pt)
	{
	case ItemPack::PT_SHOPCENTER:
		{
			return mShopCenter.getPageNum();
		}
		break;
	case ItemPack::PT_SHOPCENTER_RECOMMEND:
		{
			return mShopCenterRecommend.getPageNum();
		}
		break;
	case ItemPack::PT_FITMODE_SHOPCENTER:
		{
			return mFitModeShopCenter.getPageNum();
		}
		break;
	case ItemPack::PT_FITMODE_SHOPPINGCART:
		{
			return mFitModeShoppingCart.getPageNum();
		}
		break;
	case ItemPack::PT_WAREHOUSE:
		{
			return mWarehouse.getPageNum();
		}
		break;
	case ItemPack::PT_NPCSHOP:
		{
			return mNpcShop.getPageNum();
		}
		break;
	}

	return 0;
}
//-------------------------------------------------------
void Player::setGold(player_coin& coin)
{
	mGameCoin = coin.gamecoin;
	mEQCoin = coin.eqcoin;

	GameEventMgr::getSingleton().fireEvent("UPDATE_PLAYER_GOLD");
}
//-------------------------------------------------------
void Player::playAnimation(const std::string & ani, const std::string & player, const std::string & target,bool loop)
{
	setState(GOS_ANIMATION,ani);
	GameObj::playAnima(ani,loop);
	bool isMainPlayer = ObjMgr::getSingleton().isMainPlayer(player);
	if (isMainPlayer && !loop)
	{
		// TODO: 动画播放时间, 目前只是临时方案，具体时间最终可能要填表解决
		const DWORD aniTime = getAnimationLength(ani);
		getBodyActionMgr()->start_timer(aniTime);
	}

	std::string desc = AvatarTableManager::getSingleton().getSkelActionLogicDesc(mAvatar->getRoleRaceInfo()->skeleton, ani);

	std::string key("%s");
	string::size_type pos;
	if((pos=desc.find(key)) != std::string::npos)
	{
		if (isMainPlayer)
		{
			desc.replace(pos, key.length(), sMainPlayer.getName());
		}
		else
		{
			desc.replace(pos, key.length(), player);
		}

		if((pos=desc.find(key, pos)) != std::string::npos)
		{
			desc.replace(pos, key.length(), target);
		}

		MsgSystem::getSingleton().showClientMsg(MsgSystem::MSG_CHATBOX, desc);
	}
}
//-------------------------------------------------------
std::string	Player::getCurrentAnimation()
{
	return getCurrentAnima();
}
//-------------------------------------------------------
void Player::setVisible(bool visible)
{
	// 玩家在室内场景时,条件判定(2011/6/10 modify by hezhr)
	if (sObjMgr.getViewFloorNO() < sObjMgr.getPosFloorNO(getPos()))	// 当前的观察楼层<玩家所在楼层,隐藏模型
	{
		GameObj::setVisible(false);
	}
	else
	{
		GameObj::setVisible(visible);
	}
}
//-------------------------------------------------------
void Player::setHeadUpVisible(bool visible)
{
	if (!mHeadup.empty() && CEGUI::WindowManager::getSingleton().isWindowPresent(mHeadup))
	{
		// 玩家在室内场景时,条件判定(2011/6/10 modify by hezhr)
		if (sObjMgr.getViewFloorNO() < sObjMgr.getPosFloorNO(getPos()))	// 当前的观察楼层<玩家所在楼层,隐藏头顶名字
		{
			CEGUI::WindowManager::getSingleton().getWindow(mHeadup)->setAlpha(0);
		}
		else
		{
			CEGUI::WindowManager::getSingleton().getWindow(mHeadup)->setAlpha(visible?1.0f:0);
		}
	}
	if (!mChatPaoPao.empty() && CEGUI::WindowManager::getSingleton().isWindowPresent(mChatPaoPao))
	{
		if ((sObjMgr.getViewFloorNO()<sObjMgr.getPosFloorNO(getPos())) || (false==visible))	// 隐藏聊天泡泡
		{
			CEGUI::WindowManager::getSingleton().getWindow(mChatPaoPao)->setAlpha(0);
		}
	}
}
//-------------------------------------------------------
void Player::showChatPaoPao(const char* text, uint life)
{
	if (mHeadup.empty() || (0.0f==CEGUI::WindowManager::getSingleton().getWindow(mHeadup)->getAlpha()))	// 隐藏聊天泡泡(2011/6/30 add by hezhr)
	{
		return;
	}
	std::string output;
	output = sStringHandle.stringFilter(const_cast<char *>(text),0);
	DynamicUIMgr::getSingleton().createChatPaoPao(this, mChatPaoPao, output.c_str());
	setChatPaoPaoIsVisible(true);
}
//-------------------------------------------------------
void Player::setChatPaoPaoIsVisible(bool visible)
{
	mChatPaoPaoIsVisible = visible;
}
//-------------------------------------------------------
void Player::showHeadup()
{
	if(!mHeadup.empty())
		DynamicUIMgr::getSingleton().destroyWindow(mHeadup);

	mHeadup = DynamicUIMgr::getSingleton().createHeadup(this, 0);
}
//-------------------------------------------------------
void Player::showHeadupBigExpression(int id)
{
	DynamicUIMgr::getSingleton().createHeadupBigExpression(this,mBigExp,id);
}
//-------------------------------------------------------
void Player::addShoppingCart(const ShopCenterRow* row, uint uid)
{
	mFitModeShoppingCart.insertItem(row->item_id, uid, (void*)row);
}
//-------------------------------------------------------
int Player::delShoppingCart(int index)
{
	if(index >=0 && index < (int)mFitModeShoppingCart.getGridNumPerPage() )
	{
		if(mFitModeShoppingCart.getItemGirdOfCurPage(index) != NULL && mFitModeShoppingCart.getItemGirdOfCurPage(index)->mItem)
		{
			uint uid = mFitModeShoppingCart.getItemGirdOfCurPage(index)->mData;
			EQGameSceneMgr::getSingleton().getHouse()->delTempFurniture(uid);
		}
	}

	int res = mFitModeShoppingCart.delItem(index);
	mFitModeShoppingCart.changePage(mFitModeShoppingCart.getCurPage());
	return res;
}
//-------------------------------------------------------
int Player::delShoppingCartByData(uint uid)
{
	int res = mFitModeShoppingCart.delItemByData(uid);
	mFitModeShoppingCart.changePage(1);
	return res;
}
//-------------------------------------------------------
void Player::clearShoppingCart()
{
	mFitModeShoppingCart.clear();
	EQGameSceneMgr::getSingleton().getHouse()->clearTempFurniture();
}
//-------------------------------------------------------
void Player::sendShoppingCart()
{
	const std::vector<ItemGrid>& items = mFitModeShoppingCart.getTotalItemGrids();
	if(items.size() == 0)
		return;

	req_buy_sys_shop_goods_list pack;
	furniture_goods_data data;
	house_furniture furni;
	// 把更换墙纸地板和普通家具的包裹合起来(2011/9/21 modify by hezhr)
	for(int i=0; i<(int)items.size(); i++)
	{
		bool flag1 = EQGameSceneMgr::getSingleton().getHouse()->getTempFurniture(items[i].mData, furni);
		bool flag2 = luabind::call_function<bool>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_hasDataByCartNum", items[i].mData);
		if (!flag1 && !flag2)
		{
			continue;
		}
		if (flag1)	// 有要购买的家具
		{
			data.goods_id = ((ShopCenterRow*)(items[i].mPData))->good_id;
			data.floor = furni.floor;
			data.x = furni.x;
			data.z = furni.z;
			data.height = furni.height;
			data.face = furni.face;
			data.room_id = 0;
			data.type = 0;
			data.tex = "";
		}
		if (flag2)	// 有要购买墙纸地板材料
		{
			data.goods_id = ((ShopCenterRow*)(items[i].mPData))->good_id;
			data.floor = luabind::call_function<int>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_getFloorIdByCartNum", items[i].mData);
			data.x = 0;
			data.z = 0;
			data.height = 0;
			data.face = 0;
			data.room_id = luabind::call_function<int>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_getRoomIdByCartNum", items[i].mData);
			data.type = luabind::call_function<int>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_getCompByCartNum", items[i].mData);
			data.tex = luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_getTexByCartNum", items[i].mData);
		}
		pack.goods_list.push_back(data);
	}

	sGameMgr.getEQMsgManager()->Send(pack);
}
//-------------------------------------------------------
int Player::getShoppingCartPrice(int money_type)
{
	int price = 0;

	const std::vector<ItemGrid>& items = mFitModeShoppingCart.getTotalItemGrids();
	if(items.size() == 0)
		return 0;

	for(int i=0; i<(int)items.size(); i++)
		if(items[i].mPData && ((ShopCenterRow*)items[i].mPData)->money_type==money_type)
			price += ((ShopCenterRow*)items[i].mPData)->price;

	return price;
}
//-------------------------------------------------------
void Player::setStoragePack(notify_player_depot& pack)
{
	mStorageCoin = pack.money;

	mStorage.resize(pack.grid_vec.size());

	for(uint i=0; i < pack.grid_vec.size(); i++)
	{
		int year = pack.grid_vec[i].item_data.del_time.year;
		int month = pack.grid_vec[i].item_data.del_time.month;
		int day = pack.grid_vec[i].item_data.del_time.day;
		int hour = pack.grid_vec[i].item_data.del_time.hour;
		int minute= pack.grid_vec[i].item_data.del_time.minute;
		int second = pack.grid_vec[i].item_data.del_time.second;
		std::string delTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx", year, month, day, hour, minute, second,0);
		mStorage.setItem(
			i,
			pack.grid_vec[i].item_data.instance_id,
			delTime,
			pack.grid_vec[i].item_data.template_id,
			pack.grid_vec[i].count,pack.grid_vec[i].lock);
	}

	if(ObjMgr::getSingleton().isMainPlayer(getAccount()))
		updateStorage();
}
//-------------------------------------------------------
void Player::reqSaveMoneyToStorage(const uint moneyCount) const
{
	req_deposit_money_in_depot req;
	req.money = static_cast<int>(moneyCount);

	sGameMgr.getEQMsgManager()->Send(req);
}
//-------------------------------------------------------
void Player::reqGetMoneyFromStorage(const uint moneyCount) const
{
	req_withdraw_money_in_depot req;
	req.money = static_cast<int>(moneyCount);

	sGameMgr.getEQMsgManager()->Send(req);
}
//-------------------------------------------------------
void Player::addMosaicEffect(Ogre::Vector3& p1, Ogre::Vector3& p2)
{
	mMosaicP1 = p1;
	mMosaicP2 = p2;

	Ogre::Vector3 vp1 = getPos() + mMainNode->getOrientation() * mMosaicP1;
	Ogre::Vector3 vp2 = getPos() + mMainNode->getOrientation() * mMosaicP2;

	Ogre::AxisAlignedBox box;
	box.setNull();
	box.merge(vp1);
	box.merge(vp2);

	int minX,minY,maxX,maxY;
	minX = minY = maxX = maxY = 0;
	_calcuBoxToRec(box, sEQOgreSys.getViewport()->getActualWidth(), sEQOgreSys.getViewport()->getActualHeight(), minX, minY, maxX, maxY);

	mMosaicID = sEQOgreSys.addMosaic( Ogre::Vector4(minX,minY,maxX,maxY) );
}
//------------------------------------------------------------------------------
void Player::removeMosaicEffect()
{
	if(mMosaicID)
	{
		sEQOgreSys.removeMosaic(mMosaicID);
		mMosaicID = 0;
	}
}
//------------------------------------------------------------------------------
void Player::_updateMosaicEffect()
{
	if(mMosaicID)
	{
		Ogre::Vector3 vp1 = getPos() + mMainNode->getOrientation() * mMosaicP1;
		Ogre::Vector3 vp2 = getPos() + mMainNode->getOrientation() * mMosaicP2;

		Ogre::AxisAlignedBox box;
		box.setNull();
		box.merge(vp1);
		box.merge(vp2);

		int minX,minY,maxX,maxY;
		minX = minY = maxX = maxY = 0;
		_calcuBoxToRec(box, sEQOgreSys.getViewport()->getActualWidth(), sEQOgreSys.getViewport()->getActualHeight(), minX, minY, maxX, maxY);

		sEQOgreSys.updateMosaic(mMosaicID, Ogre::Vector4(minX,minY,maxX,maxY));
	}
}
//------------------------------------------------------------------------------
void Player::_calcuBoxToRec(Ogre::AxisAlignedBox& box, int width, int height, int& minX, int& minY, int& maxX, int& maxY)
{
	const Ogre::Vector3* corners = box.getAllCorners();
	for(int i=0; i<8; i++)
	{
		Ogre::Vector3 offset = sEQOgreSys.getCamera()->getProjectionMatrix() * (sEQOgreSys.getCamera()->getViewMatrix() * corners[i]); 

		if(offset.x<-1.0f) offset.x=-1.0f;
		if(offset.x>1.0f) offset.x=1.0f;
		if(offset.y<-1.0f) offset.y=-1.0f;
		if(offset.y>1.0f) offset.y=1.0f;

		int x = width/2 + (int)((float)(width/2) * offset.x); 
		int y = height/2 - (int)((float)(height/2) * offset.y); 

		if(minX==0 && maxX==0 && minY==0 && maxY==0)
		{
			minX = maxX = x;
			minY = maxY = y;
		}
		else
		{
			if(x<minX) minX = x;
			if(x>maxX) maxX = x;
			if(y<minY) minY = y;
			if(y>maxY) maxY = y;
		}
	}
}
//------------------------------------------------------------------------------