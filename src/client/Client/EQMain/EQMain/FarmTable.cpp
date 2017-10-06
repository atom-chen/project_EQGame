#include "EQMainPCH.h"
#include "FarmTable.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "XmlDataCenter.h"
#include <string>


//----------------------------------------------------------------
template<> FarmTableMgr* Ogre::Singleton<FarmTableMgr>::ms_Singleton = 0;

//----------------------------------------------------------------
void FarmTableMgr::load()
{
	loadFarmShopTable();
	loadSeedTable();
	loadOrnamentTable();
	loadFarmLevelTable();
	
	_loadFarmDisplayTable();
	_loadPlantDisplayTable();
	_loadFarmDecorationTable();
	loadMouseAnimateTable();
	loadCropStaNameTable();
	_loadFarmPropTable();
	_loadFarmTaskTable();
}

//----------------------------------------------------------------
void FarmTableMgr::_loadPlantDisplayTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "plant_display.xml" ) );

	const size_t colSeedid		= x.getColumnIndex( "seedid" );
	const size_t colStage		= x.getColumnIndex( "stage" );
	const size_t colFarmDisplay	= x.getColumnIndex( "farmDisplay" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		//PlantDisplayRow r;
		int seedid,
			farmdisplayid,
			stage;

		seedid			= Ogre::StringConverter::parseInt( x.cell( iRow, colSeedid ) );
		stage			= Ogre::StringConverter::parseInt( x.cell( iRow, colStage ) ) -1;
		farmdisplayid	= Ogre::StringConverter::parseInt( x.cell( iRow, colFarmDisplay ) );

		PlantDisplayTable::iterator it = mPlantDisplayTable.find(seedid);
		if( it == mPlantDisplayTable.end() )
		{
			PlantDisplayRow r;
			r.seedid = seedid;
			r.farmdisplayid[stage] = farmdisplayid;
			mPlantDisplayTable.insert( std::make_pair( r.seedid, r) );
		}
		else
		{
			PlantDisplayRow& row = it->second;
			if(stage >=0 && stage <CROP_STAGE_NUM)
				row.farmdisplayid[stage] = farmdisplayid;
			else
			{
				char buf[32]="/0";
				sprintf(buf,"err stage=%d > %d",stage+1,CROP_STAGE_NUM);
				LOGMSG(buf);
			}
		}
	}	

}
//----------------------------------------------------------------
void FarmTableMgr::_loadFarmDisplayTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_display.xml" ) );

	const size_t colid			= x.getColumnIndex( "id" );
	const size_t colModelDis	= x.getColumnIndex( "modelDisplayid" );
	const size_t colDisplayType	= x.getColumnIndex( "displayType" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		FarmDisplayRow r;
		r.id			= Ogre::StringConverter::parseInt( x.cell( iRow, colid ) );
		r.modelDisplayid= Ogre::StringConverter::parseInt( x.cell( iRow, colModelDis ) );
		r.displayType	= Ogre::StringConverter::parseInt( x.cell( iRow, colDisplayType ) );
		assert( mFarmDisplayTable.find( r.id ) == mFarmDisplayTable.end() );
		mFarmDisplayTable.insert( std::make_pair( r.id, r) );
	}	

}
//----------------------------------------------------------------
void FarmTableMgr::_loadFarmPropTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_prop_tplt.xml" ) );

	const size_t colid	= x.getColumnIndex( "id" );
	const size_t type	= x.getColumnIndex( "type" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		FarmPropRow r;
		r.prop_id	= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colid ) );
		r.prop_type = Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, type ) );
		assert( mFarmPropTable.find( r.prop_id ) == mFarmPropTable.end() );
		mFarmPropTable.insert( std::make_pair( r.prop_id, r) );
	}	

}
//----------------------------------------------------------------
void FarmTableMgr::_loadFarmDecorationTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_decoration_tplt.xml" ) );

	const size_t id				= x.getColumnIndex( "id" );
	const size_t type			= x.getColumnIndex( "type" );
	//const size_t period			= x.getColumnIndex( "period" );
	//const size_t experience		= x.getColumnIndex( "experience" );
	const size_t static_model	= x.getColumnIndex( "static_model" );
	const size_t dynamic_model	= x.getColumnIndex( "dynamic_model" );
	const size_t scene_name		= x.getColumnIndex( "scene_name" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		FarmDecorationRow r;
		r.id			= Ogre::StringConverter::parseInt( x.cell( iRow, id ) );
		r.type			= Ogre::StringConverter::parseInt( x.cell( iRow, type ) );
		//r.period	= Ogre::StringConverter::parseInt( x.cell( iRow, period ) );
		//r.experience			= Ogre::StringConverter::parseInt( x.cell( iRow, experience ) );

		Ogre::StringVector sv = Ogre::StringConverter::parseStringVector( x.cell( iRow, static_model ) );
		r.static_model.resize(sv.size());
		for(uint i=0; i<r.static_model.size();i++) 
			r.static_model[i]= Ogre::StringConverter::parseInt(sv[i]);

		sv = Ogre::StringConverter::parseStringVector( x.cell( iRow, dynamic_model ) );
		r.dynamic_model.resize(sv.size());
		for(uint i=0; i<r.dynamic_model.size();i++)
			r.dynamic_model[i]= Ogre::StringConverter::parseInt(sv[i]);

		sv = Ogre::StringConverter::parseStringVector( x.cell( iRow, scene_name ) );
		if (sv.size() == 1)
			r.scene_name = sv[0];

		//r.dynamic_model	= Ogre::StringConverter::parseInt( x.cell( iRow, dynamic_model ) );
		assert( mFarmDecorationTable.find( r.id ) == mFarmDecorationTable.end() );
		mFarmDecorationTable.insert( std::make_pair( r.id, r) );
	}	

}

//----------------------------------------------------------------
void FarmTableMgr::loadFarmShopTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_shop_tplt.xml" ) );

	const size_t colItemid			= x.getColumnIndex( "item_id" );
	const size_t colGamePrice		= x.getColumnIndex( "gameprice" );
	const size_t colEqPrice			= x.getColumnIndex( "eqprice" );
	const size_t colIntro			= x.getColumnIndex( "intro" );
	const size_t colItemLevel		= x.getColumnIndex( "level" );
	const size_t colSpecial			= x.getColumnIndex( "special" );
	const size_t colRepeatBuy		= x.getColumnIndex( "repeat_buy" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		FarmShopRow r;
		r.item_id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colItemid ) );
		r.gameprice			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colGamePrice ) );
		r.eqprice			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colEqPrice ) );
		r.intro				= GB2312ToUTF8(x.cell(iRow,colIntro,true).c_str());
		r.itemLevel			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colItemLevel ) );
		r.special			= GB2312ToUTF8(x.cell(iRow,colSpecial,true).c_str());
		r.repeatBuy			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colRepeatBuy ) );
		assert( mFarmShopTable.find( r.item_id ) == mFarmShopTable.end() );
		mFarmShopTable.insert( std::make_pair( r.item_id, r) );
	}	

}
//----------------------------------------------------------------
void FarmTableMgr::loadMouseAnimateTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "mouseanimation_tplt.xml" ) );

	const size_t colItemid			= x.getColumnIndex( "id" );
	const size_t colName			= x.getColumnIndex( "name" );
	const size_t colFrames			= x.getColumnIndex( "frames" );
	const size_t colTime			= x.getColumnIndex( "time" );
	const size_t colImageset		= x.getColumnIndex( "imageset" );
	const size_t colImageicon		= x.getColumnIndex( "imageicon" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		MouseAnimateRow r;
		r.id			= Ogre::StringConverter::parseInt( x.cell( iRow, colItemid ) );
		r.name			= GB2312ToUTF8(x.cell(iRow,colName,true).c_str());
		r.frames		= Ogre::StringConverter::parseInt( x.cell( iRow, colFrames ) );
		r.time			= Ogre::StringConverter::parseInt( x.cell( iRow, colTime ) );
		r.imgset		= x.cell(iRow,colImageset,true);
		r.icon			= x.cell(iRow,colImageicon,true);
		assert( mMouseAnimateTable.find( r.id ) == mMouseAnimateTable.end() );
		mMouseAnimateTable.insert( std::make_pair( r.id, r) );
	}	

}
//-----------------------------------------------------------------
void FarmTableMgr::loadCropStaNameTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "seed_title.xml" ) );

	const size_t colSeedid			= x.getColumnIndex( "seed_id" );
	const size_t colSproutName		= x.getColumnIndex( "sprout_time" );
	const size_t colS_leafName		= x.getColumnIndex( "s_leaf_time" );
	const size_t colB_leafName		= x.getColumnIndex( "b_leaf_time" );
	const size_t colBloomName		= x.getColumnIndex( "bloom_time" );
	const size_t colFruitName		= x.getColumnIndex( "fruit_time" );
	const size_t colPickName		= x.getColumnIndex( "pick_time" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		CropStaNameRow r;
		r.id			= Ogre::StringConverter::parseInt( x.cell( iRow, colSeedid ) );
		r.sprout_name	= GB2312ToUTF8(x.cell(iRow,colSproutName,true).c_str());
		r.s_leaf_name	= GB2312ToUTF8(x.cell(iRow,colS_leafName,true).c_str());
		r.b_leaf_name	= GB2312ToUTF8(x.cell(iRow,colB_leafName,true).c_str());
		r.bloom_name	= GB2312ToUTF8(x.cell(iRow,colBloomName,true).c_str());
		r.fruit_name	= GB2312ToUTF8(x.cell(iRow,colFruitName,true).c_str());
		r.pick_name		= GB2312ToUTF8(x.cell(iRow,colPickName,true).c_str());

		assert( mCropStaNameTable.find( r.id ) == mCropStaNameTable.end() );
		mCropStaNameTable.insert( std::make_pair( r.id, r) );
	}	
}
//-------------------------------------------------------------------
void FarmTableMgr::loadSeedTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_seed_tplt.xml" ) );

	const size_t colId			= x.getColumnIndex( "id" );
	const size_t colName		= x.getColumnIndex( "name" );
	const size_t colExpect_output			= x.getColumnIndex( "expect_output" );
	const size_t colSprout_time			= x.getColumnIndex( "sprout_time" );
	const size_t colS_leaf_time			= x.getColumnIndex( "s_leaf_time" );
	const size_t colB_leaf_time			= x.getColumnIndex( "b_leaf_time" );
	const size_t colBloom_time			= x.getColumnIndex( "bloom_time" );
	const size_t colFruit_time			= x.getColumnIndex( "fruit_time" );
	const size_t colPick_time			= x.getColumnIndex( "pick_time" );
	const size_t colUseLevel			= x.getColumnIndex( "use_level" );
	const size_t colGain_exp			= x.getColumnIndex( "pick_exp" );
	const size_t colFruit_id			= x.getColumnIndex( "fruit_id" );


	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		SeedRow r;
		r.id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colId ) );
		r.name			= GB2312ToUTF8( x.cell( iRow, colName, true).c_str() );
		r.expect_output			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colExpect_output ) );
		r.stageTimer[CS_SPROUT]			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colSprout_time ) );
		r.stageTimer[CS_SLEAF]		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colS_leaf_time ) );
		r.stageTimer[CS_BLEAF]			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colB_leaf_time ) );
		r.stageTimer[CS_BLOOM]			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colBloom_time ) );
		r.stageTimer[CS_FRUIT]			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colFruit_time ) );
		r.stageTimer[CS_PICK]			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPick_time ) );
		r.fruit_id						= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colFruit_id ) );

		r.sprout_time = r.stageTimer[CS_SPROUT];			
		r.s_leaf_time = r.stageTimer[CS_SLEAF];
		r.b_leaf_time = r.stageTimer[CS_BLEAF];			
		r.bloom_time  = r.stageTimer[CS_BLOOM];			
		r.fruit_time  = r.stageTimer[CS_FRUIT];			
		r.pick_time   = r.stageTimer[CS_PICK];			

		r.use_level			=	Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colUseLevel ) );;
		r.pick_exp			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colGain_exp ) );
		assert( mSeedTable.find( r.id ) == mSeedTable.end() );
		mSeedTable.insert( std::make_pair( r.id, r) );
	}	
}
//-------------------------------------------------------------------------
void FarmTableMgr::loadOrnamentTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_decoration_tplt.xml" ) );

	const size_t colId			= x.getColumnIndex( "id" );
	const size_t colType		= x.getColumnIndex( "type" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		OrnamentRow r;
		r.id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colId ) );
		r.type			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colType ) );

		assert( mOrnamentTable.find( r.id ) == mOrnamentTable.end() );
		mOrnamentTable.insert( std::make_pair( r.id, r) );
	}	
}
//-----------------------------------------------------------------------
void FarmTableMgr::loadFarmLevelTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_level_tplt.xml" ) );

	const size_t level			= x.getColumnIndex( "level" );
	const size_t next_exp		= x.getColumnIndex( "next_exp" );
	const size_t sow_exp			= x.getColumnIndex( "sow_exp" );
	const size_t water_exp			= x.getColumnIndex( "water_exp" );
	const size_t weed_exp			= x.getColumnIndex( "weed_exp" );
	const size_t worm_exp			= x.getColumnIndex( "worm_exp" );
	const size_t assart_exp			= x.getColumnIndex( "hoeing_exp" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		FarmLevelRow r;
		r.level			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, level ) );
		r.next_exp		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, next_exp ) );
		r.sow_exp		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, sow_exp ) );
		r.water_exp		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, water_exp ) );
		r.weed_exp		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, weed_exp ) );
		r.worm_exp		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, worm_exp ) );
		r.assart_exp	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, assart_exp ) );

		assert( mFarmLevelTable.find( r.level ) == mFarmLevelTable.end() );
		mFarmLevelTable.insert( std::make_pair( r.level, r) );
	}	
}
//-------------------------------------------------------------------------
void FarmTableMgr::_loadFarmTaskTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "farm_task_tplt.xml" ) );

	const size_t task_id			= x.getColumnIndex( "task_id" );
	const size_t title				= x.getColumnIndex( "title" );
	const size_t intention			= x.getColumnIndex( "intention" );
	const size_t describe			= x.getColumnIndex( "describe" );
	const size_t imageset			= x.getColumnIndex( "imageset" );
	const size_t image				= x.getColumnIndex( "image" );
	const size_t item_reward		= x.getColumnIndex( "fixed_reward_items" );
	const size_t item_count			= x.getColumnIndex( "fixed_reward_items_count" );
	const size_t game_reward		= x.getColumnIndex( "reward_game_coin" );
	const size_t eq_reward			= x.getColumnIndex( "reward_eq_coin" );
	const size_t exp				= x.getColumnIndex( "exp" );
	const size_t time_limit			= x.getColumnIndex( "time_limit" );
	const size_t is_repeat			= x.getColumnIndex( "is_repeat" );

	std::string temp_str;
	std::vector<std::string> temp_str_arr; 
	for (size_t iRow=0; x.getRowCount()!=iRow; ++iRow)
	{		
		FarmTaskRow r;
		r.id			= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, task_id));
		r.title			= GB2312ToUTF8(x.cell(iRow, title, true).c_str());
		r.intention		= GB2312ToUTF8(x.cell(iRow, intention, true).c_str());
		r.describe		= GB2312ToUTF8(x.cell(iRow, describe, true).c_str());

		// imageset
		temp_str = x.cell(iRow, imageset, true);
		Ogre::StringUtil::trim(temp_str);
		if (!temp_str.empty())
		{
			temp_str_arr = Ogre::StringUtil::split(temp_str, ",");
			for (std::vector<std::string>::iterator it=temp_str_arr.begin(); temp_str_arr.end()!=it; ++it)
			{
				r.imageset.push_back(*it);
			}
		}

		// image
		temp_str = x.cell(iRow, image, true);
		Ogre::StringUtil::trim(temp_str);
		if (!temp_str.empty())
		{
			temp_str_arr = Ogre::StringUtil::split(temp_str, ",");
			for (std::vector<std::string>::iterator it=temp_str_arr.begin(); temp_str_arr.end()!=it; ++it)
			{
				r.image.push_back(*it);
			}
		}

		r.item_reward	= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, item_reward));
		r.item_count	= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, item_count));
		r.game_reward	= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, game_reward));
		r.eq_reward		= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, eq_reward));
		r.exp			= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, exp));
		r.time_limit	= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, time_limit));
		r.is_repeat		= Ogre::StringConverter::parseUnsignedInt(x.cell(iRow, is_repeat));

		assert(m_farmTaskTable.find(r.id) == m_farmTaskTable.end());
		m_farmTaskTable.insert(std::make_pair(r.id, r));
	}	
}
//-------------------------------------------------------------------------
const FarmShopRow* FarmTableMgr::getFarmShopRow(uint index) const
{
	uint size = mFarmShopTable.size();
	FarmShopTable::const_iterator iter = mFarmShopTable.begin();
	if (index <= size)
	{
		for (uint i = 1; i< index;i++)
		{
			iter++;
		}
		if (iter == mFarmShopTable.end())
		{
			return NULL;
		}
		else
			return &iter->second;
		
	}
	else
		return NULL;
}

//--------------------------------------------------------------------------
const int FarmTableMgr::getFarmShopTableCount() const
{
	return mFarmShopTable.size();
}

//--------------------------------------------------------------------------
const FarmShopRow* FarmTableMgr::getFarmItemRow(uint id) const
{
	FarmShopTable::const_iterator it = mFarmShopTable.find( id );
	if ( it == mFarmShopTable.end() )
	{	
		return NULL;
	}
	return &it->second;

}

//------------------------------------------------------------------------
const SeedRow* FarmTableMgr::getSeedRow(uint id) const
{
	SeedTable::const_iterator it = mSeedTable.find( id );
	if ( it == mSeedTable.end() )
	{	
		return NULL;
	}
	return &it->second;
}

const OrnamentRow* FarmTableMgr::getOrnamentRow(uint id) const
{
	OrnamentTable::const_iterator it = mOrnamentTable.find(id);
	if( it == mOrnamentTable.end())
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------
const FarmLevelRow* FarmTableMgr::getFarmLevelRow(uint lv) const
{
	FarmLevelTable::const_iterator it = mFarmLevelTable.find(lv);
	if( it == mFarmLevelTable.end())
	{
		return NULL;
	}
	return &it->second;
}

//-----------------------------------------------------------------
const PlantDisplayRow* FarmTableMgr::getPlantDisplayRow(int id) const
{
	PlantDisplayTable::const_iterator it = mPlantDisplayTable.find(id);
	if( it == mPlantDisplayTable.end())
	{
		return NULL;
	}
	return &it->second;
}

//-----------------------------------------------------------------
const FarmDecorationRow* FarmTableMgr::getFarmDecorationRow(int id) const
{
	FarmDecorationTable::const_iterator it = mFarmDecorationTable.find(id);
	if( it == mFarmDecorationTable.end())
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------
const FarmPropRow* FarmTableMgr::getFarmPropRow(int id) const
{
	FarmPropTable::const_iterator it = mFarmPropTable.find(id);
	if( it == mFarmPropTable.end())
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------
const FarmDisplayRow* FarmTableMgr::getFarmDisplayRow(int id) const
{
	FarmDisplayTable::const_iterator it = mFarmDisplayTable.find(id);
	if( it == mFarmDisplayTable.end())
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------
const MouseAnimateRow* FarmTableMgr::getMouseAnimateRow(int id) const
{
	MouseAnimateTable::const_iterator it = mMouseAnimateTable.find(id);
	if( it == mMouseAnimateTable.end())
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------
const CropStaNameRow* FarmTableMgr::getCropStaNameRow(int id) const
{
	CropStaNameTable::const_iterator it = mCropStaNameTable.find(id);
	if( it == mCropStaNameTable.end())
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------
const FarmTaskRow* FarmTableMgr::getFarmTaskRow(uint id) const
{
	FarmTaskTable::const_iterator it = m_farmTaskTable.find(id);
	if (m_farmTaskTable.end() == it)
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------
int FarmTableMgr::getFarmTaskImagesetCount(uint id) const
{
	const FarmTaskRow* row = getFarmTaskRow(id);
	if (NULL == row)
	{
		return 0;
	}
	return row->imageset.size();
}
//-----------------------------------------------------------------
std::string FarmTableMgr::getFarmTaskImageset(uint id, uint i) const
{
	const FarmTaskRow* row = getFarmTaskRow(id);
	if (NULL == row)
	{
		return "";
	}
	if ((i<0) || ((size_t)i>=row->imageset.size()))
	{
		return "";
	}
	return row->imageset[i];
}
//-----------------------------------------------------------------
int FarmTableMgr::getFarmTaskImageCount(uint id) const
{
	const FarmTaskRow* row = getFarmTaskRow(id);
	if (NULL == row)
	{
		return 0;
	}
	return row->image.size();
}
//-----------------------------------------------------------------
std::string FarmTableMgr::getFarmTaskImage(uint id, uint i) const
{
	const FarmTaskRow* row = getFarmTaskRow(id);
	if (NULL == row)
	{
		return "";
	}
	if ((i<0) || ((size_t)i>=row->image.size()))
	{
		return "";
	}
	return row->image[i];
}
//-----------------------------------------------------------------