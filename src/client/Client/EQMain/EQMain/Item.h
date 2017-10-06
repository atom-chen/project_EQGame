/******************************************************************************
* ����: Hongjx
* ʱ��: 2009-11-27
* ����: ��Ʒ��
******************************************************************************/
#ifndef Item_H
#define Item_H

#include "EQMainPrerequisites.h"
#include "ModelManagerGcs.h"


struct ItemEquipRow;
struct ItemRow;
/** ��Ʒ��
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

	/** �ж��Ƿ�װ��
	*/
	bool isEquipment() const;

	/** �ж��Ƿ���Էŵ�װ����
		@param slot װ����
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