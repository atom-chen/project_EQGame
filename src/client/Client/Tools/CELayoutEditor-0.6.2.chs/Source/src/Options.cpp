///////////////////////////////////////////////////////////////////////////////
//  For project details and authors, refer to "CELayoutEditor.cpp".
//
//  This file is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This file is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//  To view the licence online, go to: http://www.gnu.org/copyleft/gpl.html
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "CELayoutEditor.h"
#ifdef __linux__
# include <unistd.h>

# define DATAPATH_VAR_NAME "CELAYOUTEDITOR_DATAPATH"
#endif

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

/** Define strings.*/
#define BIG_KEY_STEP			"BigKeyStep"
#define SMALL_KEY_STEP			"SmallKeyStep"
#define DEFAULT_FONT			"DefaultFont"
#define CURRENT_BACKGROUND      "CurrentBackground"
#define BACKGROUND_VISIBLE      "BackgroundVisible"
#define CURRENT_LAYOUT          "CurrentLayout"
#define VIEW_WIDTH              "ViewWidth"
#define VIEW_HEIGHT             "ViewHeight"
#define SNAP_MODE				"SnapMode"
#define GRID_SIZE				"GridSize"
#define GRID_VISIBLE			"GridVisible"
#define PATH_CONFIGS            "ConfigsPath"
#define PATH_FONTS              "FontsPath"
#define PATH_IMAGESETS          "ImagesetsPath"
#define PATH_LOOKNFEELS         "LookNFeelsPath"
#define PATH_SCRIPTS            "ScriptsPath"
#define PATH_SCHEMES            "SchemesPath" 
#define PATH_LAYOUTS            "LayoutsPath" 
#define SUPPORTED_PROPERTIES    "SupportedProperties"

#define INI_FILE    "CELayoutEditor.ini"
#define INI_SECTION "SETTINGS"

//////////////////////////////////////////////////////////////////////////
// OPTIONS GLOBALS
//////////////////////////////////////////////////////////////////////////

/** Declare the instance.*/
Options* Options::m_instance = NULL;

#define PROPERTY_DESC_COUNT 45
wxChar *g_property_en_chs[PROPERTY_DESC_COUNT][3] =
{
	{wxT("Name"),wxT("名称"),wxT("指示一个窗体的名称，必须为唯一。")},
	{wxT("Left Scale"),wxT("左缩放量"),wxT("指示一个窗体相对父窗体的左边缩放量。")},
	{wxT("Left Offset"),wxT("左偏移量"),wxT("指示一个窗体相对父窗体的左边偏移量。")},
	{wxT("Top Scale"),wxT("上缩放量"),wxT("指示一个窗体相对父窗体的上部缩放量。")},
	{wxT("Top Offset"),wxT("上偏移量"),wxT("指示一个窗体相对父窗体的上部偏移量。")},
	{wxT("Right Scale"),wxT("右缩放量"),wxT("指示一个窗体相对父窗体的右边缩放量。")},
	{wxT("Right Offset"),wxT("右偏移量"),wxT("指示一个窗体相对父窗体的右边偏移量。")},
	{wxT("Bottom Scale"),wxT("下缩放量"),wxT("指示一个窗体相对父窗体的下部缩放量。")},
	{wxT("Bottom Offset"),wxT("下偏移量"),wxT("指示一个窗体相对父窗体的下部偏移量。")},
	{wxT("Alpha"),wxT("透明度"),wxT("指示一个窗体的透明度值，1：完全不透明；0：完全透明。")},
	{wxT("ClickStepSize"),wxT("单击滑动大小"),wxT("指示一个窗体单击滑块时移动的大小。")},
	{wxT("MaximumValue"),wxT("最大值"),wxT("指示一个窗体属性的最大值。")},
	{wxT("Visible"),wxT("是否可见"),wxT("指示一个窗体的可见性。")},
	{wxT("AlwaysOnTop"),wxT("是否保持最顶端"),wxT("保持一个窗体一直在最顶端位置，无论是否有焦点。")},
	{wxT("ClippedByParent"),wxT("是否被父窗体裁剪"),wxT("指示当窗体部分或全部不在父窗体客户区时，是否裁剪不显示。")},
	{wxT("InheritsAlpha"),wxT("是否继承透明度"),wxT("指示窗体是否继承父窗体的透明度，若是则最终透明度根据某一算法和父窗体的混合结果。")},
	{wxT("Selected"),wxT("是否选中"),wxT("指示窗体是否被选中，如：单选和复选框。")},
	{wxT("ReadOnly"),wxT("是否只读"),wxT("指示窗体是否为只读，若是则用户无法进行编辑。")},
	{wxT("CloseButtonEnabled"),wxT("是否显示关闭按钮"),wxT("指示框架窗体是否显示关闭按钮。")},
	{wxT("DragMovingEnabled"),wxT("是否可拖拽移动"),wxT("指示窗体是否可以被拖拽移动。")},
	{wxT("FrameEnabled"),wxT("是否显示边框"),wxT("指示窗体是否显示边框。")},
	{wxT("SizingEnabled"),wxT("是否可更改大小"),wxT("指示窗体是否可以被任意更改大小。")},
	{wxT("TitlebarEnabled"),wxT("是否显示标题栏"),wxT("指示窗体是否显示标题栏。")},
	{wxT("MultiSelect"),wxT("是否多选"),wxT("指示窗体是否可以被多重选择。")},
	{wxT("Sort"),wxT("是否排序"),wxT("提示窗体是否对数据列表进行排序。")},
	{wxT("DraggingEnabled"),wxT("是否可拖拽"),wxT("指示窗体是否可以被拖拽移动。")},
	{wxT("BackgroundEnabled"),wxT("是否有背景图"),wxT("指示一个窗体是否可以有一个背景图。")},
	{wxT("InheritsTooltipText"),wxT("是否继承提示"),wxT("指示一个窗体当本身没有设置提示信息时，是否从父窗体中继承信息提示。")},
	{wxT("HoverImage"),wxT("悬停状态图片"),wxT("指示当鼠标在一窗体上悬停时显示的图片。")},
	{wxT("PushedImage"),wxT("按下状态图片"),wxT("指示当鼠标在一窗体上按下时显示的图片。")},
	{wxT("DisabledImage"),wxT("禁用状态图片"),wxT("指示当一个窗体被禁用时显示的图片。")},
	{wxT("NormalImage"),wxT("正常状态图片"),wxT("指示一个窗体在正常状态下显示的图片。")},
	{wxT("Font"),wxT("字体"),wxT("指示一个窗体绑定的字体。")},
	{wxT("TitlebarFont"),wxT("标题字体"),wxT("指示一个窗体标题显示时用的字体。")},
	{wxT("VerticalAlignment"),wxT("垂直对齐方式"),wxT("指示一个窗体的垂直对齐方式：Top，Centre，Bottom。")},
	{wxT("HorizontalAlignment"),wxT("水平对方方式"),wxT("指示一个窗体的水平对齐方式：Left，Centre，Right。")},
	{wxT("VertFormatting"),wxT("垂直格式方式"),wxT("指示一个窗体的垂直格式方式：TopAligned，BottomAligned，VertCentred。")},
	{wxT("HorzFormatting"),wxT("水平格式方式"),wxT("指示一个窗体的水平格式方式：LeftAligned，RightAligned，HorzCentred，HorzJustified，WordWrapLeftAligned，WordWrapRightAligned，WordWrapCentred，WordWrapJustified。")},
	{wxT("Tooltip"),wxT("提示信息"),wxT("指示当鼠标经过窗体时显示的提示信息。")},
	{wxT("Image"),wxT("图片"),wxT("指示窗体所要显示的图片。")},
	{wxT("TextColours"),wxT("文字颜色"),wxT("指示窗体的文字显示时的颜色，矩形四个顶点颜色。")},
	{wxT("NormalTextColour"),wxT("正常状态文字颜色"),wxT("指示一个窗体正常状态下的文本颜色。")},
	{wxT("HoverTextColour"),wxT("悬停状态文字颜色"),wxT("指示一个窗体悬停状态下的文本颜色。")},
	{wxT("PushedTextColour"),wxT("按下状态文字颜色"),wxT("指示一个窗体按下状态下的文本颜色。")},
	{wxT("DisabledTextColour"),wxT("禁用状态文字颜色"),wxT("指示一个窗体禁用状态下的文本颜色。")}
};
//////////////////////////////////////////////////////////////////////////
// OPTIONS FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
Options* Options::GetInstancePtr()
{
    // Create instance, if not done yet
    if (!m_instance)
    {
        m_instance = new Options();
    }
    return m_instance;
}

//==============================================================================================================================
//Converting a WChar string to a Ansi string
std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen<= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen -1] = 0;

	std::string strTemp(pszDst);
	delete [] pszDst;

	return strTemp;
}
std::string ws2s(std::wstring& inputws){ return WChar2Ansi(inputws.c_str()); }

//Converting a Ansi string to WChar string
std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)

{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if(nSize <= 0) return NULL;

	WCHAR *pwszDst = new WCHAR[nSize+1];
	if( NULL == pwszDst) return NULL;

	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if( pwszDst[0] == 0xFEFF) // skip Oxfeff
		for(int i = 0; i < nSize; i ++)
			pwszDst[i] = pwszDst[i+1];

	std::wstring wcharString(pwszDst);
	delete pwszDst;

	return wcharString;
}
std::wstring s2ws(const std::string& s){ return Ansi2WChar(s.c_str(),s.size());} 
//==============================================================================================================================
//------------------------------------------------------------------------
Options::Options() :

// Set configuration file path
m_iniFile(INI_FILE),

// Step defaults
m_bigKeyStep(10),        // 10 pixels per big step
m_smallKeyStep(1),       // 1 pixel per small step

// No font initially
m_defaultFont(),

// No background nor layout initially
m_currentBackground(),
m_backgroundVisible(false),
m_currentLayout(),

// Default to the smallest resolution
m_width(640),
m_height(480),

// Snap both X and Y
m_snapMode(SNAP_XY),

// Invisible grid, 10x10
m_gridSize(10),
m_gridVisible(false),

// No default data directories are set. This forces to prompt a directory browser on first use.
m_pathConfigs(),
m_pathFonts(),
m_pathImagesets(),
m_pathLookNFeels(),
m_pathScripts(),
m_pathSchemes(), 
m_pathLayouts(),

// Supported properties
m_supportedProperties(wxT("Alpha,float;ClickStepSize,float;MaximumValue,float;Visible,bool;AlwaysOnTop,bool;ClippedByParent,bool;InheritsAlpha,bool;Selected,bool;ReadOnly,bool;CloseButtonEnabled,bool;DragMovingEnabled,bool;FrameEnabled,bool;SizingEnabled,bool;TitlebarEnabled,bool;MultiSelect,bool;Sort,bool;DraggingEnabled,bool;BackgroundEnabled,bool;InheritsTooltipText,bool;HoverImage,text;PushedImage,text;DisabledImage,text;NormalImage,text;Font,font;TitlebarFont,font;VerticalAlignment,vert_align;HorizontalAlignment,horz_align;VertFormatting,vert_text_format;HorzFormatting,horz_text_format;Tooltip,text;Image,text;TextColours,text;"))
{
    // NOTE: Prefer constructor initialization lists (which are 
    // often more efficient) to assignment inside the 
    // constructor. Members in the initialization list should 
    // appear in the order they are declared in the class, as 
    // this is the order they will be initialized (no matter what).
    // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
	//===========================================================================================================
	int i, c;

	for(i = 0; i < PROPERTY_DESC_COUNT; i ++)
	{
		PROPERTYITEMINFO item;

		item.Name = wxString::Format(g_property_en_chs[i][1]);
		item.Desc = wxString::Format(g_property_en_chs[i][2]);

		m_pds.insert(std::make_pair(wxString::Format(g_property_en_chs[i][0]), item));
	}

	CIniFile iniFObj("Translate.ini");
	if(iniFObj.ReadFile())
	{
		c = iniFObj.GetValueI("COUNT", "Count");

		char kn[16];

		for(i = 0; i < c; i ++)
		{
			sprintf(kn, "T%d", i + 1);

			std::string namee = iniFObj.GetValue(kn, "NameE");
			std::string namet = iniFObj.GetValue(kn, "NameT");
			std::string desct = iniFObj.GetValue(kn, "DescT");

			std::wstring wnamee = s2ws(namee);
			std::wstring wnamet = s2ws(namet);
			std::wstring wdesct = s2ws(desct);

			PROPERTYDESCLIST::iterator fitr = m_pds.find(wxString::Format(wnamee.c_str()));
			if(fitr != m_pds.end())
			{
				fitr->second.Name = wxString::Format(wnamet.c_str());
				fitr->second.Desc = wxString::Format(wdesct.c_str());
			}
			else
			{
				PROPERTYITEMINFO item;

				item.Name = wxString::Format(wnamet.c_str());
				item.Desc = wxString::Format(wdesct.c_str());

				m_pds.insert(std::make_pair(wxString::Format(wnamee.c_str()), item));
			}
		}
	}
	//===========================================================================================================

    // Attempt to read configuration file from disk
    if (!m_iniFile.ReadFile())
        // File not found: Warn user and keep default settings
        LogWarning (wxT("The file '%s' could not be found; default settings will apply now."), wxT(INI_FILE));
    else
        // File found: Read and assign read values
        UseSettings();
}

//------------------------------------------------------------------------
Options::~Options()
{
    // Swallow any exceptions found to prevent the destructor from emitting them.
    // NOTE: An exception should never leave a destructor as it may yield
    // undefined behavior or even lead to program abnormal termination 
    // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
    try
    {
        // Save all current settings.
        (void)m_iniFile.SetValueI(INI_SECTION, BIG_KEY_STEP, m_bigKeyStep);
        (void)m_iniFile.SetValueI(INI_SECTION, SMALL_KEY_STEP, m_smallKeyStep);
        (void)m_iniFile.SetValue(INI_SECTION, DEFAULT_FONT, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_defaultFont)));
        (void)m_iniFile.SetValue(INI_SECTION, CURRENT_BACKGROUND, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_currentBackground)));
        (void)m_iniFile.SetValueI(INI_SECTION, BACKGROUND_VISIBLE, m_backgroundVisible);
        (void)m_iniFile.SetValue(INI_SECTION, CURRENT_LAYOUT, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_currentLayout)));
        (void)m_iniFile.SetValueI(INI_SECTION, VIEW_WIDTH, m_width);
        (void)m_iniFile.SetValueI(INI_SECTION, VIEW_HEIGHT, m_height);
        (void)m_iniFile.SetValueI(INI_SECTION, SNAP_MODE, m_snapMode);
        (void)m_iniFile.SetValueI(INI_SECTION, GRID_SIZE, m_gridSize);
        (void)m_iniFile.SetValueI(INI_SECTION, GRID_VISIBLE, m_gridVisible);
        (void)m_iniFile.SetValue(INI_SECTION, PATH_CONFIGS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathConfigs)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_FONTS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathFonts)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_IMAGESETS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathImagesets)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_LOOKNFEELS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathLookNFeels)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_SCRIPTS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathScripts)));
        (void)m_iniFile.SetValue(INI_SECTION, PATH_SCHEMES, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathSchemes)));
		(void)m_iniFile.SetValue(INI_SECTION, PATH_LAYOUTS, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_pathLayouts)));
        (void)m_iniFile.SetValue(INI_SECTION, SUPPORTED_PROPERTIES, wx_static_cast(const char*, wxConvLibc.cWX2MB(m_supportedProperties)));

        // Save into the same path as the Load
        (void)m_iniFile.WriteFile();
    }
    // Add exception to log
    LOG_EXCEPTION_SAFE("Options::~Options", "Failed to destroy options", "Failed to destroy options", "Failed to destroy options");
}

//------------------------------------------------------------------------
void Options::UseSettings()
{
    std::string  value;

    // Don't blindly read the values; Only overwrite defaults when found
    value = m_iniFile.GetValue(INI_SECTION, BIG_KEY_STEP);
    if (value != "")
    {
        m_bigKeyStep = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, SMALL_KEY_STEP);
    if (value != "")
    {
        m_smallKeyStep = atoi(value.c_str());
    }
    m_currentBackground = wxConvLibc.cMB2WX(m_iniFile.GetValue(INI_SECTION, CURRENT_BACKGROUND).c_str());
    value = m_iniFile.GetValue(INI_SECTION, BACKGROUND_VISIBLE);
    if (value != "")
    {
        const int number = atoi(value.c_str());
        m_backgroundVisible = (number == 1);
    }
    m_defaultFont = wxConvLibc.cMB2WX(m_iniFile.GetValue(INI_SECTION, DEFAULT_FONT).c_str());
    m_currentLayout = wxConvLibc.cMB2WX(m_iniFile.GetValue(INI_SECTION, CURRENT_LAYOUT).c_str());
    value = m_iniFile.GetValue(INI_SECTION, VIEW_WIDTH);
    if (value != "")
    {
        m_width = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, VIEW_HEIGHT);
    if (value != "")
    {
        m_height = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, SNAP_MODE);
    if (value != "")
    {
        m_snapMode = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, GRID_SIZE);
    if (value != "")
    {
        m_gridSize = atoi(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, GRID_VISIBLE);
    if (value != "")
    {
        const int number = atoi(value.c_str());
        m_gridVisible = (number == 1);
    }
    // Load path information here 
    value = m_iniFile.GetValue(INI_SECTION, PATH_CONFIGS);
    if (value != "")
    {
        m_pathConfigs = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_FONTS);
    if (value != "")
    {
        m_pathFonts = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_IMAGESETS);
    if (value != "")
    {
        m_pathImagesets = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_LOOKNFEELS);
    if (value != "")
    {
        m_pathLookNFeels = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_SCRIPTS);
    if (value != "")
    {
        m_pathScripts = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, PATH_SCHEMES);
    if (value != "")
    {
        m_pathSchemes = wxConvLibc.cMB2WX(value.c_str());
    }
	value = m_iniFile.GetValue(INI_SECTION, PATH_LAYOUTS);
    if (value != "")
    {
        m_pathLayouts = wxConvLibc.cMB2WX(value.c_str());
    }
    value = m_iniFile.GetValue(INI_SECTION, SUPPORTED_PROPERTIES);
    if (value != "")
    {
        m_supportedProperties = wxConvLibc.cMB2WX(value.c_str());
    }
}
