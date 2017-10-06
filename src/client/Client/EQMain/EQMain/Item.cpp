#include "EQMainPCH.h"

#include "EQMain.h"
#include "Item.h"
#include "ItemTable.h"




//-----------------------------------------------------------------------
Item::Item( const ItemRow * pItemInfo )
	: mItemInfo( pItemInfo ) 
{
	if ( !mItemInfo )
		EQ_EXCEPT( "物品信息不能为空", "Item::Item( const ItemRow * pItemInfo )" );
}
//-----------------------------------------------------------------------
Item::Item( uint id )
	: mItemInfo( NULL ) 
{
	mItemInfo = ItemTableManager::getSingleton().getItemRow( id );
	if ( !mItemInfo )
		EQ_EXCEPT( "物品信息不能为空", "Item::Item( const ItemRow * pItemInfo )" );
}
//-----------------------------------------------------------------------
Item::Item(uint id, std::string delTime)
	: mItemInfo( NULL )
{
	mItemInfo = ItemTableManager::getSingleton().getItemRow( id );
	mDelTime = delTime;
	if ( !mItemInfo )
		EQ_EXCEPT( "物品信息不能为空", "Item::Item( const ItemRow * pItemInfo )");
}
//-----------------------------------------------------------------------
const ItemRow * Item::getItemInfo() const
{
	return mItemInfo;
}
//-----------------------------------------------------------------------
const std::string& Item::getItemDelTime() const
{
	return mDelTime;
}
//-----------------------------------------------------------------------
const ItemEquipRow * Item::getItemEquipInfo() const
{
	const ItemEquipRow* e = NULL;
	if(mItemInfo->type == IT_EQUIPMENT)
		e = ItemTableManager::getSingleton().getItemEquipRow(mItemInfo->subID);
	
	return e;
}
//-----------------------------------------------------------------------
bool Item::isEquipment() const
{
	return (IT_EQUIPMENT == mItemInfo->type);
}
//-----------------------------------------------------------------------
//bool Item::existsEquipSlot( uint slot ) const
//{
//	//return BitContains( mItemInfo->equipSlot, slot );
//	const ItemEquipRow *  e = NULL;
//	
//	if(mItemInfo->type == IT_EQUIPMENT)
//		e = ItemTableManager::getSingleton().getItemEquipRow(mItemInfo->subID);
//
//	return e == NULL ? false : BitContains( e->equipSlot, slot );
//	//return BitContains( mItemInfo->equipRow->equipSlot, slot );
//}
//-----------------------------------------------------------------------