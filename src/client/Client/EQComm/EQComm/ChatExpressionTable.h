/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-10-29
* ����: �������������
******************************************************************************/
#ifndef ChatExpressionTable_H
#define ChatExpressionTable_H

#include "EQMainPrerequisites.h"
#include "CEGUIEx/CEGUIEx.h"


/*************************************************************************
*	�������������
*************************************************************************/
class ChatExpressionTableManager : public Ogre::Singleton<ChatExpressionTableManager>
{
public:
	//	��Ӧ�������xml���е�ÿ��
	struct ChatExpressionRow
	{
		uint id;					//	id��
		std::string transferred;	//	ת�����
		uint playtime;				//	ʱ����
		std::string firstf;			//	��һ֡
		std::string secondf;		//	�ڶ�֡
		std::string thirdf;			//	����֡
		std::string forthf;			//	����֡
		std::string fifthf;			//	����֡
		std::string sixthf;			//	����֡
		std::string seventhf;		//	����֡
		std::string eighthf;		//	�ڰ�֡
	};

public:
	/**
	*	���캯��
	*/
	ChatExpressionTableManager();

	/**
	*	��������
	*/
	~ChatExpressionTableManager();

	/**
	*	��	�ܣ�������������
	*	��	����void
	*	����ֵ��void
	*/
	void load(void);

	/**
	*	��	�ܣ���ӳ����ȡ����
	*	��	����animate:	��������
	*			index:	������
	*	����ֵ��0����ȷ����-1������û�ҵ���
	*/
	int getChatExpression(CEGUI::Animate& animate, int index);

	/**
	*	��	�ܣ���ӳ����ȡ����ת���
	*	��	����index:	������
	*	����ֵ��0������û�ҵ�������������ȷ��
	*/
	std::string getTransfer(int index);

	/**
	*	��	�ܣ���ȡ���������
	*	��	����void
	*	����ֵ��int
	*/
	int getExpressionCount(void);

protected:
	/**
	*	��ȡ����֡���ַ���
	*/
	void _addFrameFromStr(CEGUI::Animate& animate, std::string frame);

protected:
	stdext::hash_map<int, ChatExpressionRow> mDataMap;	//	����ӳ���

	int mExpressionCount;	//	��������

	bool mLoadFlag;			//	���ر�־��ȷ������һ��

};	//	end of class ChatExpressionTableManager


#endif	//	end of guard ChatExpressionTable_H
