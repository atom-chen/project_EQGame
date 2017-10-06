/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-04
* ����: ��װ��ص����ݱ�
******************************************************************************/
#ifndef AvatarTable_H
#define AvatarTable_H

#include "EQMainPrerequisites.h"


/** �ҵ�����
*/
enum EquipHangType
{
	EHT_NONE = 0x00000000,
	EHT_HAT = 0x00000001,		// ͷ/ñ��
	EHT_LEFTHAND = 0x00000002,	// ����
	EHT_RIGHTHAND = 0x00000004,	// ����
	EHT_BACK = 0x00000008,		// ����
	EHT_GLASSES = 0x00000010,	// �۾�
};
/** �Ա�����
*/
enum GenderType
{
	GT_MALE		= 1,	// ��
	GT_FEMALE	= 2,	// Ů
};

/** ͷ���滻��
*/
struct HairReplaceRow
{
	HairReplaceRow()
	{
	}

	uint		hair;		
	uint		type[5]; 
};
typedef std::map<uint, HairReplaceRow>		HairReplaceTable; 


/** ��������
*/
struct SkelActionRow
{
	SkelActionRow()
	{
	}

	std::string name;
	std::string action;
	std::string desc;
	std::string logicName;
	std::string logicDesc;
};
typedef std::map<std::string, std::vector<SkelActionRow> >	SkelActionTable; 


/** װ������Ʒ�ҽӹ�ϵ
*/ 
struct EquipSlotHangRow
{
	EquipSlotHangRow()
		: rID( 0 )
		, slot( 0 )
		, hairRep( 0 )
	{
	}

	uint			rID;
	uint 			slot;
	std::string		bone1; 
	std::string		bone2; 
	uchar			hairRep;	
};
typedef std::map<uint, std::vector<EquipSlotHangRow> >		EquipSlotHangTable;


/** װ������
*/ 
struct EquipDisplayRow
{
	EquipDisplayRow()
		: id( 0 )
		, share1( 0 )
		, share2( 0 )
		, share3( 0 )
		, hang1( 0 )
		, hang2( 0 )
		, hangID( 0 )
	{
	}

	uint			id;
	uint			share1;
	uint			share2;
	uint			share3;
	uint			hang1;
	uint			hang2;
	uint			hangID;
};
typedef stdext::hash_map<uint, EquipDisplayRow>		EquipDisplayTable; 


/** ��ɫ����
*/ 
struct RoleRaceRow
{
	RoleRaceRow()
		: id( 0 )
		, race( 0 )
		, gender( GT_MALE )
		, skinColor( 0 )
		, hairColor( 0 )
		, defHead( 0 )
		, defBody( 0 )	
		, defHair( 0 )
		, defBeard( 0 )
		, defWear1( 0 )
		, defWear2( 0 )
		, initScale(100)
	{
	}

	uint			id;
	uchar 			race;
	uchar			gender;
	std::string 	skeleton;
	uchar			skinColor;
	uchar			hairColor;
	uint			defBody;
	uint			defHead;
	uint			defHair;
	uint			defBeard;
	uint			defWear1;
	uint			defWear2;
	float           initScale;
};
typedef std::map<uint, RoleRaceRow>					RoleRaceTable;


/** ��ɫ��������
*/
enum RoleSectionType
{
	RS_BODY = 0,
	RS_HEAD,
	RS_HAIR,
	RS_BEARD,		// ����
	RS_RAWEQUIP1,	// ��ʼװ��1
	RS_RAWEQUIP2,	// ��ʼװ��2
};


/** ��ɫ����
*/
struct RoleSectionRow
{
	RoleSectionRow()
		: roleID( 0 )
		, type( 0 )
		, color( 0 )
		, modDisID( 0 )
		, gender( GT_MALE )
		, canSel( true )
		, createVisible( true )
	{
	}

	uchar			type;
	uchar			color;
	uchar			gender;	
	bool			createVisible; // ���ƴ���ʱ�Ƿ�ɼ�
	bool			canSel;
	uint			roleID;
	uint			modDisID;
};
typedef std::multimap<uint, RoleSectionRow>			RoleSectionTable;
typedef std::vector<const RoleSectionRow *>			RoleSectionRows;

/** ����
*/
struct ChangeLooksRow
{
	ChangeLooksRow()
		: id(0)
		, money_type(0)
		, money(0)
	{
	}

	uint id;         // �Ա� * 10000 +  �ı�����(1.����, 2.��ɫ 3.���� 4.��ɫ 5.����) * 1000 + �ı�����ֵ(�·��ͣ�����ɫ)
	uint money_type; // ��������(1Ϊ��Ϸ��ң�2Ϊ�㿨����)
	uint money;      // ����
};
typedef std::map<uint, ChangeLooksRow>				ChangeLooksTable;

class AvatarTableManager : public Ogre::Singleton<AvatarTableManager>
{
public:
	typedef std::set<uint> TColors;
public:
	void load();	
public:
	const HairReplaceRow *		getHairReplaceRow( uint hair ) const; 
	const EquipDisplayRow *		getEquipDisplayRow( uint id ) const; 
	const RoleRaceRow * 		getRoleRaceRow( uint id ) const;
	const EquipSlotHangRow *	getEquipSlotHangRow( uint id, uint slot ) const;

	
	/** ȡ�øı���ò��ط���
		@param sex �Ա�
		@param type �ı�����(1.����, 2.��ɫ 3.���� 4.��ɫ 5.����) �ο�AvatarAdjustType
		@param val �·��ͣ��·�ɫ ֵ
	*/
	const ChangeLooksRow *		getChangeLooksRow( uint sex, uint type, uint val ) const;

	/** ȡ��ĳ���Ǹ�����ж���
	*/
	void getSkelActions( const std::string & sSkeleton, std::vector<std::string> & arr ) const; 
	
	/** ȡ�ý�ɫ����
		@param id: ��ɫ����id
	*/
	void getRoleSectionRows( uint id, RoleSectionRows & rows ) const;

	/** ȡ�ý�ɫ����
		@param id: ��ɫ����id
		@param type: ��ɫ��λ(ͷ������֮��)
	*/
	void getRoleSectionRows( uint id, uint type, RoleSectionRows & rows ) const;

	/** ȡ�ý�ɫ����
		@param id: ��ɫ����id
		@param type: ��ɫ��λ(ͷ������֮��)
		@param color: ��ɫ
	*/
	void getRoleSectionRows( uint id, uint type, uint color, RoleSectionRows & rows ) const;

	/** ȡ�ý�ɫ����
	@param id: ��ɫ����id
	@param type: ��ɫ��λ(ͷ������֮��)
	@param color: ��ɫ
	@param forCreateRole: �Ƿ�Ϊ������ɫ�õ�
	*/
	void getRoleSectionRows( uint id, uint type, uint color, RoleSectionRows & rows, 
							bool sel, bool forCreateRole ) const;

	void getSkeletons( std::vector<std::string> & arr ) const;

	/** ȡ��ĳ����������������
	*/
	void getSkelActionDescs( const std::string & sSkeleton, std::vector<std::string> & arr ) const; 
	/** ȡ��ĳ���������߼�����
	*/
	const char* getSkelActionLogicDesc( const std::string & sSkeleton, const std::string & sLogicName ) const;
public:
	const HairReplaceTable &	getHairReplaceTable() const; 
	const SkelActionTable &		getSkelActionTable() const; 
	const EquipSlotHangTable &	getEquipSlotHangTable() const;
	const EquipDisplayTable &	getEquipDisplayTable() const; 
	const RoleRaceTable	& 		getRoleRaceTable() const;
	const RoleSectionTable &	getRoleSectionTable() const;
public:
	/** ȡ��ĳ�ֽ�ɫ�ķ�ɫ�б�
	*/
	TColors getSkinColors( uint roleID ) const;

	/** ȡ��ĳ�ֽ�ɫ�ķ�ɫ�б�
	*/
	TColors getHairColors( uint roleID ) const;
private:
	void loadHairReplaceTable();
	void loadSkelActionTable();
	void loadEquipSlotHangTable();	
	void loadEquipDisplayTable();
	void loadRoleRaceTable();
	void loadCreateRoleRaceTable();
	void loadRoleSectionTable();
	void loadCreateRoleSectionTable();
	void loadChangeLooksTable();

	void _loadRoleSectionTable(const std::string & file);
	void _loadRoleRaceTable(const std::string & file);
private:	
	HairReplaceTable	mHairReplaceTable; 
	SkelActionTable		mSkelActionTable; 
	EquipSlotHangTable	mEquipSlotHangTable;
	EquipDisplayTable	mEquipDisplayTable; 
	RoleRaceTable		mRoleRaceTable;
	RoleSectionTable	mRoleSectionTable;
	ChangeLooksTable	mChangeLooksTable;
};




#endif