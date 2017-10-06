/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-4
* 描述: 动画控件头文件，定义了Animate类
******************************************************************************/
#ifndef CEGUIAnimate_H
#define CEGUIAnimate_H

#include "CEGUI.h"


namespace CEGUI
{
	/*************************************************************************
	*	动画控件类
	*************************************************************************/
	class Animate
	{
	public:
		typedef std::vector<Image > ImageVector;	//	定义动画容器

	public:
		/**
		*	构造函数
		*/
		Animate(void);

		/**
		*	析构函数
		*/
		virtual ~Animate(void);

		/**
		*	功	能：获取动画播放时间间隔
		*	参	数：void
		*	返回值：int
		*/
		int getElapsed(void) const;

		/**
		*	功	能：获取动画播放总时间
		*	参	数：void
		*	返回值：int
		*/
		int getTimeTotal(void) const;

		/**
		*	功	能：获取动画宽度
		*	参	数：void
		*	返回值：float
		*/
		float getWidth(void) const;	

		/**
		*	功	能：获取动画高度
		*	参	数：void
		*	返回值：float
		*/
		float getHeight(void) const;

		/**
		*	功	能：增加动画帧
		*	参	数：img:	动画帧
		*	返回值：void
		*/
		void addAnimateFrame(Image img);

		/**
		*	功	能：增加动画帧
		*	参	数：imagesetName:	动画帧所在的iamgeset名称
		*			imageName:	动画帧的标识或名称
		*	返回值：void
		*/
		void addAnimateFrame(std::string imagesetName, std::string imageName);

		/**
		*	功	能：清除动画帧
		*	参	数：void
		*	返回值：void
		*/
		void clearUp(void);

		/**
		*	功	能：根据索引获取动画帧
		*	参	数：index:	索引
		*	返回值：Image&
		*/
		Image& getAnimateFrame(size_t index);

		/**
		*	功	能：初始化动画参数
		*	参	数：elapsed:	动画播放的时间间隔
		*			width:	动画宽度
		*			height:	动画高度
		*	返回值：void
		*/
		void initAnimate(int elapsed, float width, float height);

	protected:
		ImageVector mImageVec;	//	动画帧集合

		int mElapsed;	//	动画播放的时间间隔

		int mTimeTotal;	//	动画播放的总时间

		float mWidth;	//	动画宽度

		float mHeight;	//	动画高度

	};	//	end of class Animate

	/*************************************************************************
	*	CEGUI全局接口
	*************************************************************************/
	/**
	*	功	能：创建动画对象
	*	参	数：void
	*	返回值：动画对象
	*/
	Animate createAnimate(void);

}	//	end of namespace CEGUI


#endif	//	end of guard CEGUIAnimate_H
