#pragma once
#include "afxwin.h"


// CCreateTerrainDlg dialog

class CCreateTerrainDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateTerrainDlg)

public:
	CCreateTerrainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreateTerrainDlg();

public:
	int mWidth;
	int mDepth;

// Dialog Data
	enum { IDD = IDD_CREATE_TERRAIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit mWidthCtl;
	CEdit mDepthCtl;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
