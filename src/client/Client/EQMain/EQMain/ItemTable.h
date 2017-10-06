/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-04
* ����: ��Ʒ��ص����ݱ�
******************************************************************************/
#ifndef ItemTable_H
#define ItemTable_H

#include "EQMainPrerequisites.h"

/** ��Ʒ����
*/
enum ItemType 
{
	IT_EQUIPMENT = 1,
};


/** װ��������
*/
enum EquipSlotType
{
	ES_NONE		= 0,
	ES_HAT		= 0x0001,		//ñ��
	ES_COAT		= 0x0002,		//����
	ES_NOSE		= 0x0004,		//�۾�
	ES_PANTS	= 0x0008,		//����
	ES_BACK		= 0x0010,		//����
	ES_SHOES	= 0x0020,		//Ь��
	ES_HAND		= 0x0040,		//�ֳ�
	ES_ORNAMENT = 0x0080,		//����	
	ES_ALL		= 0x00FF,
};

/** װ�������
*/
enum ItemEquipBigType
{
	IEBT_NONE	= 0,	// 
	IEBT_SUIT	= 1,	// ��װ
	IEBT_COAT	= 2,	// ����
	IEBT_PANTS	= 3,	// ����
	IEBT_HAT	= 4,	// ñ��
	IEBT_BACK	= 5,	// ����/����
	IEBT_NOSE	= 6,	// �۾�
	IEBT_HAND	= 7,	// �ֳ�
	IEBT_ORNAMENT	= 8,	// ����
	IEBT_SHOES		= 9,	// Ь��
};

struct ItemEquipRow
{
	ItemEquipRow()
		: id( 0 )
		, name()
		, sex( 0 )
		, equip_pos( 0 )
		, equip_placeholder( 0 )
		, equip_disp_id( 0 )
		, place( 0 )
		, action( 0 )
	{
	}

	uint			id;
	std::string		name;				// ����
	uchar			sex;				// �Ա�
	uchar			equip_pos;			// װ��λ��
	uint			equip_placeholder;	// ռλ
	uint			equip_disp_id;		// װ������ID
	uint			place;				// ��ŷ�ʽ
	uint			action;				// ����ԭʼ����
	uint			type1;				// �����
	uint			type2;				// С����
};

/** ��ƷС������Ϣ
*/
struct SmallTypesRow
{
	SmallTypesRow()
		: sex(0)
		, big_type(0)
	{
	}

	uchar			sex;			// �Ա�
	uint			big_type;		// ����� 
	std::vector<std::string>	small_types;	// С����
};

/** ��Ʒ��Ϣ
*/
struct ItemRow
{
	ItemRow()
		: id( 0 )
		, type( 0 )
		, name("")
		, icon("")
		, imageset("")
		, tooltip(0)
		, overlap(0)
		, bind(0)
		, subID(0)
		, sellPrice(0)
		, useDel(0)
		, effectTimeID(0)
		, useID(0)
		, voiceid(-1)
		, dvoiceid(-1)
		, uvoiceid(-1)
	{
	}

	uint			id;
	std::string		name;
	uint 			type;
	std::string		imageset;
	std::string		icon;
	uint			tooltip;
	uint			overlap;
	uint			bind;
	uint			subID;
	uint            sellPrice;
	uint			useDel;
	uint			effectTimeID;
	uint			useID;
	uint			voiceid;//ʹ����Ч
	uint			dvoiceid;//������Ч
	uint			uvoiceid;//ץȡ��Ч
};

/** ��ƷʱЧ��Ϣ
*/
struct ItemEffectTimeRow
{
	ItemEffectTimeRow()
		: id( 0 )
		, type( 0 )
		, effectTime( 0 )
		, tradeCutTime( 0 )
		, delTime( 0 )

	{
	}

	uint			id;
	uint 			type;
	uint			effectTime;
	uint			tradeCutTime;
	uint			delTime;
};

/** ������ɫʱ�ķ�װѡ����Ϣ
*/
struct FashionSelectRow 
{
	FashionSelectRow()
		: id(0)
		, sex(0)
		, sort1(0)
		, sort2(0)
		, dressId(0)

	{
	}
	
	uint	id;			//����
	uint	sex;
	uint	sort1;		//����1
	uint	sort2;		//����2
	uint	dressId;	//��װId
};

/**ϵͳ��Ϣ�ṹ
*/
struct  SysMsgRow
{
	SysMsgRow()
		: id(0)
		, content("")
		,type("")
	{
	}

	/**  
		���ȡ��Ϣ���� (�ֺ�(;)�ָ���)
	*/
	const std::string randContent() const;
	
	uint id;
	std::string content;
	std::string type;
};

typedef stdext::hash_map<uint, ItemRow>				ItemTable;
typedef stdext::hash_map<std::string, ItemRow>		ItemTableEx;
typedef stdext::hash_map<uint, ItemEffectTimeRow>	ItemEffectTimeTable;
typedef stdext::hash_map<uint, ItemEquipRow>		ItemEquipTable; 
typedef stdext::hash_map<uint, FashionSelectRow>	FashionSelectTable;
typedef stdext::hash_map<uint, SysMsgRow>			SysMsgRowTable;

struct ModelDisplayRow;
class ItemTableManager : public Ogre::Singleton<ItemTableManager>
{
public:
	void load();
public:
	const ItemTable & getItemTable() const;
	const FashionSelectTable & getFashionSelectTable() const;
	const std::vector<SmallTypesRow> & getSmallTypesTable() const; 
public:
	const ItemRow * getItemRow( uint id ) const;
	const ItemRow * getItemRowEx( std::string name ) const;
	const ItemRow * getItemRowBySubId( uint sub_id, uint type ) const;
	const ItemEffectTimeRow * getItemEffectTimeRow( uint id) const;
	const ItemEquipRow * getItemEquipRow( uint id ) const;
	const ItemEquipRow * getEquipRowByItem( uint id ) const;
	//
	const FashionSelectRow * getFashionRow(uint id)	const;
	const uint getFashionRowCount() const; //��װѡ���ļ�¼��	
	const SysMsgRow * getSysMsgRow(uint id) const;
		
public:
	/** ȡ��ĳ��װ������ص�������Ʒ 
		@param slot װ����
	*/
	void getSlotCollect( uint slot, std::vector<const ItemEquipRow *> & arr ) const;

	/** ȡ�ô�����µ�������Ʒ 
		@param bigType ����� �ο�enum ItemEquipBigType
	*/
	void getBigTypeCollect( uint bigType, std::vector<const ItemEquipRow *> & arr ) const;

	/** ȡ��ĳ��װ������ص�����ģ�ͱ��� 
		@param slot װ����
	*/
	void getItemModels( uint id, std::vector<const ModelDisplayRow *> & modelRows ) const;

private:
	void loadItemTable();
	void loadItemEffectTimeTable();
	void loadItemEquipTable();
	//
	void loadFashionTable();
	void loadSysMsgTable();
	void loadMutexActionMsgTable();
	void loadSmallTypesTable();
private:
	ItemTable			mItemTable; 
	ItemTableEx			mItemTableEx;
	ItemEffectTimeTable	mItemEffectTimeTable;
	ItemEquipTable		mItemEquipTable;
	FashionSelectTable	mFashionTable;
	std::vector<SmallTypesRow> mSmallTypesTable;
	SysMsgRowTable		mSysMsgTable;
};






#endif