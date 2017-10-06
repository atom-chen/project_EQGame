#include "EQMainPCH.h"
#include "NPCTable.h"

#include "LogicTable.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "XmlDataCenter.h"


//-----------------------------------------------------------------------
template<> NPCTableMgr* Ogre::Singleton<NPCTableMgr>::ms_Singleton = 0;

//-----------------------------------------------------------------------
void NPCTableMgr::load()
{
	_loadNPCTable();
	_loadNPCMsgTable();
}
//-----------------------------------------------------------------------
const NPCRow* NPCTableMgr::getNPCRow( uint id ) const
{
	NPCTable::const_iterator it = mNPCTable.find( id );
	if ( it == mNPCTable.end() )
	{
		return NULL;
	}	
	return &it->second;
}
//-----------------------------------------------------------------------
const std::string& NPCTableMgr::getNPCMsgRow( uint id ) const
{
	NPCMsgTable::const_iterator it = mNPCMsgTable.find( id );
	assert ( it != mNPCMsgTable.end() );
	if(it == mNPCMsgTable.end())
	{
		LOGERR(FormatStr("npctalk.xml配置表里没这个id : %d",id));
	}
	return it->second;
}
//-----------------------------------------------------------------------
void NPCTableMgr::_loadNPCTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "npc.xml" ) );

	const size_t id			= x.getColumnIndex( "id" );
	const size_t name		= x.getColumnIndex( "name" );
	//const size_t skelton	= x.getColumnIndex( "skelton" );
	//const size_t skincolor	= x.getColumnIndex( "skincolor" );
	const size_t roleraceid	= x.getColumnIndex( "roleraceid" );
	//const size_t haircolor	= x.getColumnIndex( "haircolor" );
	//const size_t head		= x.getColumnIndex( "head" );
	//const size_t hair		= x.getColumnIndex( "hair" );
	//const size_t beard		= x.getColumnIndex( "beard" );
	const size_t hat		= x.getColumnIndex( "hat" );
	const size_t coat		= x.getColumnIndex( "coat" );
	const size_t bag		= x.getColumnIndex( "bag" );
	const size_t pants		= x.getColumnIndex( "pants" );
	const size_t glass		= x.getColumnIndex( "glass" );
	const size_t shoes		= x.getColumnIndex( "shoes" );
	const size_t hands		= x.getColumnIndex( "hands" );
	const size_t gender		= x.getColumnIndex( "gender" );
	const size_t height		= x.getColumnIndex( "height" );
	const size_t distance	= x.getColumnIndex( "talk_distance" );
	const size_t autoface	= x.getColumnIndex( "auto_face" );
	const size_t action		= x.getColumnIndex( "action" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		NPCRow r;
		r.id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, id ) );
		r.name		= x.cell( iRow, name, true );
//		r.skelton	= x.cell( iRow, skelton, true );

		r.roleraceid=Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, roleraceid ) );
		//r.skincolor= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, skincolor ) );
		//r.haircolor= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, haircolor ) );

	//	r.head		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, head ) );
	//	r.hair		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, hair ) );
	//	r.beard		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, beard ) );
		r.hat		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, hat ) );
		r.coat		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, coat ) );

		r.bag		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, bag ) );
		r.pants		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, pants ) );
		r.glass		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, glass ) );
		r.shoes		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, shoes ) );
		r.hands		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, hands ) );

		r.gender	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, gender ) );
		r.height	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, height ) );
		r.distance	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, distance ) );
		r.autoface	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, autoface ) ) != 0;
		r.action	= x.cell( iRow, action );

		assert( mNPCTable.find( r.id ) == mNPCTable.end() );
		mNPCTable.insert( std::make_pair( r.id, r) );
	}
}
//-----------------------------------------------------------------------
void NPCTableMgr::_loadNPCMsgTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "npctalk.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colContent		= x.getColumnIndex( "content" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		int id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		std::string content		= GB2312ToUTF8(x.cell( iRow, colContent, true).c_str());

		assert( mNPCMsgTable.find( id ) == mNPCMsgTable.end() );
		mNPCMsgTable.insert( std::make_pair( id, content) );
	}
}