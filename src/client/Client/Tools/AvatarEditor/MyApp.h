// MyApp.h : AvatarEditor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号



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

// 换装者
class Avatar;
extern Avatar * g_Avatar;

class ItemPack;
extern ItemPack * g_ItemPack;

class HouseSceneComponent;
extern HouseSceneComponent * g_HouseComp;
