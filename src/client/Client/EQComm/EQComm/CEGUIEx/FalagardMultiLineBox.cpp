/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-7-1
* ����: ʵ��ͼ�Ļ��ſؼ�����Ⱦ
******************************************************************************/
#include "FalagardMultiLineBox.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"

#define LINE_GAP	3.0f	// ÿ�м���м��
#define ELEMENT_GAP	1.0f	// ͬһ����Ԫ�صļ��


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	��Ⱦ������
	const String FalagardMultiLineBox::TypeName(FalagardMultiLineBox_TypeName);
	//-------------------------------------------------------------------------
	//	���캯��
	FalagardMultiLineBox::FalagardMultiLineBox(const String& type) :
		MultiLineBoxWindowRenderer(type)
	{
	}
	//-------------------------------------------------------------------------
	//	��ȡͼ�Ļ��ſؼ���Ⱦ����
	Rect FalagardMultiLineBox::getRenderArea(void) const
	{
		Rect renderArea(_getNameArea());
		// ������ĸ�ֵ���������壬��Ҫ�ı�
		renderArea.d_top += 1.0f;
		renderArea.d_bottom += 1.0f;
		renderArea.d_left += 0.55f;
		renderArea.d_right += 0.55f;
		return renderArea;
	}
	//-------------------------------------------------------------------------
	//	��Ⱦ����
	void FalagardMultiLineBox::render(void)
	{
		MultiLineBox* w = (MultiLineBox*)d_window;
		_cacheElementLines(getRenderArea());	// ��Ⱦ�����ı�������
		getLookNFeel().getStateImagery(w->isDisabled() ? "Disabled" : "Enabled").render(*w);
	}
	//-------------------------------------------------------------------------
	//	��ȡ��������
	Rect FalagardMultiLineBox::_getNameArea(void) const
	{
		MultiLineBox* w = (MultiLineBox*)d_window;
		const WidgetLookFeel& wlf = getLookNFeel();
		// ��ȡ�й�����������
		if (w->isScrollbarVisible())
		{
			String area_name("TextAreaVScroll");

			if (wlf.isNamedAreaDefined(area_name))
				return wlf.getNamedArea(area_name).getArea().getPixelRect(*w);
		}
		// ��ȡû�й�����������
		return wlf.getNamedArea("TextArea").getArea().getPixelRect(*w);
	}
	//-------------------------------------------------------------------------
	//	��Ⱦͼ�Ļ��ſؼ�����Ϣ
	void FalagardMultiLineBox::_cacheElementLines(const Rect& destArea)
	{
		MultiLineBox* w = (MultiLineBox*)d_window;
		//	�����ı������Ѿ��Ű���ˣ�����ֻ��Ҫ��ȡÿ����Ϣ����Ⱦ����
		Rect drawArea(destArea);
		float curScrollbarPos = w->getVertScrollbar()->getScrollPosition();
		//	��ȡ����͸����
		float alpha = w->getEffectiveAlpha();
		//	������Ⱦ����ɫ
		ColourRect textCols;	//	�ı���ɫ
		ColourRect sideCols;	//	�����ɫ
		float		gap;		//	��߼��
		ColourRect imageCols;	//	ͼ�񱳾���ɫ����ɫ
		colour imageColour(1.0f, 1.0f, 1.0f);
		imageColour.setAlpha(imageColour.getAlpha() * alpha);
		imageCols.setColours(imageColour);

		Renderer* renderer = System::getSingleton().getRenderer();
		float imageZ  = renderer->getZLayer(1) - renderer->getCurrentZ();	//	��Ⱦͼ���
		
		// �жϴ�ֱ���뷽ʽ������������Ⱦ��������������ı�ǰ�棩
		switch (w->getAlignVer())
		{
		case MultiLineBox::AT_TOP:
			drawArea.d_top = destArea.d_top;
			break;
		case MultiLineBox::AT_CENTER_VER:
			drawArea.d_top = destArea.d_top + (destArea.d_bottom - destArea.d_top - w->getFormatHeight())/2;
			break;
		case MultiLineBox::AT_BOTTOM:
			drawArea.d_top = destArea.d_top + (destArea.d_bottom - destArea.d_top - w->getFormatHeight());
			break;
		}

		// ������Ⱦ��������������ı䣨�����жϴ�ֱ���뷽ʽ���棩
		float lineHeight = w->getEachLineHeight();
		if (0.0f == lineHeight)	// ÿ��ͬ�߶�
		{
			drawArea.offset(Point(0.0f, -curScrollbarPos));	//	��Ⱦ��������
		} 
		else	// ÿ�в�ͬ�߶�
		{
			float pos = 0.0f;
			while (1)
			{
				if (pos >= curScrollbarPos)
					break;
				pos += lineHeight + LINE_GAP;
			}
			drawArea.offset(Point(0.0f, -pos));	//	��Ⱦ��������
		}

		//	ѭ����Ⱦÿ����Ϣ
		MultiLineBox::LineList d_lines = w->getLineList();	//	��ȡ����Ϣ����
		for (size_t i=0; i<d_lines.size(); ++i)
		{
			MultiLineBox::Line currLine = d_lines[i];	//	��ȡÿ����Ϣ

			// �ж�ˮƽ���뷽ʽ
			switch (w->getAlignHor())
			{
			case MultiLineBox::AT_LEFT:
				drawArea.d_left = destArea.d_left;
				break;
			case MultiLineBox::AT_CENTER_HOR:
				drawArea.d_left = destArea.d_left + (destArea.d_right - destArea.d_left - currLine.width)/2;
				break;
			case MultiLineBox::AT_RIGHT:
				drawArea.d_left = destArea.d_left + (destArea.d_right - destArea.d_left - currLine.width);
				break;
			}

			//	ѭ����Ⱦÿ��ÿ��Ԫ��
			for (size_t j=0; j<currLine.element.size(); j++)
			{
				MultiLineBox::Element *element = currLine.element[j];	//	��ȡÿ��Ԫ��

				//	�ж�Ԫ������
				switch (element->type)
				{
				case MultiLineBox::ET_TEXT:	//	�ı�
					{
						MultiLineBox::Element_Text *elementText = (MultiLineBox::Element_Text*)element;
						Font *fnt = elementText->font;
						String &text = elementText->text;
						colour textColour = elementText->textCols;
						textColour.setAlpha(textColour.getAlpha() * alpha);
						textCols.setColours(textColour);

						Rect textRect(drawArea);
						if (0.0f == lineHeight)
							textRect.d_top = drawArea.d_top + currLine.height - elementText->height;
						else
							textRect.d_top = drawArea.d_top + lineHeight - elementText->height;

						//	�����Ⱦ
						if (elementText->sideFlag)
						{
							colour sideColour = elementText->sideCols;
							sideColour.setAlpha(sideColour.getAlpha() * alpha);
							sideCols.setColours(sideColour);
							gap = elementText->sidePix;

							Rect sideRectU(textRect);	//	����
							sideRectU.d_top -= gap;
							sideRectU.d_bottom -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectU, 0.0f, sideCols);
							Rect sideRectD(textRect);	//	����
							sideRectD.d_top += gap;
							sideRectD.d_bottom += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectD, 0.0f, sideCols);
							Rect sideRectL(textRect);	//	����
							sideRectL.d_left -= gap;
							sideRectL.d_right -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectL, 0.0f, sideCols);
							Rect sideRectR(textRect);	//	����
							sideRectR.d_left += gap;
							sideRectR.d_right += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectR, 0.0f, sideCols);	
							Rect sideRectLU(textRect);	//	����
							sideRectLU.d_left -= gap;
							sideRectLU.d_right -= gap;
							sideRectLU.d_top -= gap;
							sideRectLU.d_bottom -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectLU, 0.0f, sideCols);
							Rect sideRectRU(textRect);	//	����
							sideRectRU.d_left += gap;
							sideRectRU.d_right += gap;
							sideRectRU.d_top -= gap;
							sideRectRU.d_bottom -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectRU, 0.0f, sideCols);
							Rect sideRectLD(textRect);	//	����
							sideRectLD.d_left -= gap;
							sideRectLD.d_right -= gap;
							sideRectLD.d_top += gap;
							sideRectLD.d_bottom += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectLD, 0.0f, sideCols);
							Rect sideRectRD(textRect);	//	����
							sideRectRD.d_left += gap;
							sideRectRD.d_right += gap;
							sideRectRD.d_top += gap;
							sideRectRD.d_bottom += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectRD, 0.0f, sideCols);

							if (gap > 1.0f)	//	����1������ʱ
							{
								Rect sideRectU(textRect);	//	����
								sideRectU.d_top -= 1.0f;
								sideRectU.d_bottom -= 1.0f;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectU, 0.0f, sideCols);
								Rect sideRectD(textRect);	//	����
								sideRectD.d_top += 1.0f;
								sideRectD.d_bottom += 1.0f;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectD, 0.0f, sideCols);
								Rect sideRectL1(textRect);	//	����1
								sideRectL1.d_left -= gap;
								sideRectL1.d_right -= gap;
								sideRectL1.d_top -= 1.0f;
								sideRectL1.d_bottom -= 1.0f; 
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectL1, 0.0f, sideCols);
								Rect sideRectL2(textRect);	//	����2
								sideRectL2.d_left -= gap;
								sideRectL2.d_right -= gap;
								sideRectL2.d_top += 1.0f;
								sideRectL2.d_bottom += 1.0f; 
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectL2, 0.0f, sideCols);
								Rect sideRectR1(textRect);	//	����1
								sideRectR1.d_left += gap;
								sideRectR1.d_right += gap;
								sideRectR1.d_top -= 1.0f;
								sideRectR1.d_bottom -= 1.0f;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectR1, 0.0f, sideCols);
								Rect sideRectR2(textRect);	//	����2
								sideRectR2.d_left += gap;
								sideRectR2.d_right += gap;
								sideRectR2.d_top += 1.0f;
								sideRectR2.d_bottom += 1.0f;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectR2, 0.0f, sideCols);
								Rect sideRectLUM(textRect);	//	����
								sideRectLUM.d_left -= 1.0f;
								sideRectLUM.d_right -= 1.0f;
								sideRectLUM.d_top -= gap;
								sideRectLUM.d_bottom -= gap;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectLUM, 0.0f, sideCols);
								Rect sideRectRUM(textRect);	//	����
								sideRectRUM.d_left += 1.0f;
								sideRectRUM.d_right += 1.0f;
								sideRectRUM.d_top -= gap;
								sideRectRUM.d_bottom -= gap;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectRUM, 0.0f, sideCols);
								Rect sideRectLDM(textRect);	//	����
								sideRectLDM.d_left -= 1.0f;
								sideRectLDM.d_right -= 1.0f;
								sideRectLDM.d_top += gap;
								sideRectLDM.d_bottom += gap;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectLDM, 0.0f, sideCols);
								Rect sideRectRDM(textRect);	//	����
								sideRectRDM.d_left += 1.0f;
								sideRectRDM.d_right += 1.0f;
								sideRectRDM.d_top += gap;
								sideRectRDM.d_bottom += gap;
								w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectRDM, 0.0f, sideCols);
							}
						}
						//	�ı���Ⱦ
						w->getRenderCache().cacheText(text, fnt, LeftAligned, textRect, 0.0f, textCols);
						drawArea.d_left += fnt->getTextExtent(text) + ELEMENT_GAP;
					}
					break;
				case MultiLineBox::ET_IMAGE:	//	ͼ��
					{
						MultiLineBox::Element_Image *elementImage = (MultiLineBox::Element_Image*)element;
						Image &img = elementImage->image;

						Rect imageRect(drawArea);
						if (0.0f == lineHeight)
							imageRect.d_top = drawArea.d_top + currLine.height - elementImage->height;
						else
							imageRect.d_top = drawArea.d_top + lineHeight -elementImage->height;

						imageRect.d_right = imageRect.d_left + elementImage->width;
						imageRect.d_bottom = imageRect.d_top + elementImage->height;
						w->getRenderCache().cacheImage(img, imageRect, imageZ, imageCols);
						drawArea.d_left += elementImage->width + ELEMENT_GAP;
					}
					break;
				case MultiLineBox::ET_HYPERLINK:	//	������
					{
						MultiLineBox::Element_HyperLink *elementHyperLink = (MultiLineBox::Element_HyperLink*)element;
						Font *fnt = elementHyperLink->font;
						String &text = elementHyperLink->text;
						colour textColour = elementHyperLink->textCols;
						textColour.setAlpha(textColour.getAlpha() * alpha);
						textCols.setColours(textColour);

						Rect textRect(drawArea);
						if (0.0f == lineHeight)
							textRect.d_top = drawArea.d_top + currLine.height - elementHyperLink->height;
						else
							textRect.d_top = drawArea.d_top + lineHeight - elementHyperLink->height;

						//	�����Ⱦ
						if (elementHyperLink->sideFlag)
						{
							colour sideColour = elementHyperLink->sideCols;
							sideColour.setAlpha(sideColour.getAlpha() * alpha);
							sideCols.setColours(sideColour);
							gap = elementHyperLink->sidePix;

							Rect sideRectU(textRect);	//	����
							sideRectU.d_top -= gap;
							sideRectU.d_bottom -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectU, 0.0f, sideCols);
							Rect sideRectD(textRect);	//	����
							sideRectD.d_top += gap;
							sideRectD.d_bottom += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectD, 0.0f, sideCols);
							Rect sideRectL(textRect);	//	����
							sideRectL.d_left -= gap;
							sideRectL.d_right -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectL, 0.0f, sideCols);
							Rect sideRectR(textRect);	//	����
							sideRectR.d_left += gap;
							sideRectR.d_right += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectR, 0.0f, sideCols);	
							Rect sideRectLU(textRect);	//	����
							sideRectLU.d_left -= gap;
							sideRectLU.d_right -= gap;
							sideRectLU.d_top -= gap;
							sideRectLU.d_bottom -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectLU, 0.0f, sideCols);
							Rect sideRectRU(textRect);	//	����
							sideRectRU.d_left += gap;
							sideRectRU.d_right += gap;
							sideRectRU.d_top -= gap;
							sideRectRU.d_bottom -= gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectRU, 0.0f, sideCols);
							Rect sideRectLD(textRect);	//	����
							sideRectLD.d_left -= gap;
							sideRectLD.d_right -= gap;
							sideRectLD.d_top += gap;
							sideRectLD.d_bottom += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectLD, 0.0f, sideCols);
							Rect sideRectRD(textRect);	//	����
							sideRectRD.d_left += gap;
							sideRectRD.d_right += gap;
							sideRectRD.d_top += gap;
							sideRectRD.d_bottom += gap;
							w->getRenderCache().cacheText(text, fnt, LeftAligned, sideRectRD, 0.0f, sideCols);
						}
						//	�ı���Ⱦ
						w->getRenderCache().cacheText(text, fnt, LeftAligned, textRect, 0.0f, textCols);

						Rect lineRect(textRect);
						String line = (utf8*)"";
						for (size_t i=0; i<elementHyperLink->width/fnt->getTextExtent("_"); i++)
						{
							line += (utf8*)"_";
						}
						//	�»��������Ⱦ
						if (elementHyperLink->sideFlag)
						{
							Rect sideRectU(lineRect);
							sideRectU.d_top -= gap;
							sideRectU.d_bottom -= gap;
							w->getRenderCache().cacheText(line, fnt, LeftAligned, sideRectU, 0.0f, sideCols);
							Rect sideRectD(lineRect);
							sideRectD.d_top += gap;
							sideRectD.d_bottom += gap;
							w->getRenderCache().cacheText(line, fnt, LeftAligned, sideRectD, 0.0f, sideCols);
							Rect sideRectL(lineRect);
							sideRectL.d_left -= gap;
							sideRectL.d_right -= gap;
							w->getRenderCache().cacheText(line, fnt, LeftAligned, sideRectL, 0.0f, sideCols);
							Rect sideRectR(lineRect);
							sideRectR.d_left += gap;
							sideRectR.d_right += gap;
							w->getRenderCache().cacheText(line, fnt, LeftAligned, sideRectR, 0.0f, sideCols);
						}
						//	�»�����Ⱦ
						w->getRenderCache().cacheText(line, fnt, LeftAligned, lineRect, 0.0f, textCols);
						Rect lineRectL(lineRect);
						lineRectL.d_left -= 0.5f;
						lineRectL.d_right -= 0.5f;
						w->getRenderCache().cacheText(line, fnt, LeftAligned, lineRectL, 0.0f, textCols);
						Rect lineRectD(lineRect);
						lineRectD.d_top += 0.3f;
						lineRectD.d_bottom += 0.3f;
						w->getRenderCache().cacheText(line, fnt, LeftAligned, lineRectD, 0.0f, textCols);
						Rect lineRectDL(lineRectD);
						lineRectDL.d_left -= 0.5f;
						lineRectDL.d_right -= 0.5f;
						w->getRenderCache().cacheText(line, fnt, LeftAligned, lineRectDL, 0.0f, textCols);
						drawArea.d_left += elementHyperLink->width + ELEMENT_GAP;
					}
					break;
				case MultiLineBox::ET_WIN:	//	�ؼ�
					{
						MultiLineBox::Element_Win *elementWin = (MultiLineBox::Element_Win*)element;
						Window *win = elementWin->win;

						float winX = drawArea.d_left;
						float winY = 0.0f;
						if (0.0f == lineHeight)
							winY = drawArea.d_top + currLine.height - elementWin->height;
						else
							winY = drawArea.d_top + lineHeight - elementWin->height;

						win->setPosition(UVector2(UDim(0.0f, winX), UDim(0.0f, winY)));
						drawArea.d_left += elementWin->width + ELEMENT_GAP;
					}
					break;
				}

			}	//	end of loop for

			currLine.element.clear();	//	�����Ԫ��
			//	����
			if (0.0f == lineHeight)
				drawArea.d_top += currLine.height + LINE_GAP;
			else
				drawArea.d_top += lineHeight + LINE_GAP;
		}	//	end of loop for

	}	//	end of function cacheElementLines
	//-------------------------------------------------------------------------
	//	����ͼ�Ļ��ſؼ���Ⱦ����
	WindowRenderer* FalagardMultiLineBoxWRFactory::create(void)
	{
		FalagardMultiLineBox *falMLB = new FalagardMultiLineBox(d_factoryName);
		return falMLB;
	}
	//-------------------------------------------------------------------------
	//	����ͼ�Ļ��ſؼ���Ⱦ����
	void FalagardMultiLineBoxWRFactory::destroy(WindowRenderer* wr)
	{ 
		delete wr; 
	}

}	//	end of namespace CEGUI
