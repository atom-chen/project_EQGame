/******************************************************************************
* 作者: 何展然
* 时间: 2010-10-29
* 描述: 聊天表情表管理器
******************************************************************************/
#ifndef ChatExpressionTable_H
#define ChatExpressionTable_H

#include "EQMainPrerequisites.h"
#include "CEGUIEx/CEGUIEx.h"


/*************************************************************************
*	聊天表情表管理类
*************************************************************************/
class ChatExpressionTableManager : public Ogre::Singleton<ChatExpressionTableManager>
{
public:
	//	对应聊天表情xml表中的每行
	struct ChatExpressionRow
	{
		uint id;					//	id号
		std::string transferred;	//	转义符号
		uint playtime;				//	时间间隔
		std::string firstf;			//	第一帧
		std::string secondf;		//	第二帧
		std::string thirdf;			//	第三帧
		std::string forthf;			//	第四帧
		std::string fifthf;			//	第五帧
		std::string sixthf;			//	第六帧
		std::string seventhf;		//	第七帧
		std::string eighthf;		//	第八帧
	};

public:
	/**
	*	构造函数
	*/
	ChatExpressionTableManager();

	/**
	*	析构函数
	*/
	~ChatExpressionTableManager();

	/**
	*	功	能：加载聊天表情表
	*	参	数：void
	*	返回值：void
	*/
	void load(void);

	/**
	*	功	能：从映射表获取表情
	*	参	数：animate:	动画对象
	*			index:	索引号
	*	返回值：0（正确），-1（错误，没找到）
	*/
	int getChatExpression(CEGUI::Animate& animate, int index);

	/**
	*	功	能：从映射表获取表情转义符
	*	参	数：index:	索引号
	*	返回值：0（错误，没找到），其他（正确）
	*/
	std::string getTransfer(int index);

	/**
	*	功	能：获取表情的数量
	*	参	数：void
	*	返回值：int
	*/
	int getExpressionCount(void);

protected:
	/**
	*	截取描述帧的字符串
	*/
	void _addFrameFromStr(CEGUI::Animate& animate, std::string frame);

protected:
	stdext::hash_map<int, ChatExpressionRow> mDataMap;	//	数据映射表

	int mExpressionCount;	//	表情数量

	bool mLoadFlag;			//	加载标志，确保加载一次

};	//	end of class ChatExpressionTableManager


#endif	//	end of guard ChatExpressionTable_H
