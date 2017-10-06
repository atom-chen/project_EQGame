#include "EQMainPCH.h"
#include "GraphicsTable.h"
#include "AvatarTable.h"
#include "CommFunc.h"
#include "XmlDataCenter.h"


//-----------------------------------------------------------------------
template<> AvatarTableManager* Ogre::Singleton<AvatarTableManager>::ms_Singleton = 0;
//-----------------------------------------------------------------------
void AvatarTableManager::loadHairReplaceTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "HairReplace.xml" ) );

	const size_t colHair		= x.getColumnIndex( "Hair" );
	const size_t colType1		= x.getColumnIndex( "Type1" );
	const size_t colType2		= x.getColumnIndex( "Type2" );
	const size_t colType3		= x.getColumnIndex( "Type3" );
	const size_t colType4		= x.getColumnIndex( "Type4" );
	const size_t colType5		= x.getColumnIndex( "Type5" );
	
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		HairReplaceRow r;
		assert( GetArraySize<uint>( r.type ) == 5 );

		r.hair		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colHair, true ) );
		r.type[0]	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType1, true ) );
		r.type[1]	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType2, true ) );
		r.type[2]	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType3, true ) );
		r.type[3]	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType4, true ) );
		r.type[4]	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType5, true ) );

		if ( mHairReplaceTable.find( r.hair ) != mHairReplaceTable.end() )
			EQ_EXCEPT( "HairReplace.xml Hair Repeat", "loadHairReplaceTable" );

		if ( r.hair == 0 )
			EQ_EXCEPT( "HairReplace.xml Hair null", "loadHairReplaceTable" );

		mHairReplaceTable.insert( std::make_pair( r.hair, r ) );
	}	
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadSkelActionTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "SkelAction.xml" ) );

	const size_t colName		= x.getColumnIndex( "Name" );
	const size_t colAction		= x.getColumnIndex( "Action" );
	const size_t colDesc		= x.getColumnIndex( "Desc" );
	const size_t colLogicName	= x.getColumnIndex( "LogicName" );
	const size_t colLogicDesc	= x.getColumnIndex( "LogicDesc" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		SkelActionRow r;

		r.name		= x.cell( iRow, colName, true );
		r.action	= x.cell( iRow, colAction, true );
		r.desc		= x.cell( iRow, colDesc, true );
		r.logicName = x.cell( iRow, colLogicName, true );
		r.logicDesc = GB2312ToUTF8(x.cell( iRow, colLogicDesc, true ).c_str());

		if ( r.name.empty() )
			EQ_EXCEPT( "SkelAction.xml Name null", "loadSkelActionTable" );


		mSkelActionTable[r.name].push_back( r );
	}	
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadEquipSlotHangTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "EquipSlotHang.xml" ) );

	const size_t colRID			= x.getColumnIndex( "RID" );
	const size_t colSlot		= x.getColumnIndex( "Slot" );
	const size_t colBone1		= x.getColumnIndex( "Bone1" );
	const size_t colBone2		= x.getColumnIndex( "Bone2" );
	const size_t colHairRep		= x.getColumnIndex( "HairRep" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		EquipSlotHangRow r;
		r.rID		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colRID ) );
		r.slot		= strtoul( x.cell( iRow, colSlot, true ).c_str(), NULL, 16 );
		r.bone1		= x.cell( iRow, colBone1, true );
		r.bone2		= x.cell( iRow, colBone2, true );
		r.hairRep	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colHairRep ) );


		if ( 0 == r.rID )
			EQ_EXCEPT( "EquipSlotHang.xml RID null", "loadEquipSlotHangTable" );
		if ( 0 == r.slot )
			EQ_EXCEPT( "EquipSlotHang.xml Slot null", "loadEquipSlotHangTable" );

		mEquipSlotHangTable[r.rID].push_back( r );
	}	
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadChangeLooksTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "change_looks_tplt.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colMoneyType	= x.getColumnIndex( "money_type" );
	const size_t colMoney		= x.getColumnIndex( "money" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		ChangeLooksRow r;
		r.id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.money_type= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colMoneyType ) );
		r.money		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colMoney ) );


		if ( 0 == r.id )
			EQ_EXCEPT( "change_looks_tplt.xml ID null", "loadChangeLooksTable" );

		mChangeLooksTable.insert( std::make_pair( r.id, r) );
	}	
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadEquipDisplayTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "EquipDisplay.xml" ) );

	const size_t colID			= x.getColumnIndex( "ID" );
	const size_t colShare1		= x.getColumnIndex( "Share1" );
	const size_t colShare2		= x.getColumnIndex( "Share2" );
	const size_t colShare3		= x.getColumnIndex( "Share3" );
	const size_t colHang1		= x.getColumnIndex( "Hang1" );
	const size_t colHang2		= x.getColumnIndex( "Hang2" );
	const size_t colHangID		= x.getColumnIndex( "HangID" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		EquipDisplayRow r;

		r.id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.share1	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colShare1 ) );
		r.share2	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colShare2 ) );
		r.share3	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colShare3 ) );
		r.hang1		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colHang1 ) );
		r.hang2		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colHang2 ) );
		r.hangID	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colHangID ) );
		
		if ( mEquipDisplayTable.find( r.id ) != mEquipDisplayTable.end() )
			EQ_EXCEPT( FormatStr("EquipDisplay.xml ID %d Repeat", r.id), "loadEquipDisplayTable" );

		if ( 0 == r.id )
			EQ_EXCEPT( "EquipDisplay.xml ID null", "loadEquipDisplayTable" );

		mEquipDisplayTable.insert( std::make_pair( r.id, r) );
	}	
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadRoleRaceTable()
{
	_loadRoleRaceTable("RoleRace.xml");
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadCreateRoleRaceTable()
{
	_loadRoleRaceTable("CreateRoleRace.xml");
}
//-----------------------------------------------------------------------
void AvatarTableManager::_loadRoleRaceTable(const std::string & file)
{
	XmlDataTable x;
	x.loadResource( OpenResource( file ) );
	if ( x.isEmpty() )
	{
		LOGWAR( file + " is empty." );
		return;
	}

	const size_t colID			= x.getColumnIndex( "ID" );
	const size_t colGender		= x.getColumnIndex( "Gender" );
	const size_t colSkeleton	= x.getColumnIndex( "Skeleton" );

	const size_t colSkinColor	= x.getColumnIndex( "SkinColor" );
	const size_t colHairColor	= x.getColumnIndex( "HairColor" );
	const size_t colDefHead		= x.getColumnIndex( "DefHead" );
	const size_t colDefHair		= x.getColumnIndex( "DefHair" );
	const size_t colDefBeard	= x.getColumnIndex( "DefBeard" );
	const size_t colDefWear1	= x.getColumnIndex( "DefWear1" );
	const size_t colDefWear2	= x.getColumnIndex( "DefWear2" );
	const size_t colInitScale	= x.getColumnIndex("InitScale");

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		RoleRaceRow r;
		r.id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.gender	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colGender ) );
		r.skeleton	= x.cell( iRow, colSkeleton, true );
		
		r.skinColor = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSkinColor ) );
		r.hairColor = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colHairColor ) );
		r.defHead	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDefHead ) );
		r.defHair	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDefHair ) );
		r.defBeard	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDefBeard ) );
		r.defWear1	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDefWear1 ) );
		r.defWear2	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDefWear2 ) );
		r.initScale = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colInitScale ) );

		if ( mRoleRaceTable.find( r.id ) != mRoleRaceTable.end() )
			EQ_EXCEPT( FormatStr("RoleRace.xml ID %d Repeat", r.id), "loadRoleRaceTable" );

		if ( 0 == r.id )
			EQ_EXCEPT( "RoleRace.xml ID null", "loadRoleRaceTable" );

		if ( r.skeleton.empty() )
			EQ_EXCEPT( "RoleRace.xml Skeleton null", "loadRoleRaceTable" );

		mRoleRaceTable.insert( std::make_pair( r.id, r) );
	}	
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadRoleSectionTable()
{
	_loadRoleSectionTable("RoleSection.xml");
}
//-----------------------------------------------------------------------
void AvatarTableManager::loadCreateRoleSectionTable()
{
	_loadRoleSectionTable("CreateRoleSection.xml");
}
//-----------------------------------------------------------------------
void AvatarTableManager::_loadRoleSectionTable(const std::string & file)
{
	XmlDataTable x;
	x.loadResource( OpenResource( file ) );
	if ( x.isEmpty() )
	{
		LOGWAR( file + " is empty." );
		return;
	}


	const size_t colRoleID		= x.getColumnIndex( "RoleID" );
	const size_t colType		= x.getColumnIndex( "Type" );
	const size_t colColor		= x.getColumnIndex( "Color" );
	const size_t colModDisID	= x.getColumnIndex( "ModDisID" );
	const size_t colGender		= x.getColumnIndex( "Gender" );
	const size_t colCanSel		= x.getColumnIndex( "canselect" );
	const size_t colCreateVisible = x.getColumnIndex( "CreateVisible" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		RoleSectionRow r;
		r.roleID	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colRoleID ) );
		r.type		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType ) );
		r.color		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colColor ) );
		r.modDisID	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colModDisID ) );
		r.gender	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colGender ) );
		r.canSel	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colCanSel ) ) != 0;
		r.createVisible	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colCreateVisible ) ) != 0;

		if ( 0 == r.roleID )
			EQ_EXCEPT( "RoleSection.xml RoleID null", "loadRoleSectionTable" );

		mRoleSectionTable.insert( std::make_pair( r.roleID, r) );
	}	
}
//-----------------------------------------------------------------------
void AvatarTableManager::load()
{
	loadHairReplaceTable();
	loadSkelActionTable();
	loadEquipSlotHangTable();	
	loadEquipDisplayTable();
	loadCreateRoleRaceTable();
	loadCreateRoleSectionTable();
	loadRoleRaceTable();
	loadRoleSectionTable();
	loadChangeLooksTable();
}
//-----------------------------------------------------------------------
const HairReplaceTable & AvatarTableManager::getHairReplaceTable() const
{
	return mHairReplaceTable;
}
//-----------------------------------------------------------------------
const SkelActionTable &	AvatarTableManager::getSkelActionTable() const
{
	return mSkelActionTable;
}
//-----------------------------------------------------------------------
const EquipSlotHangTable & AvatarTableManager::getEquipSlotHangTable() const
{
	 return mEquipSlotHangTable;
}
//-----------------------------------------------------------------------
const EquipDisplayTable & AvatarTableManager::getEquipDisplayTable() const
{
	return mEquipDisplayTable;
}
//-----------------------------------------------------------------------
const RoleRaceTable	& AvatarTableManager::getRoleRaceTable() const
{
	return mRoleRaceTable;
}
//-----------------------------------------------------------------------
const RoleSectionTable & AvatarTableManager::getRoleSectionTable() const
{
	return mRoleSectionTable;
}
//-----------------------------------------------------------------------
const HairReplaceRow * AvatarTableManager::getHairReplaceRow( uint hair ) const
{
	HairReplaceTable::const_iterator it = mHairReplaceTable.find( hair );
	if ( it == mHairReplaceTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
const ChangeLooksRow * AvatarTableManager::getChangeLooksRow( uint sex, uint type, uint val ) const
{
	uint id = sex * 10000 + type * 1000 + val;
	ChangeLooksTable::const_iterator it = mChangeLooksTable.find( id );
	if ( it == mChangeLooksTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
const EquipDisplayRow *	AvatarTableManager::getEquipDisplayRow( uint id ) const
{
	EquipDisplayTable::const_iterator it = mEquipDisplayTable.find( id );
	if ( it == mEquipDisplayTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
const RoleRaceRow *	AvatarTableManager::getRoleRaceRow( uint id ) const
{
	RoleRaceTable::const_iterator it = mRoleRaceTable.find( id );
	if ( it == mRoleRaceTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
void AvatarTableManager::getSkelActionDescs( const std::string & sSkeleton, std::vector<std::string> & arr ) const
{
	arr.clear();
	SkelActionTable::const_iterator itFind = mSkelActionTable.find( sSkeleton );
	if ( itFind == mSkelActionTable.end() )
		return;

	for ( SkelActionTable::value_type::second_type::const_iterator it = itFind->second.begin();
		it != itFind->second.end();
		++it )
	{
		arr.push_back( it->desc);
	}
}
//-----------------------------------------------------------------------
void AvatarTableManager::getSkelActions( const std::string & sSkeleton, std::vector<std::string> & arr ) const
{
	arr.clear();
	SkelActionTable::const_iterator itFind = mSkelActionTable.find( sSkeleton );
	if ( itFind == mSkelActionTable.end() )
		return;

	for ( SkelActionTable::value_type::second_type::const_iterator it = itFind->second.begin();
		it != itFind->second.end();
		++it )
	{
		arr.push_back( it->action );
	}
}
//-----------------------------------------------------------------------
void AvatarTableManager::getSkeletons( std::vector<std::string> & arr ) const
{
	arr.clear();

	for ( SkelActionTable::const_iterator it = mSkelActionTable.begin();
		it != mSkelActionTable.end();
		++it )
	{
		arr.push_back( it->first );
	}
}
//-----------------------------------------------------------------------
//static int convertSlot(int slot)
//{
//	int Ret = EHT_NONE;
//	switch( slot )
//	{
//	case ES_HAT:
//		Ret = EHT_HAT;
//		break;
//	case ES_HAND:
//		Ret = EHT_LEFTHAND;
//		Ret = EHT_RIGHTHAND;
//		break;
//	case ES_BACK:
//		Ret = EHT_BACK;
//		break;
//	case ES_GLASSES:
//		Ret = EHT_GLASSES;
//		break;
//	default :
//		break;
//	}
//	return Ret;
//}
//-----------------------------------------------------------------------
const EquipSlotHangRow * AvatarTableManager::getEquipSlotHangRow( uint id, uint slot ) const
{	
	EquipSlotHangTable::const_iterator itFind = mEquipSlotHangTable.find( id );
	if ( itFind == mEquipSlotHangTable.end() )
		return NULL;

	for ( EquipSlotHangTable::value_type::second_type::const_iterator it = itFind->second.begin();
		it != itFind->second.end();
		++it )
	{
		
		//if ( slot == it->slot ) 由于目前一个id只对应一条记录, 所以不比较装备栏是否正确
		//							另外 手持分左右手，即使知道是手持物品也无法确定装备在左手还是右手
			return &*it;		
	}

	return NULL;
}
//-----------------------------------------------------------------------
void AvatarTableManager::getRoleSectionRows( uint id, RoleSectionRows & rows ) const
{
	rows.clear();
	RoleSectionTable::_Paircc ret = mRoleSectionTable.equal_range( id );
	for ( RoleSectionTable::const_iterator it = ret.first; it != ret.second; ++it )
	{
		rows.push_back( &it->second );
	}
}
//-----------------------------------------------------------------------
void AvatarTableManager::getRoleSectionRows( uint id, uint type, RoleSectionRows & rows ) const
{
	rows.clear();
	RoleSectionTable::_Paircc ret = mRoleSectionTable.equal_range( id );
	for ( RoleSectionTable::const_iterator it = ret.first; it != ret.second; ++it )
	{
		if ( type == it->second.type )
			rows.push_back( &it->second );
	}
}
//-----------------------------------------------------------------------
void AvatarTableManager::getRoleSectionRows( uint id, uint type, uint color, RoleSectionRows & rows ) const
{
	rows.clear();
	RoleSectionTable::_Paircc ret = mRoleSectionTable.equal_range( id );
	for ( RoleSectionTable::const_iterator it = ret.first; it != ret.second; ++it )
	{
		if ( type == it->second.type && color == it->second.color )
			rows.push_back( &it->second );
	}
}
//-----------------------------------------------------------------------
void AvatarTableManager::getRoleSectionRows( uint id, uint type, uint color, RoleSectionRows & rows, 
											bool sel, bool forCreateRole ) const
{
	rows.clear();
	RoleSectionTable::_Paircc ret = mRoleSectionTable.equal_range( id );
	for ( RoleSectionTable::const_iterator it = ret.first; it != ret.second; ++it )
	{
		if ( type == it->second.type && color == it->second.color && sel == it->second.canSel )
		{
			if ( forCreateRole == true ) // 如果是创建角色用的, 只取createVisible为true的部件
			{
				if ( it->second.createVisible == true )
					rows.push_back( &it->second );
			}
			else
				rows.push_back( &it->second );
		}
	}
}
//-----------------------------------------------------------------------
AvatarTableManager::TColors AvatarTableManager::getSkinColors( uint RoleID ) const
{
	TColors arr;
	RoleSectionTable::_Paircc ret = mRoleSectionTable.equal_range( RoleID );
	// 收集Body的颜色列表
	for ( RoleSectionTable::const_iterator it = ret.first; it != ret.second; ++it )
	{
		if ( RS_BODY == it->second.type )
		{
			arr.insert( it->second.color );
		}
	}

	return arr;
}
//-----------------------------------------------------------------------
AvatarTableManager::TColors AvatarTableManager::getHairColors( uint RoleID ) const
{
	TColors arr;
	RoleSectionTable::_Paircc ret = mRoleSectionTable.equal_range( RoleID );
	// 收集Hair的颜色列表
	for ( RoleSectionTable::const_iterator it = ret.first; it != ret.second; ++it )
	{
		if ( RS_HAIR == it->second.type )
		{
			arr.insert( it->second.color );
		}
	}

	return arr;
}
//-----------------------------------------------------------------------
const char* AvatarTableManager::getSkelActionLogicDesc( const std::string & sSkeleton, const std::string & sLogicName ) const
{
	SkelActionTable::const_iterator itFind = mSkelActionTable.find( sSkeleton );
	if ( itFind == mSkelActionTable.end() )
		return NULL;

	for ( SkelActionTable::value_type::second_type::const_iterator it = itFind->second.begin();
		it != itFind->second.end();
		++it )
	{
		if(_stricmp((*it).logicName.c_str(), sLogicName.c_str()) == 0)
			return (*it).logicDesc.c_str();
	}

	return NULL;
}





