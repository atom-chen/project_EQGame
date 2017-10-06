#include "EQCommPCH.h"
#include "Task.h"
#include "GameEventMgr.h"

// #include "CEGUI.h"
#include "TaskTable.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "CommFunc.h"
#include "ItemTable.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "EnumDef.h"
#include "Variable.h"

//------------------------------------------------------------------------------
Task::Task():mTaskGuideTreeRoot(NULL)
{

}
//------------------------------------------------------------------------------
Task::~Task()
{
	if (mTaskGuideTreeRoot)
	{
		mTaskGuideTreeRoot->resetList();
		mTaskGuideTreeRoot = NULL;
	}
}
//------------------------------------------------------------------------------
void Task::requestTask(void)
{
	req_task_list req;
	sGameMgr.getEQMsgManager()->Send(req);
}
//------------------------------------------------------------------------------
void Task::recTask(std::vector<player_task>& tasklist)
{
	mTaskList.clear();

	for(int i=0; i<(int)tasklist.size(); i++)
		mTaskList.push_back(tasklist[i]);

	GameEventMgr::getSingleton().fireEvent("UPDATE_TASK_UI");
}
//------------------------------------------------------------------------------
void Task::recGiveup(uint64 id)
{
	int task_id = 0;
	std::vector<player_task>::iterator it;
	for(it=mTaskList.begin(); it!=mTaskList.end(); it++)
	{
		if((*it).id == id)
		{
			task_id = it->task_id;
			mTaskList.erase(it);
			GameEventMgr::getSingleton().fireEvent("UPDATE_TASK_UI");
			break;
		}
	}
	luabind::call_function<void>(sLuaMgr.getL(), "Task_giveupTask", task_id);
}
//------------------------------------------------------------------------------
static int getNeedActionAmount(const TaskRow * row, int actionType)
{
	if (!row)
		return -1;

	if (row->require_actions.size() != row->require_actions_times.size())
	{
		return -1;
	}

	for (uint i = 0; i < row->require_actions.size(); ++i)
	{
		if (actionType == row->require_actions[i])
			return row->require_actions_times[i];
	}
	return -1;
}
//------------------------------------------------------------------------------
void Task::tryCompleteDailyTask(const player_task & task)
{
	const TaskRow * row = TaskTableManager::getSingleton().getTaskRow(task.task_id);
	if (!row)
		return;

	if (tk_daily != row->kind)
		return;

	bool isTaskCompleted = true;
	// 判断所有任务动作数量是否都完成
	std::vector<player_task_action>::const_iterator itAction = task.action_list.begin();
	for (; itAction != task.action_list.end(); ++itAction)
	{
		int needAmount = getNeedActionAmount(row, itAction->type);
		if (itAction->finish_amount < needAmount)
			isTaskCompleted = false;
	}	
	
	if (isTaskCompleted)
	{
		reqCompleteTask(task.task_id);
	}
}
//------------------------------------------------------------------------------
void Task::recUpdateTaskProgress(const notify_update_task_progress & progress)
{
	std::vector<player_task>::iterator it = mTaskList.begin();
	for(; it != mTaskList.end(); it++)
	{
		if((*it).id == progress.id)
		{
			// 更新任务动作数量
			std::vector<player_task_action>::iterator itAction = it->action_list.begin();
			for (; itAction != it->action_list.end(); ++itAction)
			{
				if (itAction->type == progress.action_type)
				{
					itAction->finish_amount = progress.action_amount;
					GameEventMgr::getSingleton().fireEvent("UPDATE_TASK_UI");
					break;
				}
			}

			// 如果是每日任务，且已完成，则自动请求完成
			tryCompleteDailyTask(*it);
			break;
		}
	}	
}
//------------------------------------------------------------------------------
void Task::recComplete(uint64 id)
{
	int task_id = 0;
	std::vector<player_task>::iterator it;
	for(it=mTaskList.begin(); it!=mTaskList.end(); it++)
	{
		if((*it).id == id)
		{
			task_id = (*it).task_id;
			mTaskList.erase(it);
			GameEventMgr::getSingleton().fireEvent("UPDATE_TASK_UI");
			break;
		}
	}
	luabind::call_function<void>(sLuaMgr.getL(), "Task_completeTask", task_id);
}
//------------------------------------------------------------------------------
void Task::reqGiveup()
{
	if(!mSelTask)
		return;

	for(size_t i=0; i<mTaskList.size(); i++)
	{
		if(mTaskList[i].task_id == mSelTask->id)
		{
			req_give_up_task msg;
			msg.id = mTaskList[i].id;
			sGameMgr.getEQMsgManager()->Send(msg);
			break;
		}
	}
}
//------------------------------------------------------------------------------
void Task::handle_give_task(player_task& task)
{
	mTaskList.push_back(task);
	luabind::call_function<void>(sLuaMgr.getL(), "PlayerGuide_do_ReceveTaskScript",task.task_id);
	GameEventMgr::getSingleton().fireEvent("UPDATE_TASK_UI");
}
//------------------------------------------------------------------------------
void Task::reqCompleteTask(int task_id)
{
	req_complete_task msg;
	msg.task_id = task_id;
	sGameMgr.getEQMsgManager()->Send(msg);
}
//------------------------------------------------------------------------------
void Task::randomTeachTask()
{
	// 取出所有随机一次性任务
	std::vector<int> taskArr;
	taskArr.clear();
	for(std::vector<player_task>::iterator it = mTaskList.begin();
		mTaskList.end() != it; ++it)
	{
		int taskId = it->task_id;
		const TaskRow * row = TaskTableManager::getSingleton().getTaskRow(taskId);
		if (row)
		{
			//tk_rate_once  % 带机率的一次性任务
			if (tk_rate_once == row->kind)
				taskArr.push_back(taskId);
		}
	}

	int randId = 0;
	// 根据机率取一个
	if (taskArr.size() > 0)
	{
		int idx = rand() % static_cast<int>(taskArr.size());
		
		int rate = sVariableSystem.GetAs_Int("TeachTaskRate");
		if (rand() % 100 < rate)
		{
			randId = taskArr.at(idx);
		}
	}

	// 
	if (randId > 0)
	{
		startNewerTeachTask(randId);
	}
}
//------------------------------------------------------------------------------
void Task::startNewerTeachTask(int taskId)
{
	luabind::call_function<void>(sLuaMgr.getL(), "PlayerGuide_do_NewerTeachTaskScript",taskId);
}
//------------------------------------------------------------------------------
bool Task::existTask(int task_id)
{
	std::vector<player_task>::iterator it = mTaskList.begin();
	for(it; mTaskList.end()!=it; ++it)
	{
		if((*it).task_id == task_id)
			return true;
	}
	return false;
}
//------------------------------------------------------------------------------
int Task::getFinishActionCount(int taskId, int actionId) const
{
	for (uint i = 0; i < mTaskList.size(); ++i)
	{
		if (mTaskList[i].task_id == taskId)
		{
			for (uint x = 0; x < mTaskList[i].action_list.size(); ++x)
			{
				if (mTaskList[i].action_list[x].type == actionId)
					return mTaskList[i].action_list[x].finish_amount;
			}
		}
	}
	return 0;
}
//------------------------------------------------------------------------------
std::string Task::getTaskActionDesc(int taskId) const
{
	std::string result;
	const TaskRow * row = TaskTableManager::getSingleton().getTaskRow(taskId);
	if (!row)
		return result;
	
	for (uint i = 0; i < row->require_actions.size(); ++i)
	{
		int actionId = row->require_actions[i];
		int actionCount = row->require_actions_times[i];
		
		const TaskActionRow * actionRow = TaskTableManager::getSingleton().getTaskActionRow(actionId);
		if (actionRow)
		{
			int iFinish = getFinishActionCount(taskId, actionId);
			std::string sPercent = FormatStr("%d/%d", iFinish, actionCount);
			if (actionRow->desc.find("%s") == actionRow->desc.find_last_of('%'))
			{
				result += FormatStr(actionRow->desc.c_str(), sPercent.c_str());
				result += "\n";
			}
		}
		else
		{
			LOGERR(FormatStr("No find TaskActionRow:%d", actionId));
		}
	}
	return result;
}
//------------------------------------------------------------------------------
int Task::getNeedCount(void)
{
	if (mSelTask->item_need.empty())
	{
		return 0;
	}
	if (0 == mSelTask->item_need[0])
	{
		return 0;
	}
	return mSelTask->item_need.size();
}
//------------------------------------------------------------------------------
int Task::getChoiceCount(void)
{
	if (mSelTask->item_choice.empty())
	{
		return 0;
	}
	if (0 == mSelTask->item_choice[0])
	{
		return 0;
	}
	return mSelTask->item_choice.size();
}
//------------------------------------------------------------------------------
int Task::getRewardCount(void)
{
	if (mSelTask->item_reward.empty())
	{
		return 0;
	}
	if (0 == mSelTask->item_reward[0])
	{
		return 0;
	}
	return mSelTask->item_reward.size();
}
//------------------------------------------------------------------------------
void Task::initCeguiTree( std::string win_name, std::string treeRoot_title,std::string rootTxt_font )
{
	CEGUI::Tree* _tree = (CEGUI::Tree*)(CEGUI::WindowManager::getSingleton().getWindow(win_name));
	_tree->initialise();
	_tree->subscribeEvent(CEGUI::Tree::EventSelectionChanged, CEGUI::Event::Subscriber(&Task::handleEventSelectionChanged, this));
	_tree->subscribeEvent(CEGUI::Tree::EventBranchOpened, CEGUI::Event::Subscriber(&Task::handleEventBranchOpened, this));
	_tree->subscribeEvent(CEGUI::Tree::EventBranchClosed, CEGUI::Event::Subscriber(&Task::handleEventBranchClosed, this));
// 	CEGUI::TreeItem* newTreeCtrlEntryLvl1 = new CEGUI::TreeItem((CEGUI::utf8*)treeRoot_title.c_str());
// 	CEGUI::Imageset *drives = CEGUI::ImagesetManager::getSingleton().createImageset("DriveIcons.imageset");
// 	//newTreeCtrlEntryLvl1->setIcon(drives->getImage("Black"));
// 	newTreeCtrlEntryLvl1->setFont(rootTxt_font);
// 	newTreeCtrlEntryLvl1->setSelectionBrushImage("TaharezLook", "TextSelectionBrush");
// 	_tree->addItem(newTreeCtrlEntryLvl1);
	mTaskGuideTreeRoot = _tree;
}
//------------------------------------------------------------------------------
CEGUI::TreeItem* Task::createCeguiTreeItem( uint itmID,std::string parentItemtxt,std::string txt,std::string txt_font,std::string colrect_str,std::string imgset,std::string img )
{
	CEGUI::TreeItem* taskTitle = new CEGUI::TreeItem((CEGUI::utf8*)txt.c_str());
	//CEGUI::Imageset *img_set = CEGUI::ImagesetManager::getSingleton().getImageset(imgset);
	//taskTitle->setIcon(img_set->getImage(img));
	taskTitle->setFont(txt_font);
	CEGUI::ColourRect crect = CEGUI::PropertyHelper::stringToColourRect(colrect_str);
	taskTitle->setTextColours(crect);
	taskTitle->setID(itmID);
	//taskTitle->setSelectionBrushImage("TaharezLook", "TextSelectionBrush");
	if(mTaskGuideTreeRoot)
	{
		if (parentItemtxt == "")
		{
			if(!(mTaskGuideTreeRoot->findFirstItemWithText((CEGUI::utf8*)txt.c_str())))
				mTaskGuideTreeRoot->addItem(taskTitle);
		}
		else
		{
			CEGUI::TreeItem* itm = mTaskGuideTreeRoot->findFirstItemWithText((CEGUI::utf8*)parentItemtxt.c_str());
			if (itm)
			{	
				CEGUI::TreeItem::LBItemList itmList = itm->getItemList();
				size_t itmCount = itmList.size();
				for (size_t i = 0; i< itmCount; ++i)
				{
					if(itmList[i]->getText() == taskTitle->getText())
					{
						delete taskTitle;
						taskTitle = NULL;
						break;
					}
				}
				if(taskTitle)
					if (!itm->getIsOpen())
						itm->toggleIsOpen();
					itm->addItem(taskTitle);
			}
		}
	}
	return NULL;
}
//-------------------------------------------------------------------------------
bool Task::removeTaskGuideTitleItm( std::string task_title )
{
	if(mTaskGuideTreeRoot)
	{
		CEGUI::TreeItem* itm = mTaskGuideTreeRoot->findFirstItemWithText((CEGUI::utf8*)task_title.c_str());
		if(itm)
		{
			mTaskGuideTreeRoot->removeItem(itm);
			return true;
		}
		else
			return false;
	}
	else
		return false;

}
//-------------------------------------------------------------------------------
bool Task::handleEventSelectionChanged(const CEGUI::EventArgs& args)
{
	const CEGUI::TreeEventArgs& treeArgs = static_cast<const CEGUI::TreeEventArgs&>(args);
	CEGUI::TreeItem *selectedItem = treeArgs.treeItem;
	if (selectedItem)
	{
// 		if(selectedItem->getOwnerWindow())
		findMatchItemInTaskUIList(selectedItem->getText().c_str());
		luabind::call_function<void>(sLuaMgr.getL(), "Task_handleEventSelectionChanged", selectedItem->getID(),selectedItem->getText().c_str());
	}
	else
		LOGMSG("No Selected~");
	return true;
}
//-------------------------------------------------------------------------------
bool Task::handleEventBranchOpened(const CEGUI::EventArgs& args)
{
	luabind::call_function<void>(sLuaMgr.getL(), "Task_handleEventBranchOpened");
	return true;
}
//-------------------------------------------------------------------------------
bool Task::handleEventBranchClosed(const CEGUI::EventArgs& args)
{
	luabind::call_function<void>(sLuaMgr.getL(), "Task_handleEventBranchClosed");
	return true;
}
//-------------------------------------------------------------------------------
void Task::findMatchItemInTaskUIList( std::string itm_txt )
{
	CEGUI::Listbox * taskListbox = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().getWindow("Taskboardroot/Taskboard/list"));
	CEGUI::ListboxItem* task_itm = taskListbox->findItemWithText((CEGUI::utf8*)itm_txt.c_str(),NULL);
	if (task_itm)
	{
		taskListbox->clearAllSelections();
		std::string tmp_txt = task_itm->getText().c_str();
		if(!task_itm->isSelected())
			task_itm->setSelected(true);
		luabind::call_function<void>(sLuaMgr.getL(), "Task_openMatchTaskContent", task_itm->getID());
	}
}
//---------------------------------------------------------------------------------
size_t Task::getTaskGuideTreeItemCount()
{
	if(mTaskGuideTreeRoot)
		return mTaskGuideTreeRoot->getItemCount();
	else
		return 0;
}
//---------------------------------------------------------------------------------
void Task::caculateTreeSize( bool root_isOpened)
{
	float totalchilditm_width = 0,totalchilditm_height = 0;
	float totalparentitm_width = 0,totalparentitm_height = 0;
	float tree_width = 0, tree_height = 0;
	if (mTaskGuideTreeRoot)
	{
		size_t rootItmcount = 0;//mTaskGuideTreeRoot->getItemCount();//findFirstItemWithText((CEGUI::utf8*)parent_txt.c_str());
		CEGUI::TreeItem::LBItemList rootItm_list = mTaskGuideTreeRoot->getItemList();
		rootItmcount = rootItm_list.size();
		for (size_t i = 0; i < rootItmcount; ++i)
		{
			CEGUI::TreeItem* root_itm = rootItm_list[i];//mTaskGuideTreeRoot->findFirstItemWithID(i);
			if (root_itm)
			{
				totalparentitm_width += root_itm->getPixelSize().d_width;
				totalparentitm_height += root_itm->getPixelSize().d_height;
				if(root_isOpened)
				{
					CEGUI::TreeItem::LBItemList itm_list = root_itm->getItemList();
					size_t list_size = itm_list.size();
					for (size_t i = 0; i < list_size; ++i)
					{
						totalchilditm_width += itm_list[i]->getPixelSize().d_width;
						totalchilditm_height += itm_list[i]->getPixelSize().d_height /*+ 15.0*/;
					}
				}
				
			}
		}
		tree_width = totalparentitm_width + totalchilditm_width;
		tree_height = totalparentitm_height + totalchilditm_height + 15.0f;
		mTaskGuideTreeRoot->setSize(CEGUI::UVector2(mTaskGuideTreeRoot->getSize().d_x,CEGUI::UDim(0,tree_height)));
		mTaskGuideTreeRoot->getParent()->setSize(CEGUI::UVector2(mTaskGuideTreeRoot->getSize().d_x,CEGUI::UDim(0,tree_height)));
	}
}