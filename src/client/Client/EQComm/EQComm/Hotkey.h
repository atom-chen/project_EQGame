/******************************************************************
* ����: ��_Mike
* ʱ��: 20010-7-29
* ����: ��Ϸ��Ʒ�϶�����
*******************************************************************/
#ifndef HOTKEY_H
#define HOTKEY_H
#include "inputdef.h"

class Hotkey :
	public EQInputEventListener
{
public:
	enum keycode
	{
		KC_RETURN   = 0x1C, // ��λ�������ı��� 
		KC_R		= 0x13,	//	R����������������(hezhr)
		KC_LSHIFT   = 0x2A,
		KC_RSHIFT   = 0x36,
		KC_F1		= 0x3B,
		KC_F2		= 0x3C,
		KC_F3		= 0x3D,
		KC_F4		= 0x3E,
		KC_F9		= 0x43,	//	������ɫ����(2011/6/20 add by hezhr)
		KC_SysRq    = 0xB7,
	};

	Hotkey(void);
	virtual ~Hotkey(void);

	static Hotkey& getSingleton();

	bool isKeydown(int key);
	void setKeyFlag(int key, bool flag);

	virtual void KeyUp( UINT& flags, UINT nChar, UINT nRepCnt = 0);

	virtual void KeyDown( UINT& flags, UINT nChar, UINT nRepCnt = 0);

private:
	std::map<int,bool> mHotkeys;
};

#define sHotkey Hotkey::getSingleton()
#endif