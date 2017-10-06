// MyApp.h : AvatarEditor Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������



class CMyApp : public CWinApp
{
public:
	CMyApp();
public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	virtual BOOL PumpMessage();
private:
	BOOL _InitInstance();
	int _Run();
public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
protected:
	float m_goingTime;
};

extern CMyApp theApp;

// ��װ��
class Avatar;
extern Avatar * g_Avatar;

class ItemPack;
extern ItemPack * g_ItemPack;

class HouseSceneComponent;
extern HouseSceneComponent * g_HouseComp;
