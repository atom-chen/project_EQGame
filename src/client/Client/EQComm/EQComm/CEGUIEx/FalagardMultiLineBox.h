/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-7-1
* ����: ʵ��ͼ�Ļ��ſؼ�����Ⱦ
******************************************************************************/
#ifndef FalMultiLineBox_H
#define FalMultiLineBox_H

#include "CEGUI.h"
#include "CEGUIMultiLineBox.h"


namespace CEGUI
{
	/*************************************************************************
	*	ͼ�Ļ��ſؼ���Ⱦ����
	*************************************************************************/
	static const String FalagardMultiLineBox_TypeName = "Falagard/MultiLineBox";

	/*************************************************************************
	*	ͼ�Ļ��ſؼ���Ⱦ��
	*************************************************************************/
	class FalagardMultiLineBox : public MultiLineBoxWindowRenderer
	{
	public:
		static const String TypeName;	//	��Ⱦ������

	public:
		/**
		*	���캯��
		*/
		FalagardMultiLineBox(const String& type);

		/**
		*	��	�ܣ���ȡͼ�Ļ��ſؼ���Ⱦ�������ػ��ຯ����
		*	��	����void
		*	����ֵ��Rect
		*/
		Rect getRenderArea(void) const;

		/**
		*	��	�ܣ���Ⱦ����
		*	��	����void
		*	����ֵ��void
		*/
		void render(void);

	protected:
		/**
		*	��	�ܣ���ȡ��������
		*	��	����void
		*	����ֵ��Rect
		*/
		Rect _getNameArea(void) const;

		/**
		*	��	�ܣ���Ⱦͼ�Ļ��ſؼ�����Ϣ
		*	��	����dest_area:	��Ⱦ����
		*	����ֵ��void
		*/
		void _cacheElementLines(const Rect& dest_area);

	};	//	end of class FalagardMultiLineBox

	/*************************************************************************
	*	ͼ�Ļ��ſؼ��ؼ���Ⱦ������
	*************************************************************************/
	class FalagardMultiLineBoxWRFactory : public WindowRendererFactory
	{
	public:
		/**
		*	���캯��
		*/
		FalagardMultiLineBoxWRFactory(void) : WindowRendererFactory(FalagardMultiLineBox_TypeName){}

		/**
		*	��������
		*/
		~FalagardMultiLineBoxWRFactory(void){}

		/**
		*	����ͼ�Ļ��ſؼ���Ⱦ����
		*/
		WindowRenderer* create(void);

		/**
		*	����ͼ�Ļ��ſؼ���Ⱦ����
		*/
		void destroy(WindowRenderer* wr);
	};

}	//	end of namespace CEGUI


#endif	//	end of guard FalMultiLineBox_H
