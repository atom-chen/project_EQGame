#pragma once

#include "PropertyGrid/PropertyGrid.h"
#include "ResourceDlg.h"
#include "afxwin.h"

// CLightAdjustDlg dialog

class CLightAdjustDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightAdjustDlg)

public:
	CLightAdjustDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLightAdjustDlg();

	void open(CResourceDlg::PropSectionMap& section, CPropertyGrid* prop, bool AoP, int index, bool daynight);
	void onValueChange();
	void onRangeChange();

// Dialog Data
	enum { IDD = IDD_LIGHT_ADJUST };

protected:
	CPropertyGrid* mProp;
	HITEM mItem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
public:
	CEdit mEditRangeP;
	CEdit mEditRangeB;
	CEdit mEditRangeE;
	CScrollBar mScrollRange;
	CEdit mEditConstP;
	CEdit mEditConstB;
	CEdit mEditConstE;
	CScrollBar mScrollConst;
	CEdit mEditLineP;
	CEdit mEditLineB;
	CEdit mEditLineE;
	CScrollBar mScrollLine;
	CEdit mEditQuadP;
	CEdit mEditQuadB;
	CEdit mEditQuadE;
	CScrollBar mScrollQuad;

	afx_msg void OnEnChangeEditRangeP();
	afx_msg void OnEnChangeEditRangeB();
	afx_msg void OnEnChangeEditRangeE();
	afx_msg void OnEnChangeEditConstP();
	afx_msg void OnEnChangeEditConstB();
	afx_msg void OnEnChangeEditConstE();
	afx_msg void OnEnChangeEditLineP();
	afx_msg void OnEnChangeEditLineB();
	afx_msg void OnEnChangeEditLineE();
	afx_msg void OnEnChangeEditQuadP();
	afx_msg void OnEnChangeEditQuadB();
	afx_msg void OnEnChangeEditQuadE();
};
