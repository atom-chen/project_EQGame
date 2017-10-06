/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-11
* 描述: 实现动画控件渲染
******************************************************************************/
#include "FalagardAnimateBox.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include <Windows.h>


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	渲染类名称
	const String FalagardAnimateBox::TypeName(FalagardAnimateBox_TypeName);
	//-------------------------------------------------------------------------
	//	构造函数
	FalagardAnimateBox::FalagardAnimateBox(const String& type) :
		AnimateBoxWindowRenderer(type),
		mIndex(0),
		mTimeStart(0),
		mTimePause(0),
		mTimeCurr(0)
	{
	}
	//-------------------------------------------------------------------------
	//	渲染函数
	void FalagardAnimateBox::render(void)
	{
		AnimateBox* w = (AnimateBox*)d_window;
		assert(NULL != w);
		const WidgetLookFeel& wlf = getLookNFeel();
		cacheAnimateFrame();	//	渲染动画帧
		wlf.getStateImagery(w->isDisabled() ? "Disabled" : "Enabled").render(*w);
	}
	//-------------------------------------------------------------------------
	//	渲染动画帧
	void FalagardAnimateBox::cacheAnimateFrame(void)
	{
		AnimateBox* w = (AnimateBox*)d_window;
		assert(NULL != w);
		if (w->hasAnimate())	//	有动画对象才执行
		{
			Animate &animate = w->getAnimate();
			int elapsed = animate.getElapsed();
			int timeTotal = animate.getTimeTotal();

			float alpha = w->getEffectiveAlpha();	//	获取窗体透明度
			ColourRect animateCols;	//	背景颜色、白色
			colour aniamteColour(1.0f, 1.0f, 1.0f);
			aniamteColour.setAlpha(aniamteColour.getAlpha() * alpha);
			animateCols.setColours(aniamteColour);

			Rect animateRect;	//	渲染区域
			animateRect.d_left = (w->getWidthFunc()-animate.getWidth())/2.0f;
			animateRect.d_top = (w->getHeightFunc()-animate.getHeight())/2.0f;
			animateRect.d_right = (w->getWidthFunc()-animate.getWidth())/2.0f + animate.getWidth();
			animateRect.d_bottom = (w->getHeightFunc()-animate.getHeight())/2.0f + animate.getHeight();

			switch (w->getPlayState())
			{
			case AnimateBox::PS_PLAY:	//	播放
				{
					mTimeStart = w->getTimeStart();
					mTimeCurr = GetTickCount();
					mIndex = ((mTimeCurr-mTimeStart)%timeTotal)/elapsed;

					if (!(w->isLoop()))	//	不是循环播放模式
					{
						if (mTimeStart+timeTotal <= mTimeCurr)
						{
							mIndex = 0;
							w->firePlayOver();
							return;
						}
					} 
				}
				break;
			case AnimateBox::PS_PAUSE:	//	暂停
				{
					mTimePause = w->getTimePause();
					if (0 == mTimeStart)
					{
						mIndex = 0;
					}
					else
					{
						mIndex = ((mTimePause-mTimeStart)%timeTotal)/elapsed;
					}
				}
				break;
			case AnimateBox::PS_STOP:	//	停止
				{
					mTimePause = 0;
					mIndex = 0;
				}
				break;
			};

			Image& img = animate.getAnimateFrame(mIndex);
			w->getRenderCache().cacheImage(img, animateRect, 0.0f, animateCols);
		}

	}	//	end of function cacheChatLines
	//-------------------------------------------------------------------------
	//	创建动画控件渲染对象
	WindowRenderer* FalagardAnimateBoxWRFactory::create(void)
	{ 
		FalagardAnimateBox *falAniBox = new FalagardAnimateBox(d_factoryName);
		assert(NULL != falAniBox);
		return falAniBox;
	}
	//-------------------------------------------------------------------------
	//	销毁动画控件渲染对象
	void FalagardAnimateBoxWRFactory::destroy(WindowRenderer* wr)
	{ 
		assert(NULL != wr);
		delete wr; 
	}

}	//	end of namespace CEGUI
