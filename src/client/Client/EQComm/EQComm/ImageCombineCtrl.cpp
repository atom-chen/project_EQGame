/******************************************************************************
* 作者: 何展然
* 时间: 2011-4-25
* 描述: 图像组合控件
******************************************************************************/
#include "EQCommPCH.h"
#include "ImageCombineCtrl.h"


//-----------------------------------------------------------------------
ImageCombineCtrl::ImageCombineCtrl(void)
:m_combineCtrl(NULL)
,m_row(0)
,m_col(0)
{
}
//-----------------------------------------------------------------------
ImageCombineCtrl::~ImageCombineCtrl(void)
{
}
//-----------------------------------------------------------------------
int ImageCombineCtrl::init(int row, int col, std::string combineCtrl)
{
	clear();
	m_combineCtrl = CEGUI::WindowManager::getSingleton().getWindow(combineCtrl);
	if (NULL == m_combineCtrl)
	{
		return -1;
	}
	m_row = row;
	m_col = col;
	return 0;
}
//-----------------------------------------------------------------------
int ImageCombineCtrl::addImage(std::string imageset, std::string image)
{
	CEGUI::Imageset* set = CEGUI::ImagesetManager::getSingleton().getImageset(imageset);
	if (NULL == set)
	{
		return -1;
	}
	if (!set->isImageDefined(image))
	{
		return -2;
	}
	ImageDef img(imageset, image, set->getImage(image).getWidth(), set->getImage(image).getHeight());
	m_imageVec.push_back(img);
	_combine();
	return 0;
}
//-----------------------------------------------------------------------
void ImageCombineCtrl::_combine(void)
{
	if ((0==m_row*m_col) || (m_imageVec.empty()) || (m_row*m_col!=m_imageVec.size()))
	{
		return;
	}
	// 计算图片原始宽
	float imageW = 0.0f;
	for (size_t col=0; col<m_col; ++col)	// 列
	{
		imageW += m_imageVec[col].width;
	}
	// 计算图片原始高
	float imageH = 0.0f;
	for (size_t row=0; row<m_row; ++row)	// 行
	{
		imageH += m_imageVec[row*m_col].height;
	}
	// 计算组合控件宽高
	float combineCtrlW = m_combineCtrl->getWidth().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getWidth());
	float combineCtrlH = m_combineCtrl->getHeight().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getHeight());
	// 排列各子控件
	float y = 0.0f;
	for (size_t row=0; row<m_row; ++row)	// 行
	{
		float x = 0.0f;
		for (size_t col=0; col<m_col; ++col)	// 列
		{
			int idx = row*m_col + col;	// 每个分割部分的位置索引
			CEGUI::Window* win = CEGUI::WindowManager::getSingleton().createWindow("EQHomeLook/StaticImage");
			float w = ceil((m_imageVec[idx].width/imageW)*combineCtrlW);
			float h = ceil((m_imageVec[idx].height/imageH)*combineCtrlH);
			win->setMaxSize(CEGUI::UVector2(CEGUI::UDim(0, w), CEGUI::UDim(0, h)));
			win->setSize(CEGUI::UVector2(CEGUI::UDim(0, w), CEGUI::UDim(0, h)));
			win->setPosition(CEGUI::UVector2(CEGUI::UDim(0, x), CEGUI::UDim(0, y)));
			win->setProperty("Image", "set:" + m_imageVec[idx].imageset + " image:" + m_imageVec[idx].image);
			m_combineCtrl->addChildWindow(win);
			m_imageWin.push_back(win);
			x += w;
		}
		y += ceil((m_imageVec[row*m_col].height/imageH)*combineCtrlH);
	}
}
//-----------------------------------------------------------------------
void ImageCombineCtrl::clear(void)
{
	m_row = 0;
	m_col = 0;
	for (size_t i=0; i<m_imageWin.size(); ++i)
	{
		m_combineCtrl->removeChildWindow(m_imageWin[i]);
		CEGUI::WindowManager::getSingleton().destroyWindow(m_imageWin[i]);
	}
	m_imageWin.clear();
	m_imageVec.clear();
	m_combineCtrl = NULL;
}
//-----------------------------------------------------------------------
int ImageCombineCtrl::setWindowImage(std::string winname, std::string imageset, std::string image)
{
	return _setImage(winname, imageset, image, 256);
}
//-----------------------------------------------------------------------
int ImageCombineCtrl::_setImage(std::string winname, std::string imageset, std::string image, float side)
{
	// 参数判断
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	CEGUI::Imageset* set = CEGUI::ImagesetManager::getSingleton().getImageset(imageset);
	if ((NULL==win) || (NULL==set) || (!(set->isImageDefined(image)) || (side<=0)))
	{
		return -1;
	}
	// 窗口基本数据
	_unsetImage(winname);
	float winW = win->getWidth().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getWidth());
	float winH = win->getHeight().asAbsolute(CEGUI::System::getSingleton().getRenderer()->getRect().getHeight());
	// 图像基本数据
	CEGUI::Image img = set->getImage(image);
	size_t row = ceil(img.getHeight()/side);
	size_t col = ceil(img.getWidth()/side);
	// 分割图片
	float childYPos = 0.0f;
	for (size_t i=1; i<=row; ++i)	// 行
	{
		float y = (i-1)*side;	// 纵坐标
		float h = side;	// 行高
		// 最后一行
		if (i == row)
		{
			h = img.getHeight() - (i-1)*side;	// 计算行高
		}
		float childXPos = 0.0f;
		float offsetH = ceil((h/img.getHeight())*winH);
		for (size_t j=1; j<=col; ++j)	// 列
		{
			float x = (j-1)*side;	// 横坐标
			float w = side;	// 列宽
			char name[128] = "";
			_snprintf_s(name, sizeof(name), "%s_%d%d", image.c_str(), i, j);
			// 最后一列
			if (j == col)
			{
				w = img.getWidth() - (j-1)*side;
			}
			if (-1 == _defineImage(imageset, name, img.getSourceTextureArea().d_left+x, img.getSourceTextureArea().d_top+y, w, h))
			{
				return -2;
			}
			CEGUI::Window* childWin = CEGUI::WindowManager::getSingleton().createWindow("EQHomeLook/StaticImage");
			float offsetW = ceil((w/img.getWidth())*winW);
			childWin->setMaxSize(CEGUI::UVector2(CEGUI::UDim(0, offsetW), CEGUI::UDim(0, offsetH)));
			childWin->setSize(CEGUI::UVector2(CEGUI::UDim(0, offsetW), CEGUI::UDim(0, offsetH)));
			childWin->setPosition(CEGUI::UVector2(CEGUI::UDim(0, childXPos), CEGUI::UDim(0, childYPos)));
			childWin->setProperty("Image", "set:" + imageset + " image:" + name);
			win->addChildWindow(childWin);
			childXPos += offsetW;
		}
		childYPos += offsetH;
	}
	return 0;
}
//-----------------------------------------------------------------------
int ImageCombineCtrl::_unsetImage(std::string winname)
{
	CEGUI::Window* win = CEGUI::WindowManager::getSingleton().getWindow(winname);
	if (NULL == win)
	{
		return -1;
	}
	std::vector<CEGUI::Window*> imageWinVec;
	for (size_t i=0; i<win->getChildCount(); ++i)
	{
		CEGUI::Window* childWin = win->getChildAtIdx(i);
		if ("EQHomeLook/StaticImage" == childWin->getType())
		{
			imageWinVec.push_back(childWin);
		}
	}
	for (size_t j=0; j<imageWinVec.size(); ++j)
	{
		win->removeChildWindow(imageWinVec[j]);
		CEGUI::WindowManager::getSingleton().destroyWindow(imageWinVec[j]);
	}
	imageWinVec.clear();
	return 0;
}
//-----------------------------------------------------------------------
int ImageCombineCtrl::_defineImage(std::string imageset, std::string image, float x, float y, float w, float h)
{
	CEGUI::Imageset* set = CEGUI::ImagesetManager::getSingleton().getImageset(imageset);
	if ((NULL==set) || (""==image))
	{
		return -1;
	}
	if (set->isImageDefined(image))
	{
		return 0;
	}
	CEGUI::Point pos(x, y);		// 图片的坐标
	CEGUI::Size sz(w, h);		// 图片的大小
	CEGUI::Point offset(0, 0);	// 偏移量
	set->defineImage(image, pos, sz, offset);
	return 0;
}
//-----------------------------------------------------------------------
ImageCombineCtrl createImageCombineCtrl(void)
{
	static ImageCombineCtrl icc;	// 比较囧的做法,为了提供给Lua调用
	return icc;
}
//-----------------------------------------------------------------------