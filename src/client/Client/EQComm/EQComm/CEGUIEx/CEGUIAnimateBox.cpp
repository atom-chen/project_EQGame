/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-4
* ����: �����ؼ��߼�ʵ��
******************************************************************************/
#include "CEGUIAnimateBox.h"


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	���캯��
	AnimateBoxWindowRenderer::AnimateBoxWindowRenderer(const String& name) :
		WindowRenderer(name, AnimateBox_EventNamespace)
	{
	}
	//-------------------------------------------------------------------------
	//	ȫ���¼�����
	const String AnimateBox::EventNamespace(AnimateBox_EventNamespace);
	//-------------------------------------------------------------------------
	//	������������
	const String AnimateBox::WidgetTypeName(AnimateBox_WidgetTypeName);
	//-------------------------------------------------------------------------
	//	���캯��
	AnimateBox::AnimateBox(const String& type, const String& name) :
		Window(type, name),
		mHasAnimate(false),
		mPlayState(PS_STOP),
		mTimeStart(0),
		mTimePause(0),
		mPlayOverHandle("")
	{
	}
	//-------------------------------------------------------------------------
	//	��������
	AnimateBox::~AnimateBox(void)
	{
	}
	//-------------------------------------------------------------------------
	//	��Ӷ������󣨲��Զ����ÿؼ���СΪ��������Ĵ�С��
	void AnimateBox::addAnimate(Animate animate)
	{
		mAnimate = animate;
		setSizeFunc(animate.getWidth(), animate.getHeight());
		mHasAnimate = true;
	}
	//-------------------------------------------------------------------------
	//	ɾ����������
	void AnimateBox::clearUp(void)
	{
		mAnimate.clearUp();
		mHasAnimate = false;
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ���С 
	void AnimateBox::setSizeFunc(float width, float height)
	{
		setSize(UVector2(UDim(0.0f, width), UDim(0.0f, height)));
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ�λ��
	void AnimateBox::setPositionFunc(float x, float y)
	{
		setPosition(UVector2(UDim(0.0f, x), UDim(0.0f, y)));
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ��Ƿ�ɼ�
	void AnimateBox::setVisibleFunc(bool visible)
	{
		setVisible(visible);
	}
	//-------------------------------------------------------------------------
	//	��ȡ��������
	Animate& AnimateBox::getAnimate(void)
	{
		return mAnimate;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ؼ�����
	std::string AnimateBox::getNameFunc(void) const
	{
		String name = getName();
		std::string ctrlName = name.c_str();
		return ctrlName;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ؼ����
	float AnimateBox::getWidthFunc(void) const
	{
		float screenWidth = CEGUI::System::getSingleton().getRenderer()->getRect().getWidth();
		return getWidth().asAbsolute(screenWidth);
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ؼ��߶�
	float AnimateBox::getHeightFunc(void) const
	{
		float screenHeight = CEGUI::System::getSingleton().getRenderer()->getRect().getHeight();
		return getHeight().asAbsolute(screenHeight);
	}
	//-------------------------------------------------------------------------
	//	����
	void AnimateBox::play(bool loop)
	{
		if (mHasAnimate)	//	�ж��������ִ��
		{
			mTimeStart = GetTickCount();
			mPlayState = PS_PLAY;
			mLoop = loop;
		}
	}
	//-------------------------------------------------------------------------
	//	��ͣ
	void AnimateBox::pause(void)
	{
		if (mHasAnimate)	//	�ж��������ִ��
		{
			mTimePause = GetTickCount();
			mPlayState = PS_PAUSE;
		}
	}
	//-------------------------------------------------------------------------
	//	ֹͣ
	void AnimateBox::stop(void)
	{
		if (mHasAnimate)	//	�ж��������ִ��
		{
			mTimePause = GetTickCount();
			mPlayState = PS_STOP;
		}
	}
	//-------------------------------------------------------------------------
	//	��ȡ�Ƿ�ѭ�����ű�־
	bool AnimateBox::hasAnimate(void)
	{
		return mHasAnimate;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�Ƿ�ѭ�����ű�־
	bool AnimateBox::isLoop(void)
	{
		return mLoop;
	}
	//-------------------------------------------------------------------------
	//	��ȡ����״̬
	AnimateBox::PlayState AnimateBox::getPlayState(void)
	{
		return mPlayState;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��ʼ����ʱ��
	DWORD AnimateBox::getTimeStart(void)
	{
		return mTimeStart;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��ͣ����ʱ��
	DWORD AnimateBox::getTimePause(void)
	{
		return mTimePause;
	}
	//-------------------------------------------------------------------------
	//	ʵʱ���´���
	void AnimateBox::updateSelf(float elapsed)
	{
		requestRedraw();
	}
	//-------------------------------------------------------------------------
	//	��������
	Window* AnimateBoxFactory::createWindow(const String& name)
	{
		AnimateBox *aniBox = new AnimateBox(d_type, name);
		assert(NULL != aniBox);
		return aniBox;
	}
	//-------------------------------------------------------------------------
	//	���ٴ���
	void AnimateBoxFactory::destroyWindow(Window* window)
	{
		assert(NULL != window);
		delete window;
	}
	//-------------------------------------------------------------------------
	//	ת��Window����ΪAnimateBox����
	AnimateBox* toAnimateBox(std::string name)
	{
		Window *win = WindowManager::getSingleton().getWindow(name);
		assert(NULL != win);
		AnimateBox *aniBox = static_cast<AnimateBox*>(win);
		assert(NULL != aniBox);
		return aniBox;
	}

}	//	end of namespace CEGUI
