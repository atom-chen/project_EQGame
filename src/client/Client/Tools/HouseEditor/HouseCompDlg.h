#pragma once

//#include "GraphicsSystem.h"
#include "afxwin.h"

class CHouseCompDlg : public CDialog
{
public:
	CHouseCompDlg(CWnd* pParent = NULL);   
	virtual ~CHouseCompDlg();

	enum { IDD = IDD_COMP_DLG };
public:
	void notifySystemSetup();
	void notifySceneCreate();
	void notifySceneDestroy();
private:
	void initTree();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangePutmode();
	afx_msg void OnEditChange();
protected:
	CTreeCtrl				m_wndHouseCompTree;
	CComboBox				m_cmbPutMode;
	CEdit					m_input;
};

extern CHouseCompDlg * g_HouseCompDlg;