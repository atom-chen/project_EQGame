/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-7-1
* ����: ͼ�Ļ��ſؼ�
******************************************************************************/
#ifndef CEGUIMultiLineBox_H
#define CEGUIMultiLineBox_H

#include "CEGUI.h"


namespace CEGUI
{
	/*************************************************************************
	*	ͼ�Ļ��ſؼ��¼���
	*************************************************************************/
	static const String MultiLineBox_EventNamespace = "MultiLineBox";

	/*************************************************************************
	*	ͼ�Ļ��ſؼ�������
	*************************************************************************/
	static const String MultiLineBox_WidgetTypeName = "CEGUI/MultiLineBox";

	/*************************************************************************
	*	ͼ�Ļ��ſؼ���Ⱦ��
	*************************************************************************/
	class MultiLineBoxWindowRenderer : public WindowRenderer
	{
	public:
		/**
		*	���캯��
		*/
		MultiLineBoxWindowRenderer(const String& name);

		/**
		*	��ȡ�����ı�����Ⱦ����
		*/
		virtual Rect getRenderArea(void) const = 0;

	};	//	end of class MultiLineBoxWindowRenderer

	/*************************************************************************
	*	ͼ�Ļ��ſؼ���
	*************************************************************************/
	class MultiLineBox : public Window
	{
	public:
		static const String EventNamespace;	//	ȫ���¼�����
		static const String WidgetTypeName;	//	������������

		static const String HyperLink_MouseClick;	//	�¼����ƣ������������λ�ã�
		static const String HyperLink_MouseEnter;	//	�¼����ƣ�����ƽ�������λ�ã�
		static const String HyperLink_MouseLeave;	//	�¼����ƣ�����Ƴ�������λ�ã�

		static const String Text_MouseClick;	//	�¼����ƣ����������λ�ã�
		static const String Text_MouseEnter;	//	�¼����ƣ�����ƽ�����λ�ã�
		static const String Text_MouseLeave;	//	�¼����ƣ�����Ƴ�����λ�ã�

		static const String VertScrollbarNameSuffix;   //	�Ӳ������ƣ���ֱ������������

	public:
		//	���뷽ʽ
		enum AlignType
		{
			AT_LEFT,		//	�����
			AT_RIGHT,		//	�Ҷ���
			AT_TOP,			//	��������
			AT_BOTTOM,		//	�ײ�����
			AT_CENTER_HOR,	//	ˮƽ���ж���
			AT_CENTER_VER	//	��ֱ���ж���
		};

		//	������ݺ󣬹�������λ��
		enum ScrollbarPosition
		{
			SP_TOP,		//	�����ڶ���
			SP_OLD,		//	�ھɵ�λ��
			SP_BOTTOM	//	�����ڵײ�
		};

		//	ͼ�Ļ�����������
		enum ElementType
		{
			ET_TEXT,		//	�ı�
			ET_IMAGE,		//	ͼ��
			ET_HYPERLINK,	//	������
			ET_WIN			//	�ؼ�
		};

		//	ͼ�Ļ�������
		struct Element
		{
			Element() : font(NULL), width(0), height(0), index(-1), newLine(false)
			{
			}

			ElementType type;	//	��������
			Font *font;			//	����
			float width;		//	���
			float height;		//	�߶�
			int index;			//	����
			bool newLine;		//	�Ƿ����б�־
		};
		typedef std::vector<Element* > ElementList;	//	ͼ�Ļ�����������

		//	�ı�
		struct Element_Text : public Element
		{
			String text;		//	�ı�
			colour textCols;	//	�ı���ɫ
			colour sideCols;	//	�����ɫ
			float sidePix;		//	�������
			bool sideFlag;		//	�Ƿ���߱�־
		};

		//	ͼ��
		struct Element_Image : public Element
		{
			Image image;	//	ͼ��
		};

		//	������
		struct Element_HyperLink : public Element
		{
			String text;		//	�ı�
			colour textCols;	//	�ı���ɫ
			colour sideCols;	//	�����ɫ
			float sidePix;		//	�������
			bool sideFlag;		//	�Ƿ���߱�־
			std::string id;		//	���������ݵı�ʶ
		};

		//	�ؼ�
		struct Element_Win : public Element
		{
			Element_Win() : win(NULL)
			{
			}

			Window *win;	//	�ؼ�
		};

		//	ͼ�Ļ��ſؼ�������
		struct Line
		{
			ElementList element;	//	�е�Ԫ��
			float width;	//	�п�
			float height;	//	�и�
		};
		typedef std::vector<Line > LineList;	//	ͼ�Ļ��ſؼ�������

		//	�ؼ���չ�ӿ�
	public:
		/**
		*	���캯��
		*/
		MultiLineBox(const String& type, const String& name);

		/**
		*	��������
		*/
		virtual ~MultiLineBox(void);

		/**
		*	��	�ܣ�����ı�
		*	��	����fnt: ���壬text: �ı���cols: �ı���ɫ
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementText(Font* fnt, String text, colour cols, bool newLine=false);

		/**
		*	��	�ܣ�����ı�
		*	��	����fontName: �������ƣ�text: �ı���cols: �ı���ɫ��ʮ�����ƣ��磺"#FF00FF"��
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementText(std::string fontName, std::string text, std::string cols, bool newLine=false);

		/**
		*	��	�ܣ�����ı�������Ĭ��ΪsetFontFunc��������壩
		*	��	����text: �ı���cols: �ı���ɫ��ʮ�����ƣ��磺"#FF00FF"��
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementText(std::string text, std::string cols, bool newLine=false);

		/**
		*	��	�ܣ�����ı�������Ĭ��ΪsetFontFunc��������壬Ĭ���ı���ɫΪsetColsFunc�������ɫ��
		*	��	����text: �ı�
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementText(std::string text, bool newLine=false);

		/**
		*	��	�ܣ����ͼ��
		*	��	����image: ͼ��width: ��ȣ�height: �߶�
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementImage(Image image, float width, float height, bool newLine=false);

		/**
		*	��	�ܣ����ͼ��
		*	��	����imagesetName: ͼ������imageset���ƣ�imageName: ͼ����imageset�еı�ʶ�����ƣ�width: ��ȣ�height: �߶�
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementImage(std::string imagesetName, std::string imageName, float width, float height, bool newLine=false);

		/**
		*	��	�ܣ����ӳ�����
		*	��	����fnt: ���壬text: �ı���id: ���������ݱ�ʶ��cols: �ı���ɫ
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementHyperLink(Font* fnt, String text, std::string id, colour cols, bool newLine=false);

		/**
		*	��	�ܣ����ӳ�����
		*	��	����fontName: �������ƣ�text: �ı���id: ���������ݱ�ʶ��cols: �ı���ɫ��ʮ�����ƣ���"#FF00FF"��
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementHyperLink(std::string fontName, std::string text, std::string id, std::string cols, bool newLine=false);

		/**
		*	��	�ܣ����ӳ����ӣ�����Ĭ��ΪsetFontFunc��������壩
		*	��	����text: �ı���id: ���������ݱ�ʶ��cols: �ı���ɫ��ʮ�����ƣ���"#FF00FF"��
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementHyperLink(std::string text, std::string id, std::string cols, bool newLine=false);

		/**
		*	��	�ܣ����ӳ����ӣ�����Ĭ��ΪsetFontFunc��������壬Ĭ���ı���ɫΪsetColsFunc�������ɫ��
		*	��	����text: �ı���id: ���������ݱ�ʶ
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementHyperLink(std::string text, std::string id, bool newLine=false);

		/**
		*	��	�ܣ����ӿؼ�
		*	��	����win: �ؼ�
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementWin(Window* win, bool newLine=false);

		/**
		*	��	�ܣ����ӿؼ�
		*	��	����winName: �ؼ�����
		*			newLine: �Ƿ����б�־��true: ���У�false: �����У�
		*	����ֵ��-1������
		*/
		int addElementWin(std::string winName, bool newLine=false);

		//	�ؼ��������ýӿ�
	public:
		/**
		*	��	�ܣ����ÿؼ����壨�Կؼ�����������Ч��
		*	��	����fontName: �������ƣ���"simsun-10"��
		*	����ֵ��0����ȷ����-1������
		*/
		int setFontFunc(std::string fontName);

		/**
		*	��	�ܣ������ı���ɫ���Կؼ�����������Ч��
		*	��	����cols: �ı���ɫ��ʮ�����ƣ���"#FF00FF"��
		*	����ֵ��void
		*/
		void setColsFunc(std::string cols);

		/**
		*	��	�ܣ����������ɫ���Կؼ�����������Ч��
		*	��	����cols: �����ɫ��ʮ�����ƣ���"#FF00FF"����pix: �������
		*	����ֵ��void
		*/
		void setSideFunc(std::string cols, float pix);

		/**
		*	��	�ܣ������ض�Ԫ�ص���ߣ�ֻ���ض�Ԫ����Ч��
		*	��	����colour: �����ɫ��pix: �������
		*	����ֵ��void
		*/
		void setSpecSide(colour cols, float pix);

		/**
		*	��	�ܣ������ض�Ԫ�ص���ߣ�ֻ���ض�Ԫ����Ч��
		*	��	����cols: �����ɫ��ʮ�����ƣ���"#FF00FF"����pix: �������
		*	����ֵ��void
		*/
		void setSpecSide(std::string cols, float pix);

		/**
		*	��	�ܣ����ÿؼ�������С
		*	��	����width: ����ȣ� height: ���߶�
		*	����ֵ��void
		*/
		void setMaxSizeFunc(float maxWidth, float maxHeight);

		/**
		*	��	�ܣ����ÿؼ����
		*	��	����width: ���
		*	����ֵ��void
		*/
		void setWidthFunc(float width);

		/**
		*	��	�ܣ����ÿؼ��߶�
		*	��	����width: �߶�
		*	����ֵ��void
		*/
		void setHeightFunc(float height);

		/**
		*	��	�ܣ����ÿؼ���С
		*	��	����width: ��ȣ�height: �߶�
		*	����ֵ��void
		*/
		void setSizeFunc(float width, float height);

		/**
		*	��	�ܣ����ÿؼ�λ��
		*	��	����x: x���꣬y: y����
		*	����ֵ��void
		*/
		void setPositionFunc(float x, float y);

		/**
		*	��	�ܣ����ÿؼ��Ƿ�ɼ�
		*	��	����visible: �Ƿ�ɼ���־��true: �ɼ���false: ���ɼ���
		*	����ֵ��void
		*/
		void setVisibleFunc(bool visible=true);

		/**
		*	��	�ܣ����ô�ֱ�������Ƿ�ɼ�
		*	��	����pos: �ؼ�������ݺ󣬹�������λ�ã������ڶ���: "SP_TOP"���ھɵ�λ��: "SP_OLD"�������ڵײ�: "SP_BOTTOM"��
		*			visible: �Ƿ�ɼ���־��true: �ɼ���false: ���ɼ���
		*			mode: ��ʾģʽ��0: ���ݸ߶ȳ����ؼ��߶Ȳ���ʾ��������1: ֻҪvisibleΪtrue������ʾ��������
		*	����ֵ��0����ȷ����-1������
		*/
		int setScrollbarVisible(std::string pos, bool visible=true, int mode=0);

		/**
		*	��	�ܣ����ô�ֱ��������λ��
		*	��	����pos: ��������λ��
		*	����ֵ��0����ȷ����-1������
		*/
		int setScrollbarPosition(float pos);

		/**
		*	��	�ܣ����ÿؼ����ݶ��뷽ʽ
		*	��	����alignType: ���뷽ʽ�����: "AT_LEFT"��ˮƽ����: "AT_CENTER_HOR"���ұ�: "AT_RIGHT"��
		*								 ����: "AT_TOP"����ֱ����: "AT_CENTER_VER"���ײ�: "AT_BOTTOM"��
		*	����ֵ��0����ȷ����-1������
		*/
		int setAlign(std::string alignType);

		/**
		*	��	�ܣ�ǿ������ÿ�еĸ߶ȣ�ÿ�и߶�ͳһ
		*	��	����height: �и߶�
		*	����ֵ��void
		*/
		void setEachLineHeight(float height);

		/**
		*	��	�ܣ����ͼ�Ļ��ſؼ�����
		*	��	����void
		*	����ֵ��void
		*/
		void clearUp(void);

		/**
		*	��	�ܣ����id��Ӧ��ͼ�Ļ��ſؼ�����
		*	��	����id: ����id����0��ʼ�㣩
		*	����ֵ��void
		*/
		void eraseElement(size_t id);

	public:
		/**
		*	��	�ܣ���ȡ�ؼ�����
		*	��	����void
		*	����ֵ��str::string
		*/
		std::string getNameFunc(void);

		/**
		*	��	�ܣ���ȡ��������
		*	��	����void
		*	����ֵ��str::string
		*/
		std::string getFontName(void);

		/**
		*	��	�ܣ���ȡ������ɫ
		*	��	����void
		*	����ֵ��str::string
		*/
		std::string getColour(void);

		/**
		*	��	�ܣ���ȡ�����Ű��ؼ�����ĸ߶�
		*	��	����void
		*	����ֵ��float
		*/
		float getFormatHeight(void);

		/**
		*	��	�ܣ���ȡ�����Ű��ؼ�������
		*	��	����void
		*	����ֵ��int
		*/
		int getLineSize(void);

		/**
		*	��	�ܣ���ȡǿ������ÿ�еĸ߶�
		*	��	����void
		*	����ֵ��float
		*/
		float getEachLineHeight(void);

		/**
		*	��	�ܣ���ȡ�ؼ��������Ⱦ��ȵļ�϶
		*	��	����void
		*	����ֵ��float
		*/
		float getWidthInterval(void);

		/**
		*	��	�ܣ���ȡ�ؼ��߶�����Ⱦ�߶ȵļ�϶
		*	��	����void
		*	����ֵ��float
		*/
		float getHeightInterval(void);

		/**
		*	��	�ܣ�����text��ÿؼ��ĺ��ʿ��
		*	��	����fontName: �������ƣ�text: �ı�
		*	����ֵ��float
		*/
		float getFitWidth(std::string fontName, std::string text);

		/**
		*	��	�ܣ������ı��Ŀ�ȶȻ�ÿؼ��ĺ��ʿ��
		*	��	����textLen: �ı����
		*	����ֵ��float
		*/
		float getFitWidth(float textLen);

		/**
		*	��	�ܣ���ȡ��Ⱦ������
		*	��	����void
		*	����ֵ��float
		*/
		float getRenderWidth(void);

		/**
		*	��	�ܣ���ȡ��Ⱦ����߶�
		*	��	����void
		*	����ֵ��float
		*/
		float getRenderHeight(void);

		/**
		*	��	�ܣ���ȡ�ؼ������ܳ���
		*	��	����void
		*	����ֵ��float
		*/
		float getLength(void);

		/**
		*	��	�ܣ���ȡ���������ݵı�ʶ
		*	��	����void
		*	����ֵ��std::string
		*/
		std::string getHyperLinkId(void);

		/**
		*	��	�ܣ���ȡ��Ӧ���ı�����
		*	��	����void
		*	����ֵ��std::string
		*/
		std::string getTextContent(void);

		/**
		*	��	�ܣ���ȡ��Ӧ�ľ������򣨸�ʽ��"0-0-0-0"��
		*	��	����void
		*	����ֵ��Rect
		*/
		std::string getTriggerRect(void);

		/**
		*	��	�ܣ���ȡ��ʱ����ҳ���С���ȼ��ڹ������Ƶ���ײ���λ��+���ڵ���Ⱦ�߶ȣ�
		*	��	����void
		*	����ֵ��-1������
		*/
		float getScrollbarDocumentSize(void);

		/**
		*	��	�ܣ���ȡ��ʱ��������λ��
		*	��	����void
		*	����ֵ��-1������
		*/
		float getScrollbarPosition(void);

		/**
		*	��	�ܣ���������Ϊtext���ı�Ԫ��
		*	��	����text: ��������
		*	����ֵ��-1��û�ҵ���
		*/
		int findElementText(std::string text);

		//	���û����ýӿ�
	public:
		/**
		*	��ȡ�ؼ�����ˮƽ���뷽ʽ
		*/
		AlignType getAlignHor(void);

		/**
		*	��ȡ�ؼ����ݴ�ֱ���뷽ʽ
		*/
		AlignType getAlignVer(void);

		/**
		*	��ȡ�Ű��ÿ������
		*/
		LineList getLineList(void);

		/**
		*	��ȡ��ֱ������
		*/
		Scrollbar* getVertScrollbar(void);

		/**
		*	��ȡ��ֱ�������Ƿ�ɼ�
		*/
		bool isScrollbarVisible(void);

		/**
		*	��ȡͼ�Ļ��ſؼ���Ⱦ����
		*/
		Rect getRenderArea(void);

	protected:
		/**
		*	��ͼ�Ļ��ſؼ����ݽ����Ű�
		*/
		void _formatElement(void);

		/**
		*	������Ԫ��
		*/
		void _destroyLineElement(void);

		/**
		*	��������Ԫ��
		*/
		void _destroyAllElements(void);

		/**
		*	����������
		*/
		void _configureScrollbars(void);

		/**
		*	�������λ�û�ȡ����
		*/
		int _getIndexFromPosition(Point pt);

		/**
		*	��ȡ�Ű��ĸ߶�
		*/
		float _getTotalHeight(void);

		/**
		*	���ַ���ת��Ϊ��ɫ
		*/
		colour _extractColorFromString(String cols);

		/**
		*	�жϴ����Ƿ�̳�����������
		*/
		virtual bool testClassName_impl(const String& class_name) const
		{
			if ("MultiLineBox" == class_name)
			{
				return true;
			}
			return Window::testClassName_impl(class_name);
		}

		/**
		*	��ʼ�����ڶ���
		*/
		virtual void initialiseComponents(void); 

		/**
		*	�¼�����������������״̬�ı�ʱ������
		*/
		bool handle_scrollChange(const EventArgs& args);

		/**
		*	�¼�����������갴�£�
		*/
		virtual void onMouseButtonDown(MouseEventArgs& e);

		/**
		*	�¼�����������굯��
		*/
		virtual void onMouseButtonUp(MouseEventArgs& e);

		/**
		*	�¼�������������ƶ���
		*/
		virtual void onMouseMove(MouseEventArgs& e);

		/**
		*	�¼��������������룩
		*/
		virtual void onMouseEnters(MouseEventArgs& e);

		/**
		*	�¼�������������뿪��
		*/
		virtual void onMouseLeaves(MouseEventArgs& e);

		/**
		*	�¼�����������������
		*/
		virtual	void onMouseWheel(MouseEventArgs& e);

		/**
		*	�¼������������ڴ�С�ı䣩
		*/
		virtual void onSized(WindowEventArgs& e);

		/**
		*	�¼�������������
		*/
		virtual void onCaptureLost(WindowEventArgs& e);

	protected:
		float mScreenWidth;		//	��Ļ���
		float mScreenHeight;	//	��Ļ�߶�

		ElementList	mElementList;	//	ͼ�Ļ��ſؼ����ݼ���
		LineList mLines;		//	����Ϣ����
		float mEachLineHeight;	//	ǿ�����õ�ÿ�и߶�
		float mLength;			//	ͼ�Ļ������ݵ��ܳ���

		std::string mHyperLinkId;	//	���������ݵı�ʶ
		std::string mTextContent;	//	����Ӧ���ı�����
		std::string mTriggerRect;	//	��Ӧ�ľ������򣨸�ʽ��"0-0-0-0"��

		Font *mTextFont;		//	�ı�����
		std::string mTextFontName;	// �ı���������
		colour mTextCols;		//	�ı���ɫ
		std::string mTextColsName;	// �ı���ɫ�ַ���
		bool mColsFlag;			//	��ɫ��ʶ
		colour mSideCols;		//	�����ɫ
		float mSidePix;			//	��߼��
		bool mSideFlag;			//	��߱�ʶ

		AlignType mAlignHor;	//	ˮƽ���뷽ʽ
		AlignType mAlignVer;	//	��ֱ���뷽ʽ

		bool mScrollbarVisible;	//	�Ƿ��д�ֱ������
		int mScrollMode;		//	��������ʾģʽ
		ScrollbarPosition mScrollbarPos;	//	������ݺ󣬹�������λ��

		colour mSpecSideCols;	//	ָ�����ݵ������ɫ
		float mSpecSidePix;		//	ָ�����ݵ��������
		bool mSpecSideFlag;		//	ָ�����ݵ���߱�ʶ

	};	//	end of class MultiLineBox

	/*************************************************************************
	*	ͼ�Ļ��ſؼ�������
	*************************************************************************/
	class MultiLineBoxFactory : public WindowFactory
	{
	public:
		/**
		*	���캯��
		*/
		MultiLineBoxFactory(void) : WindowFactory(MultiLineBox_WidgetTypeName){}

		/**
		*	��������
		*/
		~MultiLineBoxFactory(void){}

		/**
		*	��������
		*/
		Window* createWindow(const String& name);

		/**
		*	���ٴ���
		*/
		void destroyWindow(Window* window);
	};

	/*************************************************************************
	*	CEGUIȫ�ֽӿ�
	*************************************************************************/
	/**
	*	��	�ܣ���windowת��ΪMultiLinebox����
	*	��	����ctrlName: ��������
	*	����ֵ��ͼ�Ļ��ſؼ�
	*/
	MultiLineBox* toMultiLineBox(std::string ctrlName);

	/**
	*	��	�ܣ���������font��ȡ�ַ���str�ĳ���
	*	��	����fontName: �������ƣ�str: �ַ���
	*	����ֵ��-1�����岻���ڣ�
	*/
	float getStrExtent(std::string fontName, std::string str);

	/**
	*	��	�ܣ��������ػ�ȡ��ȡ���ַ�λ��
	*	��	����fontName: �������ƣ�str: �ַ�����pixel: ��ȡ����λ�õ����أ�startChar: ��ȡ��ʼλ�õ��ַ�����
	*	����ֵ��size_t
	*/
	size_t getCharAtPixel(std::string fontName, std::string str, float pixel, size_t startChar=0);

	/**
	*	��	�ܣ���ȡ�ַ���
	*	��	����fontName: �������ƣ�str: �ַ�����pixel: ��ȡ����λ�õ����أ�startChar: ��ȡ��ʼλ�õ��ַ�����
	*	����ֵ��std::string
	*/
	std::string subStrExtent(std::string fontName, std::string str, float pixel, size_t startChar=0);

	/**
	*	��	�ܣ���ȡ�ַ���str�е��ַ�����
	*	��	����str: �ַ���
	*	����ֵ��size_t
	*/
	size_t getCharacterCount(std::string str);

	/**
	*	��	�ܣ���ȡ�ַ���str���ִ������ַ�����Ϊ��λ
	*	��	����str: �ַ�����startPos: ��ʼλ�ã�len: ��ȡ����
	*	����ֵ��std::string
	*/
	std::string subCharacter(std::string str, size_t startPos, int len);

	/**
	*	��	�ܣ���ȡ��������ĸ���ֵ
	*	��	����winname: ������
	*			xy: 0�����ȡ����x���꣩��1�����ȡ����y���꣩
	*			pos: 0�����ȡ����scale����1�����ȡ����offset��
	*	����ֵ��-1���������󣩣�-2�����ڲ����ڣ�
	*/
	float getPositionVal(std::string winname, int xy, int pos);

}	//	end of namespace CEGUI


#endif	//	end of guard CEGUIMultiLineBox_H
