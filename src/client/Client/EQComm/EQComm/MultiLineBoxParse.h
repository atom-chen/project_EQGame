/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-8-28
* ����: �������ͼ�Ļ��ſؼ���Ҫ�õ���һЩת�����
******************************************************************************/
#ifndef MultiLineBoxParse_H
#define MultiLineBoxParse_H

#include "EQMainPrerequisites.h"
#include <hash_map>
#include "CEGUIEx/CEGUIEx.h"


/*************************************************************************
*	ͼ�Ļ��ſؼ�������
*************************************************************************/
class MultiLineBoxParse
{
public:
	/**
	*	���캯��
	*/
	MultiLineBoxParse();

	/**
	*	��������
	*/
	~MultiLineBoxParse();

	/**
	*	��	�ܣ���ȡ���ݵ�ͼ�Ļ��ſؼ�
	*	��	����str:	���ݣ������ݽ����н������ڷ���mlbox
	*			mlbox:	ͼ�Ļ��ſؼ�
	*	����ֵ��0����ȷ����-1������
	*/
	int getData(std::string str, CEGUI::MultiLineBox* mlbox);

	/**
	*	��	�ܣ���ȡ���ݵ�ͼ�Ļ��ſؼ�
	*	��	����str:	���ݣ������ݽ����н������ڷ���mlbox
	*			name:	ͼ�Ļ��ſؼ�����
	*	����ֵ��0����ȷ����-1������
	*/
	int getData(std::string str, std::string name);

protected:
	/**
	*	��	�ܣ���ȡ���ݵ�ͼ�Ļ��ſؼ�
	*	��	����dataList:	ͼ�Ļ��ſؼ��ڲ������ݽṹ��������������������
	*			mlbox:	ͼ�Ļ��ſؼ�
	*	����ֵ��0����ȷ����-1������
	*/
	int _getData(CEGUI::MultiLineBox::ElementList dataList, CEGUI::MultiLineBox* mlbox);

	/**
	*	��	�ܣ�����xml�ַ���
	*	��	����str:	xml�ַ���
	*			dataList:	ͼ�Ļ��ſؼ��ڲ������ݽṹ
	*			fontName:	Ĭ����������
	*			col:	Ĭ����ɫ
	*	����ֵ��0����ȷ����-1������
	*/
	int _parseXmlString(std::string str, CEGUI::MultiLineBox::ElementList* dataList, std::string fontName, std::string col);

	/**
	*	��	�ܣ�����xml���
	*	��	����node:	xml���
	*			dataList:	ͼ�Ļ��ſؼ��ڲ������ݽṹ
	*			fnt:	Ĭ������
	*			color:	Ĭ����ɫ
	*	����ֵ��0����ȷ����-1������
	*/
	int _parseXmlNode(TiXmlNode* node, CEGUI::MultiLineBox::ElementList* dataList, CEGUI::Font* fnt, CEGUI::colour color);

	/**
	*	��	�ܣ��������ı�
	*	��	����node:	xml���
	*			fnt:	�ı�����
	*			cols:	�ı���ɫ
	*	����ֵ��0����ȷ����-1������
	*/
	int _processText(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	��	�ܣ�����ͼ��
	*	��	����node:	xml���
	*	����ֵ��0����ȷ����-1������
	*/
	int _processImage(TiXmlNode* node);

	/**
	*	��	�ܣ������������ı�
	*	��	����node:	xml���
	*			fnt:	�ı�����
	*			cols:	�ı���ɫ
	*	����ֵ��0����ȷ����-1������
	*/
	int _processHyperLinkText(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	��	�ܣ�����������
	*	��	����node:	xml���
	*			fnt:	�ı�����
	*			cols:	�ı���ɫ
	*	����ֵ��0����ȷ����-1������
	*/
	int _processHyperLink(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	��	�ܣ���������
	*	��	����node:	xml���
	*	����ֵ��0����ȷ����-1������
	*/
	int _processAnimate(TiXmlNode* node);

	/**
	*	��	�ܣ���������
	*	��	����node:	xml���
	*			cols:	�ı���ɫ
	*			isText:	���ı���־��true�����ı���false���������ı���
	*	����ֵ��0����ȷ����-1������
	*/
	int _processFont(TiXmlNode* node, CEGUI::colour cols, bool isText=true);

	/**
	*	��	�ܣ������ı���ɫ
	*	��	����node:	xml���
	*			fnt:	�ı�����
	*			isText:	���ı���־��true�����ı���false���������ı���
	*	����ֵ��0����ȷ����-1������
	*/
	int _processColor(TiXmlNode* node, CEGUI::Font* fnt, bool isText=true);

	/**
	*	��	�ܣ����������ɫ
	*	��	����node:	xml���
	*			fnt:	�ı�����
	*			cols:	�ı���ɫ
	*			isText:	���ı���־��true�����ı���false���������ı���
	*	����ֵ��0����ȷ����-1������
	*/
	int _processSide(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols, bool isText=true);

	/**
	*	��	�ܣ������ո����
	*	��	����fnt:	�ı�����
	*			cols:	�ı���ɫ
	*	����ֵ��0����ȷ����-1������
	*/
	int _processSP(CEGUI::Font* fnt, CEGUI::colour cols);

	/**
	*	��	�ܣ��������з���
	*	��	����node:	xml���
	*			fnt:	�ı�����
	*			cols:	�ı���ɫ
	*	����ֵ��0����ȷ����-1������
	*/
	int _processBR(TiXmlNode* node, CEGUI::Font* fnt, CEGUI::colour cols);

protected:
	CEGUI::MultiLineBox::ElementList *mDataList;	//	ͼ�Ļ��ſؼ��ڲ������ݽṹ����ʱ

	CEGUI::colour mSideCols;	//	�����ɫ

	float mGap;		//	�������

	bool mHasSide;	//	�Ƿ���߱�־

	bool mNewLine;	//	�Ƿ�ʼ���б�־

	std::string mLinkId;	//	�����ӱ�ʶ

};	//	end of class MultiLineBoxParse

/**
*	��	�ܣ�����ͼ�Ļ��ſؼ����������
*	��	����void
*	����ֵ��ͼ�Ļ��ſؼ����������
*/
MultiLineBoxParse createMultiLineBoxParse(void);


#endif	//	end of guard MLBoxParse_H
