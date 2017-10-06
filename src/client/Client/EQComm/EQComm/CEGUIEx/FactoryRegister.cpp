/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-11
* 描述: 注册窗口类厂和渲染窗口类厂
******************************************************************************/
#include "FactoryRegister.h"
#include "CEGUIWindowFactoryManager.h"
#include "CEGUIWindowRendererManager.h"

//	控件头文件
#include "CEGUIAnimateBox.h"
#include "CEGUIMultiLineBox.h"

//	控件渲染头文件
#include "FalagardAnimateBox.h"
#include "FalagardMultiLineBox.h"


namespace CEGUI
{
	//-----------------------------------------------------------------------
	//	静态窗口类厂对象
	static AnimateBoxFactory s_AnimateBoxFactory;
	static MultiLineBoxFactory s_MultiLineBoxFactory;
	//-----------------------------------------------------------------------
	//	静态渲染窗口类厂对象
	static FalagardAnimateBoxWRFactory s_AnimateBoxWRFactory;
	static FalagardMultiLineBoxWRFactory s_MultiLineBoxWRFactory;

	//-----------------------------------------------------------------------
	//	映射入口
	struct mapEntry
	{
		int d_flag;							//	映射集标识，值为：1，0
		WindowFactory* d_factory;			//	窗口工厂
		WindowRendererFactory* d_wrFactory;	//	窗口渲染工厂
	};
	//-----------------------------------------------------------------------
	//	初始化映射集
	mapEntry factoriesMap[] =
	{
		{1, &s_AnimateBoxFactory, &s_AnimateBoxWRFactory},
		{1, &s_MultiLineBoxFactory, &s_MultiLineBoxWRFactory},
		{0, 0, 0}	//	结束
	};
	//-----------------------------------------------------------------------
	//	注册控件到类厂
	void registerFactory(String typeName)
	{
		mapEntry* entry = factoriesMap;
		assert(NULL != entry);

		while (entry->d_flag)
		{
			if (entry->d_factory->getTypeName() == typeName)
			{
				doSafeFactoryRegistration(entry->d_factory, entry->d_wrFactory);
				return;
			}
			++entry;
		}
	}
	//-----------------------------------------------------------------------
	//	注册所有控件到类厂
	void registerAllFactories(void)
	{
		mapEntry* entry = factoriesMap;
		assert(NULL != entry);

		while (entry->d_flag)
		{
			doSafeFactoryRegistration(entry->d_factory, entry->d_wrFactory);
			++entry;
		}
	}
	//-----------------------------------------------------------------------
	//	类厂注册
	void doSafeFactoryRegistration(WindowFactory* factory, WindowRendererFactory* wrFactory)
	{
		assert(NULL != factory);
		assert(NULL != wrFactory);

		WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
		WindowRendererManager& wrMgr = WindowRendererManager::getSingleton();

		//	窗口类厂已经注册
		if (wfMgr.isFactoryPresent(factory->getTypeName()))
		{
			return;
		}
		//	渲染窗口类厂已经注册
		if (wrMgr.isFactoryPresent(wrFactory->getName()))
		{
			return;
		}
		//	窗口类厂和注册窗口类厂都未注册
		wfMgr.addFactory(factory);
		wrMgr.addFactory(wrFactory);
	}

}	//	end of namespace CEGUI
