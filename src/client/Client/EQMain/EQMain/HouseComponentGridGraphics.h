/******************************************************************************
* 作者: Hongjx
* 时间: 2009-12-25
* 描述: 房屋部件图形辅助类
*       主要用来显示 占据网格,阻挡网格,摆放网格,偏移高度
******************************************************************************/
#ifndef HouseComponentGridGraphics_H
#define HouseComponentGridGraphics_H

#include "EQMainPrerequisites.h"



struct HouseCompPropRow;


/** 
	主要用来显示占据网格,阻挡网格,摆放网格
*/
class HouseComponentGridGraphics
{
public:
	virtual ~HouseComponentGridGraphics();
	HouseComponentGridGraphics();
public:
	/** 显示占据网格
		@目前颜色表示：
		     红色 -- 无法摆放
		     绿色 -- 可以摆放
			 黄色 -- 可以替换
	*/
	void showHoldGrid( bool bShow, Ogre::ColourValue col ); 
	void showHoldGrid( bool bShow ); 
	void showRedHoldGrid();
	void showGreenHoldGrid();
	void showYellowHoldGrid();
	void hideHoldGrid();

	/** 显示阻挡网格(寻路相关)		
	*/
	void showStopGrid( bool bShow );
	
	/** 显示物件上方可供别人摆放的网格
	*/
	void showLayGrid( bool bShow );
	
	/** 显示偏移高度
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
