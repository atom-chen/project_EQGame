/******************************************************************************
* ����: Hongjx
* ʱ��: 2009-12-25
* ����: ���ݲ���ͼ�θ�����
*       ��Ҫ������ʾ ռ������,�赲����,�ڷ�����,ƫ�Ƹ߶�
******************************************************************************/
#ifndef HouseComponentGridGraphics_H
#define HouseComponentGridGraphics_H

#include "EQMainPrerequisites.h"



struct HouseCompPropRow;


/** 
	��Ҫ������ʾռ������,�赲����,�ڷ�����
*/
class HouseComponentGridGraphics
{
public:
	virtual ~HouseComponentGridGraphics();
	HouseComponentGridGraphics();
public:
	/** ��ʾռ������
		@Ŀǰ��ɫ��ʾ��
		     ��ɫ -- �޷��ڷ�
		     ��ɫ -- ���԰ڷ�
			 ��ɫ -- �����滻
	*/
	void showHoldGrid( bool bShow, Ogre::ColourValue col ); 
	void showHoldGrid( bool bShow ); 
	void showRedHoldGrid();
	void showGreenHoldGrid();
	void showYellowHoldGrid();
	void hideHoldGrid();

	/** ��ʾ�赲����(Ѱ·���)		
	*/
	void showStopGrid( bool bShow );
	
	/** ��ʾ����Ϸ��ɹ����˰ڷŵ�����
	*/
	void showLayGrid( bool bShow );
	
	/** ��ʾƫ�Ƹ߶�
	*/
	void showOffsetHeight( bool bShow );

	void detach();
	void attach( Ogre::SceneNode * mainNode, const HouseCompPropRow * r );
private:
	const HouseCompPropRow * mHouseCompPropRow;
	Ogre::SceneNode *		mMainNode;

	Ogre::SceneNode *		mHoldGridNode;
	Ogre::SceneNode *		mRedHoldGridNode;
	Ogre::SceneNode *		mStopGridNode;
	Ogre::SceneNode *		mLayGridNode;
	Ogre::ManualObject *	mHoldGridEntity;
	Ogre::ManualObject *	mRedHoldGridEntity;
	Ogre::ManualObject *	mStopGridEntity;
	Ogre::ManualObject *	mLayGridEntity;
	bool					mShowOffsetHeight;
	Ogre::ColourValue		mHoldGridColor;
};




#endif
