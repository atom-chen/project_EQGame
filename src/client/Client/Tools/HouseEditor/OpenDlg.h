#pragma once

class COpenDlg : public CDialog
{
	DECLARE_DYNAMIC(COpenDlg)

public:
	COpenDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COpenDlg();

	static void Execute();

private:
	std::vector<std::string> nNameList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnListDblClicked();
	afx_msg void OnBnClicked();

protected:
	CListBox m_list;
	CButton m_btn;
};
