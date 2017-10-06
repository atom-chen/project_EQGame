/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-11
* ����: ע�ᴰ���೧����Ⱦ�����೧
******************************************************************************/
#include "FactoryRegister.h"
#include "CEGUIWindowFactoryManager.h"
#include "CEGUIWindowRendererManager.h"

//	�ؼ�ͷ�ļ�
#include "CEGUIAnimateBox.h"
#include "CEGUIMultiLineBox.h"

//	�ؼ���Ⱦͷ�ļ�
#include "FalagardAnimateBox.h"
#include "FalagardMultiLineBox.h"


namespace CEGUI
{
	//-----------------------------------------------------------------------
	//	��̬�����೧����
	static AnimateBoxFactory s_AnimateBoxFactory;
	static MultiLineBoxFactory s_MultiLineBoxFactory;
	//-----------------------------------------------------------------------
	//	��̬��Ⱦ�����೧����
	static FalagardAnimateBoxWRFactory s_AnimateBoxWRFactory;
	static FalagardMultiLineBoxWRFactory s_MultiLineBoxWRFactory;

	//-----------------------------------------------------------------------
	//	ӳ�����
	struct mapEntry
	{
		int d_flag;							//	ӳ�伯��ʶ��ֵΪ��1��0
		WindowFactory* d_factory;			//	���ڹ���
		WindowRendererFactory* d_wrFactory;	//	������Ⱦ����
	};
	//-----------------------------------------------------------------------
	//	��ʼ��ӳ�伯
	mapEntry factoriesMap[] =
	{
		{1, &s_AnimateBoxFactory, &s_AnimateBoxWRFactory},
		{1, &s_MultiLineBoxFactory, &s_MultiLineBoxWRFactory},
		{0, 0, 0}	//	����
	};
	//-----------------------------------------------------------------------
	//	ע��ؼ����೧
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
	//	ע�����пؼ����೧
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
	//	�೧ע��
	void doSafeFactoryRegistration(WindowFactory* factory, WindowRendererFactory* wrFactory)
	{
		assert(NULL != factory);
		assert(NULL != wrFactory);

		WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
		WindowRendererManager& wrMgr = WindowRendererManager::getSingleton();

		//	�����೧�Ѿ�ע��
		if (wfMgr.isFactoryPresent(factory->getTypeName()))
		{
			return;
		}
		//	��Ⱦ�����೧�Ѿ�ע��
		if (wrMgr.isFactoryPresent(wrFactory->getName()))
		{
			return;
		}
		//	�����೧��ע�ᴰ���೧��δע��
		wfMgr.addFactory(factory);
		wrMgr.addFactory(wrFactory);
	}

}	//	end of namespace CEGUI
