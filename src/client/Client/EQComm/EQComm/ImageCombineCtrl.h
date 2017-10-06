/******************************************************************************
* 作者: 何展然
* 时间: 2011-4-25
* 描述: 图像组合控件
******************************************************************************/
#ifndef ImageCombineCtrl_H
#define ImageCombineCtrl_H

#include "EQMainPrerequisites.h"
#include "CEGUI.h"


class ImageCombineCtrl
{
public:
	// 自定义图片格式
	struct ImageDef
	{
		ImageDef(std::string imageset, std::string image, float width, float height)
			:imageset(imageset),image(image),width(width),height(height){}

		std::string imageset;	// imageset名
		std::string image;		// image名
		float width;			// 宽
		float height;			// 高
	};

public:
	ImageCombineCtrl(void);
	~ImageCombineCtrl(void);

	/**
	*	功	能：初始化（最初调用）
	*	参	数：row: 行数，col: 列数，combineCtrl:组合控件名
	*	返回值：-1（错误），0（正确）
	*/
	int init(int row, int col, std::string combineCtrl);

	/**
	*	功	能：增加图像（中间调用，调用次数=初始化时设置的row*col）
	*	参	数：imageset: imagese名，image: 图像名
	*	返回值：0（正确），-1（imageset不存在），-2（image未定义）
	*/
	int addImage(std::string imageset, std::string image);

	/**
	*	功	能：清除组合控件内容
	*	参	数：void
	*	返回值：void
	*/
	void clear(void);

	/**
	*	功	能：设置窗口图片（独立使用，脱离于上面函数）
	*	参	数：winname: 窗口名，imageset: imageset名，image: 图片名
	*	返回值：0（正确），-1（winname不存在），-2（imageset不存在），-3（image不存在），-4（定义图片出错）
	*/
	int setWindowImage(std::string winname, std::string imageset, std::string image);

protected:
	/**
	*	功	能：组合控件
	*	参	数：void
	*	返回值：void
	*/
	void _combine(void);

	/**
	*	功	能：设置窗口图片
	*	参	数：winname: 窗口名，imageset: imageset名，image: 图片名，side: 图片分割的大小
	*	返回值：0（正确），-1（参数有错），-2（定义图片出错）
	*/
	int _setImage(std::string winname, std::string imageset, std::string image, float side);

	/**
	*	功	能：取消设置窗口图片
	*	参	数：winname: 窗口名
	*	返回值：0（正确），-1（winname不存在）
	*/
	int _unsetImage(std::string winname);

	/**
	*	功	能：定义图片
	*	参	数：imageset: 窗口名，image: 图片名，x: 图片x坐标，y: 图片y坐标，w: 图片宽，h: 图片高
	*	返回值：0（正确），-1（参数有错）
	*/
	int _defineImage(std::string imageset, std::string image, float x, float y, float w, float h);

protected:
	std::vector<ImageDef> m_imageVec;		// 组合控件图像集
	std::vector<CEGUI::Window*> m_imageWin;	// 组合控件窗口集
	CEGUI::Window* m_combineCtrl;			// 组合控件
	size_t m_row;							// 组合控件行数
	size_t m_col;							// 组合控件列数

};	//	end of class ImageCombineCtrl

/**
*	功	能：创建图像组合控件对象
*	参	数：void
*	返回值：图像组合控件对象
*/
ImageCombineCtrl createImageCombineCtrl(void);


#endif	//	end of guard ImageCombineCtrl_H
