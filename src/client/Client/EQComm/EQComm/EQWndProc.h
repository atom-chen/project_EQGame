/*********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ��Ϸ���ڴ���
**********************************************************************/
#ifndef EQWndProc_H
#define EQWndProc_H

#include "EQMainPrerequisites.h"

class EQWndProc : public Ogre::Singleton<EQWndProc>
{
public:
	EQWndProc():mActive(true){}
	virtual ~EQWndProc(){}
	void			cleanup();
	HWND			createWindow(HINSTANCE hInstance);
	HWND			getHWnd(){return mHWnd;}
	HMENU			getHMenu(){return mHMenu;}
	//���úͻ�ȡ���뷨������
	void			setImeInput(bool b){ mIsImeInput = b; }
	bool			isImeInput() const { return mIsImeInput; }

	int				getWndWidth(){return mWndWidth;}
	int				getWndHeight(){return mWndHeight;}
	void			setWndSize(int w,int h){mWndHeight=h;mWndWidth=w;}
	bool			isActive(){ return mActive;}
	void			setActive(bool a){mActive = a;}

public:
	static void AddNotifyIcon(HWND hWnd, HINSTANCE hInst, UINT uId, const char *tip);
	static void DelNotifyIcon(HWND hWnd, UINT uId);

protected:
	bool			mActive;
	HWND			mHWnd;
	HMENU			mHMenu;
	int				mWndWidth,
					mWndHeight;
	bool			mIsImeInput;
};

#define sEQWndProc EQWndProc::getSingleton()

//�ͷ��ڴ棬���������õ������ڴ���������ڴ�
int releaseMemory(void*);

#endif