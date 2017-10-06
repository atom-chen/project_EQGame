/******************************************************************************
* ����: Y3
* ʱ��: 2005-6-4
* ����: ����TD
******************************************************************************/
#ifndef __IntersectPoint_H__
#define __IntersectPoint_H__

#include "HitIndicator.h"


class IntersectPoint : public HitIndicator
{
protected:
	Ogre::SceneNode* mIntersectNode;
	Ogre::Entity* mIntersectEntity;

public:
	IntersectPoint();
	~IntersectPoint();

	const std::string& getName(void) const;

	void reshape(void);
	void hide(void);

	// ������ײС��ԭ���Ĳ���
	void SetMaterial();

	// ������ײС����²���.
	void SetMaterial(std::string& strMaterialName);



protected:
	void _setHitPoint(Real x, Real y);
};



#endif // __IntersectPoint_H__
