/******************************************************************************
* 作者: 何展然
* 时间: 2010-8-28
* 描述: 负责解析图文混排控件所要用到的一些转义符号
******************************************************************************/
#ifndef MultiLineBoxParse_H
#define MultiLineBoxParse_H

#include "EQMainPrerequisites.h"
#include <hash_map>
#include "CEGUIEx/CEGUIEx.h"


/*************************************************************************
*	图文混排控件解析类
*************************************************************************/
class MultiLineBoxParse
{
public:
	/**
	*	构造函数
	*/
	MultiLineBoxParse();

	/**
	*	析构函数
	*/
	~MultiLineBoxParse();

	/**
	*	功	能：获取数据到图文混排控件
	*	参	数：str:	数据，该数据将进行解析后在放入mlbox
	*			mlbox:	图文混排控件
	*	返回值：0（正确），-1（错误）
	*/
	int getData(std::string str, CEGUI::MultiLineBox* mlbox);

	/**
	*	功	能：获取数据到图文混排控件
	*	参	数：str:	数据，该数据将进行解析后在放入mlbox
	*			name:	图文混排控件名称
	*	返回值：0（正确），-1（错误）
	*/
	int getData(std::string str, std::string name);

protected:
	/**
	*	功	能：获取数据到图文混排控件
	*	参	数：dataList:	图文混排控件内部的数据结构，用来保存解析后的数据
	*			mlbox:	图文混排控件
	*	返回值：0（正确），-1（错误）
	*/
	int _getData(CEGUI::MultiLineBox::ElementList dataList, CEGUI::MultiLineBox* mlbox);

	/**
	*	功	能：解析xml字符串
	*	参	数：str:	xml字符串
	*			dataList:	图文混排控件内部的数据结构
	*			fontName:	默认字体名字
	*			col:	默认颜色
	*	返回值：0（正确），-1（错误）
	*/
	int _parseXmlString(std::string str, CEGUI::MultiLineBox::ElementList* dataList, std::string fontName, std::string col);

	/**
	*	功	能：解析xml结点
	*	参	数：node:	xml结点
	*			dataList:	图文混排控件内部的数据结构
	*			fnt:	默认字体
	*			color:	默认颜色
	*	返回值：0（正确），-1（错误）
	*/
	int _parseXmlNode(TiXmlNode* node, CEGUI::MultiLineBox::ElementList* dataList, CEGUI::Font* fnt, CEGUI::colour color);

	/**
	*	功	能：解析纯文本
	*	参	数：node:	xml结点
	*			fnt:	文本字体
	*			cols:	文本颜色
	*	返回值：0（正确），-1（错误）
	*/
	int _processText(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	功	能：解析图像
	*	参	数：node:	xml结点
	*	返回值：0（正确），-1（错误）
	*/
	int _processImage(TiXmlNode* node);

	/**
	*	功	能：解析超链接文本
	*	参	数：node:	xml结点
	*			fnt:	文本字体
	*			cols:	文本颜色
	*	返回值：0（正确），-1（错误）
	*/
	int _processHyperLinkText(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	功	能：解析超链接
	*	参	数：node:	xml结点
	*			fnt:	文本字体
	*			cols:	文本颜色
	*	返回值：0（正确），-1（错误）
	*/
	int _processHyperLink(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	功	能：解析动画
	*	参	数：node:	xml结点
	*	返回值：0（正确），-1（错误）
	*/
	int _processAnimate(TiXmlNode* node);

	/**
	*	功	能：解析字体
	*	参	数：node:	xml结点
	*			cols:	文本颜色
	*			isText:	纯文本标志（true，纯文本；false，超链接文本）
	*	返回值：0（正确），-1（错误）
	*/
	int _processFont(TiXmlNode* node, CEGUI::colour cols, bool isText=true);

	/**
	*	功	能：解析文本颜色
	*	参	数：node:	xml结点
	*			fnt:	文本字体
	*			isText:	纯文本标志（true，纯文本；false，超链接文本）
	*	返回值：0（正确），-1（错误）
	*/
	int _processColor(TiXmlNode* node, CEGUI::Font* fnt, bool isText=true);

	/**
	*	功	能：解析描边颜色
	*	参	数：node:	xml结点
	*			fnt:	文本字体
	*			cols:	文本颜色
	*			isText:	纯文本标志（true，纯文本；false，超链接文本）
	*	返回值：0（正确），-1（错误）
	*/
	int _processSide(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols, bool isText=true);

	/**
	*	功	能：解析空格符号
	*	参	数：fnt:	文本字体
	*			cols:	文本颜色
	*	返回值：0（正确），-1（错误）
	*/
	int _processSP(CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	功	能：解析换行符号
	*	参	数：node:	xml结点
	*			fnt:	文本字体
	*			cols:	文本颜色
	*	返回值：0（正确），-1（错误）
	*/
	int _processBR(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

protected:
	CEGUI::MultiLineBox::ElementList *mDataList;	//	图文混排控件内部的数据结构，临时

	CEGUI::colour mSideCols;	//	描边颜色

	float mGap;		//	描边像素

	bool mHasSide;	//	是否描边标志

	bool mNewLine;	//	是否开始新行标志

	std::string mLinkId;	//	超链接标识

};	//	end of class MultiLineBoxParse

/**
*	功	能：构造图文混排控件解析类对象
*	参	数：void
*	返回值：图文混排控件解析类对象
*/
MultiLineBoxParse createMultiLineBoxParse(void);


#endif	//	end of guard MLBoxParse_H
