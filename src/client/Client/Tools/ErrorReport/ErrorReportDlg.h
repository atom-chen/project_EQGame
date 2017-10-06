
#pragma once
#include "afxwin.h"

#include <string>


class CErrorReportDlg : public CDialog
{

public:
	CErrorReportDlg(const std::string & cmdLine);	

	enum { IDD = IDD_ERRORREPORT_DIALOG };
private:
	bool CanUpload() const;
	void LoadTemplateFile();
	void UploadFile(const std::string & sFile);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	
protected:
	virtual void OnOK() {}

protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	CEdit mEdit1;	
	std::string mTemplateFile;
	std::string mReportServer;
	std::string mDmpFile;
	std::string mDateTimeStr;
	std::string mComputer;
	std::string mErrorType;
	std::string mAccount;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
