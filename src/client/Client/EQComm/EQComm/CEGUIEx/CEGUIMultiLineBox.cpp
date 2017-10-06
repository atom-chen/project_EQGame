/******************************************************************************
* 作者: 何展然
* 时间: 2010-7-1
* 描述: 图文混排控件
******************************************************************************/
#include "CEGUIMultiLineBox.h"
//#include <ShellAPI.h>

#define LINE_GAP	3.0f	// 每行间的行间距
#define ELEMENT_GAP	1.0f	// 同一行里元素的间距


namespace CEGUI
{
	//-------------------------------------------------------------------------
	//	构造函数
	MultiLineBoxWindowRenderer::MultiLineBoxWindowRenderer(const String& name) :
	WindowRenderer(name, MultiLineBox_EventNamespace)
	{
	}
	//-------------------------------------------------------------------------
	//	全局事件名称
	const String MultiLineBox::EventNamespace(MultiLineBox_EventNamespace);
	//-------------------------------------------------------------------------
	//	窗口类型名称
	const String MultiLineBox::WidgetTypeName(MultiLineBox_WidgetTypeName);
	//-------------------------------------------------------------------------
	//	事件名称（鼠标点击超链接位置）
	const String MultiLineBox::HyperLink_MouseClick("HyperLink_MouseClick");
	//-------------------------------------------------------------------------
	//	事件名称（鼠标移进超链接位置）
	const String MultiLineBox::HyperLink_MouseEnter("HyperLink_MouseEnter");
	//-------------------------------------------------------------------------
	//	事件名称（鼠标移出超链接位置）
	const String MultiLineBox::HyperLink_MouseLeave("HyperLink_MouseLeave");
	//-------------------------------------------------------------------------
	//	事件名称（鼠标点击文字位置）
	const String MultiLineBox::Text_MouseClick("Text_MouseClick");
	//-------------------------------------------------------------------------
	//	事件名称（鼠标移进文字位置）
	const String MultiLineBox::Text_MouseEnter("Text_MouseEnter");
	//-------------------------------------------------------------------------
	//	事件名称（鼠标移出文字位置）
	const String MultiLineBox::Text_MouseLeave("Text_MouseLeave");
	//-------------------------------------------------------------------------
	//	子部件名称（垂直滚动条部件）
	const String MultiLineBox::VertScrollbarNameSuffix( "__auto_vscrollbar__" );
	//-------------------------------------------------------------------------
	//	构造函数
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
	//	析构函数
	MultiLineBox::~MultiLineBox(void)
	{
		_destroyLineElement();
		_destroyAllElements();
	}
	//-------------------------------------------------------------------------
	//	初始化窗口
	void MultiLineBox::initialiseComponents(void)
	{
		setSize(UVector2(UDim(0.0f, 100), UDim(0.0f, 50)));	//	初始控件大小
		Scrollbar* vertScrollbar = getVertScrollbar();
		vertScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&MultiLineBox::handle_scrollChange, this));
		_formatElement();
		performChildWindowLayout();
	}
	//-------------------------------------------------------------------------
	//	配置滚动条
	void MultiLineBox::_configureScrollbars(void)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		float totalHeight = _getTotalHeight();

		//	显示或隐藏滚动条
		if (0 == mScrollMode)	// 有条件的显示滚动条
		{
			if (mScrollbarVisible && (totalHeight > getRenderArea().getHeight()))
				vertScrollbar->show();
			else
				vertScrollbar->hide();
		}
		else if (1 == mScrollMode)	// 总是显示滚动条
		{
			if (mScrollbarVisible)
				vertScrollbar->show();
			else
				vertScrollbar->hide();
		}

		Rect renderArea(getRenderArea());
		//	设置滚动条的值
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
		case SP_TOP:	//	滚动条位置在最上面
			vertScrollbar->setScrollPosition(0);
			break;
		case SP_OLD:	//	滚动条在旧的位置
			vertScrollbar->setScrollPosition(vertScrollbar->getScrollPosition());
			break;
		case SP_BOTTOM:	//	滚动条位置在最下面
			vertScrollbar->setScrollPosition(vertScrollbar->getDocumentSize());
			break;
		}
		requestRedraw();	//	重绘窗口
	}
	//-------------------------------------------------------------------------
	//	对图文混排控件内容进行排版
	void MultiLineBox::_formatElement(void)
	{
		if (mElementList.empty())
		{
			_configureScrollbars();
			return;
		}
		
		//	清除所有行信息
		_destroyLineElement();
		float pAreaWidth = getRenderArea().getWidth();	//	渲染区域的宽度
		Line pLine;				//	一行
		pLine.width = 0.0f;
		pLine.height = 0.0f;
		int pLineNum = 0;		//	行号初始为0
		float pCurrPos = 0.0f;	//	元素在所在行的位置

		//	循环遍历多行文本内容元素
		for (size_t i=0; i<mElementList.size(); i++)
		{
			Element *element = mElementList[i];
			if (element->newLine)
			{
				pCurrPos = 0.0f;
				pLine.width = 0.0f;
				pLine.height = 0.0f;
			}

			//	判断元素类型
			switch (element->type)
			{
			case ET_TEXT:	//	文本
				{
					Element_Text *elementText = (Element_Text*)element;
					int startPos = 0;	//	字符串中的截取开始位置
					int endPos = 0;		//	字符串中的截取结束位置

					if (elementText->width <= pAreaWidth-pCurrPos)	//	元素长度小等于最大行宽
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

						if (0 == pLineNum)	//	元素在首行
						{
							pLine.element.push_back(line_text);
							if (pLine.height < line_text->height)
							{
								pLine.height = line_text->height;
							}
							pLine.width += line_text->width;
							mLines.push_back(pLine);	//	往行信息集合插入一行
							pLineNum++;					//	行号增加
						}
						else	//	不在首行，往后排
						{
							if (line_text->newLine)	//	元素有强制另起一行标志
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
						pCurrPos += line_text->width + ELEMENT_GAP;	//	当前位置改变
					}
					else	//	该元素长度大于最大行框
					{ 
						//	截取字符串前部
						endPos = elementText->font->getCharAtPixel(elementText->text, pAreaWidth-pCurrPos);	//	获得字符串截取结束位置
						String text = elementText->text.substr(startPos, endPos);
						startPos = endPos;	//	字符串截取开始位置改变
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
							if (pLineNum < 1)	//	当前行为第一行
							{
								pLine.element.push_back(line_text_head);
								if (pLine.height < line_text_head->height)
								{
									pLine.height = line_text_head->height;
								}
								pLine.width += line_text_head->width;
								mLines.push_back(pLine);	//	添加新的一行
								pLineNum++;	//	行号增加
							}
							else	//	当前行不是第一行
							{
								if (line_text_head->newLine)	//	新行
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
									mLines[pLineNum-1].element.push_back(line_text_head);	//	把截取的字符串添加到上一行
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

						//	截取字符串中部
						text = elementText->text.substr(startPos, elementText->text.length()-startPos);	//	截取剩下的字符串
						int lineCount = (int)(elementText->font->getTextExtent(text)/pAreaWidth);	//	整行数
						for (size_t i=0; i<elementText->text.length(); ++i)
						{
							if (elementText->font->getTextExtent(elementText->text.substr(startPos, i))>pAreaWidth)
							{
								float lineWidth = elementText->font->getTextExtent(elementText->text.substr(startPos, i-1));
								lineCount = (int)(elementText->font->getTextExtent(text)/lineWidth);
								break;
							}
						}
						//	往行信息集合插入行
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
							pLineNum++;	//	行号增加
						}

						//	截取字符串后部
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
			case ET_IMAGE:	//	图像
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

					if (elementImage->width <= pAreaWidth-pCurrPos)	//	元素长度小等于最大行宽
					{	
						if (pLineNum == 0)	//	元素在首行
						{
							pLine.element.push_back(line_image);
							if (pLine.height < line_image->height)
							{
								pLine.height = line_image->height;
							}
							pLine.width += line_image->width;
							mLines.push_back(pLine);	//	往行信息集合插入一行
							pLineNum++;					//	行号增加
						}
						else	//	元素不在首行，往后排
						{
							if (line_image->newLine)	//	元素有强制另起一行标志
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
						pCurrPos += line_image->width + ELEMENT_GAP;	//	当前位置改变
					}
					else	//	元素长度大于最大行宽
					{
						pLine.element.clear();
						pLine.element.push_back(line_image);
						pLine.height = line_image->height;
						pLine.width += line_image->width;
						mLines.push_back(pLine);
						pLineNum++;
						pCurrPos = line_image->width + ELEMENT_GAP;	//	当前位置改变
					}
				}	//	end of ET_IMAGE
				break;
			case ET_HYPERLINK:	//	超链接
				{
					Element_HyperLink *elementHyperLink = (Element_HyperLink*)element;
					int endPos = 0;		//	字符串中的截取结束位置
					int startPos = 0;	//	字符串中的截取开始位置

					if (elementHyperLink->width <= pAreaWidth-pCurrPos)	//	元素长度小等于最大行宽
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

						if (pLineNum == 0)	//	元素在首行
						{
							pLine.element.push_back(line_hyperlink);
							if (pLine.height < line_hyperlink->height)
							{
								pLine.height = line_hyperlink->height;
							}
							pLine.width += line_hyperlink->width;
							mLines.push_back(pLine);	//	往行信息集合插入一行
							pLineNum++;					//	行号增加
						}
						else	//	元素不在首行，往后排
						{
							if (line_hyperlink->newLine)	//	元素有强制另起一行标志
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
						pCurrPos += line_hyperlink->width + ELEMENT_GAP;	//	当前位置改变
					}
					else	//	元素长度大于最大行宽
					{ 
						//	截取字符串前部
						endPos = elementHyperLink->font->getCharAtPixel(elementHyperLink->text, pAreaWidth-pCurrPos);	//	获得字符串截取结束位置
						String text = elementHyperLink->text.substr(startPos, endPos);
						startPos = endPos;	//	字符串截取开始位置改变
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

							if (pLineNum < 1)	//	当前行为第一行
							{
								pLine.element.push_back(line_hyperlink_head);
								if (pLine.height < line_hyperlink_head->height)
								{
									pLine.height = line_hyperlink_head->height;
								}
								pLine.width += line_hyperlink_head->width;
								mLines.push_back(pLine);	//	添加新的一行
								pLineNum++;	//	行号增加
							}
							else	//	当前行不是第一行
							{
								if (element->newLine)	//	新行
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
									mLines[pLineNum-1].element.push_back(line_hyperlink_head);	//	把截取的字符串添加到上一行
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

						//	截取字符串中部
						text = elementHyperLink->text.substr(startPos, elementHyperLink->text.length()-startPos);	//	截取剩下的字符串
						int lineCount = (int)(elementHyperLink->font->getTextExtent(text)/pAreaWidth);	//	整行数
						for (size_t i=0; i<elementHyperLink->text.length(); ++i)
						{
							if (elementHyperLink->font->getTextExtent(elementHyperLink->text.substr(startPos, i))>pAreaWidth)
							{
								float lineWidth = elementHyperLink->font->getTextExtent(elementHyperLink->text.substr(startPos, i-1));
								lineCount = (int)(elementHyperLink->font->getTextExtent(text)/lineWidth);
								break;
							}
						}
						//	往行信息集合插入行
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

						//	截取字符串后部
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

					if (elementWin->width <= pAreaWidth-pCurrPos)	//	元素长度小等于最大行宽
					{	
						if (pLineNum == 0)	//	元素在首行
						{
							pLine.element.push_back(line_win);
							if (pLine.height < line_win->height)
							{
								pLine.height = line_win->height;
							}
							pLine.width += line_win->width;
							mLines.push_back(pLine);	//	往行信息集合插入一行
							pLineNum++;					//	行号增加
						}
						else	//	元素不在首行，往后排
						{
							if (line_win->newLine)	//	元素有强制另起一行标志
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
						pCurrPos += line_win->width + ELEMENT_GAP;	//	当前位置改变
					}
					else	//	元素长度大于最大宽度
					{
						pLine.element.clear();
						pLine.element.push_back(line_win);
						pLine.height = line_win->height;
						pLine.width += line_win->width;
						mLines.push_back(pLine);
						pLineNum++;
						pCurrPos = line_win->width + ELEMENT_GAP;	//	当前位置改变
					}
				}	//	end of ET_WIN
				break;
			}	//	end of switch
			pLine.width = 0.0f;
			pLine.height = 0.0f;
		}	//	end of loop for

		_configureScrollbars();	//	配置滚动条
	}
	//-------------------------------------------------------------------------
	//	鼠标按下
	void MultiLineBox::onMouseButtonDown(MouseEventArgs& e)
	{
		Window::onMouseButtonDown(e);
		if (e.button == LeftButton)
		{
			captureInput();
			int index = _getIndexFromPosition(e.position);
			if ((-1 != index) && (-2 != index))	//	索引位置有效
			{
				if ((index < 0) || ((size_t)index >= mElementList.size()))
					return;
				if (ET_HYPERLINK == mElementList[index]->type)
				{
					Element_HyperLink *elementHyperLink = (Element_HyperLink*)mElementList[index];
					mHyperLinkId = elementHyperLink->id;
					fireEvent(HyperLink_MouseClick, e, EventNamespace);	//	触发鼠标点击超链接位置
				}
				else if (ET_TEXT == mElementList[index]->type)
				{
					Element_Text *elementText = (Element_Text*)mElementList[index];
					mTextContent = elementText->text.c_str();
					fireEvent(Text_MouseClick, e, EventNamespace);	//	触发鼠标点击文本位置
				}
			}
		}
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	鼠标弹起
	void MultiLineBox::onMouseButtonUp(MouseEventArgs& e)
	{
		Window::onMouseButtonUp(e);
		if (e.button == LeftButton)	//	鼠标左键有效
		{
			releaseInput();
		}
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	鼠标移动
	void MultiLineBox::onMouseMove(MouseEventArgs& e)
	{
		Window::onMouseMove(e);
		fireEvent(HyperLink_MouseLeave, e, EventNamespace);	//	触发鼠标移出超链接位置（HyperLink_MoveLeave放在HyperLink_MoveEnter前）
		fireEvent(Text_MouseLeave, e, EventNamespace);	//	触发鼠标移出文本位置（Text_MouseEnter放在Text_MouseLeave前）
		int index = _getIndexFromPosition(e.position);
		if ((-1 != index) && (-2 != index))	//	索引位置有效
		{
			if ((index < 0) || ((size_t)index >= mElementList.size()))
				return;
			if (ET_HYPERLINK == mElementList[index]->type)
			{
				Element_HyperLink *elementHyperLink = (Element_HyperLink*)mElementList[index];
				mHyperLinkId = elementHyperLink->id;
				fireEvent(HyperLink_MouseEnter, e, EventNamespace);	//	触发鼠标移进超链接位置
			}
			else if (ET_TEXT == mElementList[index]->type)
			{
				Element_Text *elementText = (Element_Text*)mElementList[index];
				mTextContent = elementText->text.c_str();
				fireEvent(Text_MouseEnter, e, EventNamespace);	//	触发鼠标移进文本位置
			}
		}
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	鼠标进入
	void MultiLineBox::onMouseEnters(MouseEventArgs& e)
	{
		Window::onMouseEnters(e);
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	鼠标离开
	void MultiLineBox::onMouseLeaves(MouseEventArgs& e)
	{
		Window::onMouseLeaves(e);
		fireEvent(HyperLink_MouseLeave, e, EventNamespace);	//	触发鼠标移出超链接位置（HyperLink_MoveLeave放在HyperLink_MoveEnter前）
		fireEvent(Text_MouseLeave, e, EventNamespace);	//	触发鼠标移出文本位置（Text_MouseEnter放在Text_MouseLeave前）
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	鼠标滚动
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
	//	窗口大小改变
	void MultiLineBox::onSized(WindowEventArgs& e)
	{
		Window::onSized(e);
		e.handled = true;
		_formatElement();	//	重新排版
	}
	//-------------------------------------------------------------------------
	//	捕获
	void MultiLineBox::onCaptureLost(WindowEventArgs& e)
	{
		Window::onCaptureLost(e);
		e.handled = true;
	}
	//-------------------------------------------------------------------------
	//	滚动条状态改变
	bool MultiLineBox::handle_scrollChange(const EventArgs& args)
	{
		requestRedraw();
		return true;
	}
	//-------------------------------------------------------------------------
	//	获取垂直滚动条
	Scrollbar* MultiLineBox::getVertScrollbar(void)
	{
		Scrollbar *scrBar = static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(getName() + VertScrollbarNameSuffix));
		return scrBar;
	}
	//-------------------------------------------------------------------------
	//	垂直滚动条是否可见
	bool MultiLineBox::isScrollbarVisible(void)
	{
		return mScrollbarVisible;
	}
	//-------------------------------------------------------------------------
	//	获取图文混排控件渲染区域
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
	//	获取渲染区域宽度
	float MultiLineBox::getRenderWidth(void)
	{
		return getRenderArea().getWidth();
	}
	//-------------------------------------------------------------------------
	//	获取渲染区域高度
	float MultiLineBox::getRenderHeight(void)
	{
		return getRenderArea().getHeight();
	}
	//-------------------------------------------------------------------------
	//	获取行信息集合
	MultiLineBox::LineList MultiLineBox::getLineList(void)
	{
		return mLines;
	}
	//-------------------------------------------------------------------------
	//	获取排版后的高度
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
	//	根据鼠标位置获取索引
	int MultiLineBox::_getIndexFromPosition(Point pt)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;

		Point wndPt = CoordConverter::screenToWindow(*this, pt);
		wndPt.d_y += vertScrollbar->getScrollPosition();

		float height = getHeightInterval();
		Rect rendedArea(getRenderArea());
		// 判断垂直对齐方式
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
		for (size_t i=0; i<line_list.size(); i++)	//	行数
		{
			Line line = line_list[i];

			if (0.0f == mEachLineHeight)	// 每行高度不一样
			{
				height += line.height;
			} 
			else	// 每行一样高度
			{
				height += mEachLineHeight;
			}
			if (0 != i)	// 不是第一行
			{
				height += LINE_GAP;
			}

			if (wndPt.d_y <= height)	// 确定行号
			{
				float width = 0;
				if (mScrollbarVisible)	// 有滚动条
				{
					switch (vertScrollbar->getHorizontalAlignment())	//	获取垂直滚动条的对齐方式
					{
					case HA_LEFT:	//	滚动条在左边
						width = getWidthInterval();
						break;
					case HA_RIGHT:	//	滚动条在右边
						width = 0;
						break;
					}
				}

				// 判断水平对齐方式
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

				for (size_t j=0; j<line.element.size(); j++)	//	当前行的元素数
				{
					Element *element = line.element[j];
					width += element->width;
					if (0 != j)	// 不是当前行的第一个元素
					{
						width += ELEMENT_GAP;
					}

					if (wndPt.d_x <= width)	// 确定元素
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
						return element->index;	//	返回元素在行信息集合中的索引号
					}
				}
				return -2;	//	索引位置无效
			}
		}
		return -2;	//	索引位置无效
	}
	//-------------------------------------------------------------------------
	//	把字符串转换为颜色
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
	//	添加文本
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
		if (String::npos != (pos=text.find('\n')))	//	判断文本中的换行符
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
		if (mSpecSideFlag)	//	设置描边
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
	//	添加文本
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
	//	添加文本
	int MultiLineBox::addElementText(std::string text, std::string cols, bool newLine)
	{
		colour tCols(_extractColorFromString(cols));
		if (-1 == addElementText(mTextFont, (utf8*)text.c_str(), tCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	添加文本
	int MultiLineBox::addElementText(std::string text, bool newLine)
	{
		if (-1 == addElementText(mTextFont, (utf8*)text.c_str(), mTextCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	添加图像
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
	//	添加图像
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
	//	添加超链接
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
		if (mSpecSideFlag)	//	设置描边
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
	//	添加超链接
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
	//	添加超链接
	int MultiLineBox::addElementHyperLink(std::string text, std::string id, std::string cols, bool newLine)
	{
		colour tCols(_extractColorFromString(cols));
		if (-1 == addElementHyperLink(mTextFont, (utf8*)text.c_str(), id, tCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	添加超链接
	int MultiLineBox::addElementHyperLink(std::string text, std::string id, bool newLine)
	{
		if (-1 == addElementHyperLink(mTextFont, (utf8*)text.c_str(), id, mTextCols, newLine))
			return -1;
		return 0;
	}
	//-------------------------------------------------------------------------
	//	添加控件
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
	//	添加控件
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
	//	设置控件字体（对控件所有内容有效）
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
	//	设置文本颜色（对控件所有内容有效）
	void MultiLineBox::setColsFunc(std::string cols)
	{
		mTextCols = _extractColorFromString(cols);
		mTextColsName = cols;
		mColsFlag = true;
	}
	//-------------------------------------------------------------------------
	//	设置描边颜色（对控件所有内容有效）
	void MultiLineBox::setSideFunc(std::string cols, float sPix)
	{
		mSideCols = _extractColorFromString(cols);
		mSidePix = sPix;
		mSideFlag = true;
	}
	//-------------------------------------------------------------------------
	//	设置特定元素的描边（只对特定元素有效）
	void MultiLineBox::setSpecSide(colour sCols, float sPix)
	{
		mSpecSideCols = sCols;
		mSpecSidePix = sPix;
		mSpecSideFlag = true;
	}
	//-------------------------------------------------------------------------
	//	设置特定元素的描边（只对特定元素有效）
	void MultiLineBox::setSpecSide(std::string cols, float sPix)
	{
		colour sCols(_extractColorFromString(cols));
		setSpecSide(sCols, sPix);
	}
	//-------------------------------------------------------------------------
	//	设置对齐方式
	int MultiLineBox::setAlign(std::string alignType)
	{
		if ("AT_LEFT" == alignType)	//	判断水平
			mAlignHor = AT_LEFT;
		else if ("AT_CENTER_HOR" == alignType)
			mAlignHor = AT_CENTER_HOR;
		else if ("AT_RIGHT" == alignType)
			mAlignHor = AT_RIGHT;
		else if ("AT_TOP" == alignType)	//	判断垂直
			mAlignVer = AT_TOP;
		else if ("AT_CENTER_VER" == alignType)
			mAlignVer = AT_CENTER_VER;
		else if ("AT_BOTTOM" == alignType)
			mAlignVer = AT_BOTTOM;
		else
			return -1;	//	alignType格式错误，设置失败

		return 0;	//	设置成功
	}
	//-------------------------------------------------------------------------
	//	强制设置每行的高度
	void MultiLineBox::setEachLineHeight(float height)
	{
		mEachLineHeight = height;
	}
	//-------------------------------------------------------------------------
	//	设置垂直滚动条可见
	int MultiLineBox::setScrollbarVisible(std::string pos, bool visible, int mode)
	{
		if ("SP_TOP" == pos)
			mScrollbarPos = SP_TOP;
		else if ("SP_OLD" == pos)
			mScrollbarPos = SP_OLD;
		else if ("SP_BOTTOM" == pos)
			mScrollbarPos = SP_BOTTOM;
		else
			return -1;	//	pos格式错误，设置失败

		if ((0 != mode) && (1 != mode))
			return -1;	//	mode格式错误，设置失败

		mScrollMode = mode;
		mScrollbarVisible = visible;
		_configureScrollbars();
		return 0;	//	设置成功
	}
	//-------------------------------------------------------------------------
	//	设置垂直滚动条位置
	int MultiLineBox::setScrollbarPosition(float pos)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;
		vertScrollbar->setScrollPosition(pos);
		return 0;
	}
	//-------------------------------------------------------------------------
	//	设置控件的最大大小
	void MultiLineBox::setMaxSizeFunc(float maxWidth, float maxHeight)
	{
		setMaxSize(UVector2(UDim(0.0f, maxWidth), UDim(0.0f, maxHeight)));
	}
	//-------------------------------------------------------------------------
	//	设置控件宽度
	void MultiLineBox::setWidthFunc(float width)
	{
		setWidth(UDim(0.0f, width));
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	设置控件高度
	void MultiLineBox::setHeightFunc(float height)
	{
		setHeight(UDim(0.0f, height));
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	设置控件大小
	void MultiLineBox::setSizeFunc(float width, float height)
	{
		setSize(UVector2(UDim(0.0f, width), UDim(0.0f, height)));
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	设置控件位置
	void MultiLineBox::setPositionFunc(float x, float y)
	{
		setPosition(UVector2(UDim(0.0f, x), UDim(0.0f, y)));
	}
	//-------------------------------------------------------------------------
	//	设置控件是否可见
	void MultiLineBox::setVisibleFunc(bool visible)
	{
		setVisible(visible);
	}
	//-------------------------------------------------------------------------
	//	获取控件名字
	std::string MultiLineBox::getNameFunc(void)
	{
		String name = getName();
		std::string ctrlName = name.c_str();
		return ctrlName;
	}
	//-------------------------------------------------------------------------
	//	获取字体名字
	std::string MultiLineBox::getFontName(void)
	{
		return mTextFontName;
	}
	//-------------------------------------------------------------------------
	//	获取文字颜色
	std::string MultiLineBox::getColour(void)
	{
		return mTextColsName;
	}
	//-------------------------------------------------------------------------
	//	获取水平对齐方式
	MultiLineBox::AlignType MultiLineBox::getAlignHor(void)
	{
		return mAlignHor;
	}
	//-------------------------------------------------------------------------
	//	获取垂直对齐方式
	MultiLineBox::AlignType MultiLineBox::getAlignVer(void)
	{
		return mAlignVer;
	}
	//-------------------------------------------------------------------------
	//	获取超链接内容的标识
	std::string MultiLineBox::getHyperLinkId(void)
	{
		return mHyperLinkId;
	}
	//-------------------------------------------------------------------------
	//	获取响应的文本内容
	std::string MultiLineBox::getTextContent(void)
	{
		return mTextContent;
	}
	//-------------------------------------------------------------------------
	//	获取响应的矩形区域
	std::string MultiLineBox::getTriggerRect(void)
	{
		return mTriggerRect;
	}
	//-------------------------------------------------------------------------
	//	获取内容排版后控件所需的高度
	float MultiLineBox::getFormatHeight(void)
	{
		float totalHeight = _getTotalHeight();
		return totalHeight + getHeightInterval();
	}
	//-------------------------------------------------------------------------
	//	获取内容排版后控件的行数
	int MultiLineBox::getLineSize(void)
	{
		int lines = mLines.size();
		return lines;
	}
	//-------------------------------------------------------------------------
	//	获取强制设置每行的高度
	float MultiLineBox::getEachLineHeight(void)
	{
		return mEachLineHeight;
	}
	//-------------------------------------------------------------------------
	//	获取控件宽度与渲染宽度的间隙
	float MultiLineBox::getWidthInterval(void)
	{
		float areaWidth = getArea().getWidth().asAbsolute(mScreenWidth);
		float renderAreaWidth = getRenderArea().getWidth();
		return areaWidth - renderAreaWidth;
	}
	//-------------------------------------------------------------------------
	//	获取控件高度与渲染高度的间隙
	float MultiLineBox::getHeightInterval(void)
	{
		float areaHeight = getArea().getHeight().asAbsolute(mScreenHeight);
		float renderAreaHeight = getRenderArea().getHeight();
		return areaHeight - renderAreaHeight;
	}
	//-------------------------------------------------------------------------
	//	根据text获取控件的合适宽度
	float MultiLineBox::getFitWidth(std::string fontName, std::string text)
	{
		float ctrlWidth = getStrExtent(fontName, text) + getWidthInterval() + 1.0f;
		return ctrlWidth;
	}
	//-------------------------------------------------------------------------
	//	根据文本的宽度度获得控件的合适宽度
	float MultiLineBox::getFitWidth(float textLen)
	{
		float ctrlWidth = textLen + getWidthInterval() + 1.0f;
		return ctrlWidth;
	}
	//-------------------------------------------------------------------------
	//	获取控件内容总长度
	float MultiLineBox::getLength(void)
	{
		return mLength;
	}
	//-------------------------------------------------------------------------
	//	获取此时滚动页面大小
	float MultiLineBox::getScrollbarDocumentSize(void)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;
		return vertScrollbar->getDocumentSize();
	}
	//-------------------------------------------------------------------------
	//	获取此时滚动条的位置
	float MultiLineBox::getScrollbarPosition(void)
	{
		Scrollbar* vertScrollbar = getVertScrollbar();
		if (NULL == vertScrollbar)
			return -1;
		return vertScrollbar->getScrollPosition();
	}
	//-------------------------------------------------------------------------
	//	查找内容为text的文本元素
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
	//	清除内容
	void MultiLineBox::clearUp(void)
	{
		_destroyLineElement();
		_destroyAllElements();
		_formatElement();
	}
	//-------------------------------------------------------------------------
	//	销毁行元素
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
	//	销毁所有元素
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
	//	清除id对应的图文混排控件内容
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
				//	移除一个元素后，其后面的元素的索引号要减1
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
	//	创建窗口
	Window* MultiLineBoxFactory::createWindow(const String& name)
	{
		MultiLineBox *mlb = new MultiLineBox(d_type, name);
		return mlb;
	}
	//-------------------------------------------------------------------------
	//	销毁窗口
	void MultiLineBoxFactory::destroyWindow(Window* window)
	{
		delete window;
	}
	//-------------------------------------------------------------------------
	//	把window转换为MultiLinebox类型
	MultiLineBox* toMultiLineBox(std::string ctrlName)
	{
		Window *win = WindowManager::getSingleton().getWindow(ctrlName);
		if (NULL == win)
			return NULL;
		MultiLineBox *mlb = static_cast<MultiLineBox*>(win);
		return mlb;
	}
	//-------------------------------------------------------------------------
	//	根据字体font获取字符串str的长度
	float getStrExtent(std::string fontName, std::string str)
	{
		Font *fnt = FontManager::getSingleton().getFont(fontName);
		if (NULL == fnt)
			return -1;
		float len = fnt->getTextExtent((utf8*)str.c_str());
		return len;
	}
	//-------------------------------------------------------------------------
	//	根据像素获取截取的字符位置
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
	//	截取字符串
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
	//	获取字符串str中的字符个数
	size_t getCharacterCount(std::string str)
	{
		int characterCount = 0;	//	字符个数
		int charCount = 0;		//	非中文符号个数
		int chineseCount = 0;	//	中文符号个数

		for (size_t i=0; i<str.length(); i++)
		{
			if (str[i] < 0)	//	中文符号占三个字节，每个字节都小于0
			{
				if (str[i] == -62)	//	有特殊符号·此符号占2个字节-62和-73
				{
					charCount++;
					++i;
					continue;
				}
				chineseCount++;
			} 
			else	//	非中文符号
			{
				charCount++;
			}
		}

		characterCount = charCount + chineseCount/3;
		return characterCount;
	}
	//-------------------------------------------------------------------------
	//	截取字符串str的字串，以字符个数为单位
	std::string subCharacter(std::string str, size_t startPos, int len)
	{
		std::vector<std::string> strVec;

		for (size_t i=0; i<str.length(); i++)
		{
			if (str[i] < 0)	//	中文符号占三个字节，每个字节都小于0
			{
				std::string tmp = "";
				if (str[i] == -62)	//	有特殊符号·此符号占2个字节-62和-73
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
			else	//	非中文符号
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
	//	获取窗口坐标的各个值
	float getPositionVal(std::string winname, int xy, int pos)
	{
		if ((0 != xy) && (1 != xy) && (0 != pos) && (1 != pos))
			return -1;

		CEGUI::Window *win = CEGUI::WindowManager::getSingleton().getWindow(winname);
		if (NULL == win)
			return -2;

		CEGUI::UDim xyPos(0, 0);
		if (0 == xy)	//	x坐标
			xyPos = win->getPosition().d_x;
		else if (1 == xy)	//	y坐标
			xyPos = win->getPosition().d_y;

		float val = 0;
		if (0 == pos)	//	scale
			val = xyPos.d_scale;
		else if (1 == pos)	//	offset
			val = xyPos.d_offset;

		return val;
	}

} //	end of namespace CEGUI
