/******************************************************************************
* 作者: Y3
* 时间: 2005-8-19
* 描述: 来自TD
******************************************************************************/
#ifndef __FAIRYOPERATORMANAGER_H__
#define __FAIRYOPERATORMANAGER_H__



class Operator;

class OperatorManager
{
public:
	typedef size_t Timestamp;
	typedef std::map<std::string, Timestamp> Snapshot;
	typedef std::list<std::string> GroupNameList;
	typedef std::list<Operator*> OperatorList;

public:
	OperatorManager(void);
	~OperatorManager();

	void reset(void);

	void addOperator(Operator* op);
	void applyOperator(Operator* op);

	size_t undo(size_t count = 1);
	size_t redo(size_t count = 1);
	size_t undo(const std::string& groupName, size_t count = 1);
	size_t redo(const std::string& groupName, size_t count = 1);

	GroupNameList getGroupNameList(void) const;

	Snapshot makeSnapshot(void) const;
	bool checkSnapshot(const Snapshot& snapshot) const;

	const OperatorList& getUndoOperators(void) const;
	const OperatorList& getRedoOperators(void) const;
	const OperatorList& getUndoOperators(const std::string& groupName) const;
	const OperatorList& getRedoOperators(const std::string& groupName) const;

protected:
	class Group;
	typedef std::map<std::string, Group*> GroupMap;

	Group* findGroup(const std::string& groupName);
	const Group* findGroup(const std::string& groupName) const;

	GroupMap mGroups;
	OperatorList mUndoList;
	OperatorList mRedoList;
	Timestamp mTimestamp;
};


#endif // 
