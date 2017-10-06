/******************************************************************************
* 作者: xj
* 时间: 2010-11-2
* 描述: 选择集窗体副本
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

	typedef std::list<struct WindowData> WindowDatas;//窗体数据列表
	typedef std::map<CEGUI::String, CEGUI::String> Properties;//属性数据列表

	//窗体数据
	//保存一个窗体可以被重新创建的最少数据副本
	struct WindowData
	{
		CEGUI::String name;//窗体名称
		CEGUI::String type;//窗体对象
		Properties properties;//窗体属性集
		WindowDatas children;//窗体的子窗体
	};

protected:
	//窗体副本数据所属的父窗体名称，如果不存在则为桌面窗体
	CEGUI::String m_parent;
	//当前保存的所有窗体数据副本
	WindowDatas m_windows;

public:
	//获取窗体数据副本
	WindowDatas &GetWindows()
	{
		return m_windows;
	}
	//获取窗体副本集的父窗体名
	CEGUI::String &GetParent()
	{
		return m_parent;
	}
	//设置窗体数据副本集的父窗体名称
	void SetParent(CEGUI::String &name)
	{
		m_parent = name;
	}

protected:
	//清除窗体数据
	void Clear(WindowDatas &windows);
	//备份窗体数据
	void Clone(WindowData &data, CEGUI::Window *source);
	void CloneRecursive(WindowData &data, CEGUI::Window *source);
	//根据窗体数据重新创建窗体并加入到对应的父窗体
	void Create(CEGUI::Window *target);
	CEGUI::Window *Create(WindowData &data, CEGUI::Window *target);
	void CreateRecursive(WindowData &data, CEGUI::Window *target);

public:
	//清除窗体数据
	void Clear();
	//备份一个窗体和它的所有子窗体数据
	void Clone(CEGUI::Window *source);
	//根据当前备份的窗体数据重新创建窗体并加入到对应的父窗体中
	void AddToParent();
	//删除当前备份的窗体数据所对应的窗体
	void DelFromParent();

public:
	ActWindowClone(void);
	~ActWindowClone(void);
};

#endif