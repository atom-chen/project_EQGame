/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-7-1
* ����: ͼ�Ļ��ſؼ�
******************************************************************************/
#include "CEGUIMultiLineBox.h"
//#include <ShellAPI.h>

#define LINE_GAP	3.0f	// ÿ�м���м��
#define ELEMENT_GAP	1.0f	// ͬһ����Ԫ�صļ��


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	���캯��
	MultiLineBoxWindowRenderer::MultiLineBoxWindowRenderer(const String& name) :
	WindowRenderer(name, MultiLineBox_EventNamespace)
	{
	}
	//-------------------------------------------------------------------------
	//	ȫ���¼�����
	const String MultiLineBox::EventNamespace(MultiLineBox_EventNamespace);
	//-------------------------------------------------------------------------
	//	������������
	const String MultiLineBox::WidgetTypeName(MultiLineBox_WidgetTypeName);
	//-------------------------------------------------------------------------
	//	�¼����ƣ������������λ�ã�
	const String MultiLineBox::HyperLink_MouseClick("HyperLink_MouseClick");
	//-------------------------------------------------------------------------
	//	�¼����ƣ�����ƽ�������λ�ã�
	const String MultiLineBox::HyperLink_MouseEnter("HyperLink_MouseEnter");
	//-------------------------------------------------------------------------
	//	�¼����ƣ�����Ƴ�������λ�ã�
	const String MultiLineBox::HyperLink_MouseLeave("HyperLink_MouseLeave");
	//-------------------------------------------------------------------------
	//	�¼����ƣ����������λ�ã�
	const String MultiLineBox::Text_MouseClick("Text_MouseClick");
	//-------------------------------------------------------------------------
	//	�¼����ƣ�����ƽ�����λ�ã�
	const String MultiLineBox::Text_MouseEnter("Text_MouseEnter");
	//-------------------------------------------------------------------------
	//	�¼����ƣ�����Ƴ�����λ�ã�
	const String MultiLineBox::Text_MouseLeave("Text_MouseLeave");
	//-------------------------------------------------------------------------
	//	�Ӳ������ƣ���ֱ������������
	const String MultiLineBox::VertScrollbarNameSuffix( "__auto_vscrollbar__" );
	//-------------------------------------------------------------------------
	//	���캯��
	MultiLineBox::MultiLineBox(const String& type, const String& name) :
	Window(type, name),
	mLength(0.0f),
	mEachLineHeight(0.0f),
	mTextFont(NULL),
	mTextFontName(""),
	mTextColsName(""),
	mColsFlag(false),
	mSidePix(1.0f),
	mSideFlag(false),
	mAlignHor(AT_LEFT),
	mAlignVer(AT_TOP),
	mScrollbarVisible(false),
	mScrollMode(0),
	mScrollbarPos(SP_TOP),
	mHyperLinkId(""),
	mTextContent(""),
	mTriggerRect("0-0-0-0"),
	mSpecSidePix(1.0f),
	mSpecSideFlag(false)
	{
		mScreenWidth = CEGUI::System::getSingleton().getRenderer()->getRect().getWidth();
		mScreenHeight = CEGUI::System::getSingleton().getRenderer()->getRect().getHeight();
	}
	//-------------------------------------------------------------------------
	//	��������
	MultiLineBox::~MultiLineBox(void)
	{
		_destroyLineElement();
		_destroyAllElements();
	}
	//-------------------------------------------------------------------------
	//	��ʼ������
	void MultiLineBox::initialiseComponents(void)
	{
		setSize(UVector2(UDim(0.0f, 100), UDim(0.0f, 50)));	//	��ʼ�ؼ���С
		Scrollbar* vertScrollbar = getVertScrollbar();
		vertScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&MultiLineBox::handle_scrollChange, this));
		_formatElement();
		performChildWindowLayout();
	}
	//-------------------------------------------------------------------------
	//	���ù�����
	void MultiLineBox::_configureScrollbars(void)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		float totalHeight = _getTotalHeight();

		//	��ʾ�����ع�����
		if (0 == mScrollMode)	// ����������ʾ������
		{
			if (mScrollbarVisible && (totalHeight > getRenderArea().getHeight()))
				vertScrollbar->show();
			else
				vertScrollbar->hide();
		}
		else if (1 == mScrollMode)	// ������ʾ������
		{
			if (mScrollbarVisible)
				vertScrollbar->show();
			else
				vertScrollbar->hide();
		}

		Rect renderArea(getRenderArea());
		//	���ù�������ֵ
		vertScrollbar->setDocumentSize(totalHeight);
		vertScrollbar->setPageSize(renderArea.getHeight());
		float steps = 0.0f;
		if (0.0f == mEachLineHeight)
			steps = 1.0f > (renderArea.getHeight()/10.0f) ? 1.0f : (renderArea.getHeight()/10.0f);
		else
			steps = mEachLineHeight + LINE_GAP;
		vertScrollbar->setStepSize(steps);

		switch (mScrollbarPos)
		{
		case SP_TOP:	//	������λ����������
			vertScrollbar->setScrollPosition(0);
			break;
		case SP_OLD:	//	�������ھɵ�λ��
			vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition());
			break;
		case SP_BOTTOM:	//	������λ����������
			vertScrollbar->setScrollPosition(vertScrollbar->getDocumentSize());
			break;
		}
		requestRedraw();	//	�ػ洰��
	}
	//-------------------------------------------------------------------------
	//	��ͼ�Ļ��ſؼ����ݽ����Ű�
	void MultiLineBox::_formatElement(void)
	{
		if (mElementList.empty())
		{
			_configureScrollbars();
			return;
		}
		
		//	�����������Ϣ
		_destroyLineElement();
		float pAreaWidth = getRenderArea().getWidth();	//	��Ⱦ����Ŀ��
		Line pLine;				//	һ��
		pLine.width = 0.0f;
		pLine.height = 0.0f;
		int pLineNum = 0;		//	�кų�ʼΪ0
		float pCurrPos = 0.0f;	//	Ԫ���������е�λ��

		//	ѭ�����������ı�����Ԫ��
		for (size_t i=0; i<mElementList.size(); i++)
		{
			Element *element = mElementList[i];
			if (element->newLine)
			{
				pCurrPos = 0.0f;
				pLine.width = 0.0f;
				pLine.height = 0.0f;
			}

			//	�ж�Ԫ������
			switch (element->type)
			{
			case ET_TEXT:	//	�ı�
				{
					Element_Text *elementText = (Element_Text*)element;
					int startPos = 0;	//	�ַ����еĽ�ȡ��ʼλ��
					int endPos = 0;		//	�ַ����еĽ�ȡ����λ��

					if (elementText->width <= pAreaWidth-pCurrPos)	//	Ԫ�س���С��������п�
					{	
						Element_Text *line_text = new Element_Text;
						line_text->type = elementText->type;
						line_text->font = elementText->font;
						line_text->width = elementText->width;
						line_text->height = elementText->height;
						line_text->index = elementText->index;
						line_text->newLine = elementText->newLine;
						line_text->text = elementText->text;
						line_text->textCols = elementText->textCols;
						line_text->sideFlag = elementText->sideFlag;
						if (line_text->sideFlag)
						{
							line_text->sideCols = elementText->sideCols;
							line_text->sidePix = elementText->sidePix;
						}

						if (0 == pLineNum)	//	Ԫ��������
						{
							pLine.element.push_back(line_text);
							if (pLine.height < line_text->height)
							{
								pLine.height = line_text->height;
							}
							pLine.width += line_text->width;
							mLines.push_back(pLine);	//	������Ϣ���ϲ���һ��
							pLineNum++;					//	�к�����
						}
						else	//	�������У�������
						{
							if (line_text->newLine)	//	Ԫ����ǿ������һ�б�־
							{
								pLine.element.clear();
								pLine.element.push_back(line_text);
								pLine.height = line_text->height;
								pLine.width += line_text->width;
								mLines.push_back(pLine);
								pLineNum++;
							}
							else
							{
								mLines[pLineNum-1].element.push_back(line_text);
								if (mLines[pLineNum-1].height < line_text->height)
								{
									mLines[pLineNum-1].height = line_text->height;
								}
								mLines[pLineNum-1].width += line_text->width; 
							}
						}
						pCurrPos += line_text->width + ELEMENT_GAP;	//	��ǰλ�øı�
					}
					else	//	��Ԫ�س��ȴ�������п�
					{ 
						//	��ȡ�ַ���ǰ��
						endPos = elementText->font->getCharAtPixel(elementText->text, pAreaWidth-pCurrPos);	//	����ַ�����ȡ����λ��
						String text = elementText->text.substr(startPos, endPos);
						startPos = endPos;	//	�ַ�����ȡ��ʼλ�øı�
						Element_Text *line_text_head = new Element_Text;
						line_text_head->type = elementText->type;
						line_text_head->font = elementText->font;
						line_text_head->width = elementText->font->getTextExtent(text);
						line_text_head->height = elementText->height;
						line_text_head->index = elementText->index;
						line_text_head->newLine = elementText->newLine;
						line_text_head->text = text;
						line_text_head->textCols = elementText->textCols;
						line_text_head->sideFlag = elementText->sideFlag;
						if (line_text_head->sideFlag)
						{
							line_text_head->sideCols = elementText->sideCols;
							line_text_head->sidePix = elementText->sidePix;
						}

						if (text.length() > 0)
						{
							if (pLineNum < 1)	//	��ǰ��Ϊ��һ��
							{
								pLine.element.push_back(line_text_head);
								if (pLine.height < line_text_head->height)
								{
									pLine.height = line_text_head->height;
								}
								pLine.width += line_text_head->width;
								mLines.push_back(pLine);	//	����µ�һ��
								pLineNum++;	//	�к�����
							}
							else	//	��ǰ�в��ǵ�һ��
							{
								if (line_text_head->newLine)	//	����
								{
									pLine.element.clear();
									pLine.element.push_back(line_text_head);
									pLine.height = line_text_head->height;
									pLine.width += line_text_head->width;
									mLines.push_back(pLine);
									pLineNum++;
								}
								else
								{
									mLines[pLineNum-1].element.push_back(line_text_head);	//	�ѽ�ȡ���ַ�����ӵ���һ��
									if (mLines[pLineNum-1].height < line_text_head->height)
									{
										mLines[pLineNum-1].height = line_text_head->height;
									}
									mLines[pLineNum-1].width += line_text_head->width;
								}
							}
						}

						pLine.element.clear();
						pLine.width = 0.0f;
						pLine.height = 0.0f;

						//	��ȡ�ַ����в�
						text = elementText->text.substr(startPos, elementText->text.length()-startPos);	//	��ȡʣ�µ��ַ���
						int lineCount = (int)(elementText->font->getTextExtent(text)/pAreaWidth);	//	������
						for (size_t i=0; i<elementText->text.length(); ++i)
						{
							if (elementText->font->getTextExtent(elementText->text.substr(startPos, i))>pAreaWidth)
							{
								float lineWidth = elementText->font->getTextExtent(elementText->text.substr(startPos, i-1));
								lineCount = (int)(elementText->font->getTextExtent(text)/lineWidth);
								break;
							}
						}
						//	������Ϣ���ϲ�����
						for (int t=0; t<lineCount; t++)
						{
							endPos = elementText->font->getCharAtPixel(text, pAreaWidth);
							String str = text.substr(0, endPos);

							Element_Text *line_text_mid = new Element_Text;
							line_text_mid->type = elementText->type;
							line_text_mid->font = elementText->font;
							line_text_mid->width = elementText->font->getTextExtent(str);
							line_text_mid->height = elementText->height;
							line_text_mid->index = elementText->index;
							line_text_mid->newLine = elementText->newLine;
							line_text_mid->text = str;
							line_text_mid->textCols = elementText->textCols;
							line_text_mid->sideFlag = elementText->sideFlag;
							if (line_text_mid->sideFlag)
							{
								line_text_mid->sideCols = elementText->sideCols;
								line_text_mid->sidePix = elementText->sidePix;
							}

							pLine.element.push_back(line_text_mid);
							pLine.height = line_text_mid->height;
							pLine.width += line_text_mid->width;
							mLines.push_back(pLine);
							text = text.substr(endPos, text.length()-endPos);
							pLine.element.clear();
							pLine.width = 0.0f;
							pLine.height = 0.0f;
							pLineNum++;	//	�к�����
						}

						//	��ȡ�ַ�����
						Element_Text *line_text_tail = new Element_Text;
						line_text_tail->type = elementText->type;
						line_text_tail->font = elementText->font;
						line_text_tail->width = elementText->font->getTextExtent(text);
						line_text_tail->height = elementText->height;
						line_text_tail->index = elementText->index;
						line_text_tail->newLine = elementText->newLine;
						line_text_tail->text = text;
						line_text_tail->textCols = elementText->textCols;
						line_text_tail->sideFlag = elementText->sideFlag;
						if (line_text_tail->sideFlag)
						{
							line_text_tail->sideCols = elementText->sideCols;
							line_text_tail->sidePix = elementText->sidePix;
						}

						pLine.element.push_back(line_text_tail);
						pLine.height = line_text_tail->height;
						pLine.width += line_text_tail->width;
						mLines.push_back(pLine);
						pLineNum++;
						pCurrPos = elementText->font->getTextExtent(text) + ELEMENT_GAP;
					}
				}	//	end of ET_TEXT
				break;
			case ET_IMAGE:	//	ͼ��
				{
					Element_Image *elementImage = (Element_Image*)element;
					Element_Image *line_image = new Element_Image;
					line_image->type = elementImage->type;
					line_image->font = elementImage->font;
					line_image->width = elementImage->width;
					line_image->height = elementImage->height;
					line_image->index = elementImage->index;
					line_image->newLine = elementImage->newLine;
					line_image->image = elementImage->image;

					if (elementImage->width <= pAreaWidth-pCurrPos)	//	Ԫ�س���С��������п�
					{	
						if (pLineNum == 0)	//	Ԫ��������
						{
							pLine.element.push_back(line_image);
							if (pLine.height < line_image->height)
							{
								pLine.height = line_image->height;
							}
							pLine.width += line_image->width;
							mLines.push_back(pLine);	//	������Ϣ���ϲ���һ��
							pLineNum++;					//	�к�����
						}
						else	//	Ԫ�ز������У�������
						{
							if (line_image->newLine)	//	Ԫ����ǿ������һ�б�־
							{
								pLine.element.clear();
								pLine.element.push_back(line_image);
								pLine.height = line_image->height;
								pLine.width += line_image->width;
								mLines.push_back(pLine);
								pLineNum++;
							}
							else
							{
								mLines[pLineNum-1].element.push_back(line_image);
								if (mLines[pLineNum-1].height < line_image->height)
								{
									mLines[pLineNum-1].height = line_image->height;
								}
								mLines[pLineNum-1].width += line_image->width;
							}
						}
						pCurrPos += line_image->width + ELEMENT_GAP;	//	��ǰλ�øı�
					}
					else	//	Ԫ�س��ȴ�������п�
					{
						pLine.element.clear();
						pLine.element.push_back(line_image);
						pLine.height = line_image->height;
						pLine.width += line_image->width;
						mLines.push_back(pLine);
						pLineNum++;
						pCurrPos = line_image->width + ELEMENT_GAP;	//	��ǰλ�øı�
					}
				}	//	end of ET_IMAGE
				break;
			case ET_HYPERLINK:	//	������
				{
					Element_HyperLink *elementHyperLink = (Element_HyperLink*)element;
					int endPos = 0;		//	�ַ����еĽ�ȡ����λ��
					int startPos = 0;	//	�ַ����еĽ�ȡ��ʼλ��

					if (elementHyperLink->width <= pAreaWidth-pCurrPos)	//	Ԫ�س���С��������п�
					{	
						Element_HyperLink *line_hyperlink = new Element_HyperLink;
						line_hyperlink->type = elementHyperLink->type;
						line_hyperlink->font = elementHyperLink->font;
						line_hyperlink->width = elementHyperLink->width;
						line_hyperlink->height = elementHyperLink->height;
						line_hyperlink->index = elementHyperLink->index;
						line_hyperlink->newLine = elementHyperLink->newLine;
						line_hyperlink->text = elementHyperLink->text;
						line_hyperlink->textCols = elementHyperLink->textCols;
						line_hyperlink->sideFlag = elementHyperLink->sideFlag;
						if (line_hyperlink->sideFlag)
						{
							line_hyperlink->sideCols = elementHyperLink->sideCols;
							line_hyperlink->sidePix = elementHyperLink->sidePix;
						}
						line_hyperlink->id = elementHyperLink->id;

						if (pLineNum == 0)	//	Ԫ��������
						{
							pLine.element.push_back(line_hyperlink);
							if (pLine.height < line_hyperlink->height)
							{
								pLine.height = line_hyperlink->height;
							}
							pLine.width += line_hyperlink->width;
							mLines.push_back(pLine);	//	������Ϣ���ϲ���һ��
							pLineNum++;					//	�к�����
						}
						else	//	Ԫ�ز������У�������
						{
							if (line_hyperlink->newLine)	//	Ԫ����ǿ������һ�б�־
							{
								pLine.element.clear();
								pLine.element.push_back(line_hyperlink);
								pLine.height = line_hyperlink->height;
								pLine.width += line_hyperlink->width;
								mLines.push_back(pLine);
								pLineNum++;
							}
							else
							{
								mLines[pLineNum-1].element.push_back(line_hyperlink);
								if (mLines[pLineNum-1].height < line_hyperlink->height)
								{
									mLines[pLineNum-1].height = line_hyperlink->height;
								}
								mLines[pLineNum-1].width += line_hyperlink->width;
							}
						}
						pCurrPos += line_hyperlink->width + ELEMENT_GAP;	//	��ǰλ�øı�
					}
					else	//	Ԫ�س��ȴ�������п�
					{ 
						//	��ȡ�ַ���ǰ��
						endPos = elementHyperLink->font->getCharAtPixel(elementHyperLink->text, pAreaWidth-pCurrPos);	//	����ַ�����ȡ����λ��
						String text = elementHyperLink->text.substr(startPos, endPos);
						startPos = endPos;	//	�ַ�����ȡ��ʼλ�øı�
						if (text.length() > 0)
						{
							Element_HyperLink *line_hyperlink_head = new Element_HyperLink;
							line_hyperlink_head->type = elementHyperLink->type;
							line_hyperlink_head->font = elementHyperLink->font;
							line_hyperlink_head->width = elementHyperLink->font->getTextExtent(text);
							line_hyperlink_head->height = elementHyperLink->height;
							line_hyperlink_head->index = elementHyperLink->index;
							line_hyperlink_head->newLine = elementHyperLink->newLine;
							line_hyperlink_head->text = text;
							line_hyperlink_head->textCols = elementHyperLink->textCols;
							line_hyperlink_head->sideFlag = elementHyperLink->sideFlag;
							if (line_hyperlink_head->sideFlag)
							{
								line_hyperlink_head->sideCols = elementHyperLink->sideCols;
								line_hyperlink_head->sidePix = elementHyperLink->sidePix;
							}
							line_hyperlink_head->id = elementHyperLink->id;

							if (pLineNum < 1)	//	��ǰ��Ϊ��һ��
							{
								pLine.element.push_back(line_hyperlink_head);
								if (pLine.height < line_hyperlink_head->height)
								{
									pLine.height = line_hyperlink_head->height;
								}
								pLine.width += line_hyperlink_head->width;
								mLines.push_back(pLine);	//	����µ�һ��
								pLineNum++;	//	�к�����
							}
							else	//	��ǰ�в��ǵ�һ��
							{
								if (element->newLine)	//	����
								{
									pLine.element.clear();
									pLine.element.push_back(line_hyperlink_head);
									pLine.height = line_hyperlink_head->height;
									pLine.width += line_hyperlink_head->width;
									mLines.push_back(pLine);
									pLineNum++;
								}
								else
								{
									mLines[pLineNum-1].element.push_back(line_hyperlink_head);	//	�ѽ�ȡ���ַ�����ӵ���һ��
									if (mLines[pLineNum-1].height < line_hyperlink_head->height)
									{
										mLines[pLineNum-1].height = line_hyperlink_head->height;
									}
									mLines[pLineNum-1].width += line_hyperlink_head->width;
								}
							}
						}

						pLine.element.clear();
						pLine.width = 0.0f;
						pLine.height = 0.0f;

						//	��ȡ�ַ����в�
						text = elementHyperLink->text.substr(startPos, elementHyperLink->text.length()-startPos);	//	��ȡʣ�µ��ַ���
						int lineCount = (int)(elementHyperLink->font->getTextExtent(text)/pAreaWidth);	//	������
						for (size_t i=0; i<elementHyperLink->text.length(); ++i)
						{
							if (elementHyperLink->font->getTextExtent(elementHyperLink->text.substr(startPos, i))>pAreaWidth)
							{
								float lineWidth = elementHyperLink->font->getTextExtent(elementHyperLink->text.substr(startPos, i-1));
								lineCount = (int)(elementHyperLink->font->getTextExtent(text)/lineWidth);
								break;
							}
						}
						//	������Ϣ���ϲ�����
						for (int t=0; t<lineCount; t++)
						{
							endPos = elementHyperLink->font->getCharAtPixel(text, pAreaWidth);
							String str = text.substr(0, endPos);

							Element_HyperLink *line_hyperlink_mid = new Element_HyperLink;
							line_hyperlink_mid->type = elementHyperLink->type;
							line_hyperlink_mid->font = elementHyperLink->font;
							line_hyperlink_mid->width = elementHyperLink->font->getTextExtent(str);
							line_hyperlink_mid->height = elementHyperLink->height;
							line_hyperlink_mid->index = elementHyperLink->index;
							line_hyperlink_mid->newLine = elementHyperLink->newLine;
							line_hyperlink_mid->text = str;
							line_hyperlink_mid->textCols = elementHyperLink->textCols;
							line_hyperlink_mid->sideFlag = elementHyperLink->sideFlag;
							if (line_hyperlink_mid->sideFlag)
							{
								line_hyperlink_mid->sideCols = elementHyperLink->sideCols;
								line_hyperlink_mid->sidePix = elementHyperLink->sidePix;
							}
							line_hyperlink_mid->id = elementHyperLink->id;

							pLine.element.push_back(line_hyperlink_mid);
							pLine.height = line_hyperlink_mid->height;
							pLine.width += line_hyperlink_mid->width;
							mLines.push_back(pLine);
							text = text.substr(endPos, text.length()-endPos);
							pLine.element.clear();
							pLine.width = 0.0f;
							pLine.height = 0.0f;
							pLineNum++;
						}

						//	��ȡ�ַ�����
						Element_HyperLink *line_hyperlink_tail = new Element_HyperLink;
						line_hyperlink_tail->type = elementHyperLink->type;
						line_hyperlink_tail->font = elementHyperLink->font;
						line_hyperlink_tail->width = elementHyperLink->font->getTextExtent(text);
						line_hyperlink_tail->height = elementHyperLink->height;
						line_hyperlink_tail->index = elementHyperLink->index;
						line_hyperlink_tail->newLine = elementHyperLink->newLine;
						line_hyperlink_tail->text = text;
						line_hyperlink_tail->textCols = elementHyperLink->textCols;
						line_hyperlink_tail->sideFlag = elementHyperLink->sideFlag;
						if (line_hyperlink_tail->sideFlag)
						{
							line_hyperlink_tail->sideCols = elementHyperLink->sideCols;
							line_hyperlink_tail->sidePix = elementHyperLink->sidePix;
						}
						line_hyperlink_tail->id = elementHyperLink->id;

						pLine.element.push_back(line_hyperlink_tail);
						pLine.height = line_hyperlink_tail->height;
						pLine.width += line_hyperlink_tail->width;
						mLines.push_back(pLine);
						pLineNum++;
						pCurrPos = elementHyperLink->font->getTextExtent(text) + ELEMENT_GAP;
					}
				}	//	end of ET_HYPERLINK
				break;
			case ET_WIN:
				{
					Element_Win *elementWin = (Element_Win*)element;
					Element_Win *line_win = new Element_Win;
					line_win->type = elementWin->type;
					line_win->font = elementWin->font;
					line_win->width = elementWin->width;
					line_win->height = elementWin->height;
					line_win->index = elementWin->index;
					line_win->newLine = elementWin->newLine;
					line_win->win = elementWin->win;

					if (elementWin->width <= pAreaWidth-pCurrPos)	//	Ԫ�س���С��������п�
					{	
						if (pLineNum == 0)	//	Ԫ��������
						{
							pLine.element.push_back(line_win);
							if (pLine.height < line_win->height)
							{
								pLine.height = line_win->height;
							}
							pLine.width += line_win->width;
							mLines.push_back(pLine);	//	������Ϣ���ϲ���һ��
							pLineNum++;					//	�к�����
						}
						else	//	Ԫ�ز������У�������
						{
							if (line_win->newLine)	//	Ԫ����ǿ������һ�б�־
							{
								pLine.element.clear();
								pLine.element.push_back(line_win);
								pLine.height = line_win->height;
								pLine.width += line_win->width;
								mLines.push_back(pLine);
								pLineNum++;
							}
							else
							{
								mLines[pLineNum-1].element.push_back(line_win);
								if (mLines[pLineNum-1].height < line_win->height)
								{
									mLines[pLineNum-1].height = line_win->height;
								}
								mLines[pLineNum-1].width += line_win->width;
							}
						}
						pCurrPos += line_win->width + ELEMENT_GAP;	//	��ǰλ�øı�
					}
					else	//	Ԫ�س��ȴ��������
					{
						pLine.element.clear();
						pLine.element.push_back(line_win);
						pLine.height = line_win->height;
						pLine.width += line_win->width;
						mLines.push_back(pLine);
						pLineNum++;
						pCurrPos = line_win->width + ELEMENT_GAP;	//	��ǰλ�øı�
					}
				}	//	end of ET_WIN
				break;
			}	//	end of switch
			pLine.width = 0.0f;
			pLine.height = 0.0f;
		}	//	end of loop for

		_configureScrollbars();	//	���ù�����
	}
	//-------------------------------------------------------------------------
	//	��갴��
	void MultiLineBox::onMouseButtonDown(MouseEventArgs& e)
	{
		Window::onMouseButtonDown(e);
		if (e.button == LeftButton)
		{
			captureInput();
			int index = _getIndexFromPosition(e.position);
			if ((-1 != index) && (-2 != index))	//	����λ����Ч
			{
				if ((index < 0) || ((size_t)index >= mElementList.size()))
					return;
				if (ET_HYPERLINK == mElementList[index]->type)
				{
					Element_HyperLink *elementHyperLink = (Element_HyperLink*)mElementList[index];
					mHyperLinkId = elementHyperLink->id;
					fireEvent(HyperLink_MouseClick, e, EventNamespace);	//	���������������λ��
				}
				else if (ET_TEXT == mElementList[index]->type)
				{
					Element_Text *elementText = (Element_Text*)mElementList[index];
					mTextContent = elementText->text.c_str();
					fireEvent(Text_MouseClick, e, EventNamespace);	//	����������ı�λ��
				}
			}
		}
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	��굯��
	void MultiLineBox::onMouseButtonUp(MouseEventArgs& e)
	{
		Window::onMouseButtonUp(e);
		if (e.button == LeftButton)	//	��������Ч
		{
			releaseInput();
		}
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	����ƶ�
	void MultiLineBox::onMouseMove(MouseEventArgs& e)
	{
		Window::onMouseMove(e);
		fireEvent(HyperLink_MouseLeave, e, EventNamespace);	//	��������Ƴ�������λ�ã�HyperLink_MoveLeave����HyperLink_MoveEnterǰ��
		fireEvent(Text_MouseLeave, e, EventNamespace);	//	��������Ƴ��ı�λ�ã�Text_MouseEnter����Text_MouseLeaveǰ��
		int index = _getIndexFromPosition(e.position);
		if ((-1 != index) && (-2 != index))	//	����λ����Ч
		{
			if ((index < 0) || ((size_t)index >= mElementList.size()))
				return;
			if (ET_HYPERLINK == mElementList[index]->type)
			{
				Element_HyperLink *elementHyperLink = (Element_HyperLink*)mElementList[index];
				mHyperLinkId = elementHyperLink->id;
				fireEvent(HyperLink_MouseEnter, e, EventNamespace);	//	��������ƽ�������λ��
			}
			else if (ET_TEXT == mElementList[index]->type)
			{
				Element_Text *elementText = (Element_Text*)mElementList[index];
				mTextContent = elementText->text.c_str();
				fireEvent(Text_MouseEnter, e, EventNamespace);	//	��������ƽ��ı�λ��
			}
		}
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	������
	void MultiLineBox::onMouseEnters(MouseEventArgs& e)
	{
		Window::onMouseEnters(e);
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	����뿪
	void MultiLineBox::onMouseLeaves(MouseEventArgs& e)
	{
		Window::onMouseLeaves(e);
		fireEvent(HyperLink_MouseLeave, e, EventNamespace);	//	��������Ƴ�������λ�ã�HyperLink_MoveLeave����HyperLink_MoveEnterǰ��
		fireEvent(Text_MouseLeave, e, EventNamespace);	//	��������Ƴ��ı�λ�ã�Text_MouseEnter����Text_MouseLeaveǰ��
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	������
	void MultiLineBox::onMouseWheel(MouseEventArgs& e)
	{
		Window::onMouseWheel(e);
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (vertScrollbar->isVisible() && (vertScrollbar->getDocumentSize() > vertScrollbar->getPageSize()))
		{
			vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition() + vertScrollbar->getStepSize() * -e.wheelChange);
		}
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	���ڴ�С�ı�
	void MultiLineBox::onSized(WindowEventArgs& e)
	{
		Window::onSized(e);
		e.handled = true;
		_formatElement();	//	�����Ű�
	}
	//-------------------------------------------------------------------------
	//	����
	void MultiLineBox::onCaptureLost(WindowEventArgs& e)
	{
		Window::onCaptureLost(e);
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	������״̬�ı�
	bool MultiLineBox::handle_scrollChange(const EventArgs& args)
	{
		requestRedraw();
		return true;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��ֱ������
	Scrollbar* MultiLineBox::getVertScrollbar(void)
	{
		Scrollbar *scrBar = static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(getName() + VertScrollbarNameSuffix));
		return scrBar;
	}
	//-------------------------------------------------------------------------
	//	��ֱ�������Ƿ�ɼ�
	bool MultiLineBox::isScrollbarVisible(void)
	{
		return mScrollbarVisible;
	}
	//-------------------------------------------------------------------------
	//	��ȡͼ�Ļ��ſؼ���Ⱦ����
	Rect MultiLineBox::getRenderArea(void)
	{
		if (d_windowRenderer)
		{
			MultiLineBoxWindowRenderer* wr = (MultiLineBoxWindowRenderer*)d_windowRenderer;
			return wr->getRenderArea();
		}
		else
		{
			throw InvalidRequestException("MultiLineBox::getRenderArea - This function must be implemented by the window renderer module");
		}
	}
	//-------------------------------------------------------------------------
	//	��ȡ��Ⱦ������
	float MultiLineBox::getRenderWidth(void)
	{
		return getRenderArea().getWidth();
	}
	//-------------------------------------------------------------------------
	//	��ȡ��Ⱦ����߶�
	float MultiLineBox::getRenderHeight(void)
	{
		return getRenderArea().getHeight();
	}
	//-------------------------------------------------------------------------
	//	��ȡ����Ϣ����
	MultiLineBox::LineList MultiLineBox::getLineList(void)
	{
		return mLines;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�Ű��ĸ߶�
	float MultiLineBox::_getTotalHeight(void)
	{
		float totalHeight = 0.0f;
		if (0.0f == mEachLineHeight)
		{
			for (size_t i=0; i<mLines.size(); i++)
				totalHeight += mLines[i].height;
		} 
		else
		{
			for (size_t i=0; i<mLines.size(); i++)
				totalHeight += mEachLineHeight;
		}
		if (mLines.size() > 0)
		{
			totalHeight += (mLines.size()-1) * LINE_GAP;
		}
		return totalHeight;
	}
	//-------------------------------------------------------------------------
	//	�������λ�û�ȡ����
	int MultiLineBox::_getIndexFromPosition(Point pt)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;

		Point wndPt = CoordConverter::screenToWindow(*this, pt);
		wndPt.d_y += vertScrollbar->getScrollPosition();

		float height = getHeightInterval();
		Rect rendedArea(getRenderArea());
		// �жϴ�ֱ���뷽ʽ
		switch (getAlignVer())
		{
		case MultiLineBox::AT_TOP:
			height += 0.0f;
			break;
		case MultiLineBox::AT_CENTER_VER:
			height += (rendedArea.d_bottom - rendedArea.d_top - getFormatHeight())/2;
			break;
		case MultiLineBox::AT_BOTTOM:
			height += rendedArea.d_bottom - rendedArea.d_top - getFormatHeight();
			break;
		}

		LineList line_list = getLineList();
		for (size_t i=0; i<line_list.size(); i++)	//	����
		{
			Line line = line_list[i];

			if (0.0f == mEachLineHeight)	// ÿ�и߶Ȳ�һ��
			{
				height += line.height;
			} 
			else	// ÿ��һ���߶�
			{
				height += mEachLineHeight;
			}
			if (0 != i)	// ���ǵ�һ��
			{
				height += LINE_GAP;
			}

			if (wndPt.d_y <= height)	// ȷ���к�
			{
				float width = 0;
				if (mScrollbarVisible)	// �й�����
				{
					switch (vertScrollbar->getHorizontalAlignment())	//	��ȡ��ֱ�������Ķ��뷽ʽ
					{
					case HA_LEFT:	//	�����������
						width = getWidthInterval();
						break;
					case HA_RIGHT:	//	���������ұ�
						width = 0;
						break;
					}
				}

				// �ж�ˮƽ���뷽ʽ
				switch (getAlignHor())
				{
				case MultiLineBox::AT_LEFT:
					width += 0.0f;
					break;
				case MultiLineBox::AT_CENTER_HOR:
					width += (rendedArea.d_right - rendedArea.d_left - line.width)/2;
					break;
				case MultiLineBox::AT_RIGHT:
					width += rendedArea.d_right - rendedArea.d_left - line.width;
					break;
				}

				for (size_t j=0; j<line.element.size(); j++)	//	��ǰ�е�Ԫ����
				{
					Element *element = line.element[j];
					width += element->width;
					if (0 != j)	// ���ǵ�ǰ�еĵ�һ��Ԫ��
					{
						width += ELEMENT_GAP;
					}

					if (wndPt.d_x <= width)	// ȷ��Ԫ��
					{
						float xPos = getPosition().d_x.asAbsolute(mScreenWidth);
						float yPos = getPosition().d_y.asAbsolute(mScreenHeight);
						float left = width - element->width + xPos;
						float top = height - line.height - vertScrollbar->getScrollPosition() + yPos;
						float right = width + xPos;
						float bottom = height - vertScrollbar->getScrollPosition() + yPos;
						char buf[128] = "";
						_snprintf_s(buf, sizeof(buf), "%f-%f-%f-%f", left, top, right, bottom);
						mTriggerRect = buf;
						return element->index;	//	����Ԫ��������Ϣ�����е�������
					}
				}
				return -2;	//	����λ����Ч
			}
		}
		return -2;	//	����λ����Ч
	}
	//-------------------------------------------------------------------------
	//	���ַ���ת��Ϊ��ɫ
	colour MultiLineBox::_extractColorFromString(String cols)
	{
		colour colorInit(1.0f, 1.0f, 1.0f);
		if ((cols.size() != 7) && (cols.size() != 9) && (cols[0] != '#'))
			return colorInit;

		char buf[3];
		buf[2] = 0;
		long l;

		buf[0] = cols[1];
		buf[1] = cols[2];
		l = strtol(buf, NULL, 16);
		float red = l/255.0f;

		buf[0] = cols[3];
		buf[1] = cols[4];
		l = strtol(buf, NULL, 16);
		float green = l/255.0f;

		buf[0] = cols[5];
		buf[1] = cols[6];
		l = strtol(buf, NULL, 16);
		float blue = l/255.0f;

		colour color(red, green, blue);
		if (cols.size() == 7)
			return color;

		buf[0] = cols[7];
		buf[1] = cols[8];
		l = strtol(buf, NULL, 16);
		float alpha = l/255.0f;
		colour colorAll(red, green, blue, alpha);
		return colorAll;
	}
	//-------------------------------------------------------------------------
	//	����ı�
	int MultiLineBox::addElementText(Font* fnt, String text, colour cols, bool newLine)
	{
		Element_Text* elementText = new Element_Text;
		if (NULL == elementText)
			return -1;
		if (NULL == fnt)
			return -1;
		elementText->type = ET_TEXT;
		elementText->font = fnt;
		int pos;
		String textCutPre = text;
		String textCutSuf = "";
		if (String::npos != (pos=text.find('\n')))	//	�ж��ı��еĻ��з�
		{
			if (text.size() != pos+1)
			{
				textCutPre = text.substr(0, pos);
				textCutSuf = text.substr(pos+1, text.size());
			}
		}
		elementText->width = elementText->font->getTextExtent(text);
		elementText->height = elementText->font->getFontHeight();
		elementText->index = mElementList.size();
		elementText->newLine = newLine;
		elementText->text = textCutPre;
		elementText->textCols = cols;
		if (mSpecSideFlag)	//	�������
		{
			elementText->sideCols = mSpecSideCols;
			elementText->sidePix = mSpecSidePix;
			elementText->sideFlag = mSpecSideFlag;
		} 
		else
		{
			if (mSideFlag)
			{
				elementText->sideCols = mSideCols;
				elementText->sidePix = mSidePix;
			}
			elementText->sideFlag = mSideFlag;
		}
		mSpecSidePix = 1.0f;
		mSpecSideFlag = false;
		mLength += elementText->width;
		mElementList.push_back(elementText);
		if (String::npos != (pos=text.find('\n')))
		{
			if (text.size() != pos+1)
				addElementText(fnt, textCutSuf, cols, true);
		}
		_formatElement();
		return 0;
	}
	//-------------------------------------------------------------------------
	//	����ı�
	int MultiLineBox::addElementText(std::string fontName, std::string text, std::string cols, bool newLine)
	{
		Font *fnt = FontManager::getSingleton().getFont(fontName);
		if (NULL == fnt)
			return -1;
		colour tCols(_extractColorFromString(cols));
		if (-1 == addElementText(fnt, (utf8*)text.c_str(), tCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	����ı�
	int MultiLineBox::addElementText(std::string text, std::string cols, bool newLine)
	{
		colour tCols(_extractColorFromString(cols));
		if (-1 == addElementText(mTextFont, (utf8*)text.c_str(), tCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	����ı�
	int MultiLineBox::addElementText(std::string text, bool newLine)
	{
		if (-1 == addElementText(mTextFont, (utf8*)text.c_str(), mTextCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	���ͼ��
	int MultiLineBox::addElementImage(Image image, float width, float height, bool newLine)
	{
		Element_Image* elementImage = new Element_Image;
		if (NULL == elementImage)
			return -1;
		elementImage->type = ET_IMAGE;
		elementImage->width = width;
		elementImage->height = height;
		elementImage->newLine = newLine;
		elementImage->index = mElementList.size();
		elementImage->image = image;
		mLength += elementImage->width;
		mElementList.push_back(elementImage);
		_formatElement();
		return 0;
	}
	//-------------------------------------------------------------------------
	//	���ͼ��
	int MultiLineBox::addElementImage(std::string imagesetName, std::string imageName, float width, float height, bool newLine)
	{
		Imageset *imgset = ImagesetManager::getSingleton().getImageset(imagesetName);
		if (NULL == imgset)
			return -1;
		const Image *img = &(imgset->getImage(imageName));
		if (NULL == img)
			return -1;
		if (-1 == addElementImage(*img, width, height, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	��ӳ�����
	int MultiLineBox::addElementHyperLink(Font* fnt, String text, std::string id, colour cols, bool newLine)
	{
		Element_HyperLink* elementHyperLink = new Element_HyperLink;
		if (NULL == elementHyperLink)
			return -1;
		if (NULL == fnt)
			return -1;
		elementHyperLink->type = ET_HYPERLINK;
		elementHyperLink->font = fnt;
		elementHyperLink->width = elementHyperLink->font->getTextExtent(text);
		elementHyperLink->height = elementHyperLink->font->getFontHeight();
		elementHyperLink->newLine = newLine;
		elementHyperLink->index = mElementList.size();
		elementHyperLink->text = text;
		elementHyperLink->id = id;
		elementHyperLink->textCols = cols;
		if (mSpecSideFlag)	//	�������
		{
			elementHyperLink->sideCols = mSpecSideCols;
			elementHyperLink->sidePix = mSpecSidePix;
			elementHyperLink->sideFlag = mSpecSideFlag;
		} 
		else
		{
			if (mSideFlag)
			{
				elementHyperLink->sideCols = mSideCols;
				elementHyperLink->sidePix = mSidePix;
			}
			elementHyperLink->sideFlag = mSideFlag;
		}
		mSpecSidePix = 1.0f;
		mSpecSideFlag = false;
		mLength += elementHyperLink->width;
		mElementList.push_back(elementHyperLink);
		_formatElement();
		return 0;
	}
	//-------------------------------------------------------------------------
	//	��ӳ�����
	int MultiLineBox::addElementHyperLink(std::string fontName, std::string text, std::string id, std::string cols, bool newLine)
	{
		Font *fnt = FontManager::getSingleton().getFont(fontName);
		if (NULL == fnt)
			return -1;
		colour tCols(_extractColorFromString(cols));
		if (-1 == addElementHyperLink(fnt, (utf8*)text.c_str(), id, tCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	��ӳ�����
	int MultiLineBox::addElementHyperLink(std::string text, std::string id, std::string cols, bool newLine)
	{
		colour tCols(_extractColorFromString(cols));
		if (-1 == addElementHyperLink(mTextFont, (utf8*)text.c_str(), id, tCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	��ӳ�����
	int MultiLineBox::addElementHyperLink(std::string text, std::string id, bool newLine)
	{
		if (-1 == addElementHyperLink(mTextFont, (utf8*)text.c_str(), id, mTextCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	��ӿؼ�
	int MultiLineBox::addElementWin(Window* win, bool newLine)
	{
		Element_Win* elementWin = new Element_Win;
		if (NULL == elementWin)
			return -1;
		elementWin->type = ET_WIN;
		elementWin->width = win->getWidth().asAbsolute(mScreenWidth);
		elementWin->height = win->getHeight().asAbsolute(mScreenHeight);
		elementWin->newLine = newLine;
		elementWin->index = mElementList.size();
		elementWin->win = win;
		addChildWindow(win);
		mLength += elementWin->width;
		mElementList.push_back(elementWin);
		_formatElement();
		return 0;
	}
	//-------------------------------------------------------------------------
	//	��ӿؼ�
	int MultiLineBox::addElementWin(std::string winName, bool newLine)
	{
		Window *win = WindowManager::getSingleton().getWindow(winName);
		if (NULL == win)
			return -1;
		if (-1 == addElementWin(win, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ����壨�Կؼ�����������Ч��
	int MultiLineBox::setFontFunc(std::string fontName)
	{
		Font *fnt = NULL;
		if (FontManager::getSingleton().isFontPresent(fontName))
			fnt = FontManager::getSingleton().getFont(fontName);
		else
			fnt = FontManager::getSingleton().createFont(fontName + ".font");

		if (NULL == fnt)
			return -1;

		mTextFont = fnt;
		mTextFontName = fontName;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	�����ı���ɫ���Կؼ�����������Ч��
	void MultiLineBox::setColsFunc(std::string cols)
	{
		mTextCols = _extractColorFromString(cols);
		mTextColsName = cols;
		mColsFlag = true;
	}
	//-------------------------------------------------------------------------
	//	���������ɫ���Կؼ�����������Ч��
	void MultiLineBox::setSideFunc(std::string cols, float sPix)
	{
		mSideCols = _extractColorFromString(cols);
		mSidePix = sPix;
		mSideFlag = true;
	}
	//-------------------------------------------------------------------------
	//	�����ض�Ԫ�ص���ߣ�ֻ���ض�Ԫ����Ч��
	void MultiLineBox::setSpecSide(colour sCols, float sPix)
	{
		mSpecSideCols = sCols;
		mSpecSidePix = sPix;
		mSpecSideFlag = true;
	}
	//-------------------------------------------------------------------------
	//	�����ض�Ԫ�ص���ߣ�ֻ���ض�Ԫ����Ч��
	void MultiLineBox::setSpecSide(std::string cols, float sPix)
	{
		colour sCols(_extractColorFromString(cols));
		setSpecSide(sCols, sPix);
	}
	//-------------------------------------------------------------------------
	//	���ö��뷽ʽ
	int MultiLineBox::setAlign(std::string alignType)
	{
		if ("AT_LEFT" == alignType)	//	�ж�ˮƽ
			mAlignHor = AT_LEFT;
		else if ("AT_CENTER_HOR" == alignType)
			mAlignHor = AT_CENTER_HOR;
		else if ("AT_RIGHT" == alignType)
			mAlignHor = AT_RIGHT;
		else if ("AT_TOP" == alignType)	//	�жϴ�ֱ
			mAlignVer = AT_TOP;
		else if ("AT_CENTER_VER" == alignType)
			mAlignVer = AT_CENTER_VER;
		else if ("AT_BOTTOM" == alignType)
			mAlignVer = AT_BOTTOM;
		else
			return -1;	//	alignType��ʽ��������ʧ��

		return 0;	//	���óɹ�
	}
	//-------------------------------------------------------------------------
	//	ǿ������ÿ�еĸ߶�
	void MultiLineBox::setEachLineHeight(float height)
	{
		mEachLineHeight = height;
	}
	//-------------------------------------------------------------------------
	//	���ô�ֱ�������ɼ�
	int MultiLineBox::setScrollbarVisible(std::string pos, bool visible, int mode)
	{
		if ("SP_TOP" == pos)
			mScrollbarPos = SP_TOP;
		else if ("SP_OLD" == pos)
			mScrollbarPos = SP_OLD;
		else if ("SP_BOTTOM" == pos)
			mScrollbarPos = SP_BOTTOM;
		else
			return -1;	//	pos��ʽ��������ʧ��

		if ((0 != mode) && (1 != mode))
			return -1;	//	mode��ʽ��������ʧ��

		mScrollMode = mode;
		mScrollbarVisible = visible;
		_configureScrollbars();
		return 0;	//	���óɹ�
	}
	//-------------------------------------------------------------------------
	//	���ô�ֱ������λ��
	int MultiLineBox::setScrollbarPosition(float pos)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;
		vertScrollbar->setScrollPosition(pos);
		return 0;
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ�������С
	void MultiLineBox::setMaxSizeFunc(float maxWidth, float maxHeight)
	{
		setMaxSize(UVector2(UDim(0.0f, maxWidth), UDim(0.0f, maxHeight)));
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ����
	void MultiLineBox::setWidthFunc(float width)
	{
		setWidth(UDim(0.0f, width));
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ��߶�
	void MultiLineBox::setHeightFunc(float height)
	{
		setHeight(UDim(0.0f, height));
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ���С
	void MultiLineBox::setSizeFunc(float width, float height)
	{
		setSize(UVector2(UDim(0.0f, width), UDim(0.0f, height)));
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ�λ��
	void MultiLineBox::setPositionFunc(float x, float y)
	{
		setPosition(UVector2(UDim(0.0f, x), UDim(0.0f, y)));
	}
	//-------------------------------------------------------------------------
	//	���ÿؼ��Ƿ�ɼ�
	void MultiLineBox::setVisibleFunc(bool visible)
	{
		setVisible(visible);
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ؼ�����
	std::string MultiLineBox::getNameFunc(void)
	{
		String name = getName();
		std::string ctrlName = name.c_str();
		return ctrlName;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��������
	std::string MultiLineBox::getFontName(void)
	{
		return mTextFontName;
	}
	//-------------------------------------------------------------------------
	//	��ȡ������ɫ
	std::string MultiLineBox::getColour(void)
	{
		return mTextColsName;
	}
	//-------------------------------------------------------------------------
	//	��ȡˮƽ���뷽ʽ
	MultiLineBox::AlignType MultiLineBox::getAlignHor(void)
	{
		return mAlignHor;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��ֱ���뷽ʽ
	MultiLineBox::AlignType MultiLineBox::getAlignVer(void)
	{
		return mAlignVer;
	}
	//-------------------------------------------------------------------------
	//	��ȡ���������ݵı�ʶ
	std::string MultiLineBox::getHyperLinkId(void)
	{
		return mHyperLinkId;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��Ӧ���ı�����
	std::string MultiLineBox::getTextContent(void)
	{
		return mTextContent;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��Ӧ�ľ�������
	std::string MultiLineBox::getTriggerRect(void)
	{
		return mTriggerRect;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�����Ű��ؼ�����ĸ߶�
	float MultiLineBox::getFormatHeight(void)
	{
		float totalHeight = _getTotalHeight();
		return totalHeight + getHeightInterval();
	}
	//-------------------------------------------------------------------------
	//	��ȡ�����Ű��ؼ�������
	int MultiLineBox::getLineSize(void)
	{
		int lines = mLines.size();
		return lines;
	}
	//-------------------------------------------------------------------------
	//	��ȡǿ������ÿ�еĸ߶�
	float MultiLineBox::getEachLineHeight(void)
	{
		return mEachLineHeight;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ؼ��������Ⱦ��ȵļ�϶
	float MultiLineBox::getWidthInterval(void)
	{
		float areaWidth = getArea().getWidth().asAbsolute(mScreenWidth);
		float renderAreaWidth = getRenderArea().getWidth();
		return areaWidth - renderAreaWidth;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ؼ��߶�����Ⱦ�߶ȵļ�϶
	float MultiLineBox::getHeightInterval(void)
	{
		float areaHeight = getArea().getHeight().asAbsolute(mScreenHeight);
		float renderAreaHeight = getRenderArea().getHeight();
		return areaHeight - renderAreaHeight;
	}
	//-------------------------------------------------------------------------
	//	����text��ȡ�ؼ��ĺ��ʿ��
	float MultiLineBox::getFitWidth(std::string fontName, std::string text)
	{
		float ctrlWidth = getStrExtent(fontName, text) + getWidthInterval() + 1.0f;
		return ctrlWidth;
	}
	//-------------------------------------------------------------------------
	//	�����ı��Ŀ�ȶȻ�ÿؼ��ĺ��ʿ��
	float MultiLineBox::getFitWidth(float textLen)
	{
		float ctrlWidth = textLen + getWidthInterval() + 1.0f;
		return ctrlWidth;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ؼ������ܳ���
	float MultiLineBox::getLength(void)
	{
		return mLength;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��ʱ����ҳ���С
	float MultiLineBox::getScrollbarDocumentSize(void)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;
		return vertScrollbar->getDocumentSize();
	}
	//-------------------------------------------------------------------------
	//	��ȡ��ʱ��������λ��
	float MultiLineBox::getScrollbarPosition(void)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;
		return vertScrollbar->getScrollPosition();
	}
	//-------------------------------------------------------------------------
	//	��������Ϊtext���ı�Ԫ��
	int MultiLineBox::findElementText(std::string text)
	{
		for (size_t i=0; i<mElementList.size(); ++i)
		{
			if (ET_TEXT == mElementList[i]->type)
			{
				Element_Text *elementText = (Element_Text*)mElementList[i];
				String temp = (utf8*)text.c_str();
				if (temp == elementText->text)
					return elementText->index;
			}
		}
		return -1;
	}
	//-------------------------------------------------------------------------
	//	�������
	void MultiLineBox::clearUp(void)
	{
		_destroyLineElement();
		_destroyAllElements();
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	������Ԫ��
	void MultiLineBox::_destroyLineElement(void)
	{
		for (size_t i=0; i<mLines.size(); i++)
		{
			Line line = mLines[i];
			for (size_t j=0; j<line.element.size(); j++)
			{
				switch (mLines[i].element[j]->type)
				{
				case ET_TEXT:
					delete (Element_Text*)mLines[i].element[j];
					break;
				case ET_IMAGE:
					delete (Element_Image*)mLines[i].element[j];
					break;
				case ET_HYPERLINK:
					delete (Element_HyperLink*)mLines[i].element[j];
					break;
				case ET_WIN:
					delete (Element_Win*)mLines[i].element[j];
					break;
				default:
					break;
				}
				mLines[i].element[j] = NULL;
			}
			line.element.clear();
		}
		mLines.clear();
	}
	//-------------------------------------------------------------------------
	//	��������Ԫ��
	void MultiLineBox::_destroyAllElements(void)
	{
		for (size_t i=0; i<mElementList.size(); ++i)
		{
			switch (mElementList[i]->type)
			{
			case ET_TEXT:
				delete (Element_Text*)mElementList[i];
				break;
			case ET_IMAGE:
				delete (Element_Image*)mElementList[i];
				break;
			case ET_HYPERLINK:
				delete (Element_HyperLink*)mElementList[i];
				break;
			case ET_WIN:
				WindowManager::getSingleton().destroyWindow(((Element_Win*)mElementList[i])->win);
				delete (Element_Win*)mElementList[i];
				break;
			default:
				break;
			}
			mElementList[i] = NULL;
		}
		mElementList.clear();
		mLength = 0.0f;
	}
	//-------------------------------------------------------------------------
	//	���id��Ӧ��ͼ�Ļ��ſؼ�����
	void MultiLineBox::eraseElement(size_t id)
	{
		if ((0 == mElementList.size()) || (id < 0) || (id >= mElementList.size()))
			return;

		ElementList::const_iterator iter = mElementList.begin();
		for (size_t i=0; iter!=mElementList.end(); ++iter,++i)
		{
			if (i == id)
			{
				float width = mElementList[i]->width;
				mLength -= width;
				switch (mElementList[i]->type)
				{
				case ET_TEXT:
					delete (Element_Text*)mElementList[i];
					break;
				case ET_IMAGE:
					delete (Element_Image*)mElementList[i];
					break;
				case ET_HYPERLINK:
					delete (Element_HyperLink*)mElementList[i];
					break;
				case ET_WIN:
					WindowManager::getSingleton().destroyWindow(((Element_Win*)mElementList[i])->win);
					delete (Element_Win*)mElementList[i];
					break;
				default:
					break;
				}
				mElementList[i] = NULL;
				mElementList.erase(iter);
				//	�Ƴ�һ��Ԫ�غ�������Ԫ�ص�������Ҫ��1
				for (; i<mElementList.size(); ++i)
				{
					--mElementList[i]->index;
				}
				_formatElement();
				return;
			}
		}
	}
	//-------------------------------------------------------------------------
	//	��������
	Window* MultiLineBoxFactory::createWindow(const String& name)
	{
		MultiLineBox *mlb = new MultiLineBox(d_type, name);
		return mlb;
	}
	//-------------------------------------------------------------------------
	//	���ٴ���
	void MultiLineBoxFactory::destroyWindow(Window* window)
	{
		delete window;
	}
	//-------------------------------------------------------------------------
	//	��windowת��ΪMultiLinebox����
	MultiLineBox* toMultiLineBox(std::string ctrlName)
	{
		Window *win = WindowManager::getSingleton().getWindow(ctrlName);
		if (NULL == win)
			return NULL;
		MultiLineBox *mlb = static_cast<MultiLineBox*>(win);
		return mlb;
	}
	//-------------------------------------------------------------------------
	//	��������font��ȡ�ַ���str�ĳ���
	float getStrExtent(std::string fontName, std::string str)
	{
		Font *fnt = FontManager::getSingleton().getFont(fontName);
		if (NULL == fnt)
			return -1;
		float len = fnt->getTextExtent((utf8*)str.c_str());
		return len;
	}
	//-------------------------------------------------------------------------
	//	�������ػ�ȡ��ȡ���ַ�λ��
	size_t getCharAtPixel(std::string fontName, std::string str, float pixel, size_t startChar)
	{
		Font *fnt = FontManager::getSingleton().getFont(fontName);
		if (NULL == fnt)
			return 0;
		String temp = (utf8*)str.c_str();
		size_t endChar = fnt->getCharAtPixel(temp, startChar, pixel);
		return endChar;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ַ���
	std::string subStrExtent(std::string fontName, std::string str, float pixel, size_t startChar)
	{
		Font *fnt = FontManager::getSingleton().getFont(fontName);
		if (NULL == fnt)
			return 0;
		String temp = (utf8*)str.c_str();
		size_t endChar = fnt->getCharAtPixel(temp, startChar, pixel);
		String text = temp.substr(startChar, endChar);
		return text.c_str();
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ַ���str�е��ַ�����
	size_t getCharacterCount(std::string str)
	{
		int characterCount = 0;	//	�ַ�����
		int charCount = 0;		//	�����ķ��Ÿ���
		int chineseCount = 0;	//	���ķ��Ÿ���

		for (size_t i=0; i<str.length(); i++)
		{
			if (str[i] < 0)	//	���ķ���ռ�����ֽڣ�ÿ���ֽڶ�С��0
			{
				if (str[i] == -62)	//	��������š��˷���ռ2���ֽ�-62��-73
				{
					charCount++;
					++i;
					continue;
				}
				chineseCount++;
			} 
			else	//	�����ķ���
			{
				charCount++;
			}
		}

		characterCount = charCount + chineseCount/3;
		return characterCount;
	}
	//-------------------------------------------------------------------------
	//	��ȡ�ַ���str���ִ������ַ�����Ϊ��λ
	std::string subCharacter(std::string str, size_t startPos, int len)
	{
		std::vector<std::string> strVec;

		for (size_t i=0; i<str.length(); i++)
		{
			if (str[i] < 0)	//	���ķ���ռ�����ֽڣ�ÿ���ֽڶ�С��0
			{
				std::string tmp = "";
				if (str[i] == -62)	//	��������š��˷���ռ2���ֽ�-62��-73
				{
					tmp = str.substr(i, 2);
					i += 1;
				}
				else
				{
					tmp = str.substr(i, 3);
					i += 2;
				}
				strVec.push_back(tmp);
			} 
			else	//	�����ķ���
			{
				std::string tmp = str.substr(i, 1);
				strVec.push_back(tmp);
			}
		}

		std::string sub = "";
		size_t endPos = startPos + len;
		if (endPos > strVec.size())
		{
			endPos = strVec.size();
		}
		for (; startPos<endPos; startPos++)
		{
			sub += strVec[startPos];
		}
		return sub;
	}
	//-------------------------------------------------------------------------
	//	��ȡ��������ĸ���ֵ
	float getPositionVal(std::string winname, int xy, int pos)
	{
		if ((0 != xy) && (1 != xy) && (0 != pos) && (1 != pos))
			return -1;

		CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(winname);
		if (NULL == win)
			return -2;

		CEGUI::UDim xyPos(0, 0);
		if (0 == xy)	//	x����
			xyPos = win->getPosition().d_x;
		else if (1 == xy)	//	y����
			xyPos = win->getPosition().d_y;

		float val = 0;
		if (0 == pos)	//	scale
			val = xyPos.d_scale;
		else if (1 == pos)	//	offset
			val = xyPos.d_offset;

		return val;
	}

} //	end of namespace CEGUI
