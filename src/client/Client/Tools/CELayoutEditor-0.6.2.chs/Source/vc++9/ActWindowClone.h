/******************************************************************************
* ����: xj
* ʱ��: 2010-11-2
* ����: ѡ�񼯴��帱��
******************************************************************************/

#pragma once

#ifndef __ActWindowClone_H
#define __ActWindowClone_H

#include <list>
#include <map>

class ActWindowClone
{
public:
	struct WindowData;

	typedef std::list<struct WindowData> WindowDatas;//���������б�
	typedef std::map<CEGUI::String, CEGUI::String> Properties;//���������б�

	//��������
	//����һ��������Ա����´������������ݸ���
	struct WindowData
	{
		CEGUI::String name;//��������
		CEGUI::String type;//�������
		Properties properties;//�������Լ�
		WindowDatas children;//������Ӵ���
	};

protected:
	//���帱�����������ĸ��������ƣ������������Ϊ���洰��
	CEGUI::String m_parent;
	//��ǰ��������д������ݸ���
	WindowDatas m_windows;

public:
	//��ȡ�������ݸ���
	WindowDatas &GetWindows()
	{
		return m_windows;
	}
	//��ȡ���帱�����ĸ�������
	CEGUI::String &GetParent()
	{
		return m_parent;
	}
	//���ô������ݸ������ĸ���������
	void SetParent(CEGUI::String &name)
	{
		m_parent = name;
	}

protected:
	//�����������
	void Clear(WindowDatas &windows);
	//���ݴ�������
	void Clone(WindowData &data, CEGUI::Window *source);
	void CloneRecursive(WindowData &data, CEGUI::Window *source);
	//���ݴ����������´������岢���뵽��Ӧ�ĸ�����
	void Create(CEGUI::Window *target);
	CEGUI::Window *Create(WindowData &data, CEGUI::Window *target);
	void CreateRecursive(WindowData &data, CEGUI::Window *target);

public:
	//�����������
	void Clear();
	//����һ����������������Ӵ�������
	void Clone(CEGUI::Window *source);
	//���ݵ�ǰ���ݵĴ����������´������岢���뵽��Ӧ�ĸ�������
	void AddToParent();
	//ɾ����ǰ���ݵĴ�����������Ӧ�Ĵ���
	void DelFromParent();

public:
	ActWindowClone(void);
	~ActWindowClone(void);
};

#endif