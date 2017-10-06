#pragma once

#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
// CBottomCommDlg dialog
enum GridDisplayMode;

class CBottomCommDlg : public CDialog
{
	DECLARE_DYNAMIC(CBottomCommDlg)

public:
	CBottomCommDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBottomCommDlg();

	void changeGridMode(int mode);

	void notifySceneCreate();
	void notifySceneDestroy();

	void notifyHouseSceneEnable();

	GridDisplayMode getGridDisplayMode() const;

	enum { IDD = IDD_BOTTOM_COMM };

protected:
	void resetContent();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBcHouseUp();
	afx_msg void OnBnClickedBcHouseDown();
	afx_msg void OnCbnSelchangeBcHouseMode();
	afx_msg void OnBnClickedBcCamWorldGrid();
	afx_msg void OnCbnSelchangeBcGridMode();
protected:
	CComboBox	m_wndGridMode;
	CButton		m_wndWorldGrid;
	CComboBox	m_wndHouseMode;
	CEdit		m_wndHouseCurrFloor;
	CEdit		m_wndCamRate;
	CSliderCtrl m_wndCamRateSld;
public:
	CComboBox mBrushTypeCtl;
	CEdit mBrushSizeXCtl;
	CEdit mBrushSizeZCtl;
	afx_msg void OnCbnSelchangeBcBrushType();
	afx_msg void OnEnKillfocusBcBrushSizeX();
	afx_msg void OnEnKillfocusBcBrushSizeZ();
	CSpinButtonCtrl mBrushSpinXCtl;
	CSpinButtonCtrl mBrushSpinZCtl;
	afx_msg void OnEnChangeBcBrushSizeX();
	afx_msg void OnEnChangeBcBrushSizeZ();
	afx_msg void OnDeltaposBcBrushSpinX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposBcBrushSpinZ(NMHDR *pNMHDR, LRESULT *pResult);
	int mBrushSizeXInt;
	int mBrushSizeZInt;
	afx_msg void OnBnClickedBcCamTerrgrid();
	CButton m_wndTerrGrid;
	afx_msg void OnBnClickedBcCamEdge();
};

extern CBottomCommDlg * g_BottomCommDlg;