/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-4
* 描述: 动画控件源文件，各函数的具体实现
******************************************************************************/
#include "CEGUIAnimate.h"


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	构造函数
	Animate::Animate(void) :
		mElapsed(-1),
		mWidth(-1),
		mHeight(-1)
	{
	}
	//-------------------------------------------------------------------------
	//	析构函数
	Animate::~Animate(void)
	{
	}
	//-------------------------------------------------------------------------
	//	初始化动画对象
	void Animate::initAnimate(int elapsed, float width, float height)
	{
		clearUp();
		mElapsed = elapsed;
		mWidth = width;
		mHeight = height;
	}
	//-------------------------------------------------------------------------
	//	获取播放时间间隔
	int Animate::getElapsed(void) const
	{
		assert(-1 != mElapsed);	//	先调用initAnimate()进行初始化
		return mElapsed;
	}
	//-------------------------------------------------------------------------
	//	获取播放总时间
	int Animate::getTimeTotal(void) const
	{
		assert(0 != mImageVec.size());	//	先调用addAnimageFrame()
		return mTimeTotal;
	}
	//-------------------------------------------------------------------------
	//	获取动画宽度
	float Animate::getWidth(void) const
	{
		assert(-1 != mWidth);	//	先调用initAnimate()进行初始化
		return mWidth;
	}
	//-------------------------------------------------------------------------
	//	获取动画高度
	float Animate::getHeight(void) const
	{
		assert(-1 != mHeight);	//	先调用initAnimate()进行初始化
		return mHeight;
	}
	//-------------------------------------------------------------------------
	//	添加动画帧
	void Animate::addAnimateFrame(Image img)
	{
		mImageVec.push_back(img);
		mTimeTotal = mElapsed * mImageVec.size();
	}
	//-------------------------------------------------------------------------
	//	添加动画帧
	void Animate::addAnimateFrame(std::string imagesetName, std::string imageName)
	{
		Imageset *imgSet = NULL;
		std::string setName = imagesetName + ".imageset";
		if (ImagesetManager::getSingleton().isImagesetPresent(imagesetName))
		{
			imgSet = ImagesetManager::getSingleton().getImageset(imagesetName);
		} 
		else
		{
			imgSet = ImagesetManager::getSingleton().createImageset(setName);
		}
		assert(NULL != imgSet);
		const Image *img = &(imgSet->getImage(imageName));
		assert(NULL != img);
		addAnimateFrame(*img);
	}
	//-------------------------------------------------------------------------
	//	清除动画帧
	void Animate::clearUp(void)
	{
		mImageVec.clear();
		mElapsed = -1;
		mWidth = -1;
		mHeight = -1;
	}
	//-------------------------------------------------------------------------
	//	获取动画帧
	Image& Animate::getAnimateFrame(size_t index)
	{
		assert((index >= 0) && (index < mImageVec.size()));
		return mImageVec[index];
	}
	//-------------------------------------------------------------------------
	//	创建动画对象
	Animate createAnimate(void)
	{
		static Animate ani;	// 比较囧的做法,为了提供给Lua调用
		return ani;
	}

}	//	end of namespace CEGUI
