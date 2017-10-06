/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-11
* ����: ʵ�ֶ����ؼ���Ⱦ
******************************************************************************/
#include "FalagardAnimateBox.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include <Windows.h>


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	��Ⱦ������
	const String FalagardAnimateBox::TypeName(FalagardAnimateBox_TypeName);
	//-------------------------------------------------------------------------
	//	���캯��
	FalagardAnimateBox::FalagardAnimateBox(const String& type) :
		AnimateBoxWindowRenderer(type),
		mIndex(0),
		mTimeStart(0),
		mTimePause(0),
		mTimeCurr(0)
	{
	}
	//-------------------------------------------------------------------------
	//	��Ⱦ����
	void FalagardAnimateBox::render(void)
	{
		AnimateBox* w = (AnimateBox*)d_window;
		assert(NULL != w);
		const WidgetLookFeel& wlf = getLookNFeel();
		cacheAnimateFrame();	//	��Ⱦ����֡
		wlf.getStateImagery(w->isDisabled() ? "Disabled" : "Enabled").render(*w);
	}
	//-------------------------------------------------------------------------
	//	��Ⱦ����֡
	void FalagardAnimateBox::cacheAnimateFrame(void)
	{
		AnimateBox* w = (AnimateBox*)d_window;
		assert(NULL != w);
		if (w->hasAnimate())	//	�ж��������ִ��
		{
			Animate &animate = w->getAnimate();
			int elapsed = animate.getElapsed();
			int timeTotal = animate.getTimeTotal();

			float alpha = w->getEffectiveAlpha();	//	��ȡ����͸����
			ColourRect animateCols;	//	������ɫ����ɫ
			colour aniamteColour(1.0f, 1.0f, 1.0f);
			aniamteColour.setAlpha(aniamteColour.getAlpha() * alpha);
			animateCols.setColours(aniamteColour);

			Rect animateRect;	//	��Ⱦ����
			animateRect.d_left = (w->getWidthFunc()-animate.getWidth())/2.0f;
			animateRect.d_top = (w->getHeightFunc()-animate.getHeight())/2.0f;
			animateRect.d_right = (w->getWidthFunc()-animate.getWidth())/2.0f + animate.getWidth();
			animateRect.d_bottom = (w->getHeightFunc()-animate.getHeight())/2.0f + animate.getHeight();

			switch (w->getPlayState())
			{
			case AnimateBox::PS_PLAY:	//	����
				{
					mTimeStart = w->getTimeStart();
					mTimeCurr = GetTickCount();
					mIndex = ((mTimeCurr-mTimeStart)%timeTotal)/elapsed;

					if (!(w->isLoop()))	//	����ѭ������ģʽ
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
			case AnimateBox::PS_PAUSE:	//	��ͣ
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
			case AnimateBox::PS_STOP:	//	ֹͣ
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
	//	���������ؼ���Ⱦ����
	WindowRenderer* FalagardAnimateBoxWRFactory::create(void)
	{ 
		FalagardAnimateBox *falAniBox = new FalagardAnimateBox(d_factoryName);
		assert(NULL != falAniBox);
		return falAniBox;
	}
	//-------------------------------------------------------------------------
	//	���ٶ����ؼ���Ⱦ����
	void FalagardAnimateBoxWRFactory::destroy(WindowRenderer* wr)
	{ 
		assert(NULL != wr);
		delete wr; 
	}

}	//	end of namespace CEGUI
