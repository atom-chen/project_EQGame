/******************************************************************************
* ����: Y3
* ʱ��: 2011-3-15
* ����: 
******************************************************************************/
#ifndef EQOCTREESPRIRIT_H
#define EQOCTREESPRIRIT_H

#include <Ogre/OgreAxisAlignedBox.h>

class EQOctreeNode;

/** ʹ���߼̳������
*/
class EQOctreeSpirit
{
	friend class EQOctreeNode;
public:
	EQOctreeSpirit() : mAtt(0) {}
	virtual ~EQOctreeSpirit() {deattach();}


	virtual const Ogre::AxisAlignedBox& _getWorldAABB(void) const = 0;

	//֪ͨ�ϴα����
	virtual void notifyLastCheck() = 0;

	//֪ͨ�����
	virtual void notifyCheck() = 0;


private:
	// ֪ͨ����, �Һ���û��ϵ��
	void deattach();

	EQOctreeNode* mAtt;
};



#endif
