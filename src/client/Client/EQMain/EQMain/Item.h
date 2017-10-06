/******************************************************************************
* 作者: Hongjx
* 时间: 2009-11-27
* 描述: 物品类
******************************************************************************/
#ifndef Item_H
#define Item_H

#include "EQMainPrerequisites.h"
#include "ModelManagerGcs.h"


struct ItemEquipRow;
struct ItemRow;
/** 物品类
*/
class Item
{
public:
	explicit Item( uint id );
	explicit Item( uint id, std::string delTime);
	explicit Item( const ItemRow * pItemInfo );

	const ItemRow * getItemInfo() const;
	const std::string& getItemDelTime() const;
	const ItemEquipRow * getItemEquipInfo() const;
	
	//
	const ItemRow * getAvatarItemInfo() const;

	/** 判断是否装备
	*/
	bool isEquipment() const;

	/** 判断是否可以放到装备栏
		@param slot 装备栏
	*/
	bool existsEquipSlot( uint slot ) const;

	void setInstanceID(unsigned __int64 instanceID){mInstanceID = instanceID;}
	unsigned __int64 getInstanceID(){return mInstanceID;}
private:
	const ItemRow * mItemInfo;

	unsigned __int64 mInstanceID;

	std::string	mDelTime;
};








#endif