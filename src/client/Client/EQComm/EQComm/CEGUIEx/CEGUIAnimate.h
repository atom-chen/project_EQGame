/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-4
* ����: �����ؼ�ͷ�ļ���������Animate��
******************************************************************************/
#ifndef CEGUIAnimate_H
#define CEGUIAnimate_H

#include "CEGUI.h"


namespace CEGUI
{
	/*************************************************************************
	*	�����ؼ���
	*************************************************************************/
	class Animate
	{
	public:
		typedef std::vector<Image > ImageVector;	//	���嶯������

	public:
		/**
		*	���캯��
		*/
		Animate(void);

		/**
		*	��������
		*/
		virtual ~Animate(void);

		/**
		*	��	�ܣ���ȡ��������ʱ����
		*	��	����void
		*	����ֵ��int
		*/
		int getElapsed(void) const;

		/**
		*	��	�ܣ���ȡ����������ʱ��
		*	��	����void
		*	����ֵ��int
		*/
		int getTimeTotal(void) const;

		/**
		*	��	�ܣ���ȡ�������
		*	��	����void
		*	����ֵ��float
		*/
		float getWidth(void) const;	

		/**
		*	��	�ܣ���ȡ�����߶�
		*	��	����void
		*	����ֵ��float
		*/
		float getHeight(void) const;

		/**
		*	��	�ܣ����Ӷ���֡
		*	��	����img:	����֡
		*	����ֵ��void
		*/
		void addAnimateFrame(Image img);

		/**
		*	��	�ܣ����Ӷ���֡
		*	��	����imagesetName:	����֡���ڵ�iamgeset����
		*			imageName:	����֡�ı�ʶ������
		*	����ֵ��void
		*/
		void addAnimateFrame(std::string imagesetName, std::string imageName);

		/**
		*	��	�ܣ��������֡
		*	��	����void
		*	����ֵ��void
		*/
		void clearUp(void);

		/**
		*	��	�ܣ�����������ȡ����֡
		*	��	����index:	����
		*	����ֵ��Image&
		*/
		Image& getAnimateFrame(size_t index);

		/**
		*	��	�ܣ���ʼ����������
		*	��	����elapsed:	�������ŵ�ʱ����
		*			width:	�������
		*			height:	�����߶�
		*	����ֵ��void
		*/
		void initAnimate(int elapsed, float width, float height);

	protected:
		ImageVector mImageVec;	//	����֡����

		int mElapsed;	//	�������ŵ�ʱ����

		int mTimeTotal;	//	�������ŵ���ʱ��

		float mWidth;	//	�������

		float mHeight;	//	�����߶�

	};	//	end of class Animate

	/*************************************************************************
	*	CEGUIȫ�ֽӿ�
	*************************************************************************/
	/**
	*	��	�ܣ�������������
	*	��	����void
	*	����ֵ����������
	*/
	Animate createAnimate(void);

}	//	end of namespace CEGUI


#endif	//	end of guard CEGUIAnimate_H
