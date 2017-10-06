// MainFrm.h : CMainFrame 类的接口
//


#pragma once

#include "3DView.h"
#include "RoleSectionDlg.h"
#include "AnimationDlg.h"
#include "RoleRaceDlg.h"
#include "ModelDlg.h"


#include "Dock/scbarg.h"

#include "PropertyGrid/PropertyGrid.h"


#include "GraphicsSystem.h"

#include "NodeAdjuster.h"

#define IDC_CHARBAR				11500
#define IDC_CHARTREE			11501
#define IDC_AVATARBAR			11600
#define IDC_ANIMATIONBAR		11700





class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
public:
	/** 创建换装角色之前
	*/
	void doBeforeAvatarCreate();

	/** 创建换装角色之后
	*/
	void doAfterAvatarCreate();

	int getPage() const
	{
		return mPage;
	}

	void setPage( int val );

	void clearPage();

protected:
	void initToolBarItems();
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	/** 换装视图
	*/
	afx_msg void OnShowAvatarView();

	/** 模型视图
	*/
	afx_msg void OnShowModelView();

	/** 物件编辑视图
	*/
	afx_msg void OnShowObjectEditView();

	/** 物件房屋视图
	*/
	afx_msg void OnShowHouseView();

	/** 新建场景
	*/
	afx_msg void OnCreateNewScene();

	/** 打开场景
	*/
	afx_msg void OnOpenScene();

	/** 保存场景
	*/
	afx_msg void OnSaveScene();

	/** 另存场景
	*/
	afx_msg void OnSaveAsScene();

	/** 关闭场景
	*/
	afx_msg void OnCloseScene();

	/** 进入选择模式
	*/
	afx_msg void OnSelectMode();

	/** 进入移动模式
	*/
	afx_msg void OnMoveMode();

	/** 进入旋转模式
	*/
	afx_msg void OnRotateMode();

	/** 进入缩放模式
	*/
	afx_msg void OnScaleMode();

	/** 上一步
	*/
	afx_msg void OnPrevStep();

	/** 下一步
	*/
	afx_msg void OnNextStep();

	/** 锁定
	*/
	afx_msg void OnLockModel();

	/** 贴近
	*/
	afx_msg void OnMoveNear();

	DECLARE_MESSAGE_MAP()
public:
	// 状态条
	CStatusBar		m_wndStatusBar;

	// 工具栏
	CToolBar		m_wndToolBar;

	// 3d窗口
	C3DView			m_wnd3DView;

	// 角色种类相关
	CCoolBar		m_wndRoleRaceBar;
	CRoleRaceDlg	m_wndRoleRaceDlg;

	// 模型相关
	CCoolBar		m_wndModelBar;
	CModelDlg		m_wndModelDlg;

	// 角色部件相关
	CCoolBar		m_wndRoleSectionBar;
	CRoleSectionDlg	m_wndRoleSectionDlg;


	// 动画相关
	CCoolBar		m_wndAnimationBar;
	CAnimationDlg	m_wndAnimationDlg;

	// 属性相关
	CCoolBar		m_wndPropertyBar;
	CPropertyGrid	m_wndPropertyDlg;
protected:
	int				mPage;
public:
	afx_msg void OnShowxml();
	afx_msg void OnCommonKey();
};


