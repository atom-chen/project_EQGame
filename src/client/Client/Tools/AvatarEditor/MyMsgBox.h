#pragma once

#include "resource.h"
#include <string>


class CMyMsgBox : public CDialog
{
public:
	CMyMsgBox(CWnd* pParent = NULL);   
	virtual ~CMyMsgBox();

	enum { IDD = IDD_FORMVIEW_MSGBOX };
public:
	static void Show( const std::string & msg );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOK();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
private:	
	CEdit		mEdit1;
	HICON		mIcon;
	std::string mCaption;
	std::string mMsg;
};
