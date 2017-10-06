/*******************************************************************************
* 作者: lf
* 时间: 2010-6-24
* 描述: 任务
*******************************************************************************/
#ifndef TASK_H
#define TASK_H

#include "EQMainPrerequisites.h"
#include "NetPacket.h"
#include "CEGUI.h"
#include "TaskTable.h"

class Task
{
public:
	Task();
	~Task();

	void requestTask(void);
	void recTask(std::vector<player_task>& tasklist);
	void recUpdateTaskProgress(const notify_update_task_progress & progress);
	void recGiveup(uint64 id);
	void recComplete(uint64 id);

	void reqGiveup();

	void handle_give_task(player_task& task);
	void reqCompleteTask(int task_id);
	bool existTask(int task_id);
	int  getTaskCount() { return mTaskList.size(); }
	int  getTaskIDByIndex(int i) { return mTaskList[i].task_id; }

	void setSelTask(uint id) { mSelTask = TaskTableManager::getSingleton().getTaskRow(id); }

	int  getNeedCount();
	int  getNeedItem(int i) { return mSelTask->item_need[i]; }
	int  getNeedAmount(int i) { return mSelTask->amount_need[i]; }

	int  getChoiceCount();
	int  getChoiceItem(int i) { return mSelTask->item_choice[i]; }
	int  getChoiceAmount(int i) { return mSelTask->amount_choice[i]; }

	int  getRewardCount();
	int  getRewardItem(int i) { return mSelTask->item_reward[i]; }
	int  getRewardAmount(int i) { return mSelTask->amount_reward[i]; }

	void clearTaskList(void) { mTaskList.clear(); }
	//
	void initCeguiTree( std::string win_name, std::string treeRoot_title,std::string rootTxt_font );
	CEGUI::TreeItem* createCeguiTreeItem(uint itmID,std::string parentItemtxt,std::string txt,std::string txt_font,std::string colrect_str,std::string imgset,std::string img);
	bool removeTaskGuideTitleItm(std::string task_title);
	bool handleEventSelectionChanged(const CEGUI::EventArgs& args);
	bool handleEventBranchOpened(const CEGUI::EventArgs& args);
	bool handleEventBranchClosed(const CEGUI::EventArgs& args);
	void findMatchItemInTaskUIList(std::string itm_txt);
	size_t getTaskGuideTreeItemCount();
	void caculateTreeSize( bool root_isOpened);

	// 随机产生新手教学任务
	void randomTeachTask();
	// 开始新手教学任务
	void startNewerTeachTask(int taskId);
	std::string getTaskActionDesc(int taskId) const;
private:
	int getFinishActionCount(int taskId, int actionId) const;
	void tryCompleteDailyTask(const player_task & task);
private:
	std::vector<player_task>	mTaskList;
	std::vector<uint>	mTraceTask;
	const TaskRow*		mSelTask;
	CEGUI::Tree*			mTaskGuideTreeRoot;
};

#endif