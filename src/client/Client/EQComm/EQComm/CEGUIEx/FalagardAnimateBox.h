/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-11
* 描述: 实现动画控件渲染
******************************************************************************/
#ifndef FalAnimateBox_H
#define FalAnimateBox_H

#include "CEGUI.h"
#include "CEGUIAnimateBox.h"


namespace CEGUI
{
	/*************************************************************************
	*	动画控件渲染类名
	*************************************************************************/
	static const String FalagardAnimateBox_TypeName = "Falagard/AnimateBox";

	/*************************************************************************
	*	动画控件渲染类
	*************************************************************************/
	class FalagardAnimateBox : public AnimateBoxWindowRenderer
	{
	public:
		static const String TypeName;	//	渲染类名称

	public:
		/**
		*	构造函数
		*/
		FalagardAnimateBox(const String& type);

		/**
		*	功	能：渲染函数
		*	参	数：void
		*	返回值：void
		*/
		void render(void);

	protected:
		/**
		*	功	能：渲染动画帧
		*	参	数：void
		*	返回值：void
		*/
		void cacheAnimateFrame(void);

	protected:
		int	mIndex;			//	索引

		DWORD mTimeStart;	//	开始时刻

		DWORD mTimePause;	//	暂停时刻

		DWORD mTimeCurr;	//	当前时刻

	};	//	end of class FalagardAnimateBox


	/*************************************************************************
	*	动画控件渲染工厂类
	*************************************************************************/
	class FalagardAnimateBoxWRFactory : public WindowRendererFactory
	{
	public:
		/**
		*	构造函数
		*/
		FalagardAnimateBoxWRFactory(void) : WindowRendererFactory(FalagardAnimateBox_TypeName){}

		/**
		*	析构函数
		*/
		~FalagardAnimateBoxWRFactory(void){}

		/**
		*	创建动画控件渲染对象
		*/
		WindowRenderer* create(void);

		/**
		*	销毁动画控件渲染对象
		*/
		void destroy(WindowRenderer* wr);
	};

}	//	end of namespace CEGUI


#endif	//	end of guard FalAnimateBox_H
