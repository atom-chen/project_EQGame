/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-09
* ����: ��Ⱦ֡������ļ���
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