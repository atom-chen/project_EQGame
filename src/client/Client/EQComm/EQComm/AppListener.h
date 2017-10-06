/*********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ��Ϸ����
*		��Ϸ��ÿ֡��Ҫ���µ�����	
*		��Ϸ��������ʾ�������ݣ���������ӦOlerlayElement
**********************************************************************/
#ifndef DebugListener_H
#define DebugListener_H

#include <OgreFrameListener.h>
#include "EQMainPrerequisites.h"

class AppListener : public Ogre::FrameListener
	,public Ogre::Singleton<AppListener>
{
public:
					AppListener();
	virtual 		~AppListener();
	void			cleanup();
	/**
		��ʼ��
		@param renderTarget Ŀ����ȾrederTarget
	*/
	void			initial(Ogre::RenderTarget* renderTarget);
	//��ʾ��忪��
    void			showDebugOverlay(bool show);
    bool			isDebugOverlayShown(void) const;
	//״̬���
    void			showStatPanel(bool show);
    bool			isStatPanelShown(void) const;
	//LOGO���
    void			showLogoPanel(bool show);
    bool			isLogoPanelShown(void) const;
	//֡����
	virtual bool	frameStarted(const Ogre::FrameEvent& evt);
    virtual bool	frameEnded(const Ogre::FrameEvent& evt);

protected:
	void			updateStats(void);
	Ogre::Overlay*			mDebugOverlay;
	Ogre::OverlayElement*	mStatPanel;		//��ߵ������
	Ogre::OverlayElement*	mLogoPanel;		//�ұ�LOGO
	Ogre::RenderTarget*		mRenderTarget;
};

#endif