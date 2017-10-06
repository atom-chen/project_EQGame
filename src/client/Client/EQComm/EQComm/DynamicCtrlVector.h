/******************************************************************************
* 作者: 何展然
* 时间: 2010-9-3
* 描述: 动态控件面板容器
******************************************************************************/
#ifndef DynamicCtrlVector_H
#define DynamicCtrlVector_H

#include "EQMainPrerequisites.h"
#include "CEGUI.h"


/*************************************************************************
*	动态控件面板容器类
*************************************************************************/
class DynamicCtrlVector
{
public:
	//	控件对齐方式
	enum AlignType
	{
		AT_LEFT,	//	左对齐
		AT_CENTER,	//	居中对齐
		AT_RIGHT,	//	右对齐
		RIGHT_SIDE	//	控件在前一个控件右边标志
	};

	// 滚动条显示方式
	enum ScrollbarShowType
	{
		SST_SHOW,	// 高度固定，总是显示
		SST_HIDE,	// 高度不固定，不显示，高度根据内容自动调整
		SST_AUTO	// 高度固定，根据内容自动显示或隐藏
	};

	//	控件对象结构体
	struct ctrlObject
	{
		CEGUI::Window *ctrl;	//	控件对象
		float x;				//	x坐标
		float y;				//	y坐标
		float hor;				//	控件水平前驱间隙
		float ver;				//	控件垂直前驱间隙
		AlignType alignType;	//	控件的水平对齐方式
	};

public:
	/**
	*	构造函数
	*/
	DynamicCtrlVector();

	/**
	*	析构函数
	*/
	~DynamicCtrlVector();

	/**
	*	功	能：初始化动态控件面板容器
	*	参	数：parent:	父窗口
	*	返回值：void
	*/
	void initialise(CEGUI::Window* parent);

	/**
	*	功	能：初始化动态控件面板容器
	*	参	数：parentName:	父窗口名称
	*	返回值：void
	*/
	void initialise(std::string parentName);

	/**
	*	功	能：添加控件
	*	参	数：ctrl:	控件
	*			x:	控件在面板中的x坐标
	*			y:	控件在面板中的y坐标
	*	返回值：void
	*/
	void addControl(CEGUI::Window* ctrl, float x, float y);

	/**
	*	功	能：添加控件
	*	参	数：ctrlName:	控件名称
	*			x:	控件在面板中的x坐标
	*			y:	控件在面板中的y坐标
	*	返回值：void
	*/
	void addControl(std::string ctrlName, float x, float y);

	/**
	*	功	能：添加控件
	*	参	数：ctrl:	控件
	*	返回值：void
	*/
	void addControl(CEGUI::Window* ctrl);

	/**
	*	功	能：添加控件
	*	参	数：ctrlName:	控件名称
	*	返回值：void
	*/
	void addControl(std::string ctrlName);

	/**
	*	功	能：设置滚动条显示方式
	*	参	数：showType:	滚动条显示方式（SST_SHOW；SST_HIDE；SST_AUTO）
	*	返回值：void
	*/
	void setScrollbarVisible(std::string showType);

	/**
	*	功	能：设置滚动条位置
	*	参	数：pos:	位置
	*	返回值：void
	*/
	void setScrollbarPosition(float pos);

	/**
	*	功	能：设置控件到其他控件和边框的水平，垂直间隙
	*	参	数：val:	间隙
	*	返回值：void
	*/
	void setInterval(float val);

	/**
	*	功	能：设置控件的水平前驱间隙
	*	参	数：hor:	间隙
	*	返回值：void
	*/
	void setHorInterval(float hor);

	/**
	*	功	能：设置控件的垂直前驱间隙
	*	参	数：ver:	间隙
	*	返回值：void
	*/
	void setVerInterval(float ver);

	/**
	*	功	能：设置控件的对齐方式
	*	参	数：alignType:	对齐方式（AT_LEFT，左对齐；AT_CENTER，居中对齐；AT_RIGHT，右对齐）	
	*	返回值：0（设置成功），-1（设置失败）
	*/
	int setAlign(std::string alignType);

	/**
	*	功	能：设置控件在前一个控件的右边
	*	参	数：void	
	*	返回值：void
	*/
	void setAtRight(void);

	/**
	*	功	能：获得面板垂直滚动条的宽度
	*	参	数：void	
	*	返回值：float
	*/
	float getScrollbarWidth(void);

	/**
	*	功	能：移除动态控件面板容器里的所有控件
	*	参	数：void
	*	返回值：void
	*/
	void removeAllControl(void);

	/**
	*	功	能：控件排版（最后调用）
	*	参	数：void
	*	返回值：void
	*/
	void controlTypeset(void);

protected:
	float mScreenWidth;			//	屏幕宽度

	float mScreenHeight;		//	屏幕高度

	std::vector<ctrlObject> mCtrlVector;	//	控件容器

	CEGUI::Window *mRoot;		//	父窗口

	CEGUI::ScrollablePane *mPane;	//	面板

	ScrollbarShowType mScrollbarShowType;	// 滚动条显示方式

	float mInterval;			//	间隙

	float mHorInterval;			//	控件水平前驱间隙

	float mVerInterval;			//	控件垂直前驱间隙

	AlignType mAlignType;		//	控件位置

};	//	end of class DynamicCtrlVector

/**
*	功	能：创建动态控件面板容器对象（比较囧的做法,为了提供给Lua调用）
*	参	数：void
*	返回值：动态控件面板容器对象
*/
DynamicCtrlVector createDynamicCtrlVector(void);


#endif	//	end of guard DynamicCtrlVector_H
