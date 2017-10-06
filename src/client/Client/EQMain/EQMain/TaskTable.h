/******************************************************************************
* ����: lf
* ʱ��: 2010-6-24
* ����: ������صı��
******************************************************************************/
#ifndef TaskTable_H
#define TaskTable_H

#include "EQMainPrerequisites.h"

/** ������
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
	uint				kind;                  // �������  (�ο�enum TASK_KIND)
	std::vector<uint>	require_actions;       // ��Ҫ������(�ݷú��ѣ���������֮���)
	std::vector<uint>	require_actions_times; // ��Ҫ����������
};
typedef stdext::hash_map<uint, TaskRow>		TaskTable; 


/** ����������
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
