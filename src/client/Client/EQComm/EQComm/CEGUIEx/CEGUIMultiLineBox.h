/******************************************************************************
* 作者: 何展然
* 时间: 2010-7-1
* 描述: 图文混排控件
******************************************************************************/
#ifndef CEGUIMultiLineBox_H
#define CEGUIMultiLineBox_H

#include "CEGUI.h"


namespace CEGUI
{
	/*************************************************************************
	*	图文混排控件事件名
	*************************************************************************/
	static const String MultiLineBox_EventNamespace = "MultiLineBox";

	/*************************************************************************
	*	图文混排控件类型名
	*************************************************************************/
	static const String MultiLineBox_WidgetTypeName = "CEGUI/MultiLineBox";

	/*************************************************************************
	*	图文混排控件渲染类
	*************************************************************************/
	class MultiLineBoxWindowRenderer : public WindowRenderer
	{
	public:
		/**
		*	构造函数
		*/
		MultiLineBoxWindowRenderer(const String& name);

		/**
		*	获取多行文本框渲染区域
		*/
		virtual Rect getRenderArea(void) const = 0;

	};	//	end of class MultiLineBoxWindowRenderer

	/*************************************************************************
	*	图文混排控件类
	*************************************************************************/
	class MultiLineBox : public Window
	{
	public:
		static const String EventNamespace;	//	全局事件名称
		static const String WidgetTypeName;	//	窗口类型名称

		static const String HyperLink_MouseClick;	//	事件名称（鼠标点击超链接位置）
		static const String HyperLink_MouseEnter;	//	事件名称（鼠标移进超链接位置）
		static const String HyperLink_MouseLeave;	//	事件名称（鼠标移出超链接位置）

		static const String Text_MouseClick;	//	事件名称（鼠标点击文字位置）
		static const String Text_MouseEnter;	//	事件名称（鼠标移进文字位置）
		static const String Text_MouseLeave;	//	事件名称（鼠标移出文字位置）

		static const String VertScrollbarNameSuffix;   //	子部件名称（垂直滚动条部件）

	public:
		//	对齐方式
		enum AlignType
		{
			AT_LEFT,		//	左对齐
			AT_RIGHT,		//	右对齐
			AT_TOP,			//	顶部对齐
			AT_BOTTOM,		//	底部对齐
			AT_CENTER_HOR,	//	水平居中对齐
			AT_CENTER_VER	//	垂直居中对齐
		};

		//	添加内容后，滚动条的位置
		enum ScrollbarPosition
		{
			SP_TOP,		//	总是在顶部
			SP_OLD,		//	在旧的位置
			SP_BOTTOM	//	总是在底部
		};

		//	图文混排内容类型
		enum ElementType
		{
			ET_TEXT,		//	文本
			ET_IMAGE,		//	图像
			ET_HYPERLINK,	//	超链接
			ET_WIN			//	控件
		};

		//	图文混排内容
		struct Element
		{
			Element() : font(NULL), width(0), height(0), index(-1), newLine(false)
			{
			}

			ElementType type;	//	内容类型
			Font *font;			//	字体
			float width;		//	宽度
			float height;		//	高度
			int index;			//	索引
			bool newLine;		//	是否新行标志
		};
		typedef std::vector<Element* > ElementList;	//	图文混排内容容器

		//	文本
		struct Element_Text : public Element
		{
			String text;		//	文本
			colour textCols;	//	文本颜色
			colour sideCols;	//	描边颜色
			float sidePix;		//	描边像素
			bool sideFlag;		//	是否描边标志
		};

		//	图像
		struct Element_Image : public Element
		{
			Image image;	//	图像
		};

		//	超链接
		struct Element_HyperLink : public Element
		{
			String text;		//	文本
			colour textCols;	//	文本颜色
			colour sideCols;	//	描边颜色
			float sidePix;		//	描边像素
			bool sideFlag;		//	是否描边标志
			std::string id;		//	超链接内容的标识
		};

		//	控件
		struct Element_Win : public Element
		{
			Element_Win() : win(NULL)
			{
			}

			Window *win;	//	控件
		};

		//	图文混排控件行内容
		struct Line
		{
			ElementList element;	//	行的元素
			float width;	//	行宽
			float height;	//	行高
		};
		typedef std::vector<Line > LineList;	//	图文混排控件行容器

		//	控件扩展接口
	public:
		/**
		*	构造函数
		*/
		MultiLineBox(const String& type, const String& name);

		/**
		*	析构函数
		*/
		virtual ~MultiLineBox(void);

		/**
		*	功	能：添加文本
		*	参	数：fnt: 字体，text: 文本，cols: 文本颜色
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementText(Font* fnt, String text, colour cols, bool newLine=false);

		/**
		*	功	能：添加文本
		*	参	数：fontName: 字体名称，text: 文本，cols: 文本颜色（十六进制，如："#FF00FF"）
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementText(std::string fontName, std::string text, std::string cols, bool newLine=false);

		/**
		*	功	能：添加文本（字体默认为setFontFunc所设的字体）
		*	参	数：text: 文本，cols: 文本颜色（十六进制，如："#FF00FF"）
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementText(std::string text, std::string cols, bool newLine=false);

		/**
		*	功	能：添加文本（字体默认为setFontFunc所设的字体，默认文本颜色为setColsFunc所设的颜色）
		*	参	数：text: 文本
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementText(std::string text, bool newLine=false);

		/**
		*	功	能：添加图像
		*	参	数：image: 图像，width: 宽度，height: 高度
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementImage(Image image, float width, float height, bool newLine=false);

		/**
		*	功	能：添加图像
		*	参	数：imagesetName: 图像所在imageset名称，imageName: 图像在imageset中的标识或名称，width: 宽度，height: 高度
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementImage(std::string imagesetName, std::string imageName, float width, float height, bool newLine=false);

		/**
		*	功	能：增加超链接
		*	参	数：fnt: 字体，text: 文本，id: 超链接内容标识，cols: 文本颜色
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementHyperLink(Font* fnt, String text, std::string id, colour cols, bool newLine=false);

		/**
		*	功	能：增加超链接
		*	参	数：fontName: 字体名称，text: 文本，id: 超链接内容标识，cols: 文本颜色（十六进制，如"#FF00FF"）
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementHyperLink(std::string fontName, std::string text, std::string id, std::string cols, bool newLine=false);

		/**
		*	功	能：增加超链接（字体默认为setFontFunc所设的字体）
		*	参	数：text: 文本，id: 超链接内容标识，cols: 文本颜色（十六进制，如"#FF00FF"）
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementHyperLink(std::string text, std::string id, std::string cols, bool newLine=false);

		/**
		*	功	能：增加超链接（字体默认为setFontFunc所设的字体，默认文本颜色为setColsFunc所设的颜色）
		*	参	数：text: 文本，id: 超链接内容标识
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementHyperLink(std::string text, std::string id, bool newLine=false);

		/**
		*	功	能：增加控件
		*	参	数：win: 控件
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementWin(Window* win, bool newLine=false);

		/**
		*	功	能：增加控件
		*	参	数：winName: 控件名称
		*			newLine: 是否新行标志（true: 新行，false: 非新行）
		*	返回值：-1（错误）
		*/
		int addElementWin(std::string winName, bool newLine=false);

		//	控件属性设置接口
	public:
		/**
		*	功	能：设置控件字体（对控件所有内容有效）
		*	参	数：fontName: 字体名称（如"simsun-10"）
		*	返回值：0（正确），-1（错误）
		*/
		int setFontFunc(std::string fontName);

		/**
		*	功	能：设置文本颜色（对控件所有内容有效）
		*	参	数：cols: 文本颜色（十六进制，如"#FF00FF"）
		*	返回值：void
		*/
		void setColsFunc(std::string cols);

		/**
		*	功	能：设置描边颜色（对控件所有内容有效）
		*	参	数：cols: 描边颜色（十六进制，如"#FF00FF"），pix: 描边像素
		*	返回值：void
		*/
		void setSideFunc(std::string cols, float pix);

		/**
		*	功	能：设置特定元素的描边（只对特定元素有效）
		*	参	数：colour: 描边颜色，pix: 描边像素
		*	返回值：void
		*/
		void setSpecSide(colour cols, float pix);

		/**
		*	功	能：设置特定元素的描边（只对特定元素有效）
		*	参	数：cols: 描边颜色（十六进制，如"#FF00FF"），pix: 描边像素
		*	返回值：void
		*/
		void setSpecSide(std::string cols, float pix);

		/**
		*	功	能：设置控件的最大大小
		*	参	数：width: 最大宽度， height: 最大高度
		*	返回值：void
		*/
		void setMaxSizeFunc(float maxWidth, float maxHeight);

		/**
		*	功	能：设置控件宽度
		*	参	数：width: 宽度
		*	返回值：void
		*/
		void setWidthFunc(float width);

		/**
		*	功	能：设置控件高度
		*	参	数：width: 高度
		*	返回值：void
		*/
		void setHeightFunc(float height);

		/**
		*	功	能：设置控件大小
		*	参	数：width: 宽度，height: 高度
		*	返回值：void
		*/
		void setSizeFunc(float width, float height);

		/**
		*	功	能：设置控件位置
		*	参	数：x: x坐标，y: y坐标
		*	返回值：void
		*/
		void setPositionFunc(float x, float y);

		/**
		*	功	能：设置控件是否可见
		*	参	数：visible: 是否可见标志（true: 可见，false: 不可见）
		*	返回值：void
		*/
		void setVisibleFunc(bool visible=true);

		/**
		*	功	能：设置垂直滚动条是否可见
		*	参	数：pos: 控件添加内容后，滚动条的位置（总是在顶部: "SP_TOP"，在旧的位置: "SP_OLD"，总是在底部: "SP_BOTTOM"）
		*			visible: 是否可见标志（true: 可见，false: 不可见）
		*			mode: 显示模式（0: 内容高度超过控件高度才显示滚动条，1: 只要visible为true，就显示滚动条）
		*	返回值：0（正确），-1（错误）
		*/
		int setScrollbarVisible(std::string pos, bool visible=true, int mode=0);

		/**
		*	功	能：设置垂直滚动条的位置
		*	参	数：pos: 滚动条的位置
		*	返回值：0（正确），-1（错误）
		*/
		int setScrollbarPosition(float pos);

		/**
		*	功	能：设置控件内容对齐方式
		*	参	数：alignType: 对齐方式（左边: "AT_LEFT"，水平居中: "AT_CENTER_HOR"，右边: "AT_RIGHT"，
		*								 顶部: "AT_TOP"，垂直居中: "AT_CENTER_VER"，底部: "AT_BOTTOM"）
		*	返回值：0（正确），-1（错误）
		*/
		int setAlign(std::string alignType);

		/**
		*	功	能：强制设置每行的高度，每行高度统一
		*	参	数：height: 行高度
		*	返回值：void
		*/
		void setEachLineHeight(float height);

		/**
		*	功	能：清除图文混排控件内容
		*	参	数：void
		*	返回值：void
		*/
		void clearUp(void);

		/**
		*	功	能：清除id对应的图文混排控件内容
		*	参	数：id: 内容id（从0开始算）
		*	返回值：void
		*/
		void eraseElement(size_t id);

	public:
		/**
		*	功	能：获取控件名字
		*	参	数：void
		*	返回值：str::string
		*/
		std::string getNameFunc(void);

		/**
		*	功	能：获取字体名字
		*	参	数：void
		*	返回值：str::string
		*/
		std::string getFontName(void);

		/**
		*	功	能：获取文字颜色
		*	参	数：void
		*	返回值：str::string
		*/
		std::string getColour(void);

		/**
		*	功	能：获取内容排版后控件所需的高度
		*	参	数：void
		*	返回值：float
		*/
		float getFormatHeight(void);

		/**
		*	功	能：获取内容排版后控件的行数
		*	参	数：void
		*	返回值：int
		*/
		int getLineSize(void);

		/**
		*	功	能：获取强制设置每行的高度
		*	参	数：void
		*	返回值：float
		*/
		float getEachLineHeight(void);

		/**
		*	功	能：获取控件宽度与渲染宽度的间隙
		*	参	数：void
		*	返回值：float
		*/
		float getWidthInterval(void);

		/**
		*	功	能：获取控件高度与渲染高度的间隙
		*	参	数：void
		*	返回值：float
		*/
		float getHeightInterval(void);

		/**
		*	功	能：根据text获得控件的合适宽度
		*	参	数：fontName: 字体名称，text: 文本
		*	返回值：float
		*/
		float getFitWidth(std::string fontName, std::string text);

		/**
		*	功	能：根据文本的宽度度获得控件的合适宽度
		*	参	数：textLen: 文本宽度
		*	返回值：float
		*/
		float getFitWidth(float textLen);

		/**
		*	功	能：获取渲染区域宽度
		*	参	数：void
		*	返回值：float
		*/
		float getRenderWidth(void);

		/**
		*	功	能：获取渲染区域高度
		*	参	数：void
		*	返回值：float
		*/
		float getRenderHeight(void);

		/**
		*	功	能：获取控件内容总长度
		*	参	数：void
		*	返回值：float
		*/
		float getLength(void);

		/**
		*	功	能：获取超链接内容的标识
		*	参	数：void
		*	返回值：std::string
		*/
		std::string getHyperLinkId(void);

		/**
		*	功	能：获取响应的文本内容
		*	参	数：void
		*	返回值：std::string
		*/
		std::string getTextContent(void);

		/**
		*	功	能：获取响应的矩形区域（格式："0-0-0-0"）
		*	参	数：void
		*	返回值：Rect
		*/
		std::string getTriggerRect(void);

		/**
		*	功	能：获取此时滚动页面大小（等价于滚动条移到最底部的位置+窗口的渲染高度）
		*	参	数：void
		*	返回值：-1（错误）
		*/
		float getScrollbarDocumentSize(void);

		/**
		*	功	能：获取此时滚动条的位置
		*	参	数：void
		*	返回值：-1（错误）
		*/
		float getScrollbarPosition(void);

		/**
		*	功	能：查找内容为text的文本元素
		*	参	数：text: 查找内容
		*	返回值：-1（没找到）
		*/
		int findElementText(std::string text);

		//	非用户调用接口
	public:
		/**
		*	获取控件内容水平对齐方式
		*/
		AlignType getAlignHor(void);

		/**
		*	获取控件内容垂直对齐方式
		*/
		AlignType getAlignVer(void);

		/**
		*	获取排版后每行内容
		*/
		LineList getLineList(void);

		/**
		*	获取垂直滚动条
		*/
		Scrollbar* getVertScrollbar(void);

		/**
		*	获取垂直滚动条是否可见
		*/
		bool isScrollbarVisible(void);

		/**
		*	获取图文混排控件渲染区域
		*/
		Rect getRenderArea(void);

	protected:
		/**
		*	对图文混排控件内容进行排版
		*/
		void _formatElement(void);

		/**
		*	销毁行元素
		*/
		void _destroyLineElement(void);

		/**
		*	销毁所有元素
		*/
		void _destroyAllElements(void);

		/**
		*	滚动条配置
		*/
		void _configureScrollbars(void);

		/**
		*	根据鼠标位置获取索引
		*/
		int _getIndexFromPosition(Point pt);

		/**
		*	获取排版后的高度
		*/
		float _getTotalHeight(void);

		/**
		*	把字符串转换为颜色
		*/
		colour _extractColorFromString(String cols);

		/**
		*	判断窗口是否继承自所给类名
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
		*	初始化窗口对象
		*/
		virtual void initialiseComponents(void); 

		/**
		*	事件触发函数（滚动条状态改变时触发）
		*/
		bool handle_scrollChange(const EventArgs& args);

		/**
		*	事件处理函数（鼠标按下）
		*/
		virtual void onMouseButtonDown(MouseEventArgs& e);

		/**
		*	事件处理函数（鼠标弹起）
		*/
		virtual void onMouseButtonUp(MouseEventArgs& e);

		/**
		*	事件处理函数（鼠标移动）
		*/
		virtual void onMouseMove(MouseEventArgs& e);

		/**
		*	事件处理函数（鼠标进入）
		*/
		virtual void onMouseEnters(MouseEventArgs& e);

		/**
		*	事件处理函数（鼠标离开）
		*/
		virtual void onMouseLeaves(MouseEventArgs& e);

		/**
		*	事件处理函数（鼠标滚动）
		*/
		virtual	void onMouseWheel(MouseEventArgs& e);

		/**
		*	事件处理函数（窗口大小改变）
		*/
		virtual void onSized(WindowEventArgs& e);

		/**
		*	事件处理函数（捕获）
		*/
		virtual void onCaptureLost(WindowEventArgs& e);

	protected:
		float mScreenWidth;		//	屏幕宽度
		float mScreenHeight;	//	屏幕高度

		ElementList	mElementList;	//	图文混排控件内容集合
		LineList mLines;		//	行信息集合
		float mEachLineHeight;	//	强制设置的每行高度
		float mLength;			//	图文混排内容的总长度

		std::string mHyperLinkId;	//	超链接内容的标识
		std::string mTextContent;	//	所响应的文本内容
		std::string mTriggerRect;	//	响应的矩形区域（格式："0-0-0-0"）

		Font *mTextFont;		//	文本字体
		std::string mTextFontName;	// 文本字体名字
		colour mTextCols;		//	文本颜色
		std::string mTextColsName;	// 文本颜色字符串
		bool mColsFlag;			//	颜色标识
		colour mSideCols;		//	描边颜色
		float mSidePix;			//	描边间距
		bool mSideFlag;			//	描边标识

		AlignType mAlignHor;	//	水平对齐方式
		AlignType mAlignVer;	//	垂直对齐方式

		bool mScrollbarVisible;	//	是否有垂直滚动条
		int mScrollMode;		//	滚动条显示模式
		ScrollbarPosition mScrollbarPos;	//	添加内容后，滚动条的位置

		colour mSpecSideCols;	//	指定内容的描边颜色
		float mSpecSidePix;		//	指定内容的描边像素
		bool mSpecSideFlag;		//	指定内容的描边标识

	};	//	end of class MultiLineBox

	/*************************************************************************
	*	图文混排控件工厂类
	*************************************************************************/
	class MultiLineBoxFactory : public WindowFactory
	{
	public:
		/**
		*	构造函数
		*/
		MultiLineBoxFactory(void) : WindowFactory(MultiLineBox_WidgetTypeName){}

		/**
		*	析构函数
		*/
		~MultiLineBoxFactory(void){}

		/**
		*	创建窗口
		*/
		Window* createWindow(const String& name);

		/**
		*	销毁窗口
		*/
		void destroyWindow(Window* window);
	};

	/*************************************************************************
	*	CEGUI全局接口
	*************************************************************************/
	/**
	*	功	能：把window转换为MultiLinebox类型
	*	参	数：ctrlName: 窗口名称
	*	返回值：图文混排控件
	*/
	MultiLineBox* toMultiLineBox(std::string ctrlName);

	/**
	*	功	能：根据字体font获取字符串str的长度
	*	参	数：fontName: 字体名称，str: 字符串
	*	返回值：-1（字体不存在）
	*/
	float getStrExtent(std::string fontName, std::string str);

	/**
	*	功	能：根据像素获取截取的字符位置
	*	参	数：fontName: 字体名称，str: 字符串，pixel: 截取结束位置的像素，startChar: 截取开始位置的字符索引
	*	返回值：size_t
	*/
	size_t getCharAtPixel(std::string fontName, std::string str, float pixel, size_t startChar=0);

	/**
	*	功	能：截取字符串
	*	参	数：fontName: 字体名称，str: 字符串，pixel: 截取结束位置的像素，startChar: 截取开始位置的字符索引
	*	返回值：std::string
	*/
	std::string subStrExtent(std::string fontName, std::string str, float pixel, size_t startChar=0);

	/**
	*	功	能：获取字符串str中的字符个数
	*	参	数：str: 字符串
	*	返回值：size_t
	*/
	size_t getCharacterCount(std::string str);

	/**
	*	功	能：截取字符串str的字串，以字符个数为单位
	*	参	数：str: 字符串，startPos: 开始位置，len: 截取个数
	*	返回值：std::string
	*/
	std::string subCharacter(std::string str, size_t startPos, int len);

	/**
	*	功	能：获取窗口坐标的各个值
	*	参	数：winname: 窗口名
	*			xy: 0（表获取的是x坐标），1（表获取的是y坐标）
	*			pos: 0（表获取的是scale），1（表获取的是offset）
	*	返回值：-1（参数错误），-2（窗口不存在）
	*/
	float getPositionVal(std::string winname, int xy, int pos);

}	//	end of namespace CEGUI


#endif	//	end of guard CEGUIMultiLineBox_H
