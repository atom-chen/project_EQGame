#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "resource.h"
#include "UIGirdEvent.h"
//#include "GraphicsSystem.h"



class CHouseGridDlg : public CDialog
{
	DECLARE_DYNAMIC(CHouseGridDlg)
public:
	CHouseGridDlg(CWnd* pParent = NULL);  
	virtual ~CHouseGridDlg();

	ushort getGridTypeID() const;

	int getRoomID() const;
	void resetContent();
	void enableAllCtrl(bool enable);
	
	void notifySystemSetup();

	void notifySceneCreate();

	void notifySceneDestroy();

	void setFloorHeight( int height );

	enum { IDD = IDD_HOUSE_GRID };

protected:
	void _resetFloorHeightSld();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedHgAsspla();
	afx_msg void OnNMReleasedcaptureHgFloorheiSld(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedHgCreategrid();
	afx_msg void OnBnClickedHgWallgrid();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedHgStairGrid();
	afx_msg void OnEnChangeHgFloorhei();
	afx_msg void OnBnClickedHgEditroom();
	afx_msg void OnCbnSelchangeHgRoomno();
	afx_msg void OnEnChangeHgAssplaGrid();
protected:
	/// 上个楼层高度滚动条的位置
	int			mLastFloorHeightSldPos;

	CButton		m_wndAssistPlane;
	CEdit		m_wndAssistPlaneGridNum;
	CSliderCtrl m_wndFloorHeightSld;
	CComboBox	m_wndGridType;
	CComboBox	m_wndRoomID;
	CEdit		m_wndFloorHeight;
public:
	CButton		m_wndLinkGrid;
	CButton		m_wndEditRoom;
	CButton		m_wndCreateGrid;
	CButton		m_wndWallGrid;
};

extern CHouseGridDlg * g_HouseGridDlg;