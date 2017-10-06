/******************************************************************************
* ����: xj
* ʱ��: 2010-11-2
* ����: ���������Ĵ���ѡ��
******************************************************************************/

#pragma once

#ifndef __ActWindows_H
#define __ActWindows_H

#include <map>

//������������
//������������
//ֵ����������ı���ֵ���п��ܺʹ������ڵ�ֵ��һ����
typedef std::map<CEGUI::String, CEGUI::URect, CEGUI::String::FastLessCompare> NRects;

class ActWindows
{
protected:
	//�����������
	NRects m_winNRects;

public:
	//��ȡ�����������
	NRects &GetNRects()
	{
		return m_winNRects;
	}
	//��������������������ֵ
	void Clear()
	{
		m_winNRects.clear();
	}

public:
	//���´�����������ڵ���������ֵ������μ��͵�ǰ��������ͬ�ĲŻ���¡�
	void UpdateRects(NRects &nrects);
	//���ݴ����������������ֵ�������д�������Ӧ�Ĵ�������
	void UpdateAreas();
	void UpdateAreas(NRects &nrects);
	//���´���������������м�������ֵ
	void UpdateNRects(NRects &nrects);
	//�����ȡ�ã�����������������д�������Ӧ������ֵ
	void OuputRects(NRects &nrects);

public:
	ActWindows(void);
	virtual ~ActWindows(void);
};

#endif