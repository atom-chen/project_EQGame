/******************************************************************************
* 作者: xj
* 时间: 2010-11-2
* 描述: 撤消动作的窗体选择集
******************************************************************************/

#pragma once

#ifndef __ActWindows_H
#define __ActWindows_H

#include <map>

//窗体区域类型
//键：窗体名称
//值：窗体区域的备份值，有可能和窗体现在的值不一样。
typedef std::map<CEGUI::String, CEGUI::URect, CEGUI::String::FastLessCompare> NRects;

class ActWindows
{
protected:
	//窗体区域变量
	NRects m_winNRects;

public:
	//获取窗体区域变量
	NRects &GetNRects()
	{
		return m_winNRects;
	}
	//清除窗体区域变量的所有值
	void Clear()
	{
		m_winNRects.clear();
	}

public:
	//更新窗体区域变量内的所有区域值，输入参键和当前变量键相同的才会更新。
	void UpdateRects(NRects &nrects);
	//根据窗体区域变量的区域值更新所有窗体名对应的窗体区域
	void UpdateAreas();
	void UpdateAreas(NRects &nrects);
	//更新窗体区域变量的所有键和区域值
	void UpdateNRects(NRects &nrects);
	//输出（取得）窗体区域变量中所有窗体名对应的区域值
	void OuputRects(NRects &nrects);

public:
	ActWindows(void);
	virtual ~ActWindows(void);
};

#endif