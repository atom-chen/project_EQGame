#include "EQMainPCH.h"

#include "ItemTable.h"
#include "GraphicsTable.h"
#include "AvatarTable.h"
#include "CommFunc.h"
#include "XmlDataCenter.h"
#include <string>

//-----------------------------------------------------------------------
template<> ItemTableManager* Ogre::Singleton<ItemTableManager>::ms_Singleton = 0;
//-----------------------------------------------------------------------
void ItemTableManager::loadSmallTypesTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "item_small_type_desc.xml" ) );

	const size_t colSex			= x.getColumnIndex( "sex" );
	const size_t colBigType		= x.getColumnIndex( "big_type" );
	const size_t colSmallTypes	= x.getColumnIndex( "small_types" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		SmallTypesRow r;
		r.sex			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSex ) );
		r.big_type		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colBigType ) );
		std::string sDescs 	= x.cell( iRow, colSmallTypes, true );
		r.small_types	= Ogre::StringUtil::split(sDescs, ",");
		for ( uint i = 0; i < r.small_types.size(); ++i )
		{
			Ogre::StringUtil::trim(r.small_types[i]);
		}
		mSmallTypesTable.push_back(r);
	}	
}
//-----------------------------------------------------------------------
void ItemTableManager::loadItemTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "Item_tplt.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colName		= x.getColumnIndex( "name" );
	const size_t colType		= x.getColumnIndex( "type" );
	const size_t colImageset	= x.getColumnIndex( "imageset" );
	const size_t colIcon		= x.getColumnIndex( "icon" );
	const size_t colTooltip		= x.getColumnIndex( "intro_id" );
	const size_t colOverlap		= x.getColumnIndex( "overlap" );
	const size_t colBind		= x.getColumnIndex( "bind" );
	const size_t subID			= x.getColumnIndex( "sub_id" );
	const size_t sellPrice      = x.getColumnIndex( "sell_price" );
	const size_t useDel			= x.getColumnIndex( "use_del" );
	const size_t effectTimeID	= x.getColumnIndex( "effect_time_id" );
	const size_t useID			= x.getColumnIndex( "use_id" );
	const size_t voiceid		= x.getColumnIndex( "voiceid" );
	const size_t dvoiceid		= x.getColumnIndex( "dvoiceid" );
	const size_t uvoiceid		= x.getColumnIndex( "uvoiceid" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		ItemRow r;
		r.id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.name			= GB2312ToUTF8(x.cell( iRow, colName, true ).c_str());
		r.type			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType ) );
		r.icon			= x.cell( iRow, colIcon, true );
		r.imageset		= x.cell( iRow, colImageset, true );
		r.tooltip		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colTooltip ) );

		r.overlap		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colOverlap ) );
		r.bind			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colBind ) );
		r.subID			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, subID ) );
		r.sellPrice     = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, sellPrice ) ); 

		r.useDel		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, useDel ) );
		r.effectTimeID	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, effectTimeID ) );
		r.useID			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, useID ) );
		r.voiceid		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, voiceid ) );
		r.dvoiceid		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, dvoiceid ) );
		r.uvoiceid		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, uvoiceid ) );
		
		assert( mItemTable.find( r.id ) == mItemTable.end() );
		if ( mItemTable.find( r.id ) != mItemTable.end() )
			EQ_EXCEPT( FormatStr("Item_tplt.xml ID %d Repeat", r.id), "loadItemTable" );

		mItemTable.insert( std::make_pair( r.id, r) );
		mItemTableEx.insert( std::make_pair( r.name, r) );
	}	
}
//-----------------------------------------------------------------------
void ItemTableManager::loadItemEffectTimeTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "item_effect_time_tplt.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colType		= x.getColumnIndex( "type" );
	const size_t colEffectTime	= x.getColumnIndex( "effect_time" );
	const size_t colTradeCutTime	= x.getColumnIndex( "trade_cut_time" );
	const size_t colDelTime		= x.getColumnIndex( "del_time" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		ItemEffectTimeRow r;
		r.id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.type			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType ) );
		r.effectTime	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colEffectTime ) );
		r.tradeCutTime	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colTradeCutTime ) );
		r.delTime		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDelTime ) );


		assert( mItemEffectTimeTable.find( r.id ) == mItemEffectTimeTable.end() );
		if ( mItemEffectTimeTable.find( r.id ) != mItemEffectTimeTable.end() )
			EQ_EXCEPT( FormatStr("item_effect_time_tplt.xml ID %d Repeat", r.id), "loadItemEffectTimeTable" );

		mItemEffectTimeTable.insert( std::make_pair( r.id, r) );
	}	
}
//-----------------------------------------------------------------------
void ItemTableManager::loadItemEquipTable()
{	
	XmlDataTable x;
	x.loadResource( OpenResource( "item_dress_tplt.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colName		= x.getColumnIndex( "name" );
	const size_t colSex			= x.getColumnIndex( "sex" );
	const size_t colEquipPos	= x.getColumnIndex( "equip_pos" );
	const size_t colEquipPlaceHolder	= x.getColumnIndex( "equip_placeholder" );
	const size_t colEquipDispID	= x.getColumnIndex( "equip_disp_id" );

	const size_t colPlace		= x.getColumnIndex( "place" );
	const size_t colAction		= x.getColumnIndex( "action" );
	const size_t colType1		= x.getColumnIndex( "type1" ); // 大分类
	const size_t colType2		= x.getColumnIndex( "type2" ); // 小分类

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		ItemEquipRow r;
		r.id				= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.name				= x.cell( iRow, colName, true );
		r.sex				= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSex ) );
		r.equip_pos			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colEquipPos ) );
		r.equip_placeholder	= StringBitsetToUInt( x.cell( iRow, colEquipPlaceHolder ) );
		r.equip_disp_id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colEquipDispID ) );
		r.place				= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPlace ) );
		r.action			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colAction ) );
		r.type1				= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType1 ) );
		r.type2				= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType2 ) );

		assert( mItemEquipTable.find( r.id ) == mItemEquipTable.end() );
		mItemEquipTable.insert( std::make_pair( r.id, r) );
	}	
}
//-----------------------------------------------------------------------
void ItemTableManager::loadFashionTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "SelectFashion.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colSex			= x.getColumnIndex( "sex" );
	const size_t colSort1		= x.getColumnIndex( "sort1" );
	const size_t colSort2		= x.getColumnIndex( "sort2" );
	const size_t colDressId		= x.getColumnIndex( "dressid" );


	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		FashionSelectRow fashionRow;
		fashionRow.id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		fashionRow.sex			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSex ) );
		fashionRow.sort1		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSort1 ) );
		fashionRow.sort2		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSort2 ) );
		fashionRow.dressId		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDressId ) );

		assert( mFashionTable.find( fashionRow.id ) == mFashionTable.end() );
		mFashionTable.insert( std::make_pair( fashionRow.id, fashionRow) );
	}
}

//-----------------------------------------------------------------------
void ItemTableManager::loadSysMsgTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "sysmsg.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colContent		= x.getColumnIndex( "content" );
	const size_t colType		= x.getColumnIndex( "type" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		SysMsgRow sysMsgRow;
		sysMsgRow.id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		sysMsgRow.content		= GB2312ToUTF8(x.cell( iRow, colContent, true).c_str());
		sysMsgRow.type			= x.cell( iRow, colType, true);

		assert( mSysMsgTable.find( sysMsgRow.id ) == mSysMsgTable.end() );
		mSysMsgTable.insert( std::make_pair( sysMsgRow.id, sysMsgRow) );
	}
}
//-----------------------------------------------------------------------
void ItemTableManager::loadMutexActionMsgTable()
{
	// 加入互斥系统错误信息(号码段10000 ~ 19999)

	XmlDataTable x;
	x.loadResource( OpenResource( "mutex_actions_sysmsg.xml" ) );

	
	const size_t colID	 = x.getColumnIndex( "id" );
	const size_t colName = x.getColumnIndex( "name" );
	const size_t colAllowActions = x.getColumnIndex( "allow_actions" );

	std::vector<std::string> nameList; 	

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		uint id = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		const std::string name = x.cell( iRow, colName, true);
		nameList.push_back(name);
	}

	for ( uint iRow = 0; iRow < nameList.size(); ++iRow )
	{
		const std::string nameX = nameList[iRow];
		std::vector<std::string> typeArr = Ogre::StringUtil::split(x.cell( iRow, colAllowActions ), ",");
		//arrType = x.cell( iRow, colAllowActions );
		
		for ( uint iCol = 0; iCol < typeArr.size(); ++iCol )
		{
			const std::string nameY = nameList[iCol];
			// 错误ID
			uint errID = 10000 + (iRow + 1) * 100 + (iCol + 1);

			// 错误内容
			std::string content;
			if ( iRow == iCol )
				content = FormatStr("正在%s中", nameX.c_str());
			else
				content = FormatStr("%s时，无法%s", nameX.c_str(), nameY.c_str());

			std::string sShowType = typeArr[iCol];
			if (sShowType != "0") // 0表示不需要报错
			{
				SysMsgRow sysMsgRow;
				sysMsgRow.id = errID;
				sysMsgRow.content = GB2312ToUTF8(content.c_str());
				sysMsgRow.type = sShowType; 
				// 没有才添加
				if (mSysMsgTable.find( sysMsgRow.id ) == mSysMsgTable.end() )
				{
					mSysMsgTable.insert( std::make_pair( sysMsgRow.id, sysMsgRow) );
				}
			}
		}		
	}
}

//-----------------------------------------------------------------------
void ItemTableManager::load()
{
	loadItemTable();
	//
	loadItemEffectTimeTable();
	loadItemEquipTable();
	//
	loadFashionTable();

	//
	loadSmallTypesTable();
	loadSysMsgTable();
	loadMutexActionMsgTable(); // 加入互斥系统错误信息(号码段10000 ~ 19999)
}
//-----------------------------------------------------------------------
const ItemTable & ItemTableManager::getItemTable() const
{
	return mItemTable;
}
//-----------------------------------------------------------------------
const ItemRow * ItemTableManager::getItemRow( uint id ) const
{
	ItemTable::const_iterator it = mItemTable.find( id );
	if ( it == mItemTable.end() )
	{
		return NULL;
	}
	return &it->second;

}
//-----------------------------------------------------------------------
const ItemRow * ItemTableManager::getItemRowEx( std::string name ) const
{
	ItemTableEx::const_iterator it = mItemTableEx.find( name );
	if ( it == mItemTableEx.end() )
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------------
const ItemRow * ItemTableManager::getItemRowBySubId( uint sub_id, uint type ) const
{
	ItemTable::const_iterator it = mItemTable.begin();
	for (it; mItemTable.end()!=it; ++it)
	{
		 if ((it->second.subID==sub_id) && (it->second.type==type))
		 {
			 return &it->second;
		 }
	}
	return NULL;
}
//-----------------------------------------------------------------------
const ItemEffectTimeRow * ItemTableManager::getItemEffectTimeRow( uint id ) const
{
	ItemEffectTimeTable::const_iterator it = mItemEffectTimeTable.find( id );
	if ( it == mItemEffectTimeTable.end() )
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------------
const ItemEquipRow * ItemTableManager::getItemEquipRow( uint id ) const
{
	ItemEquipTable::const_iterator it = mItemEquipTable.find( id );
	if ( it == mItemEquipTable.end() )
	{	
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------------
const ItemEquipRow * ItemTableManager::getEquipRowByItem( uint id ) const
{
	const ItemRow* itemrow = getItemRow(id);

	if (NULL == itemrow || itemrow->type != IT_EQUIPMENT)
		return NULL;
	
	//

	ItemEquipTable::const_iterator it = mItemEquipTable.find( itemrow->subID );
	if ( it == mItemEquipTable.end() )
		return NULL;
	return &it->second;

}
//-----------------------------------------------------------------------
const FashionSelectTable & ItemTableManager::getFashionSelectTable() const
{
	return mFashionTable;
}
//-----------------------------------------------------------------------
const std::vector<SmallTypesRow> & ItemTableManager::getSmallTypesTable() const
{
	return mSmallTypesTable;
}
//-----------------------------------------------------------------------
const FashionSelectRow * ItemTableManager::getFashionRow(uint id) const
{
	FashionSelectTable::const_iterator it = mFashionTable.find( id );
	if ( it == mFashionTable.end() )
	{	
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------------
const uint ItemTableManager::getFashionRowCount() const
{
	return mFashionTable.size();
}

//-----------------------------------------------------------------------
const SysMsgRow * ItemTableManager::getSysMsgRow( uint id ) const
{
	SysMsgRowTable::const_iterator it = mSysMsgTable.find( id );
	if ( it == mSysMsgTable.end() )
	{
		return NULL;
	}
	return &it->second;

}
//-----------------------------------------------------------------------
void ItemTableManager::getSlotCollect( uint slot, std::vector<const ItemEquipRow *> & arr ) const
{
	arr.clear();
	for ( ItemEquipTable::const_iterator it = mItemEquipTable.begin();
		it != mItemEquipTable.end();
		++it )
	{		
		if ( BitContains( it->second.equip_placeholder, slot ) )
			arr.push_back( &it->second );
	}
}
//-----------------------------------------------------------------------
void ItemTableManager::getBigTypeCollect( uint bigType, std::vector<const ItemEquipRow *> & arr ) const
{
	arr.clear();
	for ( ItemEquipTable::const_iterator it = mItemEquipTable.begin();
		it != mItemEquipTable.end();
		++it )
	{		
		if ( it->second.type1 == bigType )
			arr.push_back( &it->second );
	}
}
//-----------------------------------------------------------------------
void ItemTableManager::getItemModels( uint id, std::vector<const ModelDisplayRow *> & modelRows ) const
{
	modelRows.clear();
	const ItemEquipRow * pItem = ItemTableManager::getSingleton().getItemEquipRow( id );
	if ( !pItem )
		return;

	const EquipDisplayRow * pEquip = AvatarTableManager::getSingleton().getEquipDisplayRow( pItem->equip_disp_id );
	if ( !pEquip )
		return;

	{
		const ModelDisplayRow * pDisp = GraphicsTableManager::getSingleton().getModelDisplayRow( pEquip->share1 );
		if ( pDisp )
			modelRows.push_back( pDisp );
	}
	{
		const ModelDisplayRow * pDisp = GraphicsTableManager::getSingleton().getModelDisplayRow( pEquip->share2 );
		if ( pDisp )
			modelRows.push_back( pDisp );
	}
	{
		const ModelDisplayRow * pDisp = GraphicsTableManager::getSingleton().getModelDisplayRow( pEquip->share3 );
		if ( pDisp )
			modelRows.push_back( pDisp );
	}
	{
		const ModelDisplayRow * pDisp = GraphicsTableManager::getSingleton().getModelDisplayRow( pEquip->hang1 );
		if ( pDisp )
			modelRows.push_back( pDisp );
	}
	{
		const ModelDisplayRow * pDisp = GraphicsTableManager::getSingleton().getModelDisplayRow( pEquip->hang2 );
		if ( pDisp )
			modelRows.push_back( pDisp );
	}
}
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
const std::string SysMsgRow::randContent() const
{
	// 去除尾部分号
	std::string::const_iterator itEnd = content.end();
	if (content.size() > 0 )
	{
		if (*(itEnd - 1) == ';')
			--itEnd;
	}
	std::string sContent(content.begin(), itEnd);

	// 用';'切分字串
	std::vector<Ogre::String> arr = Ogre::StringUtil::split(sContent, ";");
	
	if (arr.size() > 0)
	{
		// 随机取一条
		size_t idx = rand() % arr.size();
		return arr[idx];
	}
	return "";
}
