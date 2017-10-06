/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-11
* ����: ʵ�ֶ����ؼ���Ⱦ
******************************************************************************/
#ifndef FalAnimateBox_H
#define FalAnimateBox_H

#include "CEGUI.h"
#include "CEGUIAnimateBox.h"


namespace CEGUI
{
	/*************************************************************************
	*	�����ؼ���Ⱦ����
	*************************************************************************/
	static const String FalagardAnimateBox_TypeName = "Falagard/AnimateBox";

	/*************************************************************************
	*	�����ؼ���Ⱦ��
	*************************************************************************/
	class FalagardAnimateBox : public AnimateBoxWindowRenderer
	{
	public:
		static const String TypeName;	//	��Ⱦ������

	public:
		/**
		*	���캯��
		*/
		FalagardAnimateBox(const String& type);

		/**
		*	��	�ܣ���Ⱦ����
		*	��	����void
		*	����ֵ��void
		*/
		void render(void);

	protected:
		/**
		*	��	�ܣ���Ⱦ����֡
		*	��	����void
		*	����ֵ��void
		*/
		void cacheAnimateFrame(void);

	protected:
		int	mIndex;			//	����

		DWORD mTimeStart;	//	��ʼʱ��

		DWORD mTimePause;	//	��ͣʱ��

		DWORD mTimeCurr;	//	��ǰʱ��

	};	//	end of class FalagardAnimateBox


	/*************************************************************************
	*	�����ؼ���Ⱦ������
	*************************************************************************/
	class FalagardAnimateBoxWRFactory : public WindowRendererFactory
	{
	public:
		/**
		*	���캯��
		*/
		FalagardAnimateBoxWRFactory(void) : WindowRendererFactory(FalagardAnimateBox_TypeName){}

		/**
		*	��������
		*/
		~FalagardAnimateBoxWRFactory(void){}

		/**
		*	���������ؼ���Ⱦ����
		*/
		WindowRenderer* create(void);

		/**
		*	���ٶ����ؼ���Ⱦ����
		*/
		void destroy(WindowRenderer* wr);
	};

}	//	end of namespace CEGUI


#endif	//	end of guard FalAnimateBox_H
