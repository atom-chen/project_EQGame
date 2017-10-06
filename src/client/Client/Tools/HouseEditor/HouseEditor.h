// HouseEditor.h : AvatarEditor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


class CHouseEditorApp : public CWinApp
{
public:
	CHouseEditorApp();
public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	virtual BOOL PumpMessage();
private:
	BOOL _InitInstance();
	int _Run();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
protected:
	float m_goingTime;
};

extern CHouseEditorApp theApp;