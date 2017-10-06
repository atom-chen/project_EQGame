/******************************************************************************
* 作者: 何展然
* 时间: 2010-7-1
* 描述: 实现图文混排控件的渲染
******************************************************************************/
#ifndef FalMultiLineBox_H
#define FalMultiLineBox_H

#include "CEGUI.h"
#include "CEGUIMultiLineBox.h"


namespace CEGUI
{
	/*************************************************************************
	*	图文混排控件渲染类名
	*************************************************************************/
	static const String FalagardMultiLineBox_TypeName = "Falagard/MultiLineBox";

	/*************************************************************************
	*	图文混排控件渲染类
	*************************************************************************/
	class FalagardMultiLineBox : public MultiLineBoxWindowRenderer
	{
	public:
		static const String TypeName;	//	渲染类名称

	public:
		/**
		*	构造函数
		*/
		FalagardMultiLineBox(const String& type);

		/**
		*	功	能：获取图文混排控件渲染区域（重载基类函数）
		*	参	数：void
		*	返回值：Rect
		*/
		Rect getRenderArea(void) const;

		/**
		*	功	能：渲染函数
		*	参	数：void
		*	返回值：void
		*/
		void render(void);

	protected:
		/**
		*	功	能：获取命名区域
		*	参	数：void
		*	返回值：Rect
		*/
		Rect _getNameArea(void) const;

		/**
		*	功	能：渲染图文混排控件行信息
		*	参	数：dest_area:	渲染区域
		*	返回值：void
		*/
		void _cacheElementLines(const Rect& dest_area);

	};	//	end of class FalagardMultiLineBox

	/*************************************************************************
	*	图文混排控件控件渲染工厂类
	*************************************************************************/
	class FalagardMultiLineBoxWRFactory : public WindowRendererFactory
	{
	public:
		/**
		*	构造函数
		*/
		FalagardMultiLineBoxWRFactory(void) : WindowRendererFactory(FalagardMultiLineBox_TypeName){}

		/**
		*	析构函数
		*/
		~FalagardMultiLineBoxWRFactory(void){}

		/**
		*	创建图文混排控件渲染对象
		*/
		WindowRenderer* create(void);

		/**
		*	销毁图文混排控件渲染对象
		*/
		void destroy(WindowRenderer* wr);
	};

}	//	end of namespace CEGUI


#endif	//	end of guard FalMultiLineBox_H
