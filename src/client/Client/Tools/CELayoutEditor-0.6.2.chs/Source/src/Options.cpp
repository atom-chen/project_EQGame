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
	{wxT("Name"),wxT("����"),wxT("ָʾһ����������ƣ�����ΪΨһ��")},
	{wxT("Left Scale"),wxT("��������"),wxT("ָʾһ��������Ը�����������������")},
	{wxT("Left Offset"),wxT("��ƫ����"),wxT("ָʾһ��������Ը���������ƫ������")},
	{wxT("Top Scale"),wxT("��������"),wxT("ָʾһ��������Ը�������ϲ���������")},
	{wxT("Top Offset"),wxT("��ƫ����"),wxT("ָʾһ��������Ը�������ϲ�ƫ������")},
	{wxT("Right Scale"),wxT("��������"),wxT("ָʾһ��������Ը�������ұ���������")},
	{wxT("Right Offset"),wxT("��ƫ����"),wxT("ָʾһ��������Ը�������ұ�ƫ������")},
	{wxT("Bottom Scale"),wxT("��������"),wxT("ָʾһ��������Ը�������²���������")},
	{wxT("Bottom Offset"),wxT("��ƫ����"),wxT("ָʾһ��������Ը�������²�ƫ������")},
	{wxT("Alpha"),wxT("͸����"),wxT("ָʾһ�������͸����ֵ��1����ȫ��͸����0����ȫ͸����")},
	{wxT("ClickStepSize"),wxT("����������С"),wxT("ָʾһ�����嵥������ʱ�ƶ��Ĵ�С��")},
	{wxT("MaximumValue"),wxT("���ֵ"),wxT("ָʾһ���������Ե����ֵ��")},
	{wxT("Visible"),wxT("�Ƿ�ɼ�"),wxT("ָʾһ������Ŀɼ��ԡ�")},
	{wxT("AlwaysOnTop"),wxT("�Ƿ񱣳����"),wxT("����һ������һֱ�����λ�ã������Ƿ��н��㡣")},
	{wxT("ClippedByParent"),wxT("�Ƿ񱻸�����ü�"),wxT("ָʾ�����岿�ֻ�ȫ�����ڸ�����ͻ���ʱ���Ƿ�ü�����ʾ��")},
	{wxT("InheritsAlpha"),wxT("�Ƿ�̳�͸����"),wxT("ָʾ�����Ƿ�̳и������͸���ȣ�����������͸���ȸ���ĳһ�㷨�͸�����Ļ�Ͻ����")},
	{wxT("Selected"),wxT("�Ƿ�ѡ��"),wxT("ָʾ�����Ƿ�ѡ�У��磺��ѡ�͸�ѡ��")},
	{wxT("ReadOnly"),wxT("�Ƿ�ֻ��"),wxT("ָʾ�����Ƿ�Ϊֻ�����������û��޷����б༭��")},
	{wxT("CloseButtonEnabled"),wxT("�Ƿ���ʾ�رհ�ť"),wxT("ָʾ��ܴ����Ƿ���ʾ�رհ�ť��")},
	{wxT("DragMovingEnabled"),wxT("�Ƿ����ק�ƶ�"),wxT("ָʾ�����Ƿ���Ա���ק�ƶ���")},
	{wxT("FrameEnabled"),wxT("�Ƿ���ʾ�߿�"),wxT("ָʾ�����Ƿ���ʾ�߿�")},
	{wxT("SizingEnabled"),wxT("�Ƿ�ɸ��Ĵ�С"),wxT("ָʾ�����Ƿ���Ա�������Ĵ�С��")},
	{wxT("TitlebarEnabled"),wxT("�Ƿ���ʾ������"),wxT("ָʾ�����Ƿ���ʾ��������")},
	{wxT("MultiSelect"),wxT("�Ƿ��ѡ"),wxT("ָʾ�����Ƿ���Ա�����ѡ��")},
	{wxT("Sort"),wxT("�Ƿ�����"),wxT("��ʾ�����Ƿ�������б��������")},
	{wxT("DraggingEnabled"),wxT("�Ƿ����ק"),wxT("ָʾ�����Ƿ���Ա���ק�ƶ���")},
	{wxT("BackgroundEnabled"),wxT("�Ƿ��б���ͼ"),wxT("ָʾһ�������Ƿ������һ������ͼ��")},
	{wxT("InheritsTooltipText"),wxT("�Ƿ�̳���ʾ"),wxT("ָʾһ�����嵱����û��������ʾ��Ϣʱ���Ƿ�Ӹ������м̳���Ϣ��ʾ��")},
	{wxT("HoverImage"),wxT("��ͣ״̬ͼƬ"),wxT("ָʾ�������һ��������ͣʱ��ʾ��ͼƬ��")},
	{wxT("PushedImage"),wxT("����״̬ͼƬ"),wxT("ָʾ�������һ�����ϰ���ʱ��ʾ��ͼƬ��")},
	{wxT("DisabledImage"),wxT("����״̬ͼƬ"),wxT("ָʾ��һ�����屻����ʱ��ʾ��ͼƬ��")},
	{wxT("NormalImage"),wxT("����״̬ͼƬ"),wxT("ָʾһ������������״̬����ʾ��ͼƬ��")},
	{wxT("Font"),wxT("����"),wxT("ָʾһ������󶨵����塣")},
	{wxT("TitlebarFont"),wxT("��������"),wxT("ָʾһ�����������ʾʱ�õ����塣")},
	{wxT("VerticalAlignment"),wxT("��ֱ���뷽ʽ"),wxT("ָʾһ������Ĵ�ֱ���뷽ʽ��Top��Centre��Bottom��")},
	{wxT("HorizontalAlignment"),wxT("ˮƽ�Է���ʽ"),wxT("ָʾһ�������ˮƽ���뷽ʽ��Left��Centre��Right��")},
	{wxT("VertFormatting"),wxT("��ֱ��ʽ��ʽ"),wxT("ָʾһ������Ĵ�ֱ��ʽ��ʽ��TopAligned��BottomAligned��VertCentred��")},
	{wxT("HorzFormatting"),wxT("ˮƽ��ʽ��ʽ"),wxT("ָʾһ�������ˮƽ��ʽ��ʽ��LeftAligned��RightAligned��HorzCentred��HorzJustified��WordWrapLeftAligned��WordWrapRightAligned��WordWrapCentred��WordWrapJustified��")},
	{wxT("Tooltip"),wxT("��ʾ��Ϣ"),wxT("ָʾ����꾭������ʱ��ʾ����ʾ��Ϣ��")},
	{wxT("Image"),wxT("ͼƬ"),wxT("ָʾ������Ҫ��ʾ��ͼƬ��")},
	{wxT("TextColours"),wxT("������ɫ"),wxT("ָʾ�����������ʾʱ����ɫ�������ĸ�������ɫ��")},
	{wxT("NormalTextColour"),wxT("����״̬������ɫ"),wxT("ָʾһ����������״̬�µ��ı���ɫ��")},
	{wxT("HoverTextColour"),wxT("��ͣ״̬������ɫ"),wxT("ָʾһ��������ͣ״̬�µ��ı���ɫ��")},
	{wxT("PushedTextColour"),wxT("����״̬������ɫ"),wxT("ָʾһ�����尴��״̬�µ��ı���ɫ��")},
	{wxT("DisabledTextColour"),wxT("����״̬������ɫ"),wxT("ָʾһ���������״̬�µ��ı���ɫ��")}
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
