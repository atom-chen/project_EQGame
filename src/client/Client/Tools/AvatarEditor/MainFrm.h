// MainFrm.h : CMainFrame ��Ľӿ�
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
	/** ������װ��ɫ֮ǰ
	*/
	void doBeforeAvatarCreate();

	/** ������װ��ɫ֮��
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
	/** ��װ��ͼ
	*/
	afx_msg void OnShowAvatarView();

	/** ģ����ͼ
	*/
	afx_msg void OnShowModelView();

	/** ����༭��ͼ
	*/
	afx_msg void OnShowObjectEditView();

	/** ���������ͼ
	*/
	afx_msg void OnShowHouseView();

	/** �½�����
	*/
	afx_msg void OnCreateNewScene();

	/** �򿪳���
	*/
	afx_msg void OnOpenScene();

	/** ���泡��
	*/
	afx_msg void OnSaveScene();

	/** ��泡��
	*/
	afx_msg void OnSaveAsScene();

	/** �رճ���
	*/
	afx_msg void OnCloseScene();

	/** ����ѡ��ģʽ
	*/
	afx_msg void OnSelectMode();

	/** �����ƶ�ģʽ
	*/
	afx_msg void OnMoveMode();

	/** ������תģʽ
	*/
	afx_msg void OnRotateMode();

	/** ��������ģʽ
	*/
	afx_msg void OnScaleMode();

	/** ��һ��
	*/
	afx_msg void OnPrevStep();

	/** ��һ��
	*/
	afx_msg void OnNextStep();

	/** ����
	*/
	afx_msg void OnLockModel();

	/** ����
	*/
	afx_msg void OnMoveNear();

	DECLARE_MESSAGE_MAP()
public:
	// ״̬��
	CStatusBar		m_wndStatusBar;

	// ������
	CToolBar		m_wndToolBar;

	// 3d����
	C3DView			m_wnd3DView;

	// ��ɫ�������
	CCoolBar		m_wndRoleRaceBar;
	CRoleRaceDlg	m_wndRoleRaceDlg;

	// ģ�����
	CCoolBar		m_wndModelBar;
	CModelDlg		m_wndModelDlg;

	// ��ɫ�������
	CCoolBar		m_wndRoleSectionBar;
	CRoleSectionDlg	m_wndRoleSectionDlg;


	// �������
	CCoolBar		m_wndAnimationBar;
	CAnimationDlg	m_wndAnimationDlg;

	// �������
	CCoolBar		m_wndPropertyBar;
	CPropertyGrid	m_wndPropertyDlg;
protected:
	int				mPage;
public:
	afx_msg void OnShowxml();
	afx_msg void OnCommonKey();
};


