/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-26
* 描述: 负责解析图文混排控件所要用到的一些转义符号
******************************************************************************/
#include "EQCommPCH.h"
#include "MultiLineBoxParse.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "CEGUI.h"


//-----------------------------------------------------------------------
//	构造函数
MultiLineBoxParse::MultiLineBoxParse() :
mGap(1.0),
mHasSide(false),
mNewLine(false),
mLinkId("")
{
}
//-----------------------------------------------------------------------
//	析构函数
MultiLineBoxParse::~MultiLineBoxParse()
{
}
//-----------------------------------------------------------------------
//	解析xml字符串
int MultiLineBoxParse::_parseXmlString(std::string str, CEGUI::MultiLineBox::ElementList* dataList, std::string fontName, std::string col)
{
	if ((""==str) || (NULL == dataList) || (""==fontName) || ("" == col))
	{
		return -1;
	}
	std::string val = "<value>" + str + "</value>";
	TiXmlDocument doc;
	doc.Parse(val.c_str());
	TiXmlElement *root = doc.RootElement();
	if (NULL == root)
	{
		return -1;
	}
	TiXmlNode *node = root->FirstChild();
	if (NULL == node)
	{
		return -1;
	}
	CEGUI::Font *fnt = NULL;
	if (CEGUI::FontManager::getSingleton().isFontPresent(fontName))
	{
		fnt = CEGUI::FontManager::getSingleton().getFont(fontName);
	}
	else
	{
		fnt = CEGUI::FontManager::getSingleton().createFont(fontName+".font");
	}
	if (NULL == fnt)
	{
		return -1;
	}
	Ogre::ColourValue ogreCols(extractColorFromString(col));
	CEGUI::colour color(ogreCols.r, ogreCols.g, ogreCols.b);
	if (-1 == _parseXmlNode(node, dataList, fnt, color))
	{
		return -1;
	}
	return 0;
}
//-----------------------------------------------------------------------
//	解析xml结点
int MultiLineBoxParse::_parseXmlNode(TiXmlNode* node, CEGUI::MultiLineBox::ElementList* dataList, CEGUI::Font* fnt, CEGUI::colour color)
{
	if ((NULL==node) || (dataList==NULL) || (NULL==fnt))
	{
		return -1;
	}
	mDataList = dataList;
	while (node)
	{
		int res = 0;
		if (0 == strcmp(node->Value(), "img"))		//	图像
			res = _processImage(node);
		else if (0 == strcmp(node->Value(), "lnk"))	//	超链接
			res = _processHyperLink(node, fnt, color);
		else if (0 == strcmp(node->Value(), "ani"))	//	动画
			res = _processAnimate(node);
		else if (0 == strcmp(node->Value(), "fnt"))	//	字体
			res = _processFont(node, color);
		else if (0 == strcmp(node->Value(), "clr"))	//	颜色
			res = _processColor(node, fnt);
		else if (0 == strcmp(node->Value(), "sid"))	//	描边
			res = _processSide(node, fnt, color);
		else if (0 == strcmp(node->Value(), "sp"))	//	空格
			res = _processSP(fnt, color);
		else if (0 == strcmp(node->Value(), "br"))	//	回车
			res = _processBR(node, fnt, color);
		else	//	纯文本
			res = _processText(node, fnt, color);
		// 某个解析出现错误
		if (-1 == res)
		{
			return -1;
		}
		node = node->NextSibling();
	}
	return 0;
}
//-----------------------------------------------------------------------
//	解析纯文本
int MultiLineBoxParse::_processText(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols)
{
	if ((NULL==node) || (NULL==fnt))
	{
		return -1;
	}
	std::string text = node->Value();
	CEGUI::MultiLineBox::Element_Text *elementText = new CEGUI::MultiLineBox::Element_Text;
	elementText->type = CEGUI::MultiLineBox::ET_TEXT;
	elementText->font = fnt;
	elementText->newLine = mNewLine;
	mNewLine = false;
	elementText->textCols = cols;
	elementText->sideFlag = mHasSide;
	if (elementText->sideFlag)
	{
		elementText->sideCols = mSideCols;
		elementText->sidePix = mGap;
	}
	elementText->text = (CEGUI::utf8*)text.c_str();
	mDataList->push_back(elementText);
	return 0;
}
//-----------------------------------------------------------------------
//	解析图像
int MultiLineBoxParse::_processImage(TiXmlNode* node)
{
	if (NULL == node)
	{
		return -1;
	}
	std::string imgSetName = xmlGetAttrib((TiXmlElement*)node, "src");
	std::string imgName = xmlGetAttrib((TiXmlElement*)node, "id");
	float imgWidth = xmlGetAttribFloat((TiXmlElement*)node, "w");
	float imgHeight = xmlGetAttribFloat((TiXmlElement*)node, "h");
	CEGUI::Imageset *imgSet = NULL;
	if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imgSetName))
	{
		imgSet = CEGUI::ImagesetManager::getSingleton().getImageset(imgSetName);
	}
	else
	{
		imgSet = CEGUI::ImagesetManager::getSingleton().createImageset(imgSetName + ".imageset");
	}
	if (NULL == imgSet)
	{
		return -1;
	}
	CEGUI::Image img = imgSet->getImage(imgName);
	CEGUI::MultiLineBox::Element_Image *elementImage = new CEGUI::MultiLineBox::Element_Image;
	elementImage->type = CEGUI::MultiLineBox::ET_IMAGE;
	elementImage->image = img;
	elementImage->width = imgWidth;
	elementImage->height = imgHeight;
	elementImage->newLine = mNewLine;
	mNewLine = false;
	mDataList->push_back(elementImage);
	return 0;
}
//-----------------------------------------------------------------------
//	解析超链接文本
int MultiLineBoxParse::_processHyperLinkText(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols)
{
	if ((NULL==node) && (NULL==fnt))
	{
		return -1;
	}
	std::string text = node->Value();
	CEGUI::MultiLineBox::Element_HyperLink *elementHyperLinkText = new CEGUI::MultiLineBox::Element_HyperLink;
	elementHyperLinkText->type = CEGUI::MultiLineBox::ET_HYPERLINK;
	elementHyperLinkText->font = fnt;
	elementHyperLinkText->newLine = mNewLine;
	mNewLine = false;
	elementHyperLinkText->textCols = cols;
	elementHyperLinkText->sideFlag = mHasSide;
	if (elementHyperLinkText->sideFlag)
	{
		elementHyperLinkText->sideCols = mSideCols;
		elementHyperLinkText->sidePix = mGap;
	}
	elementHyperLinkText->text = (CEGUI::utf8*)text.c_str();
	elementHyperLinkText->id = mLinkId;
	mDataList->push_back(elementHyperLinkText);
	return 0;
}
//-----------------------------------------------------------------------
//	解析超链接
int MultiLineBoxParse::_processHyperLink(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols)
{
	if ((NULL==node) && (NULL==fnt))
	{
		return -1;
	}
	mLinkId = xmlGetAttrib((TiXmlElement*)node, "id");
	TiXmlNode *child = node->FirstChild();
	if (NULL == child)
	{
		return -1;
	}
	while (child)
	{
		int res = 0;
		if (0 == strcmp(child->Value(), "fnt"))
		{
			res = _processFont(child, cols, false);
		}
		else if (0 == strcmp(child->Value(), "clr"))
		{
			res = _processColor(child, fnt, false);
		}
		else if (0 == strcmp(child->Value(), "sid"))
		{
			res = _processSide(child, fnt, cols, false);
		}
		else
		{
			res = _processHyperLinkText(child, fnt, cols);
		}
		if (-1 == res)
		{
			return -1;
		}
		child = child->NextSibling();
	}
	return 0;
}
//-----------------------------------------------------------------------
//	解析动画
int MultiLineBoxParse::_processAnimate(TiXmlNode* node)
{
	if (NULL == node)
	{
		return -1;
	}
	int  elapsed = xmlGetAttribInt((TiXmlElement*)node, "t", 200);
	float width = xmlGetAttribFloat((TiXmlElement*)node, "w", 16);
	float height = xmlGetAttribFloat((TiXmlElement*)node, "h", 16);
	CEGUI::Animate ani;
	ani.initAnimate(elapsed, width, height);
	TiXmlNode *child = node->FirstChild();
	if (NULL == child)
	{
		return -1;
	}
	while (child)
	{
		if (0 == strcmp(child->Value(), "img"))
		{
			std::string imgSetName = xmlGetAttrib((TiXmlElement*)child, "src");
			std::string imgName = xmlGetAttrib((TiXmlElement*)child, "id");
			CEGUI::Imageset *imgSet = NULL;
			if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imgSetName))
			{
				imgSet = CEGUI::ImagesetManager::getSingleton().getImageset(imgSetName);
			}
			else
			{
				imgSet = CEGUI::ImagesetManager::getSingleton().createImageset(imgSetName + ".imageset");
			}
			if (NULL == imgSet)
			{
				return -1;
			}
			CEGUI::Image img = imgSet->getImage(imgName);
			ani.addAnimateFrame(img);
		}
		child = child->NextSibling();
	}
	CEGUI::AnimateBox *aniBox = static_cast<CEGUI::AnimateBox*>(CEGUI::WindowManager::getSingleton().createWindow("EQHomeLook/AnimateBox"));
	if (NULL == aniBox)
	{
		return -1;
	}
	aniBox->addAnimate(ani);
	aniBox->setSizeFunc(width, height);
	CEGUI::MultiLineBox::Element_Win* elementWin = new CEGUI::MultiLineBox::Element_Win;
	elementWin->type = CEGUI::MultiLineBox::ET_WIN;
	elementWin->win = aniBox;
	elementWin->newLine = mNewLine;
	mNewLine = false;
	mDataList->push_back(elementWin);
	return 0;
}
//-----------------------------------------------------------------------
//	解析字体
int MultiLineBoxParse::_processFont(TiXmlNode* node, CEGUI::colour cols, bool isText)
{
	if (NULL == node)
	{
		return -1;
	}
	std::string fntName = xmlGetAttrib((TiXmlElement*)node, "v");
	CEGUI::Font *fnt = NULL;
	if (CEGUI::FontManager::getSingleton().isFontPresent(fntName))
	{
		fnt = CEGUI::FontManager::getSingleton().getFont(fntName);
	}
	else
	{
		fnt = CEGUI::FontManager::getSingleton().createFont(fntName+".font");
	}
	if (NULL == fnt)
	{
		return -1;
	}
	TiXmlNode *child = node->FirstChild();
	if (NULL == child)
	{
		return -1;
	}
	while (child)
	{
		int res = 0;
		if (0 == strcmp(child->Value(), "img"))
		{
			res = _processImage(child);
		}
		else if (0 == strcmp(child->Value(), "lnk"))
		{
			res = _processHyperLink(child, fnt, cols);
		}
		else if (0 == strcmp(child->Value(), "ani"))
		{
			res = _processAnimate(child);
		}
		else if (0 == strcmp(child->Value(), "fnt"))
		{
			res = _processFont(child, cols, isText);
		} 
		else if (0 == strcmp(child->Value(), "clr"))
		{
			res = _processColor(child, fnt, isText);
		}
		else if (0 == strcmp(child->Value(), "sid"))
		{
			res = _processSide(child, fnt, cols, isText);
		}
		else if (0 == strcmp(child->Value(), "sp"))
		{
			res = _processSP(fnt, cols);
		}
		else if (0 == strcmp(child->Value(), "br"))
		{
			res = _processBR(child, fnt, cols);
		}
		else
		{
			if (isText)
			{
				res = _processText(child, fnt, cols);
			}
			else
			{
				res = _processHyperLinkText(child, fnt, cols);
			}
		}
		if (-1 == res)
		{
			return -1;
		}
		child = child->NextSibling();
	}
	return 0;
}
//-----------------------------------------------------------------------
//	解析文本颜色
int MultiLineBoxParse::_processColor(TiXmlNode* node, CEGUI::Font* fnt, bool isText)
{
	if ((NULL==node) && (NULL==fnt))
	{
		return -1;
	}
	std::string xmlCols = xmlGetAttrib((TiXmlElement*)node, "v");
	Ogre::ColourValue ogreCols(extractColorFromString(xmlCols));
	float r = ogreCols.r;
	float g = ogreCols.g;
	float b = ogreCols.b;
	CEGUI::colour cols(r, g, b);
	TiXmlNode *child = node->FirstChild();
	if (NULL == child)
	{
		return -1;
	}
	while (child)
	{
		int res = 0;
		if (0 == strcmp(child->Value(), "img"))
		{
			res = _processImage(child);
		}
		else if (0 == strcmp(child->Value(), "lnk"))
		{
			res = _processHyperLink(child, fnt, cols);
		}
		else if (0 == strcmp(child->Value(), "ani"))
		{
			res = _processAnimate(child);
		}
		else if (0 == strcmp(child->Value(), "fnt"))
		{
			res = _processFont(child, cols, isText);
		} 
		else if (0 == strcmp(child->Value(), "clr"))
		{
			res = _processColor(child, fnt, isText);
		}
		else if (0 == strcmp(child->Value(), "sid"))
		{
			res = _processSide(child, fnt, cols, isText);
		}
		else if (0 == strcmp(child->Value(), "sp"))
		{
			res = _processSP(fnt, cols);
		}
		else if (0 == strcmp(child->Value(), "br"))
		{
			res = _processBR(child, fnt, cols);
		}
		else
		{
			if (isText)
			{
				res = _processText(child, fnt, cols);
			}
			else
			{
				res = _processHyperLinkText(child, fnt, cols);
			}
		}
		if (-1 == res)
		{
			return -1;
		}
		child = child->NextSibling();
	}
	return 0;
}
//-----------------------------------------------------------------------
//	解析描边颜色
int MultiLineBoxParse::_processSide(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols, bool isText)
{
	if ((NULL==node) && (NULL==fnt))
	{
		return -1;
	}
	std::string xmlSide = xmlGetAttrib((TiXmlElement*)node, "v");
	float gap = xmlGetAttribFloat((TiXmlElement*)node, "g");
	Ogre::ColourValue ogreSide(extractColorFromString(xmlSide));
	float r = ogreSide.r;
	float g = ogreSide.g;
	float b = ogreSide.b;
	CEGUI::colour side(r, g, b);
	mHasSide = true;
	mSideCols = side;
	mGap = gap;
	TiXmlNode *child = node->FirstChild();
	if (NULL == child)
	{
		return -1;
	}
	while (child)
	{
		int res = 0;
		if (0 == strcmp(child->Value(), "img"))
		{
			res = _processImage(child);
		}
		else if (0 == strcmp(child->Value(), "lnk"))
		{
			res = _processHyperLink(child, fnt, cols);
		}
		else if (0 == strcmp(child->Value(), "ani"))
		{
			res = _processAnimate(child);
		}
		else if (0 == strcmp(child->Value(), "fnt"))
		{
			res = _processFont(child, cols, isText);
		} 
		else if (0 == strcmp(child->Value(), "clr"))
		{
			res = _processColor(child, fnt, isText);
		}
		else if (0 == strcmp(child->Value(), "sid"))
		{
			res = _processSide(child, fnt, cols, isText);
		}
		else if (0 == strcmp(child->Value(), "sp"))
		{
			res = _processSP(fnt, cols);
		}
		else if (0 == strcmp(child->Value(), "br"))
		{
			res = _processBR(child, fnt, cols);
		}
		else
		{
			if (isText)
			{
				res = _processText(child, fnt, cols);
			}
			else
			{
				res = _processHyperLinkText(child, fnt, cols);
			}
		}
		if (-1 == res)
		{
			return -1;
		}
		child = child->NextSibling();
	}
	mHasSide = false;
	return 0;
}
//-----------------------------------------------------------------------
//	解析空格符号
int MultiLineBoxParse::_processSP(CEGUI::Font* fnt, CEGUI::colour cols)
{
	if (NULL == fnt)
	{
		return -1;
	}
	CEGUI::MultiLineBox::Element_Text *elementText = new CEGUI::MultiLineBox::Element_Text;
	elementText->type = CEGUI::MultiLineBox::ET_TEXT;
	elementText->font = fnt;
	elementText->newLine = mNewLine;
	mNewLine = false;
	elementText->textCols = cols;
	elementText->text = (CEGUI::utf8*)" ";
	mDataList->push_back(elementText);
	return 0;
}
//-----------------------------------------------------------------------
//	解析换行符号
int MultiLineBoxParse::_processBR(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols)
{
	if ((NULL==node) && (NULL==fnt))
	{
		return -1;
	}
	mNewLine = true;
	if (0 == strcmp(node->NextSibling()->Value(), "br"))
	{
		CEGUI::MultiLineBox::Element_Text *elementText = new CEGUI::MultiLineBox::Element_Text;
		elementText->type = CEGUI::MultiLineBox::ET_TEXT;
		elementText->font = fnt;
		elementText->textCols = cols;
		elementText->text = (CEGUI::utf8*)" ";
		elementText->newLine = mNewLine;
		mDataList->push_back(elementText);
	}
	return 0;
}
//-----------------------------------------------------------------------
//	获取数据到图文混排控件
int MultiLineBoxParse::_getData(CEGUI::MultiLineBox::ElementList dataList, CEGUI::MultiLineBox* mlbox)
{
	if (NULL == mlbox)
	{
		return -1;
	}
	for (size_t i=0; dataList.size()!=i; i++)
	{
		switch (dataList[i]->type)
		{
		case CEGUI::MultiLineBox::ET_TEXT:
			{
				CEGUI::MultiLineBox::Element_Text *elementText = (CEGUI::MultiLineBox::Element_Text*)dataList[i];
				CEGUI::Font *fnt = elementText->font;
				bool newLine = elementText->newLine;
				CEGUI::String text = elementText->text;
				CEGUI::colour cols = elementText->textCols;
				if (elementText->sideFlag)
				{
					mlbox->setSpecSide(elementText->sideCols, elementText->sidePix);
				}
				mlbox->addElementText(fnt, text, cols, newLine);
				delete (CEGUI::MultiLineBox::Element_Text*)dataList[i];
			}
			break;
		case CEGUI::MultiLineBox::ET_IMAGE:
			{
				CEGUI::MultiLineBox::Element_Image *elementImage = (CEGUI::MultiLineBox::Element_Image*)dataList[i];
				CEGUI::Image img = elementImage->image;
				float width = elementImage->width;
				float height = elementImage->height;
				bool newLine = elementImage->newLine;
				mlbox->addElementImage(img, width, height, newLine);
				delete (CEGUI::MultiLineBox::Element_Image*)dataList[i];
			}
			break;
		case CEGUI::MultiLineBox::ET_HYPERLINK:
			{
				CEGUI::MultiLineBox::Element_HyperLink *elementHyperLink = (CEGUI::MultiLineBox::Element_HyperLink*)dataList[i];
				CEGUI::Font *fnt = elementHyperLink->font;
				bool newLine = elementHyperLink->newLine;
				CEGUI::String text = elementHyperLink->text;
				std::string id = elementHyperLink->id;
				CEGUI::colour cols = elementHyperLink->textCols;
				if (elementHyperLink->sideFlag)
				{
					mlbox->setSpecSide(elementHyperLink->sideFlag, elementHyperLink->sidePix);
				}
				mlbox->addElementHyperLink(fnt, text, id, cols, newLine);
				delete (CEGUI::MultiLineBox::Element_HyperLink*)dataList[i];
			}
			break;
		case CEGUI::MultiLineBox::ET_WIN:
			{
				CEGUI::MultiLineBox::Element_Win *elementWin = (CEGUI::MultiLineBox::Element_Win*)dataList[i];
				CEGUI::Window *win = elementWin->win;
				bool newLine = elementWin->newLine;
				mlbox->addElementWin(win, newLine);
				delete (CEGUI::MultiLineBox::Element_Win*)dataList[i];
			}
			break;
		default:
			break;
		}
		dataList[i] = NULL;
	}
	dataList.clear();
	return 0;
}
//-----------------------------------------------------------------------
//	获取数据到图文混排控件
int MultiLineBoxParse::getData(std::string str, CEGUI::MultiLineBox* mlbox)
{
	if (NULL == mlbox)
	{
		return -1;
	}
	CEGUI::MultiLineBox::ElementList itemList;
	if (-1 == _parseXmlString(str, &itemList, mlbox->getFontName(), mlbox->getColour()))
	{
		return -1;
	}
	if (-1 == _getData(itemList, mlbox))
	{
		return -1;
	}
	return 0;
}
//-----------------------------------------------------------------------
//	获取数据到图文混排控件
int MultiLineBoxParse::getData(std::string str, std::string name)
{
	CEGUI::MultiLineBox *mlbox = CEGUI::toMultiLineBox(name);
	if (NULL == mlbox)
	{
		return -1;
	}
	if (-1 == getData(str, mlbox))
	{
		return -1;
	}
	return 0;
}
//-----------------------------------------------------------------------
//	构造图文混排控件解析类对象
MultiLineBoxParse createMultiLineBoxParse(void)
{
	static MultiLineBoxParse mlbp;	// 比较囧的做法,为了提供给Lua调用
	return mlbp;
}
//-----------------------------------------------------------------------