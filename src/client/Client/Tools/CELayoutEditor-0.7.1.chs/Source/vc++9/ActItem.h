/******************************************************************************
* ����: xj
* ʱ��: 2010-10-29
* ����: ����������ӿ�
******************************************************************************/

#pragma once

#ifndef __ActItem_H
#define __ActItem_H

//������������
enum ActType
{
	ACT_UNDEFINED,//δ�������ͣ���ʹ���ã�Ϊ��Ч�����
	ACT_MOVE,//�ƶ����嶯����
	ACT_RESIZE,//���������С������
	ACT_NEW,//�½����嶯����
	ACT_DEL//ɾ�����嶯����
};

class ActItem
{
protected:
	ActType m_actType;//��������

public:
	//��ȡ��������
	ActType GetType()
	{
		return m_actType;
	}

public:
	//���������ӿڣ��������ĸ��ֶ�����ʵ�֡�
	virtual ActType Undo() = 0;
	//���������ӿڣ��������ĸ��ֶ�����ʵ�֡�
	virtual ActType Redo() = 0;

public:
	ActItem(void) : m_actType(ACT_UNDEFINED) {};
	virtual ~ActItem(void) {};
};

#endif