/******************************************************************
* 作者: 麦_Mike
* 时间: 20010-5-23
* 描述: NPCTable管理
*******************************************************************/
#ifndef NPCTable_H
#define NPCTable_H
#include "EQMainPrerequisites.h"

#include <string>
/*
NPC Row-------------------------------------------------------------------
*/
//struct  NPCMsgRow
//{
//	NPCMsgRow()
//		: 
//		,content("")
//	{
//	}
//	std::string content;
//};

struct NPCRow
{
	NPCRow()
	{
	}

	uint			id;
	std::string 	name
					//,skelton
					;	
	uint			roleraceid
					//,skincolor	
					//,haircolor	
					//,head		
					//,hair		
					//,beard		
					,hat		
					,coat		
					,bag		
					,pants		
					,glass		
					,shoes		
					,hands		
					,gender		
					;
	uint			height;
	uint			distance;
	bool			autoface;
	std::string		action;
};
typedef std::map<uint, std::string>NPCMsgTable;
typedef std::map<uint, NPCRow> NPCTable; 
/*
NPC Table-----------------------------------------------------------------
*/
class NPCTableMgr : public Ogre::Singleton<NPCTableMgr>
{
public:
	void load();

	const std::string&	getNPCMsgRow( uint id ) const; 
	const NPCRow*	getNPCRow( uint id ) const; 
	const NPCTable&	getNPCTable() const { return mNPCTable; }

private:	
	NPCTable			mNPCTable; 
	NPCMsgTable	mNPCMsgTable;

private:
	void			_loadNPCTable();
	void			_loadNPCMsgTable();

};

#endif
