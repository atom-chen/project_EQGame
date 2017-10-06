#include "EQMainPCH.h"

#include "HouseTable.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "XmlDataCenter.h"
#include "AABBBox.h"

//-----------------------------------------------------------------------
template<> HouseTableManager* Ogre::Singleton<HouseTableManager>::ms_Singleton = 0;

//-----------------------------------------------------------------------
void HouseTableManager::load()
{
	loadPlayerHouseTable();
	loadHouseGridTypeTable();
	loadHouseCompPropTable();
	loadHouseCompTable();
}
//-----------------------------------------------------------------------
const HouseCompPropRow*	HouseTableManager::getHouseCompPropRow( ushort id ) const
{
	HouseCompPropTable::const_iterator it = mHouseCompPropTable.find( id );
	if ( it == mHouseCompPropTable.end() )
	{	
		std::string err = "id '' in HouseCompProp.xml is not exist!";
		char strID[10];
		sprintf(strID, "%d",id);
		err.insert(4,strID);
		LOGERR(err);
		return NULL;
	}
	return &it->second;
}
//----------------------------------------------------------------------
void HouseTableManager::loadPlayerHouseTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource("house_tplt.xml") );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colName		= x.getColumnIndex( "name" );
	const size_t collevel		= x.getColumnIndex( "level" );
	const size_t coltype		= x.getColumnIndex( "type" );
	const size_t colSceneID		= x.getColumnIndex( "scene_id" );
	const size_t colhouse_furniture	= x.getColumnIndex( "house_furniture" );
	const size_t colMaxPlayer   = x.getColumnIndex( "max_players" );
	const size_t colPreviewBorn = x.getColumnIndex( "preview_born" );

	std::string str;
	std::vector<std::string> strArr;
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		PlayerHouseRow r;
		r.id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.name		= GB2312ToUTF8(x.cell( iRow, colName, true ).c_str());
		r.level		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, collevel ) );
		r.type		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, coltype ) );
		r.house_furniture= x.cell( iRow, colhouse_furniture, true );
		r.scene_id	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSceneID, true ) );
		r.maxPlayer = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colMaxPlayer ) );
		//r.type	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, coltype ) );
		r.previewBorn = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPreviewBorn ) );

		if ( mPlayerHouseTable.find( r.id ) != mPlayerHouseTable.end() )
			EQ_EXCEPT( FormatStr("HouseComp.xml ID %d Repeat", r.id), "HouseTableManager::loadHouseCompTable" );


		mPlayerHouseTable.insert( std::make_pair( r.id, r ) );
	}
}
//-----------------------------------------------------------------------
void HouseTableManager::loadHouseCompPropTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "HouseCompProp.xml" ) );

	const size_t colPropID		= x.getColumnIndex( "PropID" );
	const size_t colName		= x.getColumnIndex( "Name" );
	const size_t colBeWatchMode	= x.getColumnIndex( "BeWatchMode" );
	const size_t colHoldGrid	= x.getColumnIndex( "HoldGrid" );
	const size_t colStopGrid	= x.getColumnIndex( "StopGrid" );
	const size_t colLayGrid		= x.getColumnIndex( "LayGrid" );
	const size_t colSelfHeight	= x.getColumnIndex( "SelfHeight" );
	const size_t colOffsetHeight= x.getColumnIndex( "OffsetHeight" );

	const size_t colLayType		= x.getColumnIndex( "LayType" );
	const size_t colAcceptLay	= x.getColumnIndex( "AcceptLay" );
	const size_t colCanRep		= x.getColumnIndex( "CanRep" );
	const size_t colLightID		= x.getColumnIndex( "LightID" );
	const size_t colHoldType	= x.getColumnIndex( "HoldType" );

	std::string str;
	std::vector<std::string> posArr; 
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{	
		HouseCompPropRow r;
		r.PropID	= (ushort)Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPropID ) );
		r.Name		= x.cell( iRow, colName, true );
		str = x.cell( iRow, colBeWatchMode );
		if ( str.empty() )
			r.BeWatchMode = 3;		//默认值
		else
			r.BeWatchMode = (uchar)Ogre::StringConverter::parseUnsignedLong( str );

		//
		if ( mHouseCompPropTable.find( r.PropID ) != mHouseCompPropTable.end() )
			EQ_EXCEPT( FormatStr("HouseCompProp.xml PropID %d Repeat", r.PropID), "HouseTableManager::loadHouseCompPropTable" );

		if ( 0 == r.PropID )
			EQ_EXCEPT( "HouseCompProp.xml PropID ivaild", "HouseTableManager::loadHouseCompPropTable" );

		if ( r.BeWatchMode < 0 || r.BeWatchMode > 3 )
			EQ_EXCEPT( "HouseCompProp.xml BeWatchMode ivaild", "HouseTableManager::loadHouseCompPropTable" );


		// 占据网格
		str = x.cell( iRow, colHoldGrid, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			posArr = Ogre::StringUtil::split( str, " \r\n\t()" );
			for ( std::vector<std::string>::iterator it = posArr.begin();
				it != posArr.end();
				++it )
			{
				HouseCompPropRow::GridPos pos;

				if ( !extract2DPos( *it, pos.x, pos.y ) )
					EQ_EXCEPT( str + ", HouseCompProp.xml HoldGrid ivaild", "HouseTableManager::loadHouseCompPropTable" );

				r.HoldGrid.push_back( pos );
			}
		}
		if ( r.HoldGrid.size() == 0 )
			r.HoldGrid.push_back(HouseCompPropRow::GridPos(0,0));
		
		Int2AABBBox box;
		for ( HouseCompPropRow::GridPosArray::iterator it = r.HoldGrid.begin(); it != r.HoldGrid.end(); ++it )
			box.merge(it->x, it->y);

		assert(!box.mIsNull);
		if ( box.volume() != (int)r.HoldGrid.size() || !box.intersects(0,0) )
		{
			//占据网格的集合必须是矩形, 且有0,0点
			EQ_EXCEPT( "HouseCompProp.xml '" + Ogre::StringConverter::toString(r.PropID) + "' HoldGrid invaild!",
				"HouseTableManager::loadHouseCompPropTable" );
		}


		// 阻挡网格
		str = x.cell( iRow, colStopGrid, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			posArr = Ogre::StringUtil::split( str, " \r\n\t()" );
			for ( std::vector<std::string>::iterator it = posArr.begin();
				it != posArr.end();
				++it )
			{
				HouseCompPropRow::GridPos pos;

				if ( !extract2DPos( *it, pos.x, pos.y ) )
					EQ_EXCEPT( str + ", HouseCompProp.xml StopGrid ivaild", "HouseTableManager::loadHouseCompPropTable" );
				
				// 必须占据网格有的格子,阻挡网格才能有.
				/*if ( std::find( r.HoldGrid.begin(), r.HoldGrid.end(), pos ) == r.HoldGrid.end() )*/
				if ( !box.intersects(pos.x, pos.y) )
					EQ_EXCEPT( FormatStr("can't find StopGrid pos(%d,%d) in HoldGrid", pos.x, pos.y ), "HouseTableManager::loadHouseCompPropTable" );

				r.StopGrid.push_back( pos );
			}
		}

		// 被摆放网格
		str = x.cell( iRow, colLayGrid, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			posArr = Ogre::StringUtil::split( str, " \r\n\t()" );
			for ( std::vector<std::string>::iterator it = posArr.begin();
				it != posArr.end();
				++it )
			{
				HouseCompPropRow::GridPos pos;

				if ( !extract2DPos( *it, pos.x, pos.y ) )
					EQ_EXCEPT( str + ", HouseCompProp.xml LayGrid ivaild", "HouseTableManager::loadHouseCompPropTable" );
				
				// 必须占据网格有的格子,阻挡网格才能有.
				/*if ( std::find( r.HoldGrid.begin(), r.HoldGrid.end(), pos ) == r.HoldGrid.end() )*/
				if ( !box.intersects(pos.x, pos.y) )
					EQ_EXCEPT( FormatStr("can't find LayGrid pos(%d,%d) in HoldGrid", pos.x, pos.y ), "HouseTableManager::loadHouseCompPropTable" );

				r.LayGrid.push_back( pos );
			}
		}

		// 自身高度
		str = x.cell( iRow, colSelfHeight, true );
		if ( str.empty() )
		{
			r.SelfHeight	= 10;
		}
		else
			r.SelfHeight	= (ushort)Ogre::StringConverter::parseUnsignedLong( str );

		r.OffsetHeight	= (ushort)Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colOffsetHeight ) );
		
		// 类型
		r.LayType = strtoul( x.cell( iRow, colLayType, true ).c_str(), NULL, 16 );

		// 接受放置类型
		r.AcceptLay = strtoul( x.cell( iRow, colAcceptLay, true ).c_str(), NULL, 16 );
		
		// 是否可替换
		r.CanRep = ( 0 != Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colCanRep ) ) );

		// 占据类型，是否靠墙
		r.HoldType = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colHoldType ) );

		// 携带光源
		r.LightID = (ushort)Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLightID ) );

		//
		mHouseCompPropTable.insert( std::make_pair( r.PropID, r ) );
	}	
}
//-----------------------------------------------------------------------
const HouseCompRow*	HouseTableManager::getHouseCompRow( uint id ) const
{
	HouseCompTable::const_iterator it = mHouseCompTable.find( id );
	if ( it == mHouseCompTable.end() )
	{	
		std::string err = "id '' in house_comp.xml is not exist!";
		char strID[10];
		sprintf(strID, "%d",id);
		err.insert(4,strID);
		LOGERR(err);
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------------
void HouseTableManager::loadHouseCompTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "house_comp.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colName		= x.getColumnIndex( "name" );
	const size_t colPropID		= x.getColumnIndex( "prop_id" );
	const size_t colDisID		= x.getColumnIndex( "dis_id" );
	const size_t colPos			= x.getColumnIndex( "pos" );
	const size_t colRot			= x.getColumnIndex( "rot" );
	const size_t colScale		= x.getColumnIndex( "scale" );
	const size_t colLoopID		= x.getColumnIndex( "loop_id" );
	const size_t colLampType	= x.getColumnIndex( "lamp_type" );
	const size_t colParticle	= x.getColumnIndex( "particle" );

	std::string str;
	std::vector<std::string> strArr;
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		HouseCompRow r;
		r.ID		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.Name		= x.cell( iRow, colName, true );
		r.PropID	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPropID ) );
		r.DisID		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colDisID ) );
		r.LoopID	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLoopID ) );
		r.LampType	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLampType ) );

		// 位移信息
		str			= x.cell( iRow, colPos, true );
		Ogre::StringUtil::trim( str );
		if ( str.empty() )
		{
			r.Pos = Ogre::Vector3( 0, 0, 0 );
		}
		else
		{
			strArr = Ogre::StringUtil::split( str, "\t ," );
			if ( strArr.size() != 3 )
				EQ_EXCEPT( "HouseComp.xml Pos data error", "HouseTableManager::loadHouseCompTable" );

			r.Pos.x = Ogre::StringConverter::parseReal( strArr[0] );
			r.Pos.y = Ogre::StringConverter::parseReal( strArr[1] );
			r.Pos.z = Ogre::StringConverter::parseReal( strArr[2] );
		}

		// 旋转信息
		str			= x.cell( iRow, colRot, true );
		Ogre::StringUtil::trim( str );
		if ( str.empty() )
		{
			r.Rot = Ogre::Quaternion( 1, 0, 0, 0 );
		}
		else
		{
			strArr = Ogre::StringUtil::split( str, "\t ," );
			if ( strArr.size() != 4 )
				EQ_EXCEPT( "HouseComp.xml Rot data error", "HouseTableManager::loadHouseCompTable" );
			r.Rot.x = Ogre::StringConverter::parseReal( strArr[0] );
			r.Rot.y = Ogre::StringConverter::parseReal( strArr[1] );
			r.Rot.z = Ogre::StringConverter::parseReal( strArr[2] );
			r.Rot.w = Ogre::StringConverter::parseReal( strArr[3] );
			if(r.Rot.x==0 && r.Rot.y==0 && r.Rot.z==0 && r.Rot.w==0)
				EQ_EXCEPT( "HouseComp.xml Rot data invalid", "HouseTableManager::loadHouseCompTable" );
		}

		// 缩放信息
		str			= x.cell( iRow, colScale, true );
		Ogre::StringUtil::trim( str );
		if ( str.empty() )
		{
			r.Scale = Ogre::Vector3( 1, 1, 1 );
		}
		else
		{
			strArr = Ogre::StringUtil::split( str, "\t ," );
			if ( strArr.size() != 3 )
				EQ_EXCEPT( "HouseComp.xml Scale data error", "HouseTableManager::loadHouseCompTable" );
			r.Scale.x = Ogre::StringConverter::parseReal( strArr[0] );
			r.Scale.y = Ogre::StringConverter::parseReal( strArr[1] );
			r.Scale.z = Ogre::StringConverter::parseReal( strArr[2] );
		}

		str = x.cell( iRow, colParticle, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, " \r\n\t()," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.Particle.push_back( Ogre::StringConverter::parseUnsignedInt(*it) );
			}
		}

		if ( mHouseCompTable.find( r.ID ) != mHouseCompTable.end() )
			EQ_EXCEPT( "HouseComp.xml ID Repeat", "HouseTableManager::loadHouseCompTable" );


		mHouseCompTable.insert( std::make_pair( r.ID, r ) );
	}	
}
//-----------------------------------------------------------------------
bool HouseTableManager::extract2DPos( const std::string & s, int & x, int & y )
{
	std::vector<std::string> strs = Ogre::StringUtil::split( s, ", " );

	if ( strs.size() != 2 )
		return false;

	x = Ogre::StringConverter::parseInt( strs[0] );
	y = Ogre::StringConverter::parseInt( strs[1] );

	return true;		
}
//-----------------------------------------------------------------------
void HouseTableManager::loadHouseGridTypeTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "HouseGridType.xml" ) );

	const size_t colTypeID			= x.getColumnIndex( "TypeID" );
	const size_t colName			= x.getColumnIndex( "Name" );
	const size_t colColorSign		= x.getColumnIndex( "ColorSign" );
	const size_t colShowGridFitment	= x.getColumnIndex( "ShowGridFitment" );
	const size_t colAcceptLay		= x.getColumnIndex( "AcceptLay" );
	const size_t colInDoor			= x.getColumnIndex( "InDoor" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		HouseGridTypeRow r;
		r.TypeID	= (ushort)Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colTypeID ) );
		r.Name		= x.cell( iRow, colName, true );
		r.ColorSign	= extractColorFromString(x.cell( iRow, colColorSign ));
		r.ShowGridFitment = Ogre::StringConverter::parseBool(x.cell( iRow, colShowGridFitment ));

		r.AcceptLay = strtoul( x.cell( iRow, colAcceptLay, true ).c_str(), NULL, 16 );

		r.InDoor = Ogre::StringConverter::parseBool(x.cell( iRow, colInDoor ));

		//
		if ( mHouseGridTypeTable.find( r.TypeID ) != mHouseGridTypeTable.end() )
			EQ_EXCEPT( FormatStr("HouseGridType.xml TypeID %d Repeat!", r.TypeID), "HouseTableManager::loadHouseGridTypeTable" );

		mHouseGridTypeTable.insert( std::make_pair( r.TypeID, r ) );
	}

}
//-----------------------------------------------------------------------
const HouseGridTypeRow* HouseTableManager::getHouseGridTypeRow( ushort id ) const
{
	HouseGridTypeTable::const_iterator it = mHouseGridTypeTable.find( id );
	if ( it == mHouseGridTypeTable.end() )
	{	
		return NULL;
	}	
	return &it->second;
}
//-----------------------------------------------------------------------
const PlayerHouseRow* HouseTableManager::getPlayerHouseRow( int id ) const
{
	PlayerHouseTable::const_iterator it = mPlayerHouseTable.find( id );
	if ( it == mPlayerHouseTable.end() )
	{	
		return NULL;
	}
	return &it->second;
}




