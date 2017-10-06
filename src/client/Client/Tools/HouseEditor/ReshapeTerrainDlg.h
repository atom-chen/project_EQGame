#pragma once
#include "afxwin.h"


// CReshapeTerrainDlg dialog

class CReshapeTerrainDlg : public CDialog
{
	DECLARE_DYNAMIC(CReshapeTerrainDlg)

public:
	CReshapeTerrainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReshapeTerrainDlg();

	int mLeft;
	int mRight;
	int mTop;
	int mDown;

	int mRLeft;
	int mRRight;
	int mRTop;
	int mRDown;

	void setTypeModifyEdge();
	//处理类型
	int mType;

// Dialog Data
	enum { IDD = IDD_RESHAPE_TERRAIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit mLeftCtl;
	CEdit mRightCtl;
	CEdit mTopCtl;
	CEdit mDownCtl;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
