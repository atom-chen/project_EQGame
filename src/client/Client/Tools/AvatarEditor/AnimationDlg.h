#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAnimationDlg dialog

class CAnimationDlg : public CDialog
{
public:
	CAnimationDlg(CWnd* pParent = NULL);   
	virtual ~CAnimationDlg();

// Dialog Data
	enum { IDD = IDD_ANIMATION_DLG };

public:
	void resetContent();
	void notifyAvatarCreate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAnPlay();
	afx_msg void OnCbnSelchangeAnActionlist();
	afx_msg void OnBnClickedAnPause();
	afx_msg void OnBnClickedAnStop();
	afx_msg void OnBnClickedAnLoop();
	afx_msg void OnBnClickedAnReset();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CComboBox	m_wndAnimCB;
	CSliderCtrl m_wndRateSlider;
	CStatic		m_wndRateText;
	CButton		m_wndAnimCheck;
	std::map<std::string, std::string> mActionMap;
};
