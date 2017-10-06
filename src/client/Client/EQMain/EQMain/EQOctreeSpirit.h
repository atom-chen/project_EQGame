/******************************************************************************
* 作者: Y3
* 时间: 2011-3-15
* 描述: 
******************************************************************************/
#ifndef EQOCTREESPRIRIT_H
#define EQOCTREESPRIRIT_H

#include <Ogre/OgreAxisAlignedBox.h>

class EQOctreeNode;

/** 使用者继承这个类
*/
class EQOctreeSpirit
{
	friend class EQOctreeNode;
public:
	EQOctreeSpirit() : mAtt(0) {}
	virtual ~EQOctreeSpirit() {deattach();}


	virtual const Ogre::AxisAlignedBox& _getWorldAABB(void) const = 0;

	//通知上次被检测
	virtual void notifyLastCheck() = 0;

	//通知被检测
	virtual void notifyCheck() = 0;


private:
	// 通知宿主, 我和你没关系拉
	void deattach();

	EQOctreeNode* mAtt;
};



#endif
