#include "EQMainPCH.h"

#include "TaskTable.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "XmlDataCenter.h"
#include <string>


//-----------------------------------------------------------------------
template<> TaskTableManager* Ogre::Singleton<TaskTableManager>::ms_Singleton = 0;

//-----------------------------------------------------------------------
void TaskTableManager::load()
{
	loadTaskTable();
	loadTaskActionTable();
	loadTaskGuideStepTable();
}
//-----------------------------------------------------------------------
const TaskActionRow* TaskTableManager::getTaskActionRow( uint id ) const
{
	TaskActionTable::const_iterator it = mTaskActionTable.find( id );
	if ( it == mTaskActionTable.end() )
		return NULL;

	return &it->second;
}
//-----------------------------------------------------------------------
const TaskRow* TaskTableManager::getTaskRow( uint id ) const
{
	TaskTable::const_iterator it = mTaskTable.find( id );
	if ( it == mTaskTable.end() )
		return NULL;

	return &it->second;
}
//-----------------------------------------------------------------------
void TaskTableManager::loadTaskTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "player_task_tplt.xml" ) );

	const size_t colID				= x.getColumnIndex( "task_id" );
	const size_t colTitle			= x.getColumnIndex( "title" );
	const size_t colIntention		= x.getColumnIndex( "intention" );
	const size_t colItemNeed		= x.getColumnIndex( "require_items" );
	const size_t colAmountNeed		= x.getColumnIndex( "require_items_count" );
	const size_t colDescribe		= x.getColumnIndex( "describe" );
	const size_t colItemChoice		= x.getColumnIndex( "select_reward_items" );
	const size_t colAmountChoice	= x.getColumnIndex( "select_reward_items_count" );
	const size_t colItemReward		= x.getColumnIndex( "fixed_reward_items" );
	const size_t colAmountReward	= x.getColumnIndex( "fixed_reward_items_count" );
	const size_t colGameReward		= x.getColumnIndex( "reward_game_coin" );
	const size_t colEqReward		= x.getColumnIndex( "reward_eq_coin" );
	const size_t colIsShow			= x.getColumnIndex( "is_show" );
	const size_t colIsGiveUp		= x.getColumnIndex( "is_give_up" );
	const size_t colSubID		= x.getColumnIndex( "sub_id" );
	const size_t colKind		= x.getColumnIndex( "kind" );
	const size_t colRequireActions = x.getColumnIndex( "require_actions" );
	const size_t colRequireActionTimes = x.getColumnIndex( "require_actions_times" );

	std::string str;
	std::vector<std::string> strArr; 
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		TaskRow r;
		r.id			= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colID ) );
		r.title			= GB2312ToUTF8(x.cell( iRow, colTitle, true ).c_str());
		r.intention		= GB2312ToUTF8(x.cell( iRow, colIntention, true ).c_str());
		r.describe		= x.cell( iRow, colDescribe ).c_str();
		r.game_reward	= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colGameReward ) );
		r.eq_reward		= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colEqReward ) );
		r.is_show		= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colIsShow ) );
		r.is_give_up	= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colIsGiveUp ) );
		r.sub_id		= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colSubID ) );
		r.kind			= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colKind ) );


		//need
		str = x.cell( iRow, colItemNeed, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.item_need.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		str = x.cell( iRow, colAmountNeed, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.amount_need.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		if(r.item_need.size() != r.amount_need.size())
			EQ_EXCEPT( "task.xml need item count invalid", "loadTaskTable" );

		//need actions
		str = x.cell( iRow, colRequireActions, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.require_actions.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		str = x.cell( iRow, colRequireActionTimes, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.require_actions_times.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		if(r.require_actions.size() != r.require_actions_times.size())
			EQ_EXCEPT( "task.xml require_actions count invalid", "loadTaskTable" );

		//choice
		str = x.cell( iRow, colItemChoice, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.item_choice.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		str = x.cell( iRow, colAmountChoice, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.amount_choice.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		if(r.item_choice.size() != r.amount_choice.size())
			EQ_EXCEPT( "task.xml choice item count invalid", "loadTaskTable" );

		//reward
		str = x.cell( iRow, colItemReward, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.item_reward.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		str = x.cell( iRow, colAmountReward, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, "," );
			for ( std::vector<std::string>::iterator it = strArr.begin(); it != strArr.end(); ++it )
			{
				r.amount_reward.push_back(Ogre::StringConverter::parseInt(*it));
			}
		}

		if(r.item_reward.size() != r.amount_reward.size())
			EQ_EXCEPT( "task.xml reward item count invalid", "loadTaskTable" );

		if ( mTaskTable.find( r.id ) != mTaskTable.end() )
			EQ_EXCEPT( "task.xml ID Repeat", "loadTaskTable" );

		if ( 0 == r.id )
			EQ_EXCEPT( "task.xml ID null", "loadTaskTable" );		

		mTaskTable.insert( std::make_pair( r.id, r ) );
	}	
}
//-----------------------------------------------------------------------
void TaskTableManager::loadTaskGuideStepTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "task_step_tplt.xml" ) );

	const size_t colID				= x.getColumnIndex( "id" );
	const size_t colStepNum			= x.getColumnIndex( "stepNum" );
	const size_t colStepDetail	= x.getColumnIndex( "stepDetail" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		TaskGuideStepRow r;
		r.id			= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colID ) );
		r.stepNum			= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colStepNum ) );
		r.content		= GB2312ToUTF8(x.cell( iRow, colStepDetail, true ).c_str());
		if ( mTaskGuideStepTable.find( r.id ) != mTaskGuideStepTable.end() )
			EQ_EXCEPT( "task_step_tplt.xml ID Repeat", "loadTaskGuideStepTable" );

		if ( 0 == r.id )
			EQ_EXCEPT( "task_step_tplt.xml ID null", "loadTaskGuideStepTable" );		

		mTaskGuideStepTable.insert( std::make_pair( r.id, r ) );
	}
}
//-----------------------------------------------------------------------
void TaskTableManager::loadTaskActionTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "player_task_action_tplt.xml" ) );

	const size_t colID				= x.getColumnIndex( "id" );
	const size_t colDesc			= x.getColumnIndex( "desc" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		TaskActionRow r;
		r.id		= Ogre::StringConverter::parseUnsignedInt( x.cell( iRow, colID ) );
		r.desc		= GB2312ToUTF8(x.cell( iRow, colDesc, true ).c_str());
		if ( mTaskActionTable.find( r.id ) != mTaskActionTable.end() )
			EQ_EXCEPT( "player_task_action_tplt.xml ID Repeat", "loadTaskActionTable" );

		if ( 0 == r.id )
			EQ_EXCEPT( "player_task_action_tplt.xml ID null", "loadTaskActionTable" );		

		mTaskActionTable.insert( std::make_pair( r.id, r ) );
	}
}
//------------------------------------------------------------------------
const TaskGuideStepRow* TaskTableManager::getTaskGuideStepRow(uint id) const
{
	TaskGuideStepTable::const_iterator it = mTaskGuideStepTable.find( id );
	if ( it == mTaskGuideStepTable.end() )
		return NULL;

	return &it->second;
}