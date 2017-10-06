/////////////////////////////////////////////////////////////////////////////
// Name:        equpdatermainwin.h
// Purpose:     
// Author:      xj
// Modified by: 
// Created:     15/11/2010 09:26:40
// RCS-ID:      
// Copyright:   XJ-1
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 15/11/2010 09:26:40

#ifndef _EQUPDATERMAINWIN_H_
#define _EQUPDATERMAINWIN_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/html/htmlwin.h"
#include "wx/timer.h"
#include "curl.h"
#include "tinyxml.h"
#include "XIniFile.h"
#include "XFuns.h"
#include "XLog.h"
#include "XMD5Calc.h"
#include "DirFileManager.h"
#include "PackFileManager.h"
#include "IWxThread.h"
#include "wxStaticTextX.h"
#include "wxGaugeX.h"
#include "wxButtonX.h"
#include <list>
#include <vector>
#include <map>
#include <hash_map>
////@end includes

//目录组类型
#define nGROUP_TYPE_DIR 0
//包组类型
#define nGROUP_TYPE_PACK 1

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_EQUPDATERMAINWIN 10000
#define ID_HTMLWINDOW 10001
#define ID_TEXTCTRL 10002
#define ID_GAUGE 10004
#define ID_GAUGE1 10005
#define ID_BTN_RUN_GAME 10003
#define ID_CHECKBOX 10006
#define ID_TIME_LEAVE 10007
#define ID_TIMER_MAIN 10008
#define ID_BUTTON_CLOSE 10009
#define SYMBOL_EQUPDATERMAINWIN_STYLE wxFRAME_SHAPED|wxNO_BORDER//wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxSIMPLE_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_EQUPDATERMAINWIN_TITLE _("EQUpdaterI")
#define SYMBOL_EQUPDATERMAINWIN_IDNAME ID_EQUPDATERMAINWIN
#define SYMBOL_EQUPDATERMAINWIN_SIZE wxSize(640, 480)
#define SYMBOL_EQUPDATERMAINWIN_POSITION wxDefaultPosition
////@end control identifiers

#pragma pack(1)

typedef struct tagTEMPFILEDESC
{
	DWORD Size;
	char Code[33];
} TEMPFILEDESC, *LPTEMPFILEDESC;//临时文件描述

typedef struct tagLOADFILEITEM
{
	bool IsError;//是否下载错误
	bool IsExit;//该文件已经下载，但未移动到目标目录或打包到目标包
	DWORD StartPos;//开始下载偏移量，已存在临时目录中但未下载完
	DWORD Size;//未下载完的文件的总大小
	std::string Url;
	std::string Path;
	std::string GroupDir;//原Group Directory
	std::string GroupType;//原Group Type
	std::string ItemFile;//原Item File
	std::string ItemCode;//原Item Code
} LOADFILEITEM, *LPLOADFILEITEM;//仅用于文件下载的项

typedef std::list<LPLOADFILEITEM> LOADFILEITEMLIST;

typedef struct tagLOADFILEINFO
{
	std::string Url;//该文件所在服务器的地址
	std::string Path;//该文件下载保存的临时相对目录
	std::string DstPath;//该文件的目标保存目录
	std::string Code;//原Code值
	std::string File;//原File值
	bool IsError;
} LOADFILEINFO, *LPLOADFILEINFO;

typedef std::list<LOADFILEINFO> LOADFILELIST;
typedef stdext::hash_map<std::string, LOADFILEINFO> LOADFILEMAPS;

typedef struct tagLOADFILEGROUPINFO
{
	BYTE nType;//组数字类型
	std::string Dir;//组目录
	std::string sType;//组字符串类型
	LOADFILELIST Files;//该组的所有文件项
	LOADFILEMAPS FMaps;
} LOADFILEGROUPINFO, *LPLOADFILEGROUPINFO;

typedef std::list<LOADFILEGROUPINFO> LOADFILEGROUPLIST;

typedef struct tagLOADFILELISTINFO
{
	std::string Dir;//列表根目录
	LOADFILEGROUPLIST Groups;//列表组列表
} LOADFILELISTINFO, *LPLOADFILELISTINFO;

#pragma pack()

/*!
 * EQUpdaterMainWin class declaration
 */

class EQUpdaterMainWin: public wxFrame, public IWxThread
{    
    DECLARE_CLASS( EQUpdaterMainWin )
    DECLARE_EVENT_TABLE()

public:
	wxStaticTextX *m_pLBFile;
	wxStaticTextX *m_pLBTotal;
	wxStaticTextX *m_pTimeFile;
	wxStaticTextX *m_pTimeTotal;

	wxGaugeX *m_pGaugeFile;
	wxGaugeX *m_pGaugeAll;

	wxButtonX *m_pBtnClose;

	short m_loadState;
	DWORD m_stime, m_totalUseTime, m_totalUseTimeT;

	wxTimer m_timer;

	CURL *m_loadUrlObj;
	WxThreadCls *m_loadThreadObj;
	FILE *m_loadHFile;
	bool m_loading;
	bool m_terminate;
	bool m_abort;
	DWORD m_loadedCnt;
	DWORD m_totalLoadCnt;
	DWORD m_totalMoveCnt;
	int m_loadedSize, m_curTotal, m_curDown;
	bool m_runAfter;
	bool m_hasUpdateMe;
	bool m_allSuccess;

	TEMPFILEDESC m_curDesc;
	FILE *m_curHDesc;
	TiXmlDocument *m_curListDoc;

	LOADFILEITEMLIST m_loadFiles;//下载文件列表
	LOADFILELISTINFO m_updateList;//下载文件详细列表

	std::string m_serverAddr;
	std::string m_listFName;//列表文件名
	std::string m_listFile;//列表文件
	std::string m_tmpListFile;//临时列表文件
	std::string m_runExe;
	std::string m_targetDir;//目标文件存储根目录

	wxChar*		m_para;
	wxChar*		m_user;
	wxChar*		m_pwd;

	wxPoint m_ptDown, m_ptOld;
	//wxBitmap m_bmpRg;
	wxBitmap m_bmpBg, m_bmpPb, m_bmpPh, m_bmpBtnN, m_bmpBtnH, m_bmpBtnP;
	bool m_mouseHasDown;

public:
	//开始文件下载
	void StartLoad(LPLOADFILEITEM dlInfo);
	//结束文件下载时触发
	void EndLoad(LPLOADFILEITEM dlInfo);
	//下载列表文件
	void LoadList();
	//下载一个文件
	void LoadFile(LPLOADFILEITEM dlInfo);
	//下载线程函数
	virtual void ThreadPro(void *pv);
	//更新下载状态
	void UpdateLoadStatus(int total, int down);
	//从列表xml字符串文档中获取下载列表
	void GetLoadFileItems(std::string &listFile, LOADFILELISTINFO &loadList, bool toMap = false);
	//生成下载列表方法1:采用顺序读取比较法,对于客户端和服务端的列表文件更改和顺序变化相对较少时接近n次比较速度,反之则可能接近n*n次!!!!
	void BuildLoadList1();
	bool IsChanged1(TiXmlElement *listRoot, TiXmlElement **lastGroup, TiXmlElement **lastItem, const char *groupDir, const char *itemFile, const char *code, const char *dstFile, const char *tmpFile, int &nWhere, DWORD &startPos, DWORD &size);
	//生成下载列表方法2:先用哈希表缓存列表,在进行查找比对,整体速度在2*n左右,缓存这个表需消耗一定时间!
	void BuildLoadList2();
	bool IsChanged2(LOADFILEINFO &loadInfo, LOADFILELISTINFO &loadList, int &nWhere, DWORD &startPos, DWORD &size);
	bool GetFileDesc(const char *file, LPTEMPFILEDESC desc);
	//加载文件配置
	void LoadConfig();
	//释放下载列表
	void ClearFileList(LOADFILELISTINFO &fileList);
	//打开创建一个文件，若目录不存在会自动创建
	FILE *OpenFile(const char *file, const char *mode);
	//将下载到临时目录的文件移动到目标目录或包中
	void MoveToTarget();
	void SetErrorFile(std::string &path);
	//更新当前下载列表文件
	void UpdateListFile(const char *groupDir, const char *groupType, const char *itemFile, const char *itemCode);
	void OpenListFile();
	void CloseListFile();
	void CheckIsMe(std::string &file);
	void CheckIsMe(const char *file);
	//void SetWindowShape(wxBitmap &bmpShape);

public:
    /// Constructors
    EQUpdaterMainWin();
    EQUpdaterMainWin( wxWindow* parent, wxWindowID id = SYMBOL_EQUPDATERMAINWIN_IDNAME, const wxString& caption = SYMBOL_EQUPDATERMAINWIN_TITLE, const wxPoint& pos = SYMBOL_EQUPDATERMAINWIN_POSITION, const wxSize& size = SYMBOL_EQUPDATERMAINWIN_SIZE, long style = SYMBOL_EQUPDATERMAINWIN_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_EQUPDATERMAINWIN_IDNAME, const wxString& caption = SYMBOL_EQUPDATERMAINWIN_TITLE, const wxPoint& pos = SYMBOL_EQUPDATERMAINWIN_POSITION, const wxSize& size = SYMBOL_EQUPDATERMAINWIN_SIZE, long style = SYMBOL_EQUPDATERMAINWIN_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

    /// Destructor
    ~EQUpdaterMainWin();

    /// Initialises member variables
    void Init();
	
	void Dispose();

	void StartGame();

////@begin EQUpdaterMainWin event handler declarations
	void OnTimer(wxTimerEvent &event);

	void OnCloseWindow( wxCloseEvent& event );

	void OnDestroy( wxWindowDestroyEvent& event );

////@end EQUpdaterMainWin event handler declarations

////@begin EQUpdaterMainWin member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end EQUpdaterMainWin member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

	/// All mouse events event handler for ID_WXTESTMAIN
	void OnMouse( wxMouseEvent& event );

	/// wxEVT_ERASE_BACKGROUND event handler for ID_WXTESTMAIN
	void OnEraseBackground( wxEraseEvent& event );

	/// wxEVT_PAINT event handler for ID_WXTESTMAIN
	void OnPaint( wxPaintEvent& event );
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);

	/// wxEVT_LEFT_UP event handler for ID_BUTTON_CLOSE
	void OnBtnCloseLeftUp(wxMouseEvent& event);

////@begin EQUpdaterMainWin member variables
////@end EQUpdaterMainWin member variables
};

#endif
    // _EQUPDATERMAINWIN_H_
