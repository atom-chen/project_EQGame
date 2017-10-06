/*********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 游戏监听
*		游戏中每帧需要更新的内容	
*		游戏调试用显示调试数据，可增加相应OlerlayElement
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
		初始化
		@param renderTarget 目标渲染rederTarget
	*/
	void			initial(Ogre::RenderTarget* renderTarget);
	//显示面板开启
    void			showDebugOverlay(bool show);
    bool			isDebugOverlayShown(void) const;
	//状态面板
    void			showStatPanel(bool show);
    bool			isStatPanelShown(void) const;
	//LOGO面板
    void			showLogoPanel(bool show);
    bool			isLogoPanelShown(void) const;
	//帧监听
	virtual bool	frameStarted(const Ogre::FrameEvent& evt);
    virtual bool	frameEnded(const Ogre::FrameEvent& evt);

protected:
	void			updateStats(void);
	Ogre::Overlay*			mDebugOverlay;
	Ogre::OverlayElement*	mStatPanel;		//左边调试面板
	Ogre::OverlayElement*	mLogoPanel;		//右边LOGO
	Ogre::RenderTarget*		mRenderTarget;
};

#endif