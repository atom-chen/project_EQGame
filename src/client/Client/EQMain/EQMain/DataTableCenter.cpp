#include "EQMainPCH.h"

#include "DataTableCenter.h"
#include "AvatarTable.h"
#include "ItemTable.h"
#include "GraphicsTable.h"
#include "HouseTable.h"
#include "LogicTable.h"
#include "NPCTable.h"
#include "FarmTable.h"
#include "TaskTable.h"

//-----------------------------------------------------------------------
void DataTableCenter::setup()
{
	new GraphicsTableManager();
	new AvatarTableManager();
	new HouseTableManager();
	new ItemTableManager();
	new LogicTableManager();
	new NPCTableMgr();
	new FarmTableMgr;
	new TaskTableManager();
}
//-----------------------------------------------------------------------
void DataTableCenter::shutdown()
{
	delete AvatarTableManager::getSingletonPtr();
	delete HouseTableManager::getSingletonPtr();
	delete ItemTableManager::getSingletonPtr();
	delete GraphicsTableManager::getSingletonPtr();
	delete LogicTableManager::getSingletonPtr();
	delete NPCTableMgr::getSingletonPtr();
	delete FarmTableMgr::getSingletonPtr();
	delete TaskTableManager::getSingletonPtr();
}
