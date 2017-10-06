/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-4
* 描述: 动画控件逻辑实现
******************************************************************************/
#include "CEGUIAnimateBox.h"


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	构造函数
	AnimateBoxWindowRenderer::AnimateBoxWindowRenderer(const String& name) :
		WindowRenderer(name, AnimateBox_EventNamespace)
	{
	}
	//-------------------------------------------------------------------------
	//	全局事件名称
	const String AnimateBox::EventNamespace(AnimateBox_EventNamespace);
	//-------------------------------------------------------------------------
	//	窗口类型名称
	const String AnimateBox::WidgetTypeName(AnimateBox_WidgetTypeName);
	//-------------------------------------------------------------------------
	//	构造函数
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
	//	析构函数
	AnimateBox::~AnimateBox(void)
	{
	}
	//-------------------------------------------------------------------------
	//	添加动画对象（并自动设置控件大小为动画对象的大小）
	void AnimateBox::addAnimate(Animate animate)
	{
		mAnimate = animate;
		setSizeFunc(animate.getWidth(), animate.getHeight());
		mHasAnimate = true;
	}
	//-------------------------------------------------------------------------
	//	删除动画对象
	void AnimateBox::clearUp(void)
	{
		mAnimate.clearUp();
		mHasAnimate = false;
	}
	//-------------------------------------------------------------------------
	//	设置控件大小 
	void AnimateBox::setSizeFunc(float width, float height)
	{
		setSize(UVector2(UDim(0.0f, width), UDim(0.0f, height)));
	}
	//-------------------------------------------------------------------------
	//	设置控件位置
	void AnimateBox::setPositionFunc(float x, float y)
	{
		setPosition(UVector2(UDim(0.0f, x), UDim(0.0f, y)));
	}
	//-------------------------------------------------------------------------
	//	设置控件是否可见
	void AnimateBox::setVisibleFunc(bool visible)
	{
		setVisible(visible);
	}
	//-------------------------------------------------------------------------
	//	获取动画对象
	Animate& AnimateBox::getAnimate(void)
	{
		return mAnimate;
	}
	//-------------------------------------------------------------------------
	//	获取控件名字
	std::string AnimateBox::getNameFunc(void) const
	{
		String name = getName();
		std::string ctrlName = name.c_str();
		return ctrlName;
	}
	//-------------------------------------------------------------------------
	//	获取控件宽度
	float AnimateBox::getWidthFunc(void) const
	{
		float screenWidth = CEGUI::System::getSingleton().getRenderer()->getRect().getWidth();
		return getWidth().asAbsolute(screenWidth);
	}
	//-------------------------------------------------------------------------
	//	获取控件高度
	float AnimateBox::getHeightFunc(void) const
	{
		float screenHeight = CEGUI::System::getSingleton().getRenderer()->getRect().getHeight();
		return getHeight().asAbsolute(screenHeight);
	}
	//-------------------------------------------------------------------------
	//	播放
	void AnimateBox::play(bool loop)
	{
		if (mHasAnimate)	//	有动画对象才执行
		{
			mTimeStart = GetTickCount();
			mPlayState = PS_PLAY;
			mLoop = loop;
		}
	}
	//-------------------------------------------------------------------------
	//	暂停
	void AnimateBox::pause(void)
	{
		if (mHasAnimate)	//	有动画对象才执行
		{
			mTimePause = GetTickCount();
			mPlayState = PS_PAUSE;
		}
	}
	//-------------------------------------------------------------------------
	//	停止
	void AnimateBox::stop(void)
	{
		if (mHasAnimate)	//	有动画对象才执行
		{
			mTimePause = GetTickCount();
			mPlayState = PS_STOP;
		}
	}
	//-------------------------------------------------------------------------
	//	获取是否循环播放标志
	bool AnimateBox::hasAnimate(void)
	{
		return mHasAnimate;
	}
	//-------------------------------------------------------------------------
	//	获取是否循环播放标志
	bool AnimateBox::isLoop(void)
	{
		return mLoop;
	}
	//-------------------------------------------------------------------------
	//	获取播放状态
	AnimateBox::PlayState AnimateBox::getPlayState(void)
	{
		return mPlayState;
	}
	//-------------------------------------------------------------------------
	//	获取开始播放时刻
	DWORD AnimateBox::getTimeStart(void)
	{
		return mTimeStart;
	}
	//-------------------------------------------------------------------------
	//	获取暂停播放时刻
	DWORD AnimateBox::getTimePause(void)
	{
		return mTimePause;
	}
	//-------------------------------------------------------------------------
	//	实时更新窗口
	void AnimateBox::updateSelf(float elapsed)
	{
		requestRedraw();
	}
	//-------------------------------------------------------------------------
	//	创建窗口
	Window* AnimateBoxFactory::createWindow(const String& name)
	{
		AnimateBox *aniBox = new AnimateBox(d_type, name);
		assert(NULL != aniBox);
		return aniBox;
	}
	//-------------------------------------------------------------------------
	//	销毁窗口
	void AnimateBoxFactory::destroyWindow(Window* window)
	{
		assert(NULL != window);
		delete window;
	}
	//-------------------------------------------------------------------------
	//	转化Window类型为AnimateBox类型
	AnimateBox* toAnimateBox(std::string name)
	{
		Window *win = WindowManager::getSingleton().getWindow(name);
		assert(NULL != win);
		AnimateBox *aniBox = static_cast<AnimateBox*>(win);
		assert(NULL != aniBox);
		return aniBox;
	}

}	//	end of namespace CEGUI
