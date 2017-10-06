/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-4
* 描述: 动画控件逻辑实现
******************************************************************************/
#ifndef CEGUIAnimateBox_H
#define CEGUIAnimateBox_H

#include "CEGUI.h"
#include "CEGUIAnimate.h"
#include <Windows.h>


namespace CEGUI
{
	/*************************************************************************
	*	动画控件事件名
	*************************************************************************/
	static const String AnimateBox_EventNamespace = "AnimateBox";

	/*************************************************************************
	*	动画控件类型名
	*************************************************************************/
	static const String AnimateBox_WidgetTypeName = "CEGUI/AnimateBox";

	/*************************************************************************
	*	动画控件渲染类
	*************************************************************************/
	class AnimateBoxWindowRenderer : public WindowRenderer
	{
	public:
		/**
		*	构造函数
		*/
		AnimateBoxWindowRenderer(const String& name);

	};	//	end of class AnimateBoxWindowRenderer

	/*************************************************************************
	*	动画控件类
	*************************************************************************/
	class AnimateBox : public Window
	{
	public:
		//	动画播放状态
		enum PlayState
		{
			PS_PLAY,	//	播放
			PS_PAUSE,	//	暂停
			PS_STOP		//	停止
		};

	public:
		static const String EventNamespace;	//	全局事件名称

		static const String WidgetTypeName;	//	窗口类型名称

	public:
		/**
		*	构造函数
		*/
		AnimateBox(const String& type, const String& name);

		/**
		*	析构函数
		*/
		virtual ~AnimateBox(void);

	public:
		/**
		*	功	能：添加动画对象（并自动设置控件大小为动画对象的大小）
		*	参	数：aniamte:	动画对象
		*	返回值：void
		*/
		void addAnimate(Animate animate);

		/**
		*	功	能：删除动画对象
		*	参	数：void
		*	返回值：void
		*/
		void clearUp(void);

		/**
		*	功	能：设置控件大小
		*	参	数：width:	宽度
		*			height:	高度
		*	返回值：void
		*/
		void setSizeFunc(float width, float height);

		/**
		*	功	能：设置控件位置
		*	参	数：x:	x坐标
		*			y:	y坐标
		*	返回值：void
		*/
		void setPositionFunc(float x, float y);

		/**
		*	功	能：设置控件是否可见
		*	参	数：visible:	是否可见标志（true，可见；false，不可见）
		*	返回值：void
		*/
		void setVisibleFunc(bool visible=true);

		/**
		*	功	能：获取动画对象
		*	参	数：void
		*	返回值：动画对象
		*/
		Animate& getAnimate(void);

		/**
		*	功	能：获取控件名字
		*	参	数：void
		*	返回值：str::string
		*/
		std::string getNameFunc(void) const;

		/**
		*	功	能：获取动画控件宽度
		*	参	数：void
		*	返回值：float
		*/
		float getWidthFunc(void) const;

		/**
		*	功	能：获取动画控件高度
		*	参	数：void
		*	返回值：float
		*/
		float getHeightFunc(void) const;

		/**
		*	功	能：播放
		*	参	数：loop:	播放模式（true，循环播放；false，非循环播放）
		*	返回值：void
		*/
		void play(bool loop=false);

		/**
		*	功	能：暂停
		*	参	数：void
		*	返回值：void
		*/
		void pause(void);

		/**
		*	功	能：停止
		*	参	数：void
		*	返回值：void
		*/
		void stop(void);

		/**
		*	功	能：设置播放结束触发事件
		*	参	数：scriptFunc: 事件函数（lua中的脚本函数）
		*	返回值：void
		*/
		void setPlayOverHandle(std::string scriptFunc){mPlayOverHandle = scriptFunc;}

	public:
		/**
		*	判断控件是否存在动画对象
		*/
		bool hasAnimate(void);

		/**
		*	返回动画控件播放模式
		*/
		bool isLoop(void);

		/**
		*	获取动画控件播放状态
		*/
		PlayState getPlayState(void);

		/**
		*	获取动画控件开始播放时刻
		*/
		DWORD getTimeStart(void);

		/**
		*	获取动画控件暂停播放时刻
		*/
		DWORD getTimePause(void);

		/**
		*	触发自动播放结束事件
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
		*	实时更新窗口
		*/
		virtual void updateSelf(float elapsed);

		/**
		*	判断窗口是否继承自所给类名
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
		Animate mAnimate;	//	动画对象

		bool mHasAnimate;	//	控件存在动画对象

		DWORD mTimeStart;	//	开始播放时刻

		DWORD mTimePause;	//	暂停播放时刻

		bool mLoop;			//	动画播放模式

		PlayState mPlayState;	//	动画播放状态

		std::string mPlayOverHandle;	// 播放结束触发的函数

	};	//	end of class AnimateBox


	/*************************************************************************
	*	动画控件工厂类
	*************************************************************************/
	class AnimateBoxFactory : public WindowFactory
	{
	public:
		/**
		*	构造函数
		*/
		AnimateBoxFactory(void) : WindowFactory(AnimateBox_WidgetTypeName){}

		/**
		*	析构函数
		*/
		~AnimateBoxFactory(void){}

		/**
		*	创建窗口
		*/
		Window* createWindow(const String& name);

		/**
		*	销毁窗口
		*/
		void destroyWindow(Window* window);
	};

	/*************************************************************************
	*	CEGUI全局接口
	*************************************************************************/
	/**
	*	功	能：转化Window类型为AnimateBox类型
	*	参	数：name:	窗口名称
	*	返回值：动画控件
	*/
	AnimateBox* toAnimateBox(std::string name);

}	//	end of namespace CEGUI


#endif	//	end of guard CEGUIAnimateBox_H
