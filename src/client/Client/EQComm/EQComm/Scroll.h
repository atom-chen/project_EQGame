/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-12-27
* ����: ʵ�ִ��ڵĹ�������Ϊ����ʹ��
******************************************************************************/
#ifndef Scroll_H
#define Scroll_H


#include "EQMainPrerequisites.h"
#include "GameEventMgr.h"


class Scroll
{
public:
	//	����ģʽ
	enum ScrollMode
	{
		SM_NONE,		//	������
		SM_UP_DOWN,		//	���ϵ��¹���
		SM_DOWN_UP,		//	���µ��Ϲ���
		SM_LEFT_RIGHT,	//	�����ҹ���
		SM_RIGHT_LEFT	//	���ҵ������
	};

	//	��Ϣ
	struct Msg
	{
		Msg()
			: id("")
			, content("")
			, playTimes(0)
		{
		}

		std::string id;	// ��Ϣid
		std::string content;//	��Ϣ����
		int playTimes;		//	���Ŵ���
	};

	typedef std::map<std::string, Msg> MsgMap;	//	��Ϣ��
	typedef std::vector<std::string> MsgIdVec;	//	��Ϣid��

public:
	/**
	*	���캯��
	*/
	Scroll();

	/**
	*	��������
	*/
	virtual ~Scroll();

	/**
	*	��	�ܣ�������Ϣ
	*	��	����id: ��Ϣid�� msg: ��Ϣ�� times: ���Ŵ���
	*	����ֵ��void
	*/
	void saveMsg(std::string id, std::string msg, int times);

	/**
	*	��	�ܣ���ȡ��Ϣ
	*	��	����id: ��Ϣid
	*	����ֵ��Msg*������const
	*/
	Msg* getMsg(std::string id);

	/**
	*	��	�ܣ���ȡ���ڲ��ŵ���Ϣ��id
	*	��	����void
	*	����ֵ��std::string
	*/
	std::string getPlayingId(void);

	/**
	*	��	�ܣ���ȡ��Ϣ����
	*	��	����void
	*	����ֵ��const Msg&
	*/
	const MsgMap& getMsgMap(void) const;

	/**
	*	��	�ܣ���ȡ��Ϣ����
	*	��	����void
	*	����ֵ��int
	*/
	int getMsgCount(void);

	/**
	*	��	�ܣ�ɾ����Ϣ
	*	��	����id: ��Ϣid
	*	����ֵ��void
	*/
	void deleteMsg(std::string id);

	/**
	*	��	�ܣ������Ϣ
	*	��	����void
	*	����ֵ��void
	*/
	void clearUp(void);

	/**
	*	��	�ܣ�������Ϣ
	*	��	����pareName: ��ֹ������������ childName: ����������
	*	����ֵ��0�������ݲ��ţ���1��û���ݲ��ţ�
	*/
	int play(std::string pareName, std::string childName);

	/**
	*	��	�ܣ�ֹͣ����
	*	��	����void
	*	����ֵ��void
	*/
	void stop(void);

	/**
	*	��	�ܣ�����
	*	��	����begin: ���ſ�ʼ��־�� end: ���Ž�����־
	*	����ֵ��0��1��û����Ϣ��������ϣ���2���ⲿֹͣ��������ϣ�
	*/
	int update(int& begin, int& end);

	/**
	*	��	�ܣ����ã����ſ�ʼ�����š����Ž���������ʱ�̵��õ�Lua������
	*	��	����beginName: ��ʼʱ�̺������� name: ����ʱ�̺������� endName: ����ʱ�̺�����
	*	����ֵ��void
	*/
	void setCalledFuncName(std::string beginName, std::string name, std::string endName);

	/**
	*	��	�ܣ����ò��ŵĲ���
	*	��	����pMode: ����ģʽ����������"NONE"�����ϵ��£�"UP_DOWN"�����µ��ϣ�"DOWN_UP"�������ң�"LEFT_RIGHT"�����ҵ���"RIGHT_LEFT"��
	*			dTime: �ӳ�ʱ�䣬 rTime: ���²��ŵ��ӳ�ʱ�䣬 sStep: ���벽���� tStep: ʱ�䲽��
	*	����ֵ��-1��pMode�������󣩣�-2��sStepС��0��tStepС�ڵ���0��
	*/
	int setPlayParams(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);

	/**
	*	��	�ܣ�������Ӧ��ʼ���źͽ������ŵĺ���ָ��
	*	��	����begin: ��ʼ������Ӧ������ end: ����������Ӧ����
	*	����ֵ��void
	*/
	void setFuncPointer(EVENT_HANDLE begin, EVENT_HANDLE end);

	/**
	*	��	�ܣ������ǵ�һ�β�����Ϣ������ĵ�һ����Ϣ
	*	��	����first: ��
	*	����ֵ��void
	*/
	void setPlayFirst(bool first=true);

private:
	/**
	*	��	�ܣ���ȡ��Ϣid�������Ϣid
	*	��	����id: ��Ϣid
	*	����ֵ��std::string
	*/
	std::string _getMsgId(size_t id);

	/**
	*	��	�ܣ����ŵ�����Ϣ
	*	��	����winname: ��������
	*			pareW: �����ڿ�ȣ� pareH: �����ڸ߶ȣ� childW: �Ӵ��ڿ�ȣ� childH: �Ӵ��ڸ߶ȣ� scrollM: ����ģʽ
	*	����ֵ��void
	*/
	void _playMsg(std::string winname, float pareW, float pareH, float childW, float childH, ScrollMode scrollM);

	/**
	*	��	�ܣ����Ź���Ч��
	*	��	����winname: �������� dT: �ӳ�ʱ�䣬 pT: ����ʱ�䣬
	*			xS: x��ʼ���꣬ yS: y��ʼ���꣬ xE: x�������꣬ yE: y��������
	*	����ֵ��void
	*/
	void _playScroll(std::string winname, int dT, int pT, int xS, int yS, int xE, int yE);

private:
	MsgMap m_msgMap;		//	��Ϣ
	MsgIdVec m_msgIdVec;	//	��Ϣid����
	int m_playingIndex;		//	��ǰ���ŵ���Ϣ�ڼ����е�����
	Msg* m_playingMsg;		//	��ǰ���ŵ���Ϣ
	std::string m_playingId;//	��ǰ���ŵ���Ϣ��id

	std::string m_pareWinname;		//	��ֹ��������
	std::string m_childWinname;		//	��������

	std::string m_playBeginFuncName;//	���ſ�ʼʱ�̵��õ�Lua������
	std::string m_playFuncName;		//	����ʱ�̵��õ�Lua������
	std::string m_playEndFuncName;	//	���Ž���ʱ�̵��õ�Lua������

	EVENT_HANDLE m_beginFunc;	//	��ʼ������Ӧ����
	EVENT_HANDLE m_endFunc;		//	����������Ӧ����

	float m_spaceStep;	//	�������벽��
	int m_timeStep;		//	����ʱ�䲽��

	ScrollMode m_scrollMode;		//	����ģʽ
	int m_delayTime;	//	�ӳ�ʱ��
	int m_replayTime;	//	���²���ʱ����
	bool m_replayFlag;	//	���²��ű�ʶ
	bool m_playStop;	//	��ǰ������Ϣ��ֹͣ��ʶ
	bool m_playFirst;	//	��һ�鲥����Ϣ������ĵ�һ����Ϣ

};	//	end of class Scroll


#endif	//	end of guard Scroll_H
