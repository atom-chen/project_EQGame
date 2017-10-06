/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-4
* ����: �����ؼ��߼�ʵ��
******************************************************************************/
#ifndef CEGUIAnimateBox_H
#define CEGUIAnimateBox_H

#include "CEGUI.h"
#include "CEGUIAnimate.h"
#include <Windows.h>


namespace CEGUI
{
	/*************************************************************************
	*	�����ؼ��¼���
	*************************************************************************/
	static const String AnimateBox_EventNamespace = "AnimateBox";

	/*************************************************************************
	*	�����ؼ�������
	*************************************************************************/
	static const String AnimateBox_WidgetTypeName = "CEGUI/AnimateBox";

	/*************************************************************************
	*	�����ؼ���Ⱦ��
	*************************************************************************/
	class AnimateBoxWindowRenderer : public WindowRenderer
	{
	public:
		/**
		*	���캯��
		*/
		AnimateBoxWindowRenderer(const String& name);

	};	//	end of class AnimateBoxWindowRenderer

	/*************************************************************************
	*	�����ؼ���
	*************************************************************************/
	class AnimateBox : public Window
	{
	public:
		//	��������״̬
		enum PlayState
		{
			PS_PLAY,	//	����
			PS_PAUSE,	//	��ͣ
			PS_STOP		//	ֹͣ
		};

	public:
		static const String EventNamespace;	//	ȫ���¼�����

		static const String WidgetTypeName;	//	������������

	public:
		/**
		*	���캯��
		*/
		AnimateBox(const String& type, const String& name);

		/**
		*	��������
		*/
		virtual ~AnimateBox(void);

	public:
		/**
		*	��	�ܣ���Ӷ������󣨲��Զ����ÿؼ���СΪ��������Ĵ�С��
		*	��	����aniamte:	��������
		*	����ֵ��void
		*/
		void addAnimate(Animate animate);

		/**
		*	��	�ܣ�ɾ����������
		*	��	����void
		*	����ֵ��void
		*/
		void clearUp(void);

		/**
		*	��	�ܣ����ÿؼ���С
		*	��	����width:	���
		*			height:	�߶�
		*	����ֵ��void
		*/
		void setSizeFunc(float width, float height);

		/**
		*	��	�ܣ����ÿؼ�λ��
		*	��	����x:	x����
		*			y:	y����
		*	����ֵ��void
		*/
		void setPositionFunc(float x, float y);

		/**
		*	��	�ܣ����ÿؼ��Ƿ�ɼ�
		*	��	����visible:	�Ƿ�ɼ���־��true���ɼ���false�����ɼ���
		*	����ֵ��void
		*/
		void setVisibleFunc(bool visible=true);

		/**
		*	��	�ܣ���ȡ��������
		*	��	����void
		*	����ֵ����������
		*/
		Animate& getAnimate(void);

		/**
		*	��	�ܣ���ȡ�ؼ�����
		*	��	����void
		*	����ֵ��str::string
		*/
		std::string getNameFunc(void) const;

		/**
		*	��	�ܣ���ȡ�����ؼ����
		*	��	����void
		*	����ֵ��float
		*/
		float getWidthFunc(void) const;

		/**
		*	��	�ܣ���ȡ�����ؼ��߶�
		*	��	����void
		*	����ֵ��float
		*/
		float getHeightFunc(void) const;

		/**
		*	��	�ܣ�����
		*	��	����loop:	����ģʽ��true��ѭ�����ţ�false����ѭ�����ţ�
		*	����ֵ��void
		*/
		void play(bool loop=false);

		/**
		*	��	�ܣ���ͣ
		*	��	����void
		*	����ֵ��void
		*/
		void pause(void);

		/**
		*	��	�ܣ�ֹͣ
		*	��	����void
		*	����ֵ��void
		*/
		void stop(void);

		/**
		*	��	�ܣ����ò��Ž��������¼�
		*	��	����scriptFunc: �¼�������lua�еĽű�������
		*	����ֵ��void
		*/
		void setPlayOverHandle(std::string scriptFunc){mPlayOverHandle = scriptFunc;}

	public:
		/**
		*	�жϿؼ��Ƿ���ڶ�������
		*/
		bool hasAnimate(void);

		/**
		*	���ض����ؼ�����ģʽ
		*/
		bool isLoop(void);

		/**
		*	��ȡ�����ؼ�����״̬
		*/
		PlayState getPlayState(void);

		/**
		*	��ȡ�����ؼ���ʼ����ʱ��
		*/
		DWORD getTimeStart(void);

		/**
		*	��ȡ�����ؼ���ͣ����ʱ��
		*/
		DWORD getTimePause(void);

		/**
		*	�����Զ����Ž����¼�
		*/
		void firePlayOver(void)
		{
			stop();
			if ("" != mPlayOverHandle)
			{
				CEGUI::System::getSingleton().executeScriptGlobal(mPlayOverHandle); 
			}
		}

	protected:
		/**
		*	ʵʱ���´���
		*/
		virtual void updateSelf(float elapsed);

		/**
		*	�жϴ����Ƿ�̳�����������
		*/
		virtual bool testClassName_impl(const String& class_name) const
		{
			if ("AnimateBox" == class_name)
			{
				return true;
			}

			return Window::testClassName_impl(class_name);
		}

	protected:
		Animate mAnimate;	//	��������

		bool mHasAnimate;	//	�ؼ����ڶ�������

		DWORD mTimeStart;	//	��ʼ����ʱ��

		DWORD mTimePause;	//	��ͣ����ʱ��

		bool mLoop;			//	��������ģʽ

		PlayState mPlayState;	//	��������״̬

		std::string mPlayOverHandle;	// ���Ž��������ĺ���

	};	//	end of class AnimateBox


	/*************************************************************************
	*	�����ؼ�������
	*************************************************************************/
	class AnimateBoxFactory : public WindowFactory
	{
	public:
		/**
		*	���캯��
		*/
		AnimateBoxFactory(void) : WindowFactory(AnimateBox_WidgetTypeName){}

		/**
		*	��������
		*/
		~AnimateBoxFactory(void){}

		/**
		*	��������
		*/
		Window* createWindow(const String& name);

		/**
		*	���ٴ���
		*/
		void destroyWindow(Window* window);
	};

	/*************************************************************************
	*	CEGUIȫ�ֽӿ�
	*************************************************************************/
	/**
	*	��	�ܣ�ת��Window����ΪAnimateBox����
	*	��	����name:	��������
	*	����ֵ�������ؼ�
	*/
	AnimateBox* toAnimateBox(std::string name);

}	//	end of namespace CEGUI


#endif	//	end of guard CEGUIAnimateBox_H
