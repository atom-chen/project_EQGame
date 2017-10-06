/******************************************************************************
* 作者: Y3
* 时间: 2009-10-09
* 描述: 渲染帧和输入的监听
******************************************************************************/
#ifndef AppFrameListener_H
#define AppFrameListener_H


class AppFrameListener : public Ogre::FrameListener
{
public:
	AppFrameListener();
	virtual ~AppFrameListener();

public:
	// Overrides
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);

};

#endif