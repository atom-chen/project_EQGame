// HouseEditor.h : AvatarEditor Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


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