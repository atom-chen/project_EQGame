// MainFrm.h : CMainFrame 类的接口
//


#pragma once

#include "ChildView.h"
#include "Dock/scbarg.h"
#include "Dock/CoolTabCtrl.h"
#include "GraphicsSystem.h"
#include "BottomCommDlg.h"
#include "HouseGridDlg.h"
#include "ResourceDlg.h"
#include "PropertyGrid/PropertyGrid.h"
#include "HouseCompDlg.h"
#include "BrushSelectorDlg.h"
#include "SceneObjectDlg.h"
#include "PathSearchDlg.h"
#include "LightAdjustDlg.h"


#define IDC_LEFTBAR				11500

#define IDC_RIGHTBAR			11600
#define IDC_RIGHTTABCTRL		11601

#define IDC_BOTTOMBAR			11700


class CMainFrame : public CFrameWnd, public GraphicsSystem::Listener
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 属性
public:
	CChildView&		getView() { return m_wndView; }

	CPropertyGrid&	getPropertyDlg() { return m_wndPropertyDlg; }

	CPropertyGrid&	getAmbientDlg() { return m_wndAmbientDlg; }

	CPropertyGrid&	getLightDlg() { return m_wndLightDlg; }
	CLightAdjustDlg& getLightAdjust() { return m_wndLightAdjust; }

	CResourceDlg&	getResourceDlg() { return m_wndResourceDlg; }

	CPropertyGrid&	getPostProcessDlg() { return m_wndPostProcessDlg; }

// 操作
public:
	void showPanels( bool bShow );
	void showError( const std::string & errMsg );
	void showPos( const std::string& msg );
	void UnCheckedToolBarItem( int barID );
	void OpenScene(std::string fileName);

	void saveRegion(const std::string& name, const std::string& path);
	void loadRegion(const std::string& name);

	void saveNav(const std::string& name, const std::string& path);
	void loadNav(const std::string& name);

protected:
	void initToolBarItems();
	void OnSceneCreate(bool house, bool terrain);
	void OnSceneDestroy();
// 重写
public:
	void notifySystemSetup();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnBrowseGridMode();
	afx_msg void OnBrowseComponentMode();
	afx_msg void OnShowProperty();
	afx_msg void OnAppImportscene();
	afx_msg void OnAppTerrain();
	afx_msg void OnAppSavehouse();
	afx_msg void OnAppClose();
	afx_msg void OnAppOpen();
	afx_msg void OnShowPanel();
	afx_msg void OnClose();
	afx_msg void OnSetAmbient();
	afx_msg void OnSetLight();
	afx_msg void OnCreateTerrain();
	afx_msg void OnSetPostProcess();
	afx_msg void OnTerrainShot();
protected:  
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CChildView    m_wndView;

	CCoolBar		m_wndLeftBar;
	CCoolBar		m_wndRightBar;
	CCoolBar		m_wndBottomBar;
	CCoolTabCtrl	m_wndRightTabCtrl;

	CBottomCommDlg	m_wndBottomCommDlg;
	CHouseGridDlg	m_wndHouseGridDlg;
	CHouseCompDlg	m_wndHouseCompDlg;
	CBrushSelectorDlg	m_wndBrushSelectorDlg;
	CSceneObjectDlg m_wndSceneObjectDlg;
	CPathSearchDlg	m_wndCPathSearchDlg;

	CResourceDlg		m_wndResourceDlg;

	// 属性相关
	CCoolBar		m_wndPropertyBar;
	CPropertyGrid	m_wndPropertyDlg;

	// 环境光设置
	CCoolBar		m_wndAmbientBar;
	CPropertyGrid	m_wndAmbientDlg;

	// 灯光设置
	CCoolBar		m_wndLightBar;
	CPropertyGrid	m_wndLightDlg;
	CLightAdjustDlg m_wndLightAdjust;

	// 后处理设置
	CCoolBar		m_wndPostProcessBar;
	CPropertyGrid	m_wndPostProcessDlg;

	//
	bool			mFromOgreMaxScene;

	std::string		mLastResFileName;

	bool			mOpening;

public:
	afx_msg void OnTestAutoFindPath();
	afx_msg void OnLoadRemoveFarmScene();
	afx_msg void OnRunFarmScript();
	afx_msg void OnUpdateRunFarmScript(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLoadRemoveFarmScene(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTerrainShadowmap(CCmdUI *pCmdUI);
	afx_msg void OnTerrainShadowmap();
	afx_msg void OnProjView();
	afx_msg void OnUpdateTestOutsideCut(CCmdUI *pCmdUI);
	afx_msg void OnTestOutsideCut();
};

extern CMainFrame * g_MainFrame;
extern bool g_MainFrameClosing;

