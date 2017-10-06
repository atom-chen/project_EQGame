#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSceneObjectDlg dialog

class CSceneObjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CSceneObjectDlg)

public:
	CSceneObjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSceneObjectDlg();

// Dialog Data
	enum { IDD = IDD_SCENE_OBJECT };

public:
	void notifySystemSetup();
	void notifySceneDestroy();

	void reloadResource();

	void unselectTree();
	void changeTransformButton(const std::string& button);

	void OnCheck(int button, bool press);

protected:
	bool mTreeChanging;
	bool mTransformButtonChanging;

public:
	CToolTipCtrl	mSelectTT;
	CToolTipCtrl	mMoveTT;
	CToolTipCtrl	mRotationTT;
	CToolTipCtrl	mScaleTT;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl mTreeCtl;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedSoTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedSoMove();
	afx_msg void OnBnClickedSoRotation();
	afx_msg void OnBnClickedSoScale();
	afx_msg void OnBnClickedSoSelect();
	CButton mSelectCtl;
	CButton mMoveCtl;
	CButton mRotationCtl;
	CButton mScaleCtl;
	CEdit	m_input;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeEdit1();
};

extern CSceneObjectDlg* g_CSceneObjectDlg;