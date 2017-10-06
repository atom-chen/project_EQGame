/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-11
* 描述: 注册窗口类厂和渲染窗口类厂
******************************************************************************/
#ifndef FactoryRegister_H
#define FactoryRegister_H

#include "CEGUIWindowFactory.h"
#include "CEGUIWindowRenderer.h"


namespace CEGUI
{
	/**
	*	功	能：注册控件到类厂
	*	参	数：typeName:	控件类型名称，如"CEGUI/AnimateBox"
	*	返回值：void
	*/
	void registerFactory(String typeName);

	/**
	*	功	能：注册所有控件到类厂
	*	参	数：void
	*	返回值：void
	*/
	void registerAllFactories(void);

	/**
	*	功	能：类厂注册
	*	参	数：factory:	控件类厂
	*			wfFactory:	渲染类厂
	*	返回值：void
	*/
	void doSafeFactoryRegistration(WindowFactory* factory, WindowRendererFactory* wrfactory);

}	//	end of namespace CEGUI


#endif	//	end of guard FactoryRegister_H
