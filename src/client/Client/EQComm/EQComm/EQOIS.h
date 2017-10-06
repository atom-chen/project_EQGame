/*********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: OIS���� ���봦��ϵͳ
*		
**********************************************************************/
#ifndef EQOIS_H
#define EQOIS_H

#include "EQMainPrerequisites.h"
#include "OIS.h"

namespace OIS
{
	class InputManager;
	class Keyboard;
	class Mouse;
	class KeyListener;
	class MouseListener;
	class MouseEvent;
	class KeyEvent;
	enum  MouseButtonID;
}

class EQOIS : 
	public Ogre::Singleton<EQOIS>,
	public Ogre::WindowEventListener, 
	public OIS::KeyListener, 
	public OIS::MouseListener
{
public:
	friend class AppListener;
	EQOIS(void);
	~EQOIS(void);
	void	initial(size_t hWnd);
	void	update(float timElapsed);
	void	cleanup();
	
	//�¼���������
	bool 	mouseMoved( const OIS::MouseEvent &arg );
	bool 	mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool 	mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool 	keyPressed( const OIS::KeyEvent &arg );
	bool 	keyReleased( const OIS::KeyEvent &arg );

	OIS::Mouse* getMouse(){return mMouse;}

	bool	getSkip() { return bSkip; }
	void    setSkip(bool skip) { bSkip = skip; }

	bool	isKeyDown( OIS::KeyCode key );

private:/*
	bool 	_intersects(POINT mouse, Ogre::Vector3& out);
	void 	_rmouseup(POINT mouse);
	void 	_mmouseMove(POINT delta, POINT mouse);
	void 	_zoom( float zoom);
	void 	_lmousedown(POINT p);
	void	_lmouseup(POINT p);
	void	_pickMove(POINT mouse);
	bool	_pickClick(POINT mouse);*/

	//POINT						mMouseLastPoint;			//���걣��
	//bool							mMidmousedown			//�м�����
	//									,mLMousedown		
	//									,mLMouseDraged;

	//float							mLmousedownElapsed;

	// OIS stuff
	OIS::InputManager	*mInputManager;
	OIS::Keyboard			*mKeyboard;
protected:
	OIS::Mouse				*mMouse;
	float					mActiveElapsed;
	bool                    bSkip;
};

#endif