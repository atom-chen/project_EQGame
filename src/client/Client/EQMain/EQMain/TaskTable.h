/******************************************************************************
* 作者: lf
* 时间: 2010-6-24
* 描述: 任务相关的表格
******************************************************************************/
#ifndef TaskTable_H
#define TaskTable_H

#include "EQMainPrerequisites.h"

/** 任务功能
*/
struct TaskRow
{
	TaskRow()
		: id( 0 )
		,sub_id(0)
	{
	}

	uint				id;
	std::string 		title;
	std::string			intention;
	std::vector<uint>	item_need;
	std::vector<uint>	amount_need;
	std::string			describe;
	std::vector<uint>	item_choice;
	std::vector<uint>	amount_choice;
	std::vector<uint>	item_reward;
	std::vector<uint>	amount_reward;
	uint				game_reward;
	uint				eq_reward;
	uint				is_show;
	uint				is_give_up;
	uint				sub_id;
	uint				kind;                  // 任务分类  (参考enum TASK_KIND)
	std::vector<uint>	require_actions;       // 需要任务动作(拜访好友，清理垃圾之类的)
	std::vector<uint>	require_actions_times; // 需要任务动作次数
};
typedef stdext::hash_map<uint, TaskRow>		TaskTable; 


/** 任务动作描述
*/
struct TaskActionRow
{
	TaskActionRow()
		:id(0)
		,desc()
	{
	}
	uint	id;
	std::string desc;
};
typedef stdext::hash_map<uint, TaskActionRow>	TaskActionTable; 


/**
*/
struct TaskGuideStepRow
{
	TaskGuideStepRow()
		:id(0)
		,stepNum(0)
		,content("")
	{
	}
	uint	id;
	uint	stepNum;
	std::string content;
};
typedef stdext::hash_map<uint, TaskGuideStepRow>	TaskGuideStepTable; 

class TaskTableManager : public Ogre::Singleton<TaskTableManager>
{
public:
	void load();

	const TaskActionRow* getTaskActionRow( uint id ) const; 
	const TaskRow*		getTaskRow( uint id ) const; 
	const TaskGuideStepRow* getTaskGuideStepRow(uint id) const;
	const TaskTable&	getTaskTable() const { return mTaskTable; }

private:	
	TaskTable	mTaskTable; 
	TaskActionTable	mTaskActionTable; 
	TaskGuideStepTable mTaskGuideStepTable;

private:
	void loadTaskTable();
	void loadTaskActionTable();
	void loadTaskGuideStepTable();
};



#endif
