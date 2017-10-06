/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-11
* ����: ע�ᴰ���೧����Ⱦ�����೧
******************************************************************************/
#ifndef FactoryRegister_H
#define FactoryRegister_H

#include "CEGUIWindowFactory.h"
#include "CEGUIWindowRenderer.h"


namespace CEGUI
{
	/**
	*	��	�ܣ�ע��ؼ����೧
	*	��	����typeName:	�ؼ��������ƣ���"CEGUI/AnimateBox"
	*	����ֵ��void
	*/
	void registerFactory(String typeName);

	/**
	*	��	�ܣ�ע�����пؼ����೧
	*	��	����void
	*	����ֵ��void
	*/
	void registerAllFactories(void);

	/**
	*	��	�ܣ��೧ע��
	*	��	����factory:	�ؼ��೧
	*			wfFactory:	��Ⱦ�೧
	*	����ֵ��void
	*/
	void doSafeFactoryRegistration(WindowFactory* factory, WindowRendererFactory* wrfactory);

}	//	end of namespace CEGUI


#endif	//	end of guard FactoryRegister_H
